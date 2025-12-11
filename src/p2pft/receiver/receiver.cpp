#include "receiver.hpp"

#include <cstdio>
#include <print>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <proto/FileTransferProposalReq.pb.h>

#include "lib.comms/connection_manager/connection_manager.hpp"

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

    auto work_guard = boost::asio::make_work_guard(*io);

    io->run();
}

}  // namespace p2pft
