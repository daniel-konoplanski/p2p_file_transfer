#pragma once

#include <string>

namespace p2pft::cert
{

class CertificateManager
{
public:
    static bool create();
    static std::string getCertPath();
    static std::string getKeyPath();
    static bool isCertCreated();

private:
    static std::string getConfPath();

private:
    static std::string certFileName;
    static std::string keyFileName;
};

}  // namespace p2pft::cert
