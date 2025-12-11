#pragma once

#include <cstddef>

#include "lib.comms/i_sender.hpp"
#include "lib.comms/session.hpp"

namespace p2pft::comms
{

using SenderCallback = std::function<void(const std::error_code&, size_t)>;

class MessageSender : public IMessageSender
{
public:
    MessageSender(SessionPtr session);
    virtual ~MessageSender() = default;

public:
    void send(const google::protobuf::Message& message, SenderCallback callback) override;

private:
    SessionPtr             session_;
    std::vector<std::byte> buffer_;
};

}  // namespace p2pft::comms
