#include "parser.hpp"

#include <cstdlib>
#include <optional>

#include <CLI/CLI.hpp>

namespace cli
{

CliArgs Parser::parse(int argc, char* argv[])
{
    CLI::App app{ "P2P File Transfer" };

    std::string dest;
    std::string path;
    uint16_t port = 52000;

    auto send = app.add_subcommand("send", "Send files to a receiver");
    send->add_option("-t,--target", dest, "Address of the receiver")->required();
    send->add_option("-i,--input", path, "File to transfer")->required();
    send->add_option("-p,--port", port, "Port number");

    auto receive = app.add_subcommand("receive", "Receive files from a sender");
    receive->add_option("-o,--out-dir", path, "Port number")->required();
    receive->add_option("-p,--port", port, "Port number");

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        return std::nullopt;
    }

    CliArgs cliArgs{};
    cliArgs = send->parsed() ? cliArgs = SenderArgs{ dest, path, port } : cliArgs = ReceiverArgs{ path, port };

    return cliArgs;
}

}  // namespace cli
