#ifndef TYPES_H
#define TYPES_H

#include <array>
#include <cinttypes>
#include <cmath>
#include <numeric>
#include <vector>


namespace PHARE
{
namespace core
{
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;
    using int32  = std::int32_t;
    using int64  = std::int64_t;

    enum class Basis { Magnetic, Cartesian };




    template<typename T>
    std::vector<T> arange(T start, T stop, T step = 1)
    {
        std::vector<T> values;
        for (T value = start; value < stop; value += step)
            values.push_back(value);
        return values;
    }

    template<typename T>
    T norm(std::array<T, 3> vec)
    {
        auto squarreSum = std::inner_product(std::begin(vec), std::end(vec), std::begin(vec), 0.);
        return std::sqrt(squarreSum);
    }




    enum class Edge { Xmin, Xmax, Ymin, Ymax, Zmin, Zmax };

    template<typename T>
    class StrongType
    {
    public:
        constexpr StrongType(T val)
            : value_(val)
        {
        }
        constexpr StrongType()
            : value_(0)
        {
        }
        constexpr operator T() const noexcept { return value_; }
        constexpr inline const T& value() const { return value_; }

    private:
        T value_;
    }; 


} // namespace core
} // namespace PHARE

#endif // TYPES_H
