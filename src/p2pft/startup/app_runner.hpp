#pragma once

#include <memory>

namespace p2pft::startup
{

class AppVisitor
{
public:
    std::unique_ptr<IApplication> operator()(cli::SenderArgs)
    {
        return nullptr;
    }

    std::unique_ptr<IApplication> operator()(cli::ReceiverArgs)
    {
        return nullptr;
    }

    std::unique_ptr<IApplication> operator()(std::nullopt_t)
    {
        return nullptr;
    }
};

}  // namespace p2pft::startup
