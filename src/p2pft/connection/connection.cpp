#include "connection.hpp"

#include <memory>

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

    auto& socket = session_->socketPtr_;

    if (socket->is_open()) socket->close();

    session_.reset();
}

comms::IMessageSender& Connection::accessMsgSender()
{
    return *messageSender_;
}

comms::IMessageReceiver& Connection::accessMsgReceiver()
{
    return *messageReceiver_;
}

p2pft::comms::Session& Connection::accessSession()
{
    return *session_;
}

}  // namespace p2pft
