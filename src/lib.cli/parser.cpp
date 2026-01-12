#include "parser.hpp"
#include <expected>

#include <CLI/CLI.hpp>

namespace cli
{

std::expected<CliArgs, CLI::ParseError> Parser::parse(CLI::App& app, const int argc, char* argv[])
{
    std::string address;
    std::string path;
    uint16_t port;

    const auto send = app.add_subcommand("send", "Send files to a receiver");
    send->add_option("-a,--address", address, "Address of the receiver")->required();
    send->add_option("-f,--file", path, "File to transfer")->required();
    send->add_option("-p,--port", port, "Port number")->required();

    const auto receive = app.add_subcommand("receive", "Receive files from a sender");
    receive->add_option("-o,--out-dir", path, "Port number")->required();
    receive->add_option("-p,--port", port, "Port number");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        return std::unexpected<CLI::ParseError>(e);
    }

    CliArgs cliArgs{};
    cliArgs = send->parsed() ? cliArgs = SenderArgs{ address, path, port }
                             : cliArgs = ReceiverArgs{ path, port };

    return cliArgs;
}

}  // namespace cli
