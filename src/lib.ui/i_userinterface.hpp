#pragma once

#include <cstdint>
#include <string_view>

namespace p2pft::ui
{

class IUserInterface
{
public:
    virtual ~IUserInterface()                         = default;
    virtual void displayMessage(std::string_view msg) = 0;
    virtual void displayError(std::string_view msg)   = 0;
    virtual bool confirm()                            = 0;
    virtual void createProgressTracker(uint64_t size) = 0;
    virtual void updateProgress(uint64_t size)        = 0;
};

}  // namespace p2pft::ui
