#include "session.hpp"

namespace p2pft::comms
{

Session::Session(std::unique_ptr<SslSocket> sslSocket, std::unique_ptr<SslContext> sslContext)
    : socketPtr_(std::move(sslSocket))
    , sslContextPtr_(std::move(sslContext))
{
}

[[nodiscard]] EndPoint Session::getRemoteEndpoint() const
{
    return socketPtr_->lowest_layer().remote_endpoint();
}

}  // namespace p2pft::comms
