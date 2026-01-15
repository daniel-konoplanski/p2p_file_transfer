add_library(message_receiver OBJECT message_receiver.cpp)

target_link_libraries(message_receiver
    PRIVATE connection_manager
    PRIVATE Boost::asio
    PRIVATE protobuf::libprotobuf)

target_include_directories(message_receiver PRIVATE ${CMAKE_SOURCE_DIR}/src)
