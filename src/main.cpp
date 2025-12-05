#include <memory>
#include <print>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/startup/startup.hpp"

enum ExitCodes : int
{
    SUCCESS,
    STARTUP_FAILURE
};

int main(int argc, char* argv[])
{
    auto args = cli::Parser::parse(argc, argv);
    auto app  = std::visit(p2pft::startup::AppVisitor{}, args);

    if (!app)
    {
        std::print("Failed to start the application!");
        return ExitCodes::STARTUP_FAILURE;
    }

    app->run();

    return ExitCodes::SUCCESS;
    // probably do return app->run()
}
