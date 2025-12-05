#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace p2pft::comms
{

using Port         = uint16_t;
using TcpSocket    = boost::asio::ip::tcp::socket;
using TcpSocketPtr = std::shared_ptr<TcpSocket>;
using IoContext    = boost::asio::io_context;
using IoContextPtr = std::shared_ptr<IoContext>;

struct Session
{
    std::unique_ptr<TcpSocket> socketPtr_;
};

using SessionPtr = std::shared_ptr<Session>;

}  // namespace p2pft::comms
