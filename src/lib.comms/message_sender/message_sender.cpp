#include "message_sender.hpp"

#include <boost/asio/write.hpp>

#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>

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

    const uint64_t anySize    = any.ByteSizeLong();
    const uint64_t headerSize = anySize;

    buffer_.clear();
    buffer_.resize(sizeof(headerSize) + anySize);

    std::memcpy(buffer_.data(), &headerSize, sizeof(headerSize));

    any.SerializeToArray(buffer_.data() + sizeof(headerSize), static_cast<int>(anySize));

    boost::asio::async_write(session_->accessSslSocket(), boost::asio::buffer(buffer_), callback);
}

}  // namespace p2pft::comms
