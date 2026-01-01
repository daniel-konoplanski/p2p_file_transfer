#include "file_writer.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <print>

namespace p2pft::files
{

FileWriter::FileWriter(const std::string& path, const std::string& fileName)
{
    buffer_.reserve(BUFFER_SIZE);

    file_      = File{};
    file_.name = fileName;
    file_.path = path + "/" + fileName;
    file_.stream.open(file_.path, std::ios::binary | std::ios::out);
}

void FileWriter::write(const std::string& bytes, bool isLast)
{
    if (buffer_.size() + bytes.size() > BUFFER_SIZE)
    {
        std::println(stderr, "This shouldn't happend, it should be devisible");
        return;
    }

    if (buffer_.size() == BUFFER_SIZE)
    {
        file_.stream.write(bytes.c_str(), bytes.size());
        buffer_.clear();
    }

    std::transform(bytes.begin(), bytes.end(), std::back_inserter(buffer_), [](const char b) {
        return static_cast<std::byte>(b);
    });

    if (isLast)
    {
        file_.stream.write(bytes.c_str(), bytes.size());
        file_.stream.close();

        std::println("File write successfull!");
    }
}

}  // namespace p2pft::files
