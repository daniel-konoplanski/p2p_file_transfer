#pragma once

#include <expected>
#include <string_view>

#include <boost/asio/ip/address_v4.hpp>
#include <boost/system/detail/error_code.hpp>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

using SessionOrError = std::expected<SessionPtr, boost::system::error_code>;

class ConnectionManager
{
public:
    ConnectionManager(IoContextPtr io, Port port);

public:
    SessionOrError listen();
    SessionOrError connect(std::string_view address);

private:
    IoContextPtr io_;
    Port         port_;
};

}  // namespace p2pft::comms
