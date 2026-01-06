#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/connection/connection.hpp"
#include "p2pft/progress_bar/progress_bar.hpp"

namespace p2pft
{

struct FileInfo
{
    uint64_t    fileSize;
    std::string fileName;
};

class Sender : public IApplication
{
public:
    explicit Sender(cli::SenderArgs args);
    ~Sender() override = default;

public:
    void run() override;

private:
    void handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr);
    void handleFileTransferProposalResp(std::unique_ptr<google::protobuf::Any> anyPtr);
    void handleFileTransferComplete(std::unique_ptr<google::protobuf::Any> anyPtr);
    void startFileTransfer();
    void cleanup();
    void sendChunk(std::shared_ptr<std::ifstream> file, uint64_t totalChunks, uint64_t chunkId);
    std::error_code establishConnection();
    void            setupMessageReceiver();
    std::error_code validateFile();
    std::error_code sendFileProposal();

private:
    constexpr static uint64_t                CHUNK_SIZE{ 8192U };
    cli::SenderArgs                          args_;
    std::shared_ptr<boost::asio::io_context> io_;
    std::unique_ptr<Connection>              connection_;
    FileInfo                                 fileInfo_;
    std::unique_ptr<ProgressBar>             progressBar_;
};

}  // namespace p2pft
