#include "lib.comms/message_receiver/message_receiver.hpp"

#include <cstddef>
#include <cstdio>
#include <memory>
#include <print>
#include <utility>

#include <boost/asio/read.hpp>

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

    readHeader();
}

void MessageReceiver::readHeader()
{
    auto& socketPtr = session_->socketPtr_;

    auto processBody = [this](std::error_code ec, auto) {
        if (ec)
        {
            std::println(stderr, "Header read failed: {}", ec.message());
            return;
        }

        uint64_t size;
        std::memcpy(&size, headerBuffer_.data(), sizeof(uint64_t));
        readBody(size);
    };

    boost::asio::async_read(*socketPtr, boost::asio::buffer(headerBuffer_), processBody);
}

void MessageReceiver::readBody(uint64_t size)
{
    auto& socketPtr = session_->socketPtr_;

    buffer_.clear();
    buffer_.resize(size);

    auto getMessage = [this, size](std::error_code ec, auto) {
        if (ec)
        {
            std::println(stderr, "Message read failed: {}", ec.message());
            return;
        }

        std::println("Received a message of size {}B", size);

        auto anyPtr      = std::make_unique<google::protobuf::Any>();
        auto parseResult = anyPtr->ParseFromArray(buffer_.data(), static_cast<int>(size));

        if (!parseResult)
        {
            std::println(stderr, "Failed to unpack the received message");
            return;
        }

        if (callback_)
            callback_(ec, std::move(anyPtr));

        readHeader();
    };

    boost::asio::async_read(*socketPtr, boost::asio::buffer(buffer_), getMessage);
}

}  // namespace p2pft::comms
