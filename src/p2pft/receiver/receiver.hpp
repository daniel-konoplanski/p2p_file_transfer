#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"

namespace p2pft
{

class Receiver : public IApplication
{
public:
    Receiver(cli::ReceiverArgs args);
    virtual ~Receiver() = default;

public:
    void run() override;

private:
    cli::ReceiverArgs args_;
};

}  // namespace p2pft
