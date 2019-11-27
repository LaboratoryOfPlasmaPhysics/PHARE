
#include "cppdict/include/dict.hpp"
#include "python_data_provider.h"

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>


using PHARE::initializer::ScalarFunction;


template<typename T,
         typename = std::enable_if_t<cppdict::is_in<T, int, double, std::string, ScalarFunction<1>,
                                                    ScalarFunction<2>, ScalarFunction<3>>>>
void add(std::string path, T&& value)
{
    cppdict::add(path, std::forward<T>(value),
                 PHARE::initializer::PHAREDictHandler::INSTANCE().dict());
}




PYBIND11_MODULE(pyphare, m)
{
    m.def("add", add<int, void>, "add");
    m.def("add", add<double, void>, "add");
    m.def("add", add<std::string, void>, "add");
    m.def("addScalarFunction1D", add<ScalarFunction<1>, void>, "add");
    m.def("addScalarFunction2D", add<ScalarFunction<2>, void>, "add");
    m.def("addScalarFunction3D", add<ScalarFunction<3>, void>, "add");
}
