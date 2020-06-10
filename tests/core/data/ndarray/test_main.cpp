
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <random>
#include <string>

#include "core/data/ndarray/ndarray_vector.h"


using namespace PHARE::core;


template<class NdArray>
class GenericNdArray1D : public ::testing::Test
{
public:
    GenericNdArray1D()
        : a{{nx}}
    {
    }

protected:
    const uint32_t nx = 10;
    NdArray a;
};


template<class NdArray>
class GenericNdArray2D : public ::testing::Test
{
public:
    GenericNdArray2D()
        : a{{nx, ny}}
    {
    }

protected:
    const uint32_t nx = 10;
    const uint32_t ny = 20;
    NdArray a;
};


template<class NdArray>
class GenericNdArray3D : public ::testing::Test
{
public:
    GenericNdArray3D()
        : a{{nx, ny, nz}}
    {
    }

protected:
    const uint32_t nx = 10;
    const uint32_t ny = 20;
    const uint32_t nz = 30;
    NdArray a;
};


using NdArray1D = ::testing::Types<NdArrayVector<double, 1>>;
using NdArray2D = ::testing::Types<NdArrayVector<double, 2>>;
using NdArray3D = ::testing::Types<NdArrayVector<double, 3>>;


TYPED_TEST_SUITE(GenericNdArray1D, NdArray1D);
TYPED_TEST_SUITE(GenericNdArray2D, NdArray2D);
TYPED_TEST_SUITE(GenericNdArray3D, NdArray3D);



TYPED_TEST(GenericNdArray1D, SizeIsOkAfterNonEmptyCreation)
{
    EXPECT_EQ(this->nx, this->a.size());
}


TYPED_TEST(GenericNdArray1D, IsModifiable)
{
    uint32_t i{2};
    this->a(i) = 12.;
    EXPECT_EQ(12., this->a(i));
}


TYPED_TEST(GenericNdArray1D, CanBeReadOnly)
{
    uint32_t i{2};
    this->a(i)                          = 12.;
    NdArrayVector<double, 1> const& ref = this->a;
    EXPECT_EQ(12., ref(i));
}


TYPED_TEST(GenericNdArray1D, AccessWholeArray)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 2.0);
    std::vector<double> numbers(this->a.size());

    for (auto& v : numbers)
        v = dis(gen);

    for (uint32_t i = 0; i < this->nx; ++i)
    {
        this->a(i) = numbers[i];
        EXPECT_DOUBLE_EQ(numbers[i], this->a(i));
    }
}


TYPED_TEST(GenericNdArray1D, HasCopyCtor)
{
    for (auto& e : this->a)
        e = 12.;

    NdArrayVector<double, 1> other{this->nx};
    other = this->a;

    for (auto const& e : other)
        EXPECT_DOUBLE_EQ(12., e);
}


TYPED_TEST(GenericNdArray1D, HasMoveCtor)
{
    for (auto& e : this->a)
        e = 12.;

    NdArrayVector<double, 1> other = std::move(this->a);

    for (auto const& e : other)
        EXPECT_DOUBLE_EQ(12., e);
}



TYPED_TEST(GenericNdArray2D, SizeIsOkAfterNonEmptyCreation)
{
    EXPECT_EQ(this->nx * this->ny, this->a.size());
}


TYPED_TEST(GenericNdArray2D, IsModifiable)
{
    uint32_t i{2}, j{3};
    this->a(i, j) = 12.;
    EXPECT_EQ(12., this->a(i, j));
}


TYPED_TEST(GenericNdArray2D, CanBeReadOnly)
{
    uint32_t i{2}, j{3};
    this->a(i, j)                       = 12.;
    NdArrayVector<double, 2> const& ref = this->a;
    EXPECT_EQ(12., ref(i, j));
}


TYPED_TEST(GenericNdArray2D, AccessWholeArray)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 2.0);
    std::vector<double> numbers(this->a.size());

    for (auto& v : numbers)
        v = dis(gen);

    uint32_t idx{0};
    for (uint32_t i = 0; i < this->nx; ++i)
    {
        for (uint32_t j = 0; j < this->ny; ++j)
        {
            this->a(i, j) = numbers[idx];
            EXPECT_DOUBLE_EQ(numbers[idx++], this->a(i, j));
        }
    }
}


TYPED_TEST(GenericNdArray2D, HasCopyCtor)
{
    for (auto& e : this->a)
        e = 12.;

    NdArrayVector<double, 2> other{this->nx, this->ny};
    other = this->a;

    for (auto const& e : other)
        EXPECT_DOUBLE_EQ(12., e);
}


