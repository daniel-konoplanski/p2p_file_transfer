#include "message_sender.hpp"

#include <cstddef>
#include <cstdint>

#include <boost/asio/registered_buffer.hpp>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

MessageSender::MessageSender(SessionPtr session)
    : session_{ std::move(session) }
{
}

void MessageSender::send(const google::protobuf::Message& message, SenderCallback callback)
{
    uint64_t msgSize = message.ByteSizeLong();
    uint64_t sizeHeader = static_cast<uint64_t>(msgSize);

    buffer_.clear();
    buffer_.resize(sizeof(sizeHeader) + msgSize);

    std::memcpy(buffer_.data(), &sizeHeader, sizeof(sizeHeader));

    message.SerializeToArray(buffer_.data() + sizeof(sizeHeader), static_cast<int>(msgSize));

    auto& socketPtr = session_->socketPtr_;

    socketPtr->async_send(boost::asio::buffer(buffer_), callback);
}

}  // namespace p2pft::comms
