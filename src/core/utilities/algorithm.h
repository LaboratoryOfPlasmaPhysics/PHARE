#ifndef PHARE_ALGORITHM_H
#define PHARE_ALGORITHM_H

#include "utilities/types.h"



#include <algorithm>
#include <string>

namespace PHARE
{
namespace core
{
    template<uint32 lhs, uint32 rhs>
    constexpr uint32 max()
    {
        if constexpr (lhs < rhs)
        {
            return rhs;
        }
        else if constexpr (lhs >= rhs)
        {
            return lhs;
        }
    }


    template<typename T>
    std::string to_str(T&& t)
    {
        return t.to_str();
    }



    template<typename Container, typename ContainedT = typename Container::value_type>
    bool notIn(ContainedT& obj, Container& list)
    {
        auto sameItem = std::find_if(std::begin(list), std::end(list), [&obj](auto& currentItem) {
            return obj->name() == currentItem->name();
        });

        return sameItem == std::end(list);
    }



} // namespace core
} // namespace PHARE

#endif
