#pragma once

namespace p2pft
{

class IApplication
{
public:
    virtual ~IApplication() = default;
    virtual void run()      = 0;
};

}  // namespace p2pft
