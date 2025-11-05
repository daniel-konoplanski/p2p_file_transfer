add_library(cli STATIC parser.cpp)

target_link_libraries(cli
    PRIVATE CLI11::CLI11
)
