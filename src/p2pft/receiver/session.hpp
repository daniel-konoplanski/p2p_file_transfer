#pragma once

#include <array>
#include <cstdint>
#include <memory>

#include <boost/asio/ip/tcp.hpp>

#include <FileTransferProposalReq.pb.h>

namespace p2pft
{

class Session;

using TcpSocket  = boost::asio::ip::tcp::socket;
using SessionPtr = std::unique_ptr<Session>;
using SocketPtr  = std::unique_ptr<TcpSocket>;
using Byte       = uint8_t;

struct Context
{
    bool isTransferAccepted{};
};

class Session
{
public:
    Session(SocketPtr socket);
    void start();

private:
    void readData(const boost::system::error_code ec,  const std::uint64_t length);
    void handleFileTransferProposalReq(const proto::FileTransferProposalReq& req);

private:
    Context context_;
    SocketPtr socket_;
    std::array<Byte, 2048> buffer_; // TODO: consider the buffer size here
};

}  // namespace p2pft
