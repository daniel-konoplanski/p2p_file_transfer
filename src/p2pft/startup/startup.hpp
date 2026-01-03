#pragma once

#include <memory>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"

namespace p2pft::startup
{

class AppVisitor
{
public:
    std::unique_ptr<IApplication> operator()(cli::SenderArgs args) const;
    std::unique_ptr<IApplication> operator()(cli::ReceiverArgs args) const;
    std::unique_ptr<IApplication> operator()(std::nullopt_t) const;
};

}  // namespace p2pft::startup
