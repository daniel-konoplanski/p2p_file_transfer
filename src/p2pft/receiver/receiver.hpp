#pragma once

#include <memory>
#include <string>

#include <boost/asio/io_context.hpp>

#include <google/protobuf/any.pb.h>

#include <proto/Result.pb.h>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/connection/connection.hpp"
#include "lib.ui/i_userinterface.hpp"

namespace p2pft
{

class Receiver : public IApplication
{
private:
    struct FileInfo
    {
        std::string name_;
        uint64_t size_;
    };

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
    cli::ReceiverArgs args_;
    FileInfo fileInfo_;
    std::shared_ptr<boost::asio::io_context> io_;
    std::unique_ptr<Connection> connection_;
    std::unique_ptr<ui::IUserInterface> ui_;
};

}  // namespace p2pft
