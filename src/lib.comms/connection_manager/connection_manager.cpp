#include "connection_manager.hpp"

#include <algorithm>
#include <expected>
#include <memory>
#include <print>
#include <system_error>
#include <utility>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/system/detail/error_code.hpp>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

using boost::asio::ip::tcp;

ConnectionManager::ConnectionManager(IoContextPtr io, Port port)
    : io_{ std::move(io) }
    , port_{ port }
{
}

SessionOrError ConnectionManager::listen()
{
    auto endpoint = tcp::endpoint(tcp::v4(), port_);
    auto acceptor = tcp::acceptor(*io_, endpoint);

    boost::system::error_code ec{};

    auto socket = acceptor.accept(ec);

    if (ec) return std::unexpected(ec);

    acceptor.close();

    return std::make_shared<Session>(std::make_unique<TcpSocket>(std::move(socket)));
}

SessionOrError ConnectionManager::connect(std::string_view address)
{
    auto addressV4 = boost::asio::ip::make_address_v4(address);
    auto endpoint  = tcp::endpoint(addressV4, port_);

    boost::system::error_code ec{};

    tcp::socket socket(*io_);

    if (socket.connect(endpoint, ec)) return std::unexpected(ec);

    return std::make_shared<Session>(std::make_unique<TcpSocket>(std::move(socket)));
}

}  // namespace p2pft::comms
