#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace p2p_ft::comms
{

using IoContextPtr = std::shared_ptr<boost::asio::io_context>;
using TcpSocket    = std::shared_ptr<boost::asio::ip::tcp::socket>;
using IoContextPtr = std::shared_ptr<boost::asio::io_context>;
using Port         = uint16_t;

struct Session
{
    std::unique_ptr<TcpSocket> socketPtr_;
};

using SessionPtr = std::shared_ptr<Session>;

}  // namespace p2p_ft::comms
