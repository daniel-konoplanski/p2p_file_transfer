#include "receiver.hpp"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <print>
#include <system_error>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <google/protobuf/any.pb.h>

#include <proto/FileTransferProposalReq.pb.h>
#include <proto/FileTransferProposalResp.pb.h>

#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/message_receiver/message_receiver.hpp"
#include "lib.comms/message_sender/message_sender.hpp"
#include "proto/FileChunk.pb.h"
#include "proto/Result.pb.h"

namespace p2pft
{

namespace
{

bool getUserConfirmation()
{
    std::string response;
    std::println("Accept the transfer? (yes/no): ");
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
    auto io = std::make_shared<boost::asio::io_context>();

    std::println("Listening on port {} for incomming requests", args_.port);

    auto connectionMgrPtr = std::make_unique<comms::ConnectionManager>(io, args_.port);
    auto maybeSession     = connectionMgrPtr->listen();

    if (!maybeSession)
    {
        std::println(stderr, "Failed to receive connection");
        return;
    }

    auto sessionPtr = *maybeSession;

    if (!sessionPtr)
    {
        std::println(stderr, "sessionPtr is nullptr!!!!");
        return;
    }

    auto remoteEndpoint = sessionPtr->socketPtr_->remote_endpoint();

    std::println(
        "Successfully connected to sender with address {}:{}",
        remoteEndpoint.address().to_string(),
        remoteEndpoint.port());

    messageSender_   = std::make_unique<comms::MessageSender>(sessionPtr);
    messageReceiver_ = std::make_unique<comms::MessageReceiver>(sessionPtr);

    messageReceiver_->subscribe(
        [this](const std::error_code& ec, std::unique_ptr<google::protobuf::Any> anyPtr) {
            if (ec)
            {
                std::println("Message recival failed: {}", ec.message());
                return;
            }

            handleMessage(std::move(anyPtr));
        });

    auto work_guard = boost::asio::make_work_guard(*io);

    io->run();
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

    auto fileSize = req.files().size();
    auto fileName = req.files().name();

    std::println(
        "Received file transfer proposal\n"
        "File size: {}\n"
        "File name: {}\n",
        fileSize,
        fileName);

    sendFileTransferProposalResp();
}

void Receiver::handleFileChunk(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    proto::FileChunk msg;

    auto unpackResult = anyPtr->UnpackTo(&msg);

    if (!unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferProposalReq");
        return;
    }

    // TODO: implement a fileWriter so the chunks can be written
}

void Receiver::sendFileTransferProposalResp()
{
    using enum proto::Result;
    proto::FileTransferProposalResp resp;

    proto::Result reqResult = getUserConfirmation() ? ACCEPTED : REJECTED;

    resp.set_result(reqResult);

    messageSender_->send(resp, [](const std::error_code& ec, size_t) {
        std::println("Send the FileTransferProposalResp with status {}", ec.message());
    });
}

}  // namespace p2pft
