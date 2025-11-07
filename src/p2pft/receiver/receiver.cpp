#include "receiver.hpp"

#include <memory>
#include <print>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

namespace p2pft
{

Receiver::Receiver(cli::ReceiverArgs args)
    : args_{ args }
    , ioContext_{ std::make_shared<boost::asio::io_context>() }
{
}

void Receiver::start()
{
    using boost::asio::ip::tcp;

    acceptor_ = std::make_shared<TcpAcceptor>(
        *ioContext_,
        tcp::endpoint(boost::asio::ip::tcp::v4(), args_.port));

    std::print("Listening for incomming requests...");

    acceptor_->async_accept([](boost::system::error_code ec, tcp::socket socket) {
        if (ec)
        {
            std::print("Failed to accept the connection {}", ec.message());
            return;
        }
    });
}

}  // namespace p2pft
