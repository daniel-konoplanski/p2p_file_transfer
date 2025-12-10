#pragma once

#include <cstddef>

#include "lib.comms/Session.hpp"

namespace p2pft::comms
{

using SenderCallback = std::function<void(const std::error_code&, size_t)>;

class MessageSender
{
public:
    MessageSender(SessionPtr session);

public:
    void send(const google::protobuf::Message& message, const SenderCallback& callback);

private:
    SessionPtr             session_;
    std::vector<std::byte> buffer_;
};

}  // namespace p2pft::comms
