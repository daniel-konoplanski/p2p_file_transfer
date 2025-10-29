#include <CLI/CLI.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>

enum class Mode : u_int8_t
{
    SENDER,
    RECEIVER
};

int main(int argc, char* argv[])
{
    CLI::App app{ "P2P File Transfer" };

    std::string dest;
    std::string path;
    int         port = 8888;

    auto send = app.add_subcommand("send", "Send files to a receiver");
    send->add_option("-t,--target", dest, "Address of the receiver")->required();
    send->add_option("-i,--input", path, "File to transfer")->required();
    send->add_option("-p,--port", port, "Port number");

    auto receive = app.add_subcommand("receive", "Receive files from a sender");
    receive->add_option("-o,--out-dir", port, "Port number")->required();
    receive->add_option("-p,--port", port, "Port number");

    CLI11_PARSE(app, argc, argv);

    std::cout << "dest: " << dest << "\n"
              << "path: " << path << "\n"
              << "port: " << port << "\n";

    return 0;
}
