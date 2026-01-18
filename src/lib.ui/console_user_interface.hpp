#pragma once

#include <cstdint>
#include <memory>

#include "i_userinterface.hpp"
#include "progress_bar/progress_bar.hpp"

namespace p2pft::ui
{

class ConsoleUserInterface : public IUserInterface
{
private:
    struct Components
    {
        std::unique_ptr<ProgressBar> progressBar;
    };

public:
    ~ConsoleUserInterface() override = default;
    void displayMessage(std::string_view msg) override;
    void displayError(std::string_view msg) override;
    bool confirm() override;
    void createProgressTracker(uint64_t size) override;
    void updateProgress(uint64_t size) override;

private:
    Components components_;
};

}  // namespace p2pft::ui
