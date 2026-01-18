#include "receiver.hpp"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <format>
#include <memory>
#include <print>
#include <string>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <google/protobuf/any.pb.h>

#include <proto/FileChunk.pb.h>
#include <proto/FileTransferComplete.pb.h>
#include <proto/FileTransferProposalReq.pb.h>
#include <proto/FileTransferProposalResp.pb.h>
#include <proto/Result.pb.h>

#include "p2pft/connection/connection.hpp"

#include "lib.certs/certificate_manager.hpp"
#include "lib.comms/connection_manager/connection_manager.hpp"
#include "lib.comms/i_receiver.hpp"
#include "lib.comms/message_sender/message_sender.hpp"
#include "lib.filesystem/file_writer.hpp"
#include "lib.ui/console_user_interface.hpp"
#include "lib.utils/format.hpp"

namespace p2pft
{

namespace
{

}  // namespace

Receiver::Receiver(cli::ReceiverArgs args)
    : args_{ std::move(args) }
    , ui_{ std::make_unique<ui::ConsoleUserInterface>() }
{
}

void Receiver::run()
{
    if (!cert::CertificateManager::isCertCreated()) cert::CertificateManager::create();

    io_ = std::make_shared<boost::asio::io_context>();

    ui_->displayMessage(std::format("Listening on 0.0.0.0:{}...", args_.port));

    auto maybeSession = comms::ConnectionManager::listen(io_, args_.port);

    if (!maybeSession)
    {
        ui_->displayError("Failed to receive connection");
        return;
    }

    connection_ = std::make_unique<Connection>(*maybeSession);

    const auto& endpoint = connection_->accessSession().getRemoteEndpoint();

    ui_->displayMessage(std::format("Incoming connection from {}:{}", endpoint.address().to_string(), endpoint.port()));

    connection_->accessMsgReceiver().subscribe(
        [this](const std::error_code& ec, std::unique_ptr<google::protobuf::Any> anyPtr) {
            if (ec)
            {
                ui_->displayError(std::format("Message receiving failed: {}", ec.message()));
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
        ui_->displayError("Failed to unpack message to FileTransferProposalReq");
        return;
    }

    fileInfo_.name_ = req.files().name();
    fileInfo_.size_ = req.files().size();

    if (auto spaceInfo = std::filesystem::space(args_.outDir); spaceInfo.available < fileInfo_.size_)
    {
        auto msg = std::format(
            "Not enough available space in the provided location {}, needed: {}, available: {}",
            args_.outDir,
            fileInfo_.size_,
            spaceInfo.available);

        ui_->displayError(msg);
        return;
    }

    ui_->displayMessage(std::format("Proposal: {} — {}", fileInfo_.name_, utils::formatBytes(fileInfo_.size_)));

    sendFileTransferProposalResp();
}

void Receiver::handleFileChunk(std::unique_ptr<google::protobuf::Any> anyPtr)
{
    using enum proto::Result;

    proto::FileChunk msg;

    if (const auto unpackResult = anyPtr->UnpackTo(&msg); !unpackResult)
    {
        ui_->displayError("Failed to unpack message to FileTransferProposalReq");
        sendFileTransferComplete(REJECTED);
        return;
    }

    ui_->createProgressTracker(fileInfo_.size_);

    const bool isLast = msg.is_last();
    const auto& data  = msg.data();

    static auto fileWriter = std::make_unique<files::FileWriter>(args_.outDir, fileInfo_.name_);
    fileWriter->write(data, isLast);
    ui_->updateProgress(data.size());

    if (isLast)
    {
        fileWriter = nullptr;
        ui_->displayMessage(std::format("File saved: {}", args_.outDir + "/" + fileInfo_.name_));
        sendFileTransferComplete(ACCEPTED);
    }
}

void Receiver::sendFileTransferProposalResp()
{
    using enum proto::Result;
    proto::FileTransferProposalResp resp;

    ui_->displayMessage("Accept transfer? (y/N): ");
    auto reqResult = ui_->confirm() ? ACCEPTED : REJECTED;

    resp.set_result(reqResult);

    connection_->accessMsgSender().send(resp, [this, reqResult](const std::error_code& ec, size_t) {
        if (ec)
        {
            ui_->displayError(std::format("Sending FileTransferProposalResp failed: {}", ec.message()));
            return;
        }

        if (reqResult == REJECTED)
        {
            ui_->displayMessage("File transfer rejected");
            cleanup();
        }
    });
}

void Receiver::sendFileTransferComplete(const proto::Result result)
{
    proto::FileTransferComplete resp;
    resp.set_result(result);

    connection_->accessMsgSender().send(resp, [this](const std::error_code& ec, size_t) {
        ui_->displayError(std::format("File transfer completed", ec.message()));
        cleanup();
    });
}

void Receiver::cleanup()
{
    connection_.reset();
    io_->stop();
}

}  // namespace p2pft
