#include "receiver.hpp"

#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <proto/FileTransferProposalReq.pb.h>

namespace p2pft
{

Receiver::Receiver(cli::ReceiverArgs args)
    : args_{ args }
    , ioContext_{ std::make_shared<boost::asio::io_context>() }
{
}

void Receiver::start()
{
}

}  // namespace p2pft
