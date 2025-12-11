#pragma once

#include <functional>
#include <memory>
#include <system_error>

#include <google/protobuf/message.h>

namespace p2pft::comms
{

using SenderCallback = std::function<void(const std::error_code&, size_t)>;

class IMessageSender
{
public:
    virtual ~IMessageSender() = default;

public:
    virtual void send(const google::protobuf::Message& message, SenderCallback callback) = 0;
};

}  // namespace p2pft::comms
