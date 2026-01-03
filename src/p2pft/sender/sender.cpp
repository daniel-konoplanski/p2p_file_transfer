#include "sender.hpp"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <ios>
#include <memory>
#include <print>
#include <system_error>

#include <boost/asio/io_context.hpp>

#include <proto/FileChunk.pb.h>
#include <proto/FileTransferComplete.pb.h>
#include <proto/FileTransferProposalReq.pb.h>
#include <proto/FileTransferProposalResp.pb.h>
#include <proto/Result.pb.h>

#include "lib.cli/parser.hpp"

#include "p2pft/connection/connection.hpp"

#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/i_sender.hpp"

namespace p2pft
{

Sender::Sender(cli::SenderArgs args)
    : args_{ std::move(args) }
{
}

void Sender::run()
{
    io_ = std::make_shared<boost::asio::io_context>();

    auto maybeSession = comms::ConnectionManager::connect(io_, args_.address, args_.port);

    if (!maybeSession)
    {
        std::println(
            stderr,
            "Failed to connect to receiver with address {}: {}",
            args_.address,
            maybeSession.error().message());
        return;  // TODO: return an error code;
    }

    connection_ = std::make_unique<Connection>(*maybeSession);

    const auto& remoteEndpoint = connection_->accessSession().socketPtr_->remote_endpoint();

    std::println(
        "Successfully connected to receiver with address {}:{}",
        remoteEndpoint.address().to_string(),
        remoteEndpoint.port());

    connection_->accessMsgReceiver().subscribe(
        [this](const std::error_code& ec, std::unique_ptr<google::protobuf::Any> anyPtr) {
            if (ec)
            {
                std::println("Message recival failed: {}", ec.message());
                return;
            }

            handleMessage(std::move(anyPtr));
        });

    std::filesystem::path filePath{ args_.path };

    std::error_code ec;
    auto            doesFileExist = std::filesystem::exists(filePath, ec);

    if (ec)
    {
        std::println(stderr, "Failed to read file {}: {}", filePath.string(), ec.message());
        return;  // TODO: return an error code;
    }

    if (!doesFileExist)
    {
        std::println(stderr, "File does not exist: {}", filePath.string());
        return;
    }

    auto fileSize = std::filesystem::file_size(filePath, ec);

    if (ec)
    {
        std::println(stderr, "Could not read file size: {}", filePath.string());
        return;
    }

    proto::FileInfo fileInfo;

    proto::FileTransferProposalReq req;
    p2pft::proto::FileInfo*        f = req.mutable_files();

    f->set_name(filePath.filename().string());
    f->set_size(fileSize);

    connection_->accessMsgSender().send(req, [](const auto& errorCode, auto) {
        if (errorCode)
        {
            std::println("Error during message sending: {}", errorCode.message());
            return;
        }
    });

    auto work_guard = boost::asio::make_work_guard(*io_);

    io_->run();
}

void Sender::handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    if (anyPtr->Is<proto::FileTransferProposalResp>())
    {
        handleFileTransferProposalResp(std::move(anyPtr));
    }
    else if (anyPtr->Is<proto::FileTransferComplete>())
    {
        handleFileTransferComplete(std::move(anyPtr));
    }
}

void Sender::handleFileTransferProposalResp(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    proto::FileTransferProposalResp resp;

    auto unpackResult = anyPtr->UnpackTo(&resp);

    if (!unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferProposalResp");
        return;
    }

    bool result = resp.result() == proto::Result::ACCEPTED ? true : false;

    if (!result)
    {
        std::println("Receiver rejected the file transfer");
        return;
    }

    std::println("Receiver accepted the request, starting file transfer...");
    startFileTransfer();
}

void Sender::startFileTransfer()
{
    const auto& filePath = args_.path;

    auto file = std::make_shared<std::ifstream>(args_.path, std::ios::binary);

    if (!file)
    {
        std::println(stderr, "Failed to open file: {}", filePath);
        return;
    }

    uint64_t fileSize    = std::filesystem::file_size(filePath);
    uint64_t totalChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    sendChunk(file, totalChunks, 1);
}

void Sender::sendChunk(std::shared_ptr<std::ifstream> file, uint64_t totalChunks, uint64_t chunkId)
{
    constexpr uint64_t CHUNK_SIZE = 8192U;
    std::vector<char>  chunkBuffer(CHUNK_SIZE);

    file->read(chunkBuffer.data(), CHUNK_SIZE);
    std::streamsize bytesRead = file->gcount();

    if (!bytesRead) return;

    proto::FileChunk fileChunkMsg;
    fileChunkMsg.set_id(chunkId);
    fileChunkMsg.set_size(bytesRead);
    fileChunkMsg.set_data(chunkBuffer.data(), bytesRead);
    fileChunkMsg.set_is_last(totalChunks == chunkId);

    connection_->accessMsgSender().send(
        fileChunkMsg,
        [this, file, totalChunks, chunkId](const std::error_code& ec, size_t) mutable {
            if (ec)
            {
                std::println(stderr, "Message sending failed {}", ec.message());
                return;
            }

            ++chunkId;
            sendChunk(file, totalChunks, chunkId);
        });
}

void Sender::handleFileTransferComplete(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    proto::FileTransferComplete resp;

    auto unpackResult = anyPtr->UnpackTo(&resp);

    if (!unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferComplete");
        return;
    }

    bool result = resp.result() == proto::Result::ACCEPTED ? true : false;

    if (!result)
    {
        std::println("Received FileTransferComplete with result FAILIURE");
        return;
    }

    std::println("File was transfered successfully");

    connection_->accessMsgReceiver().unsubscribe();
    cleanup();
}

void Sender::cleanup()
{
    connection_.reset();
    io_->stop();
}

}  // namespace p2pft
