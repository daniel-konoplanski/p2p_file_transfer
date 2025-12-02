cmake_minimum_required(VERSION 3.31)

project(p2p_file_transfer
        VERSION 0.0.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add extra warning flags to all targets
set(WARNING_FLAGS -Wall -Wextra -Wpedantic -Werror)
add_compile_options(${WARNING_FLAGS})

# Find all required libraries
find_package(CLI11 REQUIRED)
find_package(indicators REQUIRED)
find_package(Boost REQUIRED COMPONENTS asio system)
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(Protobuf REQUIRED)

add_subdirectory(src)
