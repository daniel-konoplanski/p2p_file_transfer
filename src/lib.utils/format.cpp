#include "format"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace p2pft::utils
{

std::string formatBytes(const uint64_t bytes)
{
    constexpr int maxIndexForUnits{ 5U };
    constexpr std::array units{ "B", "KB", "MB", "GB", "TB", "PB" };
    constexpr uint32_t base{ 1024U };

    if (bytes == 0) return "0 B";

    const int unitIndex = std::min(static_cast<int>(std::log(bytes) / std::log(base)), maxIndexForUnits);

    const double value = bytes / std::pow(base, unitIndex);

    std::ostringstream oss;

    if (value >= 100)
    {
        oss << std::fixed << std::setprecision(0);
    }
    else if (value >= 10)
    {
        oss << std::fixed << std::setprecision(1);
    }
    else
    {
        oss << std::fixed << std::setprecision(2);
    }

    oss << value << " " << units[unitIndex];

    return oss.str();
}

}  // namespace p2pft::utils
