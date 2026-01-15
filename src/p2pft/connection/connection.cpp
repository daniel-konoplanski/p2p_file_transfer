#include "connection.hpp"

#include <memory>
#include <print>

#include "lib.comms/message_receiver/message_receiver.hpp"
#include "lib.comms/message_sender/message_sender.hpp"

namespace p2pft
{

Connection::Connection(std::shared_ptr<p2pft::comms::Session> session)
    : session_(session)
{
    messageReceiver_ = std::make_unique<comms::MessageReceiver>(session);
    messageSender_   = std::make_unique<comms::MessageSender>(session);
}

Connection::~Connection()
{
    messageReceiver_.reset();
    messageSender_.reset();

    auto& sslStream = session_->accessSslSocket();

    boost::system::error_code ec;
    sslStream.shutdown(ec);

    if (ec)
    {
        std::println(stderr, "Failed to shutdown SSL stream: {}", ec.message());
        return;
    }

    if (auto& socket = sslStream.lowest_layer(); !socket.is_open())
    {
        socket.close();
    }

    session_.reset();
}

comms::IMessageSender& Connection::accessMsgSender() const
{
    return *messageSender_;
}

comms::IMessageReceiver& Connection::accessMsgReceiver() const
{
    return *messageReceiver_;
}

p2pft::comms::Session& Connection::accessSession() const
{
    return *session_;
}

}  // namespace p2pft