TYPED_TEST(GenericNdArray2D, HasMoveCtor)
{
    for (auto& e : this->a)
        e = 12.;

    NdArrayVector<double, 2> other = std::move(this->a);

    for (auto const& e : other)
        EXPECT_DOUBLE_EQ(12., e);
}



TYPED_TEST(GenericNdArray3D, SizeIsOkAfterNonEmptyCreation)
{
    EXPECT_EQ(this->nx * this->ny * this->nz, this->a.size());
}


TYPED_TEST(GenericNdArray3D, IsModifiable)
{
    uint32_t i{2}, j{3}, k{4};
    this->a(i, j, k) = 12.;
    EXPECT_EQ(12., this->a(i, j, k));
}


TYPED_TEST(GenericNdArray3D, CanBeReadOnly)
{
    uint32_t i{2}, j{3}, k{4};
    this->a(i, j, k)                    = 12.;
    NdArrayVector<double, 3> const& ref = this->a;
    EXPECT_EQ(12., ref(i, j, k));
}


TYPED_TEST(GenericNdArray3D, AccessWholeArray)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 2.0);
    std::vector<double> numbers(this->a.size());

    for (auto& v : numbers)
        v = dis(gen);


    uint32_t idx{0};
    for (uint32_t i = 0; i < this->nx; ++i)
    {
        for (uint32_t j = 0; j < this->ny; ++j)
        {
            for (uint32_t k = 0; k < this->nz; ++k)
            {
                this->a(i, j, k) = numbers[idx];
                EXPECT_DOUBLE_EQ(numbers[idx++], this->a(i, j, k));
            }
        }
    }
}


TYPED_TEST(GenericNdArray3D, HasCopyCtor)
{
    for (auto& e : this->a)
        e = 12.;

    NdArrayVector<double, 3> other{this->nx, this->ny, this->nz};
    other = this->a;

    for (auto const& e : other)
        EXPECT_DOUBLE_EQ(12., e);
}


TYPED_TEST(GenericNdArray3D, HasMoveCtor)
{
    for (auto& e : this->a)
        e = 12.;

    NdArrayVector<double, 3> other = std::move(this->a);

    for (auto const& e : other)
        EXPECT_DOUBLE_EQ(12., e);
}




#if 0
template<typename Simulator>
struct NdArrayTest : public ::testing::Test
{
};


using NdArrays
    = testing::Types<NdArrayVector<double, 1>, NdArrayVector<double, 2>, NdArrayVector<double, 3>>;

TYPED_TEST_SUITE(NdArrayTest, NdArrays);

TYPED_TEST(NdArrayTest, SizeIsOkAfterNonEmptyCreation) {}
#endif




#if 0
TEST(NdArray1D, SizeIsOkAfterNonEmptyCreation)
{
    auto size = 10u;
    NdArrayVector1D<> array1d{size};
    EXPECT_EQ(size, array1d.size());
}

TEST(NdArray2D, SizeIsOkAfterNonEmptyCreation)
{
    auto nx   = 12u;
    auto ny   = 24u;
    auto size = nx * ny;
    NdArrayVector2D<> array2d{nx, ny};
    EXPECT_EQ(size, array2d.size());
}

TEST(NdArray3D, SizeIsOkAfterNonEmptyCreation)
{
    auto nx   = 12u;
    auto ny   = 24u;
    auto nz   = 103u;
    auto size = nx * ny * nz;
    NdArrayVector3D<> array3d{nx, ny, nz};
    EXPECT_EQ(size, array3d.size());
}


TEST(NdArray1D, IsModifiable)
{
    auto nx = 12u;
    NdArrayVector1D<> array1d{nx};
    array1d(10) = 24;
    EXPECT_EQ(24, array1d(10));
}

TEST(NdArray2D, IsModifiable)
{
    auto nx = 12u;
    auto ny = 32u;
    NdArrayVector2D<> array2d{nx, ny};
    array2d(10, 8) = 24;
    EXPECT_EQ(24, array2d(10, 8));
}

TEST(NdArray3D, IsModifiable)
{
    auto nx = 12u;
    auto ny = 32u;
    auto nz = 13u;
    NdArrayVector3D<> array3d{nx, ny, nz};
    array3d(10, 8, 5) = 24;
    EXPECT_EQ(24, array3d(10, 8, 5));
}


TEST(NdArray1D, ArrayCanBeReadOnly)
{
    auto nx = 12u;
    NdArrayVector1D<> array1d{nx};
    array1d(10)                  = 24;
    NdArrayVector1D<> const& ref = array1d;
    EXPECT_EQ(24, ref(10));
}

