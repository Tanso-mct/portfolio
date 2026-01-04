#pragma once

#include <utility>

namespace riaecs
{
    struct PairLess 
    {
        template <typename T1, typename T2>
        bool operator()(
            const std::pair<T1, T2>& lhs,
            const std::pair<T1, T2>& rhs) const
        {
            if (lhs.first < rhs.first) return true;
            if (rhs.first < lhs.first) return false;
            return lhs.second < rhs.second;
        }
    };

} // namespace riaecs 