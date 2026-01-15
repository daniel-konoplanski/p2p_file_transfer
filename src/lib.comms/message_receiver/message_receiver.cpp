#include "message_receiver.hpp"

#include <cstdio>
#include <memory>
#include <print>
#include <utility>

#include <boost/asio/read.hpp>

#include <google/protobuf/message.h>

#include "../connection_manager/session.hpp"

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

void MessageReceiver::unsubscribe()
{
    stop_ = true;
}

void MessageReceiver::readHeader()
{
    auto processBody = [this](const std::error_code ec, auto) {
        if (ec)
        {
            std::println(stderr, "Header read failed: {}", ec.message());
            return;
        }

        uint64_t size;
        std::memcpy(&size, headerBuffer_.data(), sizeof(uint64_t));
        readBody(size);
    };

    boost::asio::async_read(session_->accessSslSocket(), boost::asio::buffer(headerBuffer_), processBody);
}

void MessageReceiver::readBody(uint64_t size)
{
    buffer_.clear();
    buffer_.resize(size);

    auto getMessage = [this, size](const std::error_code ec, auto) {
        if (ec)
        {
            std::println(stderr, "Message read failed: {}", ec.message());
            return;
        }

        auto anyPtr = std::make_unique<google::protobuf::Any>();

        if (const auto parseResult = anyPtr->ParseFromArray(buffer_.data(), static_cast<int>(size)); !parseResult)
        {
            std::println(stderr, "Failed to unpack the received message");
            return;
        }

        if (callback_) callback_(ec, std::move(anyPtr));

        if (!stop_) readHeader();
    };

    boost::asio::async_read(session_->accessSslSocket(), boost::asio::buffer(buffer_), getMessage);
}

}  // namespace p2pft::comms
