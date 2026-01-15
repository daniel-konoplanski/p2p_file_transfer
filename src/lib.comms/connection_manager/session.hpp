#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace p2pft::comms
{

using Port         = uint16_t;
using IoContext    = boost::asio::io_context;
using IoContextPtr = std::shared_ptr<IoContext>;
using SslContext   = boost::asio::ssl::context;
using TcpSocket    = boost::asio::ip::tcp::socket;
using SslSocket    = boost::asio::ssl::stream<TcpSocket>;
using EndPoint     = boost::asio::ip::tcp::endpoint;

class Session
{
public:
    Session(std::unique_ptr<SslSocket> sslSocket, std::unique_ptr<SslContext> sslContext);

    [[nodiscard]] EndPoint getRemoteEndpoint() const;

    [[nodiscard]] auto& accessSslSocket() const
    {
        return *socketPtr_;
    }

    [[nodiscard]] auto& accessSocket() const
    {
        return socketPtr_->lowest_layer();
    }

    [[nodiscard]] auto& accessSslContext() const
    {
        return *sslContextPtr_;
    }

private:
    std::unique_ptr<SslSocket> socketPtr_;
    std::unique_ptr<SslContext> sslContextPtr_;
};

using SessionPtr = std::shared_ptr<Session>;

}  // namespace p2pft::comms
