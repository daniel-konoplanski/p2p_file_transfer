include("${CMAKE_SOURCE_DIR}/cmake/build_proto.cmake")

add_subdirectory(lib.cli)
add_subdirectory(lib.comms)

add_executable(p2p_ft main.cpp p2pft/receiver/receiver.cpp)

target_link_libraries(p2p_ft
    PRIVATE p2pft_proto_lib
    PRIVATE cli
    PRIVATE Boost::asio
    PRIVATE Boost::system
    PRIVATE protobuf::libprotobuf
)

target_include_directories(p2p_ft PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)