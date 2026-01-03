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
    explicit Connection(std::shared_ptr<p2pft::comms::Session> session);
    ~Connection();

public:
    comms::IMessageSender&   accessMsgSender() const;
    comms::IMessageReceiver& accessMsgReceiver() const;
    p2pft::comms::Session&   accessSession() const;

private:
    std::shared_ptr<p2pft::comms::Session>   session_{};
    std::unique_ptr<comms::IMessageSender>   messageSender_{};
    std::unique_ptr<comms::IMessageReceiver> messageReceiver_{};
};

}  // namespace p2pft
