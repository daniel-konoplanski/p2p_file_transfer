add_library(connection_manager STATIC connection_manager.cpp)

target_link_libraries(connection_manager PRIVATE Boost::asio)
target_include_directories(connection_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)