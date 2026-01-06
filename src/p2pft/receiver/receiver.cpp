#include "receiver.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <print>
#include <string>
#include <system_error>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <google/protobuf/any.pb.h>

#include <proto/FileTransferProposalReq.pb.h>
#include <proto/FileTransferProposalResp.pb.h>

#include "p2pft/connection/connection.hpp"

#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/message_receiver/message_receiver.hpp"
#include "lib.comms/message_sender/message_sender.hpp"
#include "lib.filesystem/file_writer.hpp"
#include "proto/FileChunk.pb.h"
#include "proto/FileTransferComplete.pb.h"
#include "proto/Result.pb.h"

namespace p2pft
{

namespace
{

bool getUserConfirmation()
{
    std::string response;
    std::getline(std::cin, response);

    std::ranges::transform(response, response.begin(), ::tolower);

    return response == "yes" || response == "y" || response == "Y";
}

std::string formatBytes(const size_t bytes)
{
    const char*   units[] = { "B", "KB", "MB", "GB", "TB", "PB" };
    constexpr int base    = 1024;

    if (bytes == 0) return "0 B";

    const int unitIndex = std::min(
        static_cast<int>(std::log(bytes) / std::log(base)),
        5  // Max index for units array
    );

    const double value = bytes / std::pow(base, unitIndex);

    std::ostringstream oss;

    if (value >= 100)
    {
        oss << std::fixed << std::setprecision(0);
    }
    else if (value >= 10)
    {
        oss << std::fixed << std::setprecision(1);
    }
    else
    {
        oss << std::fixed << std::setprecision(2);
    }

    oss << value << " " << units[unitIndex];
    return oss.str();
}

}  // namespace

Receiver::Receiver(cli::ReceiverArgs args)
    : args_{ std::move(args) }
{
}

void Receiver::run()
{
    io_ = std::make_shared<boost::asio::io_context>();

    std::println("Listening on 0.0.0.0:{}...", args_.port);

    auto maybeSession = comms::ConnectionManager::listen(io_, args_.port);

    if (!maybeSession)
    {
        std::println(stderr, "Failed to receive connection");
        return;
    }

    connection_ = std::make_unique<Connection>(*maybeSession);

    const auto& remoteEndpoint = connection_->accessSession().socketPtr_->remote_endpoint();

    std::println(
        "Incoming connection from {}:{}",
        remoteEndpoint.address().to_string(),
        remoteEndpoint.port());

    connection_->accessMsgReceiver().subscribe(
        [this](const std::error_code& ec, std::unique_ptr<google::protobuf::Any> anyPtr) {
            if (ec)
            {
                std::println("Message receiving failed: {}", ec.message());
                return;
            }

            handleMessage(std::move(anyPtr));
        });

    auto work_guard = boost::asio::make_work_guard(*io_);

    io_->run();
}

void Receiver::handleMessage(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    if (anyPtr->Is<proto::FileTransferProposalReq>())
    {
        handleFileTransferProposalReq(std::move(anyPtr));
    }
    else if (anyPtr->Is<proto::FileChunk>())
    {
        handleFileChunk(std::move(anyPtr));
    }
}

void Receiver::handleFileTransferProposalReq(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    proto::FileTransferProposalReq req;

    if (const auto unpackResult = anyPtr->UnpackTo(&req); !unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferProposalReq");
        return;
    }

    fileInfo_.name_ = req.files().name();
    fileInfo_.size_ = req.files().size();

    if (auto spaceInfo = std::filesystem::space(args_.outDir); spaceInfo.available < fileInfo_.size_)
    {
        std::println(
            stderr,
            "Not enough available space in the provided location {}, needed: {}, available: {}",
            args_.outDir,
            fileInfo_.size_,
            spaceInfo.available);
        return;
    }

    std::println("Proposal: {} — {}", formatBytes(fileInfo_.size_), fileInfo_.name_);

    sendFileTransferProposalResp();
}

void Receiver::handleFileChunk(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    using enum proto::Result;

    proto::FileChunk msg;

    if (const auto unpackResult = anyPtr->UnpackTo(&msg); !unpackResult)
    {
        std::println(stderr, "Failed to unpack message to FileTransferProposalReq");
        sendFileTransferComplete(REJECTED);
        return;
    }

    if (!progressBar_)
        progressBar_ = std::make_unique<ProgressBar>(fileInfo_.size_);


    const bool  isLast = msg.is_last();
    const auto& data   = msg.data();

    static auto fileWriter = std::make_unique<files::FileWriter>(args_.outDir, fileInfo_.name_);
    fileWriter->write(data, isLast);
    progressBar_->add(data.size());

    if (isLast)
    {
        fileWriter = nullptr;
        std::println("File saved: {}", args_.outDir + "/" + fileInfo_.name_);
        sendFileTransferComplete(ACCEPTED);
    }
}

void Receiver::sendFileTransferProposalResp()
{
    using enum proto::Result;
    proto::FileTransferProposalResp resp;

    std::print("Accept transfer? (y/N): ");
    const proto::Result reqResult = getUserConfirmation() ? ACCEPTED : REJECTED;

    resp.set_result(reqResult);

    connection_->accessMsgSender().send(resp, [this, reqResult](const std::error_code& ec, size_t) {
        if (ec)
        {
            std::println("Sending FileTransferProposalResp failed: {}", ec.message());
            return;
        }

        if (reqResult == REJECTED)
        {
            std::println("File transfer rejected");
            cleanup();
        }
    });
}

void Receiver::sendFileTransferComplete(const proto::Result result)
{
    proto::FileTransferComplete resp;
    resp.set_result(result);

    connection_->accessMsgSender().send(resp, [this](const std::error_code& ec, size_t) {
        std::println("File transfer completed", ec.message());
        cleanup();
    });
}

void Receiver::cleanup()
{
    connection_.reset();
    io_->stop();
}

}  // namespace p2pft
