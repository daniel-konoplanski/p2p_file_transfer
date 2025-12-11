#pragma once

#include <functional>
#include <system_error>

#include <google/protobuf/message.h>

using SenderCallback = std::function<void(const std::error_code&, size_t)>;

class IMessageSender
{
public:
    virtual void send(const google::protobuf::Message& message, SenderCallback callback) = 0;
};
