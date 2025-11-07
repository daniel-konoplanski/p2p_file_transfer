#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"

namespace p2pft
{

using IoContext      = boost::asio::io_context;
using IoContextPtr   = std::shared_ptr<IoContext>;
using TcpAcceptor    = boost::asio::ip::tcp::acceptor;
using TcpAcceptorPtr = std::shared_ptr<TcpAcceptor>;

class Receiver : public IApplication
{
public:
    Receiver(cli::ReceiverArgs args);
    void start() override;
    virtual ~Receiver() = default;

private:
    cli::ReceiverArgs args_;
    IoContextPtr      ioContext_;
    TcpAcceptorPtr    acceptor_;
};

}  // namespace p2pft
