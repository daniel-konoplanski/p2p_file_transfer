add_library(message_sender OBJECT message_sender.cpp)

target_link_libraries(message_sender
    PRIVATE Boost::asio
    PRIVATE protobuf::libprotobuf)

target_include_directories(message_sender PRIVATE ${CMAKE_SOURCE_DIR}/src)
