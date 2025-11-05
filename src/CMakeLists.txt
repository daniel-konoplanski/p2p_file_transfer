add_executable(p2p_ft main.cpp)

# target_link_libraries(p2p_ft
#     PRIVATE
#         CLI11::CLI11
#         indicators::indicators
#         Boost::asio
#         Boost::system
#         OpenSSL::SSL
#         OpenSSL::Crypto
#         spdlog::spdlog
# )

add_subdirectory(lib.cli)

target_link_libraries(p2p_ft
    PRIVATE cli
    PRIVATE spdlog::spdlog
)

