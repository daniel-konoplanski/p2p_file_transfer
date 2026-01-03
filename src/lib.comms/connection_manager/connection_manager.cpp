#include "connection_manager.hpp"

#include <expected>
#include <memory>
#include <utility>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/system/detail/error_code.hpp>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

using boost::asio::ip::tcp;

SessionOrError ConnectionManager::listen(const IoContextPtr& io, const Port port)
{
    const auto endpoint = tcp::endpoint(tcp::v4(), port);
    auto       acceptor = tcp::acceptor(*io, endpoint);

    boost::system::error_code ec{};

    auto socket = acceptor.accept(ec);

    if (ec) return std::unexpected(ec);

    acceptor.close();

    return std::make_shared<Session>(std::make_unique<TcpSocket>(std::move(socket)));
}

SessionOrError
ConnectionManager::connect(const IoContextPtr& io, const std::string_view address, const Port port)
{
    const auto addressV4 = boost::asio::ip::make_address_v4(address);
    const auto endpoint  = tcp::endpoint(addressV4, port);

    boost::system::error_code ec{};

    tcp::socket socket(*io);

    if (socket.connect(endpoint, ec)) return std::unexpected(ec);

    return std::make_shared<Session>(std::make_unique<TcpSocket>(std::move(socket)));
}

}  // namespace p2pft::comms
