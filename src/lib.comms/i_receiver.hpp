#pragma once

#include <functional>
#include <memory>
#include <system_error>

#include <google/protobuf/any.pb.h>
#include <google/protobuf/message.h>

namespace p2pft::comms
{

using ReceiverCallback = std::function<void(const std::error_code&, std::unique_ptr<google::protobuf::Any>)>;

class IMessageReceiver
{
public:
    virtual ~IMessageReceiver() = default;

public:
    virtual void subscribe(ReceiverCallback callback) = 0;
    virtual void unsubscribe()                        = 0;
};

}  // namespace p2pft::comms
