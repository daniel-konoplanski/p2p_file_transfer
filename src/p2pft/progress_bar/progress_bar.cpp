#include <print>

#include "progress_bar.hpp"

namespace p2pft
{

ProgressBar::ProgressBar(const uint64_t total)
{
    using namespace indicators::option;

    bar_ = std::make_unique<indicators::ProgressBar>(
        BarWidth{30},
        Start{"["},
        End{"]"},
        Fill{"█"},
        Lead{"█"},
        ShowPercentage{true},
        ShowElapsedTime{true},
        ShowRemainingTime{true}
    );

    progress_.total = total;
}

void ProgressBar::add(const uint64_t value)
{
    progress_.done += value;
    bar_->set_progress(static_cast<double>(progress_.done) / progress_.total * 100.0);
}

}  // namespace p2pft
