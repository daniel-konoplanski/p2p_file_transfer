#pragma once

#include <memory>

#include "lib.comms/i_receiver.hpp"
#include "lib.comms/i_sender.hpp"
#include "lib.comms/session.hpp"

namespace p2pft
{

class Connection
{
public:
    Connection(std::shared_ptr<p2pft::comms::Session> session);
    ~Connection();

public:
    comms::IMessageSender&   accessMsgSender();
    comms::IMessageReceiver& accessMsgReceiver();
    p2pft::comms::Session&   accessSession();

private:
    std::shared_ptr<p2pft::comms::Session>   session_{};
    std::unique_ptr<comms::IMessageSender>   messageSender_{};
    std::unique_ptr<comms::IMessageReceiver> messageReceiver_{};
};

}  // namespace p2pft
