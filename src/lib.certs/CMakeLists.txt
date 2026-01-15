add_library(cert STATIC certificate_manager.cpp)

target_link_libraries(cert
    PUBLIC Botan::Botan-static
)
