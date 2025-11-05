#pragma once

namespace p2pft
{

class IApplication
{
public:
    virtual void start()    = 0;
    virtual ~IApplication() = default;
};

}  // namespace p2pft
