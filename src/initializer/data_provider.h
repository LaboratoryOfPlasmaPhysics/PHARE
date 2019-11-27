#ifndef DATA_PROVIDER_H
#define DATA_PROVIDER_H

#include "cppdict/include/dict.hpp"

#include <map>
#include <string>
#include <variant>

namespace PHARE
{
namespace initializer
{
    template<typename ReturnType, std::size_t dim>
    struct ScalarFunctionHelper
    {
    };

    template<>
    struct ScalarFunctionHelper<double, 1>
    {
        using type = std::function<double(double)>;
    };

    template<>
    struct ScalarFunctionHelper<double, 2>
    {
        using type = std::function<double(double, double)>;
    };

    template<>
    struct ScalarFunctionHelper<double, 3>
    {
        using type = std::function<double(double, double, double)>;
    };


    template<std::size_t dim>
    using ScalarFunction = typename ScalarFunctionHelper<double, dim>::type;



    // template<std::size_t dim>
    using PHAREDict = cppdict::Dict<int, double, std::size_t, std::string, ScalarFunction<1>,
                                    ScalarFunction<2>, ScalarFunction<3>>;



    class PHAREDictHandler
    {
        // std::unique_ptr<PHAREDict<1>> phareDict1D;
        // std::unique_ptr<PHAREDict<2>> phareDict2D;
        // std::unique_ptr<PHAREDict<3>> phareDict3D;
        std::unique_ptr<PHAREDict> phareDict;

    private:
        PHAREDictHandler() = default;



    public:
        // template<std::size_t dim>
        void init()
        {
            phareDict = std::make_unique<PHAREDict>();
            /*
            if constexpr (dim == 1)
                phareDict1D = std::make_unique<PHAREDict<1>>();

            else if constexpr (dim == 2)
            {
                phareDict2D = std::make_unique<PHAREDict<2>>();
            }

            else if constexpr (dim == 3)
            {
                phareDict3D = std::make_unique<PHAREDict<3>>();
            }*/
        }

        // template<std::size_t dim>
        void stop()
        {
            phareDict.release();
            /*if constexpr (dim == 1)
                phareDict1D.release();

            else if constexpr (dim == 2)
                phareDict2D.release();

            else if constexpr (dim == 3)
                phareDict3D.release();
                */
        }


        static PHAREDictHandler& INSTANCE();

        // template<std::size_t dim>
        auto& dict()
        {
            // static_assert(dim >= 1 and dim <= 3, "error, invalid dimension in dict<dim>()");
            /*if constexpr (dim == 1)
            {
                if (phareDict1D == nullptr)
                {
                    init<1>();
                }
                return *phareDict1D;
            }
            else if constexpr (dim == 2)
            {
                if (phareDict2D == nullptr)
                {
                    init<2>();
                }
                return *phareDict2D;
            }
            else if constexpr (dim == 3)
            {
                if (phareDict3D == nullptr)
                {
                    init<3>();
                }
                return *phareDict3D;
            }*/
            if (!phareDict)
                init();
            return *phareDict;
        }
    };


    class DataProvider
    {
    public:
        /**
         * @brief read will read the data from whatever source specific DataProvider classes
         * implement. readData() must be called before passing
         */
        virtual void read()     = 0;
        virtual ~DataProvider() = default;
    };

} // namespace initializer

} // namespace PHARE

#endif // DATA_PROVIDER_H
