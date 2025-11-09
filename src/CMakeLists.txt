include("${CMAKE_SOURCE_DIR}/cmake/build_proto.cmake")

add_executable(p2p_ft main.cpp p2pft/receiver/receiver.cpp)

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
    PRIVATE Boost::asio
    PRIVATE Boost::system
    PRIVATE gRPC::grpc++
    PRIVATE protobuf::libprotobuf
)

target_include_directories(p2p_ft PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)