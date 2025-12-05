include("${CMAKE_SOURCE_DIR}/cmake/build_proto.cmake")

add_subdirectory(lib.cli)
add_subdirectory(lib.comms)

# add_library(p2p_ft_core OBJECT p2pft/sender/sender.cpp)
# add_executable(p2p_ft main.cpp $<TARGET_OBJECTS:p2p_ft_core>)

add_executable(p2p_ft main.cpp p2pft/sender/sender.cpp p2pft/startup/startup.cpp p2pft/receiver/receiver.cpp)

target_link_libraries(p2p_ft
    PUBLIC p2pft_proto_lib
    PUBLIC cli
    PUBLIC comms
    PUBLIC Boost::asio
    PUBLIC Boost::system
    PUBLIC protobuf::libprotobuf
)

target_include_directories(p2p_ft PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)