#pragma once

#include <utility>

namespace riaecs
{
    struct PairHash 
    {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& pair) const noexcept
        {
            std::size_t h1 = std::hash<T1>()(pair.first);
            std::size_t h2 = std::hash<T2>()(pair.second);

            std::size_t seed = h1;
            seed ^= h2 + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            return seed;
        }
    };

} // namespace riaecs 