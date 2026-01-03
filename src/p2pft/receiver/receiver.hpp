#pragma once

#include <string>

#include <boost/asio/io_context.hpp>

#include <google/protobuf/any.pb.h>

#include <proto/Result.pb.h>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/connection/connection.hpp"

namespace p2pft
{

class Receiver : public IApplication
{
public:
    explicit Receiver(cli::ReceiverArgs args);
    ~Receiver() override = default;

public:
    void run() override;

private:
    void handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr);
    void sendFileTransferProposalResp();
    void handleFileTransferProposalReq(std::unique_ptr<google::protobuf::Any> anyPtr);
    void handleFileChunk(std::unique_ptr<google::protobuf::Any> anyPtr);
    void sendFileTransferComplete(proto::Result result);
    void cleanup();

private:
    cli::ReceiverArgs                        args_;
    std::string                              fileName_;
    std::shared_ptr<boost::asio::io_context> io_;
    std::unique_ptr<Connection>              connection_;
};

}  // namespace p2pft
