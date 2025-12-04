#include "connection_manager.hpp"

#include <memory>
#include <print>
#include <system_error>
#include <utility>

#include <boost/system/detail/error_code.hpp>

#include "lib.comms/Session.hpp"

namespace p2p_ft::comms
{

using boost::asio::ip::tcp;

ConnectionManager::ConnectionManager(IoContextPtr io, Port port, std::string_view address)
    : io_{ std::move(io) }
    , port_{ port }
    , address_{ address }
{
}

ConnectionManager::ConnectionManager(IoContextPtr io, Port port)
    : ConnectionManager(io, port, {})
{
}

SessionPtr ConnectionManager::listen()
{
    auto endpoint = tcp::endpoint(tcp::v4(), port_);
    auto acceptor = tcp::acceptor(*io_, endpoint);

    boost::system::error_code ec{};

    std::println("Listening on port {}...", port_);

    auto socket = acceptor.accept(ec);

    if (!ec)
    {
        std::println("Failed to handle incoming connection: {}", ec.message());
        return nullptr;
    }

    acceptor.close();

    return std::make_shared<Session>(std::move(socket));
}

}  // namespace p2p_ft::comms
