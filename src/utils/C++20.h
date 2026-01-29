#pragma once

#include <algorithm>

namespace std20
{
    // ---- erase(value) ----
    template<class Container, class T>
    constexpr typename Container::size_type
        erase(Container& c, const T& value)
    {
        auto oldSize = c.size();

        c.erase(
            std::remove(c.begin(), c.end(), value),
            c.end()
        );

        return oldSize - c.size();
    }

    // ---- erase_if(predicate) ----
    template<class Container, class Pred>
    constexpr typename Container::size_type
        erase_if(Container& c, Pred pred)
    {
        auto oldSize = c.size();

        c.erase(
            std::remove_if(c.begin(), c.end(), pred),
            c.end()
        );

        return oldSize - c.size();
    }
}
