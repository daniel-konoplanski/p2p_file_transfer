#pragma once

#include <string_view>
#include <boost/asio/ip/address_v4.hpp>
#include "lib.comms/Session.hpp"

namespace p2p_ft::comms
{

class ConnectionManager
{
public:
    ConnectionManager(IoContextPtr io, Port port, std::string_view address);
    ConnectionManager(IoContextPtr io, Port port);

public:
    SessionPtr listen();
    SessionPtr connect();

private:
    IoContextPtr io_;
    Port port_;
    std::string_view address_{};
};

}  // namespace p2p_ft::comms
