#include "parser.hpp"

#include <optional>

#include <CLI/CLI.hpp>

namespace cli
{

CliArgs Parser::parse(const int argc, char* argv[])
{
    CLI::App app{ "P2P File Transfer" };

    std::string address;
    std::string path;
    uint16_t    port = 52000;

    const auto send = app.add_subcommand("send", "Send files to a receiver");
    send->add_option("-t,--target", address, "Address of the receiver")->required();
    send->add_option("-i,--input", path, "File to transfer")->required();
    send->add_option("-p,--port", port, "Port number");

    const auto receive = app.add_subcommand("receive", "Receive files from a sender");
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
    cliArgs = send->parsed() ? cliArgs = SenderArgs{ address, path, port }
                             : cliArgs = ReceiverArgs{ path, port };

    return cliArgs;
}

}  // namespace cli
