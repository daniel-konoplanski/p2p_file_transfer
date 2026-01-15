#pragma once

#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <variant>

#include <CLI/CLI.hpp>

namespace cli
{

struct SenderArgs
{
    std::string address;
    std::string path;
    uint16_t port;
};

struct ReceiverArgs
{
    std::string outDir;
    uint16_t port;
};

using CliArgs = std::variant<SenderArgs, ReceiverArgs, std::nullopt_t>;

class Parser
{
public:
    [[nodiscard]] static std::expected<CliArgs, CLI::ParseError> parse(CLI::App& app, int argc, char* argv[]);
};

}  // namespace cli
