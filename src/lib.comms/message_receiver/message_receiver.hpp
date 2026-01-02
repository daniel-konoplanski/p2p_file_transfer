#pragma once

#include <array>
#include <cstdint>
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
    void readHeader();
    void readBody(uint64_t size);

private:
    SessionPtr               session_;
    std::array<std::byte, 8> headerBuffer_;
    std::vector<std::byte>   buffer_;
    ReceiverCallback         callback_;
};

}  // namespace p2pft::comms
