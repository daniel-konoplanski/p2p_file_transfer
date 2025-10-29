add_executable(p2p_ft main.cpp)

target_link_libraries(p2p_ft 
    PRIVATE 
        Boost::asio
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        spdlog::spdlog
)
