#include <memory>
#include <print>

#include "lib.cli/parser.hpp"

#include "p2pft/application.hpp"
#include "p2pft/startup/startup.hpp"

enum ExitCodes : int
{
    SUCCESS,
    INVALID_ARGUMENTS,
    STARTUP_FAILURE
};

int main(int argc, char* argv[])
{
    CLI::App cliApp{ "P2P File Transfer" };

    auto maybeArgs = cli::Parser::parse(cliApp, argc, argv);

    if (!maybeArgs)
    {
        return cliApp.exit(maybeArgs.error());
    }

    auto args = maybeArgs.value();

    if (std::holds_alternative<std::nullopt_t>(args))
    {
        std::println(stderr, "Failed to parse the arguments");
        return ExitCodes::INVALID_ARGUMENTS;
    }

    auto app = std::visit(p2pft::startup::AppVisitor{}, args);

    if (!app)
    {
        std::println("Failed to start the application");
        return ExitCodes::STARTUP_FAILURE;
    }

    app->run();

    return ExitCodes::SUCCESS;
}
