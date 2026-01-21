#include "sender.hpp"

#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <memory>
#include <system_error>

#include <boost/asio/io_context.hpp>

#include <proto/FileChunk.pb.h>
#include <proto/FileTransferComplete.pb.h>
#include <proto/FileTransferProposalReq.pb.h>
#include <proto/FileTransferProposalResp.pb.h>
#include <proto/Result.pb.h>

#include "lib.cli/parser.hpp"

#include "p2pft/connection/connection.hpp"

#include "lib.certs/certificate_manager.hpp"
#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/i_sender.hpp"
#include "lib.ui/console_user_interface.hpp"
#include "lib.utils/format.hpp"

namespace p2pft
{

Sender::Sender(cli::SenderArgs args)
    : args_{ std::move(args) }
    , ui_{ std::make_unique<ui::ConsoleUserInterface>() }
{
}

void Sender::run()
{
    if (!cert::CertificateManager::isCertCreated()) cert::CertificateManager::create();

    io_ = std::make_shared<boost::asio::io_context>();

    if (const auto ec = establishConnection())
    {
        ui_->displayError(std::format("Connection failed: {}", ec.message()));
        return;
    }

    setupMessageReceiver();

    if (const auto ec = validateFile())
    {
        ui_->displayError(std::format("File validation failed: {}", ec.message()));
        return;
    }

    if (const auto ec = sendFileProposal())
    {
        ui_->displayError(std::format("Failed to send file proposal: {}", ec.message()));
        return;
    }

    auto work_guard = boost::asio::make_work_guard(*io_);
    io_->run();
}

std::error_code Sender::establishConnection()
{
    ui_->displayMessage(std::format("Connecting to {}:{}...", args_.address, args_.port));

    auto maybeSession = comms::ConnectionManager::connect(io_, args_.address, args_.port);

    if (!maybeSession)
    {
        return maybeSession.error();
    }

    connection_ = std::make_unique<Connection>(*maybeSession);
    ui_->displayMessage(std::format("Connected to receiver"));

    return {};
}

void Sender::setupMessageReceiver()
{
    connection_->accessMsgReceiver().subscribe(
        [this](const std::error_code& ec, std::unique_ptr<google::protobuf::Any> anyPtr) {
            if (ec)
            {
                ui_->displayError(std::format("Message receiving failed: {}", ec.message()));
                return;
            }
            handleMessage(std::move(anyPtr));
        });
}

std::error_code Sender::validateFile()
{
    const std::filesystem::path filePath{ args_.path };
    std::error_code ec;

    const bool exists = std::filesystem::exists(filePath, ec);

    if (ec) return ec;
    if (!exists) return std::make_error_code(std::errc::no_such_file_or_directory);

    fileInfo_.fileSize = std::filesystem::file_size(filePath, ec);
    if (ec) return ec;

    fileInfo_.fileName = filePath.filename().string();
    return {};
}

std::error_code Sender::sendFileProposal()
{
    proto::FileTransferProposalReq req;
    proto::FileInfo* f = req.mutable_files();

    f->set_name(fileInfo_.fileName);
    f->set_size(fileInfo_.fileSize);

    std::error_code sendError;

    connection_->accessMsgSender().send(req, [&sendError](const auto& errorCode, auto) {
        if (errorCode)
        {
            sendError = errorCode;
        }
    });

    return sendError;
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

    if (const auto unpackResult = anyPtr->UnpackTo(&resp); !unpackResult)
    {
        ui_->displayError("Failed to unpack message to FileTransferProposalResp");
        return;
    }

    if (const bool result = resp.result() == proto::Result::ACCEPTED; !result)
    {
        if (resp.result() == proto::Result::REJECTED)
            ui_->displayMessage("Receiver rejected the file transfer");
        else if (resp.result() == proto::Result::FAILED)
            ui_->displayMessage("Receiver failed the file transfer");

        cleanup();
        return;
    }

    ui_->displayMessage(std::format("Sending → {} ({})", fileInfo_.fileName, utils::formatBytes(fileInfo_.fileSize)));
    startFileTransfer();
}

void Sender::startFileTransfer()
{
    const auto& filePath = args_.path;

    const auto file = std::make_shared<std::ifstream>(args_.path, std::ios::binary);

    if (!file)
    {
        ui_->displayError(std::format("Failed to open file: {}", filePath));
        return;
    }

    const uint64_t fileSize    = std::filesystem::file_size(filePath);
    const uint64_t totalChunks = (fileSize + CHUNK_SIZE - 1) / CHUNK_SIZE;

    ui_->createProgressTracker(totalChunks);

    sendChunk(file, totalChunks, 1);
}

void Sender::sendChunk(std::shared_ptr<std::ifstream> file, uint64_t totalChunks, uint64_t chunkId)
{
    std::vector<char> chunkBuffer(CHUNK_SIZE);

    file->read(chunkBuffer.data(), CHUNK_SIZE);
    const std::streamsize bytesRead = file->gcount();

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
                ui_->displayError(std::format("Message sending failed {}", ec.message()));
                return;
            }
            ui_->updateProgress(chunkId);
            sendChunk(file, totalChunks, ++chunkId);
        });
}

void Sender::handleFileTransferComplete(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    proto::FileTransferComplete resp;

    if (const auto unpackResult = anyPtr->UnpackTo(&resp); !unpackResult)
    {
        ui_->displayError("Failed to unpack message to FileTransferComplete");
        return;
    }

    if (const bool result = resp.result() == proto::Result::ACCEPTED; !result)
    {
        ui_->displayMessage("Receiver failed the file transfer");
        return;
    }

    ui_->displayMessage("Transfer completed");

    connection_->accessMsgReceiver().unsubscribe();
    cleanup();
}

void Sender::cleanup()
{
    connection_.reset();
    io_->stop();
}

}  // namespace p2pft
