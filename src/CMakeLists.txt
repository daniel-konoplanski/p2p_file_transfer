include("${CMAKE_SOURCE_DIR}/cmake/build_proto.cmake")

add_subdirectory(lib.cli)
add_subdirectory(lib.comms)
add_subdirectory(lib.filesystem)
add_subdirectory(lib.certs)

add_executable(
        p2p_ft main.cpp
        p2pft/sender/sender.cpp
        p2pft/startup/startup.cpp
        p2pft/receiver/receiver.cpp
        p2pft/connection/connection.cpp
        p2pft/progress_bar/progress_bar.cpp)

target_link_libraries(p2p_ft
    PUBLIC p2pft_proto_lib
    PUBLIC cli
    PUBLIC comms
    PUBLIC filesystem
    PUBLIC Boost::asio
    PUBLIC Boost::system
    PUBLIC protobuf::libprotobuf
    PUBLIC OpenSSL::SSL
)

target_include_directories(p2p_ft PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
