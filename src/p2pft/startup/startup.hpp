#pragma once

#include <memory>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"

namespace p2pft::startup
{

class AppVisitor
{
public:
    std::unique_ptr<IApplication> operator()(cli::SenderArgs args);
    std::unique_ptr<IApplication> operator()(cli::ReceiverArgs args);
    std::unique_ptr<IApplication> operator()(std::nullopt_t);
};

}  // namespace p2pft::startup
