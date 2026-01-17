#include "progress_bar.hpp"

#include <print>

namespace p2pft
{

ProgressBar::ProgressBar(const uint64_t total)
{
    using namespace indicators::option;

    bar_ = std::make_unique<indicators::ProgressBar>(
        BarWidth{ 40 },
        Start{ "[" },
        End{ "]" },
        Fill{ "#" },
        Lead{ "#" },
        ShowPercentage{ true },
        ShowElapsedTime{ true },
        ShowRemainingTime{ true });

    progress_.total = total;
}

void ProgressBar::add(const uint64_t current)
{
    progress_.done = current;
    bar_->set_progress(static_cast<double>(progress_.done) / progress_.total * 100.0);

    if (progress_.done >= progress_.total && !bar_->is_completed())
    {
        bar_->mark_as_completed();
    }
}

uint64_t ProgressBar::getCurrent()
{
    return progress_.done;
}

}  // namespace p2pft
