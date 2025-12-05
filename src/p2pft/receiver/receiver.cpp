#include "receiver.hpp"

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>

#include <proto/FileTransferProposalReq.pb.h>

namespace p2pft
{

Receiver::Receiver(cli::ReceiverArgs args)
    : args_{ args }
{
}

void Receiver::run()
{
}

}  // namespace p2pft
