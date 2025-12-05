#include "sender.hpp"

#include <cstdio>
#include <memory>
#include <print>

#include <boost/asio/io_context.hpp>

#include "lib.cli/parser.hpp"

#include "lib.comms/connection_manager/connection_manager.hpp"

namespace p2pft
{

Sender::Sender(cli::SenderArgs args)
    : args_{ std::move(args) }
{
}

void Sender::run()
{
    auto io = std::make_shared<boost::asio::io_context>();

    auto connectionMgrPtr = std::make_unique<comms::ConnectionManager>(io, args_.port);
    auto maybeSession     = connectionMgrPtr->connect(args_.address);

    if (!maybeSession)
    {
        std::println(
            stderr,
            "Failed to connect to receiver with address {}: {}",
            args_.address,
            maybeSession.error().message());
        return;
    }

    auto sessionPtr     = *maybeSession;
    auto remoteEndpoint = sessionPtr->socketPtr_->remote_endpoint();

    std::println(
        "Successfully connected to receiver with address {}:{}",
        remoteEndpoint.address().to_string(),
        remoteEndpoint.port());
}

}  // namespace p2pft
