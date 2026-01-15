#include "certificate_manager.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <print>

#include <botan/auto_rng.h>
#include <botan/ec_group.h>
#include <botan/ecdsa.h>
#include <botan/pkcs8.h>
#include <botan/x509cert.h>
#include <botan/x509self.h>

namespace p2pft::cert
{

std::string CertificateManager::certFileName{ "cert.pem" };
std::string CertificateManager::keyFileName{ "key.pem" };

bool CertificateManager::create()
{
    constexpr std::string_view name = "secp256r1";

    Botan::AutoSeeded_RNG rng;
    const auto group = Botan::EC_Group::from_name(name);
    Botan::ECDSA_PrivateKey privateKey(rng, group);

    Botan::X509_Cert_Options opts;
    opts.add_ex_constraint("PKIX.ServerAuth");
    opts.add_ex_constraint("PKIX.ClientAuth");

    Botan::X509_Certificate cert = Botan::X509::create_self_signed_cert(opts, privateKey, "SHA-256", rng);

    if (!std::filesystem::exists(getConfPath())) std::filesystem::create_directories(getConfPath());

    std::ofstream privateKeyFile(getConfPath() + "/" + keyFileName);

    if (!privateKeyFile)
    {
        std::println(stderr, "Failed to write the private key");
        return false;
    }

    privateKeyFile << Botan::PKCS8::PEM_encode(privateKey);
    privateKeyFile.close();

    std::ofstream certFile(getConfPath() + "/" + certFileName);

    if (!certFile)
    {
        std::println(stderr, "Failed to write the public key");
        return false;
    }

    certFile << cert.PEM_encode();
    certFile.close();

    return true;
}

std::string CertificateManager::getCertPath()
{
    return getConfPath() + "/" + certFileName;
}

std::string CertificateManager::getKeyPath()
{
    return getConfPath() + "/" + keyFileName;
}

std::string CertificateManager::getConfPath()
{
    const std::string homeDir = std::getenv("HOME");

    return homeDir + "/.config/p2p_ft";
}

bool CertificateManager::isCertCreated()
{
    auto isCertCreated = std::filesystem::exists(getCertPath());
    auto isKeyCreated  = std::filesystem::exists(getKeyPath());

    return isCertCreated && isKeyCreated;
}

}  // namespace p2pft::cert
