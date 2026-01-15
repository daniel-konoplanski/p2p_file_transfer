#pragma once

#include <indicators/progress_bar.hpp>

namespace p2pft
{

class ProgressBar
{
private:
    struct Progress
    {
        uint64_t total;
        uint64_t done;
    };

public:
    explicit ProgressBar(uint64_t total);
    void add(uint64_t value);

private:
    Progress progress_{};
    std::unique_ptr<indicators::ProgressBar> bar_{};
};

}  // namespace p2pft
