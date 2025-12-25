#include "message_sender.hpp"

#include <cstdint>

#include <boost/asio/registered_buffer.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/any.pb.h>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

MessageSender::MessageSender(SessionPtr session)
    : session_{ std::move(session) }
{
}

void MessageSender::send(const google::protobuf::Message& message, SenderCallback callback)
{
    google::protobuf::Any any;
    any.PackFrom(message);

    uint64_t anySize = any.ByteSizeLong();
    uint64_t sizeHeader = static_cast<uint64_t>(anySize);

    buffer_.clear();
    buffer_.resize(sizeof(sizeHeader) + anySize);

    std::memcpy(buffer_.data(), &sizeHeader, sizeof(sizeHeader));

    any.SerializeToArray(buffer_.data() + sizeof(sizeHeader), static_cast<int>(anySize));

    auto& socketPtr = session_->socketPtr_;

    socketPtr->async_send(boost::asio::buffer(buffer_), callback);
}

}  // namespace p2pft::comms
