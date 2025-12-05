#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace cli
{

struct SenderArgs
{
    std::string address;
    std::string path;
    uint16_t    port;
};

struct ReceiverArgs
{
    std::string outDir;
    uint16_t    port;
};

using CliArgs = std::variant<SenderArgs, ReceiverArgs, std::nullopt_t>;

class Parser
{
public:
    [[nodiscard]] static CliArgs parse(int argc, char* argv[]);
};

}  // namespace cli
