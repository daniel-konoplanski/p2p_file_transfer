#pragma once

#include <functional>
#include <memory>

#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>

#include "lib.comms/i_receiver.hpp"
#include "lib.comms/session.hpp"

namespace p2pft::comms
{

class MessageReceiver : public IMessageReceiver
{
public:
    MessageReceiver(SessionPtr session);
    void subscribe(ReceiverCallback callback) override;

private:
    void handleMsg(std::error_code errorCode, size_t size);

private:
    SessionPtr             session_;
    std::vector<std::byte> buffer_;
    ReceiverCallback       callback_;
};

}  // namespace p2pft::comms
