#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

namespace p2pft::files
{

class FileWriter
{
private:
    struct File
    {
        std::string   name;
        std::string   path;
        std::ofstream stream;
    };

public:
    FileWriter(const std::string& path, const std::string& fileName);

public:
    void write(const std::string& bytes, bool isLast = false);

private:
    constexpr static uint64_t BUFFER_SIZE{ 4'000'000U };
    std::vector<std::byte>    buffer_;
    File                      file_;
};

}  // namespace p2pft::files
