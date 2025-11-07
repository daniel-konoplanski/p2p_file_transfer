#pragma once

#include <memory>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/receiver/receiver.hpp"

namespace p2pft::startup
{

class AppVisitor
{
public:
    std::unique_ptr<IApplication> operator()(cli::SenderArgs)
    {
        return nullptr;
    }

    std::unique_ptr<IApplication> operator()(cli::ReceiverArgs args)
    {
        return std::make_unique<Receiver>(args);
    }

    std::unique_ptr<IApplication> operator()(std::nullopt_t)
    {
        return nullptr;
    }
};

}  // namespace p2pft::startup
