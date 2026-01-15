#include "connection_manager.hpp"

#include <expected>
#include <memory>
#include <print>
#include <string_view>
#include <utility>

#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/system/detail/error_code.hpp>

#include "lib.certs/certificate_manager.hpp"
#include "session.hpp"

namespace p2pft::comms
{

namespace
{

constexpr std::string_view CIPHER_LIST =
    "ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-GCM-SHA256:"
    "ECDHE-RSA-AES128-GCM-SHA256";
}

using boost::asio::ip::tcp;

SessionOrError ConnectionManager::listen(const IoContextPtr& io, const Port port)
{
    const auto endpoint = tcp::endpoint(tcp::v4(), port);
    auto acceptor       = tcp::acceptor(*io, endpoint);

    boost::system::error_code ec{};
    auto socket = acceptor.accept(ec);

    if (ec)
    {
        return std::unexpected(ec);
    }

    acceptor.close();

    auto sslContextPtr =
        std::make_unique<SslContext>(boost::asio::ssl::context(boost::asio::ssl::context::tlsv12_server));

    sslContextPtr->set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::no_sslv3 |
        boost::asio::ssl::context::no_tlsv1 |
        boost::asio::ssl::context::no_tlsv1_1);

    // Load certificate and private key
    const auto certPath = cert::CertificateManager::getCertPath();
    const auto keyPath  = cert::CertificateManager::getKeyPath();

    try
    {
        sslContextPtr->use_certificate_chain_file(certPath);
        sslContextPtr->use_private_key_file(keyPath, boost::asio::ssl::context::pem);
    }
    catch (const boost::system::system_error& e)
    {
        std::println(stderr, "Failed to load certificate/key: {}", e.what());
        return std::unexpected(e.code());
    }

    sslContextPtr->set_verify_mode(boost::asio::ssl::verify_none);

    SSL_CTX_set_cipher_list(sslContextPtr->native_handle(), CIPHER_LIST.data());

    boost::asio::ssl::stream<tcp::socket> sslSocket(std::move(socket), *sslContextPtr);
    auto socketPtr = std::make_unique<SslSocket>(std::move(sslSocket));
    socketPtr->handshake(boost::asio::ssl::stream_base::server);

    return std::make_shared<Session>(std::move(socketPtr), std::move(sslContextPtr));
}

SessionOrError ConnectionManager::connect(const IoContextPtr& io, const std::string_view address, const Port port)
{
    const auto addressV4 = boost::asio::ip::make_address_v4(address);
    const auto endpoint  = tcp::endpoint(addressV4, port);
    boost::system::error_code ec{};

    tcp::socket socket(*io);

    if (socket.connect(endpoint, ec))
    {
        return std::unexpected(ec);
    }

    auto sslContextPtr =
        std::make_unique<SslContext>(boost::asio::ssl::context(boost::asio::ssl::context::tlsv12_client));

    sslContextPtr->set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::no_sslv3 |
        boost::asio::ssl::context::no_tlsv1 |
        boost::asio::ssl::context::no_tlsv1_1);

    // Load certificate and private key
    const auto certPath = cert::CertificateManager::getCertPath();
    const auto keyPath  = cert::CertificateManager::getKeyPath();

    try
    {
        sslContextPtr->use_certificate_file(certPath, boost::asio::ssl::context::pem);
        sslContextPtr->use_private_key_file(keyPath, boost::asio::ssl::context::pem);
    }
    catch (const boost::system::system_error& e)
    {
        std::println(stderr, "Failed to load certificate/key: {}", e.what());
        return std::unexpected(e.code());
    }

    sslContextPtr->set_verify_mode(boost::asio::ssl::verify_none);

    SSL_CTX_set_cipher_list(sslContextPtr->native_handle(), CIPHER_LIST.data());

    boost::asio::ssl::stream<tcp::socket> sslSocket(std::move(socket), *sslContextPtr);
    auto socketPtr = std::make_unique<SslSocket>(std::move(sslSocket));
    socketPtr->handshake(boost::asio::ssl::stream_base::client);

    return std::make_shared<Session>(std::move(socketPtr), std::move(sslContextPtr));
}

}  // namespace p2pft::comms
