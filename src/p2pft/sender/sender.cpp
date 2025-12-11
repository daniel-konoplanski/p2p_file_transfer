#include "sender.hpp"

#include <cstdio>
#include <filesystem>
#include <memory>
#include <print>
#include <system_error>

#include <boost/asio/io_context.hpp>

#include "lib.cli/parser.hpp"

#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_sender.hpp"
#include "lib.comms/message_sender/message_sender.hpp"
#include "proto/FileTransferProposalReq.pb.h"

namespace p2pft
{

Sender::Sender(cli::SenderArgs args)
    : args_{ std::move(args) }
{
}

void Sender::run()
{
    auto io = std::make_shared<boost::asio::io_context>();

    auto connectionMgrPtr = std::make_unique<comms::ConnectionManager>(io, args_.port);
    auto maybeSession     = connectionMgrPtr->connect(args_.address);

    if (!maybeSession)
    {
        std::println(
            stderr,
            "Failed to connect to receiver with address {}: {}",
            args_.address,
            maybeSession.error().message());
        return; // TODO: return an error code;
    }

    auto sessionPtr     = *maybeSession;
    auto remoteEndpoint = sessionPtr->socketPtr_->remote_endpoint();

    std::println(
        "Successfully connected to receiver with address {}:{}",
        remoteEndpoint.address().to_string(),
        remoteEndpoint.port());

    std::filesystem::path filePath{ args_.path };

    std::error_code ec;
    auto doesFileExist = std::filesystem::exists(filePath, ec);

    if (ec)
    {
        std::println(stderr, "Failed to read file {}: {}", filePath.string(), ec.message());
        return; // TODO: return an error code;
    }

    if (!doesFileExist)
    {
        std::println(stderr, "File does not exist: {}", filePath.string());
        return;
    }

    auto fileSize = std::filesystem::file_size(filePath, ec);

    if (ec)
    {
        std::println(stderr, "Could not read file size: {}", filePath.string());
        return;
    }

    proto::FileInfo fileInfo;

    proto::FileTransferProposalReq req;
    p2pft::proto::FileInfo* f = req.mutable_files();

    f->set_name(filePath.filename().string());
    f->set_size(fileSize);

    std::unique_ptr<comms::IMessageSender> messageSeder = std::make_unique<comms::MessageSender>(sessionPtr);

    messageSeder->send(req, [](const auto& errorCode, auto msgSize) {
        if (errorCode)
        {
            std::println("Error during message sending: {}", errorCode.message());
            return;
        }

        std::println("Successfully send message of size {}", msgSize);
    });

    auto work_guard = boost::asio::make_work_guard(*io);

    io->run();
}

}  // namespace p2pft
