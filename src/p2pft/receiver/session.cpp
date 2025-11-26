#include "session.hpp"

#include <iostream>
#include <print>
#include <string>
#include <vector>

#include <boost/system/detail/error_code.hpp>

#include <google/protobuf/any.pb.h>

#include <FileTransferProposalReq.pb.h>

#include "p2pft/types/FileInfo.hpp"

namespace p2pft
{

Session::Session(SocketPtr socket)
    : socket_{ std::move(socket) }
{
}

void Session::start()
{
    socket_->async_read_some(
        buffer_,
        [this](const boost::system::error_code& ec, std::uint64_t length) {
            readData(ec, length);
        });
}

void Session::readData(const boost::system::error_code ec, const std::uint64_t length)
{
    if (ec)
    {
        std::print("Failed to read data from the sender \"{}\"", ec.message());
        return;
    }

    google::protobuf::Any anyMsg;

    if (!anyMsg.ParseFromArray(buffer_.data(), length))
    {
        std::print("Failed to parse protobuf message");
        return;
    }

    if (!context_.isTransferAccepted && anyMsg.Is<proto::FileTransferProposalReq>())
    {
        proto::FileTransferProposalReq req;
        anyMsg.UnpackTo(&req);
        handleFileTransferProposalReq(req);
    }
}

void Session::handleFileTransferProposalReq(const proto::FileTransferProposalReq& req)
{
    std::vector<types::FileInfo> fileInfos{};
    uint32_t                     fileCounter{};

    for (const auto& file: req.files())
    {
        ++fileCounter;
        fileInfos.push_back(types::FileInfo{ file });
    }

    std::print("File count: {}", fileCounter);

    for (const auto& file: fileInfos)
    {
        std::cout << file;
    }

    // Get input to use if should accept

    std::print("Do you want to accept the transfer? (yes/no)");
    std::string choice = "no";

    if (std::getline(std::cin, choice))
    {
        std::print("Do you want to accept the transfer? (yes/no)");
    }

    if (choice == "yes" || choice == "y")
    {
    }
    else
    {
        std::print("File transfer denied");
    }
}

}  // namespace p2pft
