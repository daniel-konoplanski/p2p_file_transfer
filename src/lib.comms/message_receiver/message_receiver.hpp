#pragma once

#include <functional>
#include <memory>

#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

using ReceiverCallback =
    std::function<void(const std::error_code&, std::unique_ptr<google::protobuf::Any>)>;

class MessageReceiver
{
public:
    MessageReceiver(SessionPtr session);
    void subscribe(ReceiverCallback callback);

private:
    void handleMsg(std::error_code errorCode, size_t size);

private:
    SessionPtr session_;
    std::vector<std::byte> buffer_;
    ReceiverCallback callback_;
};

}  // namespace p2pft::comms
