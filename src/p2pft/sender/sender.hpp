#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/connection/connection.hpp"

#include "lib.comms/i_receiver.hpp"
#include "lib.comms/i_sender.hpp"

namespace p2pft
{

class Sender : public IApplication
{
public:
    Sender(cli::SenderArgs args);
    virtual ~Sender() = default;

public:
    void run() override;

private:
    void handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr);
    void handleFileTransferProposalResp(std::unique_ptr<google::protobuf::Any> anyPtr);
    void handleFileTransferComplete(std::unique_ptr<google::protobuf::Any> anyPtr);
    void startFileTransfer();
    void cleanup();

private:
    cli::SenderArgs                          args_;
    std::shared_ptr<boost::asio::io_context> io_;
    std::unique_ptr<Connection>              connection_;
};

}  // namespace p2pft