TEST(NdArray2D, ArrayCanBeReadOnly)
{
    auto nx = 12u;
    auto ny = 87u;
    NdArrayVector2D<> array2d{nx, ny};
    array2d(10, 2)               = 24;
    NdArrayVector2D<> const& ref = array2d;
    EXPECT_EQ(24, ref(10, 2));
}

TEST(NdArray3D, ArrayCanBeReadOnly)
{
    auto nx = 12u;
    auto ny = 32u;
    auto nz = 13u;
    NdArrayVector3D<> array3d{nx, ny, nz};
    array3d(10, 8, 5)            = 24;
    NdArrayVector3D<> const& ref = array3d;
    EXPECT_EQ(24, ref(10, 8, 5));
}


MATCHER_P(FloatNearPointwise, tol, "Out of range")
{
    return (std::get<0>(arg) > std::get<1>(arg) - tol && std::get<0>(arg) < std::get<1>(arg) + tol);
}


TEST(NdArray1D, AccessWholeArray)
{
    auto nx = 20u;
    NdArrayVector1D<> array1d{nx};
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1.0, 2.0);
    std::vector<double> numbers(array1d.size());
    for (auto& v : numbers)
    {
        v = dis(gen);
    }

    for (int i = 0; i < static_cast<int>(array1d.size()); ++i)
    {
        array1d(i) = numbers[static_cast<std::vector<double>::size_type>(i)];
    }

    // TODO find better way to compare...

    for (int i = 0; i < static_cast<int>(array1d.size()); ++i)
    {
        EXPECT_DOUBLE_EQ(numbers[static_cast<std::vector<double>::size_type>(i)], array1d(i));
    }
}

TEST(NdArray2D, AccessWholeArray)
{
    auto nx = 20u;
    auto ny = 10u;
    NdArrayVector2D<> array2d{nx, ny};
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1.0, 2.0);
    std::vector<double> numbers(array2d.size());
    for (auto& v : numbers)
    {
        v = dis(gen);
    }

    std::vector<double>::size_type idx = 0;
    for (auto i = 0u; i < nx; ++i)
    {
        for (auto j = 0u; j < ny; ++j)
        {
            array2d(i, j) = numbers[idx++];
        }
    }

    // TODO find better way to compare...
    idx = 0;
    for (auto i = 0u; i < nx; ++i)
    {
        for (auto j = 0u; j < ny; ++j)
        {
            EXPECT_DOUBLE_EQ(numbers[idx++], array2d(i, j));
        }
    }
}

TEST(NdArray3D, AccessWholeArray)
{
    auto nx = 20u;
    auto ny = 10u;
    auto nz = 13u;
    NdArrayVector3D<> array3d{nx, ny, nz};
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1.0, 2.0);
    std::vector<double> numbers(array3d.size());
    for (auto& v : numbers)
    {
        v = dis(gen);
    }

    std::vector<double>::size_type idx = 0;
    for (auto i = 0u; i < nx; ++i)
    {
        for (auto j = 0u; j < ny; ++j)
        {
            for (auto k = 0u; k < nz; ++k)
            {
                array3d(i, j, k) = numbers[idx++];
            }
        }
    }

    // TODO find better way to compare...
    idx = 0;
    for (auto i = 0u; i < nx; ++i)
    {
        for (auto j = 0u; j < ny; ++j)
        {
            for (auto k = 0u; k < nz; ++k)
            {
                EXPECT_DOUBLE_EQ(numbers[idx++], array3d(i, j, k));
            }
        }
    }
}


TEST(NdArray1D, CanBeAssignedAnother)
{
    auto size = 10u;
    NdArrayVector1D<> array1d{size};
    NdArrayVector1D<> other{size};
    for (auto& v : array1d)
        v = 12.;

    other = array1d;

    for (auto const& v : other)
        EXPECT_DOUBLE_EQ(12., v);
}

TEST(NdArray2D, CanBeAssignedAnother)
{
    auto nx = 10u;
    auto ny = 11u;
    NdArrayVector2D<> array2d{nx, ny};
    NdArrayVector2D<> other{nx, ny};
    for (auto& v : array2d)
        v = 12.;

    other = array2d;

    for (auto const& v : other)
        EXPECT_DOUBLE_EQ(12., v);
}

TEST(NdArray3D, CanBeAssignedAnother)
{
    auto nx = 10u;
    auto ny = 11u;
    auto nz = 12u;
    NdArrayVector3D<> array3d{nx, ny, nz};
    NdArrayVector3D<> other{nx, ny, nz};
    for (auto& v : array3d)
        v = 12.;

    other = array3d;

    for (auto const& v : other)
        EXPECT_DOUBLE_EQ(12., v);
}
#endif




int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
