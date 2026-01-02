#include "receiver.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <print>
#include <string>
#include <system_error>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <google/protobuf/any.pb.h>

#include <proto/FileTransferProposalReq.pb.h>
#include <proto/FileTransferProposalResp.pb.h>

#include "p2pft/connection/connection.hpp"

#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/message_receiver/message_receiver.hpp"
#include "lib.comms/message_sender/message_sender.hpp"
#include "lib.filesystem/file_writer.hpp"
#include "proto/FileChunk.pb.h"
#include "proto/FileTransferComplete.pb.h"
#include "proto/Result.pb.h"

namespace p2pft
{

namespace
{

bool getUserConfirmation()
{
    std::string response;
    std::print("Accept the transfer? (yes/no): ");
    std::getline(std::cin, response);

    std::ranges::transform(response, response.begin(), ::tolower);

    return response == "yes" || response == "y";
}

}  // namespace

Receiver::Receiver(cli::ReceiverArgs args)
    : args_{ args }
{
}

void Receiver::run()
{
    io_ = std::make_shared<boost::asio::io_context>();

    std::println("Listening on port {} for incomming requests...", args_.port);

    auto maybeSession = comms::ConnectionManager::listen(io_, args_.port);

    if (!maybeSession)
    {
        std::println(stderr, "Failed to receive connection");
        return;
    }

    connection_ = std::make_unique<Connection>(*maybeSession);

    const auto& remoteEndpoint = connection_->accessSession().socketPtr_->remote_endpoint();

    std::println(
        "Successfully connected to sender with address {}:{}",
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

    auto work_guard = boost::asio::make_work_guard(*io_);

    io_->run();
}

void Receiver::handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    if (anyPtr->Is<proto::FileTransferProposalReq>())
    {
        handleFileTransferProposalReq(std::move(anyPtr));
    }
    else if (anyPtr->Is<proto::FileChunk>())
    {
        handleFileChunk(std::move(anyPtr));
    }
}

void Receiver::handleFileTransferProposalReq(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    proto::FileTransferProposalReq req;

    auto unpackResult = anyPtr->UnpackTo(&req);

    if (!unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferProposalReq");
        return;
    }

    fileName_      = req.files().name();
    auto fileSize  = req.files().size();
    auto spaceInfo = std::filesystem::space(args_.outDir);

    if (spaceInfo.available < fileSize)
    {
        std::println(
            stderr,
            "Not enough available space in the provided location {}, needed: {}, available: {}",
            args_.outDir,
            fileSize,
            spaceInfo.available);
        return;
    }

    std::println(
        "Received file transfer proposal\n"
        "File size: {}B\n"
        "File name: {}\n",
        fileSize,
        fileName_);

    sendFileTransferProposalResp();
}

void Receiver::handleFileChunk(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    using enum proto::Result;

    proto::FileChunk msg;

    auto unpackResult = anyPtr->UnpackTo(&msg);

    if (!unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferProposalReq");
        return;
    }

    const bool  isLast = msg.is_last();
    const auto& data   = msg.data();

    static auto fileWriter = std::make_unique<files::FileWriter>(args_.outDir, fileName_);
    fileWriter->write(data, isLast);

    if (isLast)
    {
        fileWriter = nullptr;
        sendFileTransferComplete(ACCEPTED);
    }
}

void Receiver::sendFileTransferProposalResp()
{
    using enum proto::Result;
    proto::FileTransferProposalResp resp;

    proto::Result reqResult = getUserConfirmation() ? ACCEPTED : REJECTED;

    resp.set_result(reqResult);

    connection_->accessMsgSender().send(resp, [](const std::error_code& ec, size_t) {
        if (ec)
        {
            std::println("Sending FileTransferProposalResp failed: {}", ec.message());
        }
    });
}

void Receiver::sendFileTransferComplete(proto::Result result)
{
    proto::FileTransferComplete resp;
    resp.set_result(result);

    connection_->accessMsgSender().send(resp, [this](const std::error_code& ec, size_t) {
        std::println("File transfer completed", ec.message());
        cleanup();
    });
}

void Receiver::cleanup()
{
    connection_.reset();
    io_->stop();
}

}  // namespace p2pft
