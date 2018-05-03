#ifndef TESTS_CORE_DATA_GRIDLAYOUT_ALLOCSIZE_H
#define TESTS_CORE_DATA_GRIDLAYOUT_ALLOCSIZE_H


#include "data/grid/gridlayout.h"
#include "data/ndarray/ndarray_vector.h"
#include "gridlayout_base_params.h"
#include "gridlayout_params.h"
#include "gridlayout_utilities.h"
#include "utilities/point/point.h"

#include <fstream>
#include <vector>




namespace PHARE
{
template<Layout layoutType, std::size_t dim>
struct GridLayoutAllocSizeParam
{
    GridLayoutTestParam<layoutType, dim> base;

    std::array<uint32, dim> expectedAllocSize;
    std::array<uint32, dim> expectedAllocSizeDerived;

    std::array<uint32, dim> actualAllocSize;
    std::array<uint32, dim> actualAllocSizeDerived;


    void init()
    {
        auto& layout          = base.layout;
        auto& currentQuantity = base.currentQuantity;

        actualAllocSize = layout->allocSize(currentQuantity);

        actualAllocSizeDerived[0] = layout->allocSizeDerived(currentQuantity, Direction::X)[0];
        if (dim > 1)
        {
            actualAllocSizeDerived[1] = layout->allocSizeDerived(currentQuantity, Direction::Y)[1];
        }
        if (dim > 2)
        {
            actualAllocSizeDerived[2] = layout->allocSizeDerived(currentQuantity, Direction::Z)[2];
        }
    }
};


template<Layout layout, std::size_t dim>
auto createAllocSizeParam()
{
    std::vector<GridLayoutAllocSizeParam<layout, dim>> params;

    std::string path{"./"};
    std::string baseName{"allocSizes"};


    std::string fullName{path + baseName + "_" + std::to_string(dim) + "d.txt"};
    std::ifstream inputFile{fullName};



    std::string layoutName{"yee"}; // hard coded for now


    if (!inputFile.is_open())
    {
        throw std::runtime_error("Error cannot open " + fullName);
    }

    while (!inputFile.eof())
    {
        uint32 interpOrder{0};
        uint32 iQuantity;
        std::array<uint32, dim> numberCells;
        std::array<double, dim> dl;

        inputFile >> interpOrder;
        inputFile >> iQuantity;


        if (inputFile.eof() || inputFile.bad())
            break;

        writeToArray(inputFile, numberCells);
        writeToArray(inputFile, dl);

        params.emplace_back();
        params.back().base = createParam<layout, dim>(layoutName, interpOrder, dl, numberCells,
                                                      Point<double, dim>{});

        writeToArray(inputFile, params.back().expectedAllocSize);
        writeToArray(inputFile, params.back().expectedAllocSizeDerived);

        params.back().base.currentQuantity = getQuantity(iQuantity);
    }

    return params;
}

} // namespace PHARE

#endif
