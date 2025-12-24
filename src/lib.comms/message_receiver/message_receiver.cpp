#include "lib.comms/message_receiver/message_receiver.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <print>
#include <utility>

#include <google/protobuf/message.h>

#include "lib.comms/session.hpp"

namespace p2pft::comms
{

MessageReceiver::MessageReceiver(SessionPtr session)
    : session_{ std::move(session) }
{
}

void MessageReceiver::subscribe(ReceiverCallback callback)
{
    callback_ = std::move(callback);

    buffer_.resize(8192);

    auto& socketPtr = session_->socketPtr_;

    auto processMsg = [this](const std::error_code& ec, uint64_t size) {
        std::println("Received a message of size {}", size);
        handleMsg(ec, size);
    };

    socketPtr->async_receive(boost::asio::buffer(buffer_), processMsg);
}

void MessageReceiver::handleMsg(std::error_code ec, size_t)
{
    // TODO: Handle the size - might not receive the entire message in one go!

    if (ec)
    {
        callback_(ec, nullptr);
        return;
    }

    uint64_t anySize;
    std::memcpy(&anySize, buffer_.data(), sizeof(uint64_t));

    std::println("Received a message of size {}", anySize);

    auto anyPtr = std::make_unique<google::protobuf::Any>();
    anyPtr->ParseFromArray(buffer_.data() + sizeof(uint64_t), static_cast<int>(anySize));

    callback_(ec, std::move(anyPtr));
}

}  // namespace p2pft::comms
