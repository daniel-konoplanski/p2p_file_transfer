#include <memory>

#include <spdlog/spdlog.h>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/startup/app_runner.hpp"

enum ExitCodes : int
{
    SUCCESS,
    STARTUP_FAILURE
};

int main(int argc, char* argv[])
{
    auto args = cli::Parser::parse(argc, argv);

    std::unique_ptr<p2pft::IApplication> app = std::visit(p2pft::startup::AppVisitor{}, args);

    if (!app)
    {
        spdlog::error("Failed to start the application!");
        return ExitCodes::STARTUP_FAILURE;
    }

    app->start();

    return ExitCodes::SUCCESS;
}
