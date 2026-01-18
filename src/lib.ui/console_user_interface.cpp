#include "console_user_interface.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <print>
#include <ranges>

namespace p2pft::ui
{

void ConsoleUserInterface::displayMessage(std::string_view msg)
{
    std::println("{}", msg);
}

void ConsoleUserInterface::displayError(std::string_view msg)
{
    std::println(stderr, "{}", msg);
}

bool ConsoleUserInterface::confirm()
{
    std::string response;
    std::getline(std::cin, response);
    std::ranges::transform(response, response.begin(), tolower);

    return response == "yes" || response == "y";
}

void ConsoleUserInterface::createProgressTracker(uint64_t size)
{
    components_.progressBar = std::make_unique<ProgressBar>(size);
}

void ConsoleUserInterface::updateProgress(uint64_t size)
{
    components_.progressBar->add(size);
}

}  // namespace p2pft::ui
