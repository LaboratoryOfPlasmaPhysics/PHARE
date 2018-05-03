
#ifndef TESTS_CORE_DATA_GRIDLAYOUT_GRIDLAYOUT_CELL_CENTERED_COORD_H
#define TESTS_CORE_DATA_GRIDLAYOUT_GRIDLAYOUT_CELL_CENTERED_COORD_H

#include <array>

#include "data/grid/gridlayout.h"
#include "gridlayout_base_params.h"
#include "gridlayout_params.h"
#include "gridlayout_utilities.h"
#include "utilities/point/point.h"

namespace PHARE
{
template<Layout layoutType, std::size_t dim>
struct GridLayoutCellCenteringParam
{
    GridLayoutTestParam<layoutType, dim> base;

    std::vector<std::array<uint32, dim>> iCellForCentering;
    std::vector<std::array<double, dim>> expectedPosition;

    std::vector<std::array<double, dim>> actualPosition;

    template<typename Array, std::size_t... I>
    auto cellCenteredCoord_impl(Array const& array, std::index_sequence<I...>)
    {
        return base.layout->cellCenteredCoordinates(array[I]...);
    }

    template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
    auto cellCenteredCoord(const std::array<T, N>& array)
    {
        return cellCenteredCoord_impl(array, Indices{});
    }

    void init()
    {
        auto& field           = base.field;
        auto& layout          = base.layout;
        auto& currentQuantity = base.currentQuantity;

        field = base.makeMyField_(layout->allocSize(currentQuantity));

        for (auto&& iCell : iCellForCentering)
        {
            Point<double, dim> pos;
            pos = cellCenteredCoord(iCell);

            std::array<double, dim> actualPos;

            for (std::size_t iDim = 0; iDim < dim; ++iDim)
            {
                actualPos[iDim] = pos[iDim];
            }


            actualPosition.push_back(actualPos);
        }
    }
};


template<Layout layout, std::size_t dim>
auto createCellCenteringParam()
{
    std::vector<GridLayoutCellCenteringParam<layout, dim>> params;

    std::string summaryName{"centeredCoords_summary"};
    std::string valueName{"centeredCoords_values"};

    std::string path{"./"};

    std::string summaryPath{path + summaryName + "_" + std::to_string(dim) + "d.txt"};
    std::string valuePath{path + valueName + "_" + std::to_string(dim) + "d.txt"};

    std::ifstream summary{summaryPath};
    std::ifstream value{valuePath};

    std::string layoutName{"yee"};

    const std::map<std::string, HybridQuantity::Scalar> namesToQuantity{
        {"Bx", HybridQuantity::Scalar::Bx}, {"By", HybridQuantity::Scalar::By},
        {"Bz", HybridQuantity::Scalar::Bz}, {"Ex", HybridQuantity::Scalar::Ex},
        {"Ey", HybridQuantity::Scalar::Ey}, {"Ez", HybridQuantity::Scalar::Ez},
        {"Jx", HybridQuantity::Scalar::Jx}, {"Jy", HybridQuantity::Scalar::Jy},
        {"Jz", HybridQuantity::Scalar::Jz}, {"rho", HybridQuantity::Scalar::rho},
        {"Vx", HybridQuantity::Scalar::Vx}, {"Vy", HybridQuantity::Scalar::Vy},
        {"Vz", HybridQuantity::Scalar::Vz}, {"P", HybridQuantity::Scalar::P}};


    while (!summary.eof())
    {
        int currentOrder{0};


        std::array<uint32, dim> nbCell;
        std::array<double, dim> dl;

        std::array<uint32, dim> iStart;
        std::array<uint32, dim> iEnd;

        std::array<double, dim> origin;

        summary >> currentOrder;

        if (summary.eof() || summary.bad())
            break;

        writeToArray(summary, nbCell);
        writeToArray(summary, dl);
        writeToArray(summary, iStart);
        writeToArray(summary, iEnd);
        writeToArray(summary, origin);

        params.emplace_back();

        // NOTE: before c++17 Point{origin} cannot deduce the corect type
        params.back().base = createParam<layout, dim>(layoutName, currentOrder, dl, nbCell,
                                                      Point<double, dim>{origin});
    }



    while (!value.eof())
    {
        int order{0};

        std::array<uint32, dim> icell;
        std::array<double, dim> realPosition;

        value >> order;

        if (value.eof() || value.bad())
            break;

        writeToArray(value, icell);

        writeToArray(value, realPosition);


        auto& param = params[order - 1];

        param.iCellForCentering.push_back(icell);
        param.expectedPosition.push_back(realPosition);
    }

    for (auto&& param : params)
    {
        param.init();
    }
    return params;
}


} // namespace PHARE
#endif
