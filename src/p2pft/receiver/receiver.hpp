#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <google/protobuf/any.pb.h>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"

namespace p2pft
{

class Receiver : public IApplication
{
public:
    Receiver(cli::ReceiverArgs args);
    virtual ~Receiver() = default;

public:
    void run() override;

private:
    void handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr);
    void handleFileTransferProposalReq(std::unique_ptr<google::protobuf::Any> anyPtr);

private:
    cli::ReceiverArgs args_;
};

}  // namespace p2pft
