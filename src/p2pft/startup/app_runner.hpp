#pragma once

#include <memory>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/receiver/receiver.hpp"
#include "p2pft/sender/sender.hpp"

namespace p2pft::startup
{

class AppVisitor
{
public:
    std::unique_ptr<IApplication> operator()(cli::SenderArgs args)
    {
        return std::make_unique<Sender>(args);
    }

    std::unique_ptr<IApplication> operator()([[maybe_unused]] cli::ReceiverArgs args)
    {
        return nullptr;
    }

    std::unique_ptr<IApplication> operator()(std::nullopt_t)
    {
        return nullptr;
    }
};

}  // namespace p2pft::startup
