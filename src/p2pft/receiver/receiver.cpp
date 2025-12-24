#include "receiver.hpp"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <print>
#include <system_error>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <google/protobuf/any.pb.h>

#include <proto/FileTransferProposalReq.pb.h>

#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/message_receiver/message_receiver.hpp"

namespace p2pft
{

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

    auto sessionPtr     = *maybeSession;

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

    std::unique_ptr<comms::IMessageReceiver> messageReceiver = std::make_unique<comms::MessageReceiver>(sessionPtr);
    messageReceiver->subscribe([this](const std::error_code& ec, std::unique_ptr<google::protobuf::Any> anyPtr)  {
        if (ec)
        {
            std::println("Message recival failed: {}", ec.message());
            return;
        }

        std::println("Successfully received a message");
        handleMessage(std::move(anyPtr));
    });

    auto work_guard = boost::asio::make_work_guard(*io);

    io->run();
}

void Receiver::handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    if (anyPtr->Is<proto::FileTransferProposalReq>())
    {
        std::println("Got FileTransferProposalReq");
        handleFileTransferProposalReq(std::move(anyPtr));
    }
    else
    {
        std::println("NOT FileTransferProposalReq!!!");
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
        "File size: {}"
        "File name: {}",
        fileSize,
        fileName);
}

}  // namespace p2pft
