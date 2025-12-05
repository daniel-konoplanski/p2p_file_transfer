#pragma once

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"

namespace p2pft
{

class Sender : public IApplication
{
public:
    Sender(cli::SenderArgs args);
    virtual ~Sender() = default;

public:
    void run() override;

private:
    cli::SenderArgs args_;
};

}  // namespace p2pft
