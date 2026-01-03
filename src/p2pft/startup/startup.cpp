#include "startup.hpp"

#include <memory>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/receiver/receiver.hpp"
#include "p2pft/sender/sender.hpp"

namespace p2pft::startup
{

std::unique_ptr<IApplication> AppVisitor::operator()(cli::SenderArgs args) const
{
    return std::make_unique<Sender>(args);
}

std::unique_ptr<IApplication> AppVisitor::operator()(cli::ReceiverArgs args) const
{
    return std::make_unique<Receiver>(args);
}

std::unique_ptr<IApplication> AppVisitor::operator()(std::nullopt_t) const
{
    return nullptr;
}

}  // namespace p2pft::startup
