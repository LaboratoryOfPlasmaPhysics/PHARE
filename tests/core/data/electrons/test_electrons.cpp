
#include "initializer/data_provider.h"

#include "core/data/electrons/electrons.h"
#include "core/data/grid/gridlayout.h"
#include "core/data/grid/gridlayout_impl.h"
#include "core/data/vecfield/vecfield.h"
#include "core/data/ions/ion_population/ion_population.h"
#include "core/data/ions/ions.h"
#include "core/data/electromag/electromag.h"
#include "src/core/utilities/types.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace PHARE::core;



//_____ only works for dim=1
// double density(double)
// {
//     return 2.;
// }


// double vx(double /*x*/)
// {
//     return 1.;
// }
//
//
// double vy(double /*x*/)
// {
//     return 1.;
// }
//
//
// double vz(double /*x*/)
// {
//     return 1.;
// }
//
//
// double vthx(double /*x*/)
// {
//     return 1.;
// }
//
//
// double vthy(double /*x*/)
// {
//     return 1.;
// }
//
//
// double vthz(double /*x*/)
// {
//     return 1.;
// }
//
//
// double bx(double x)
// {
//     return x /* + 1.*/;
// }
//
// double by(double x)
// {
//     return x /* + 2.*/;
// }
//
// double bz(double x)
// {
//     return x /*+ 3.*/;
// }
//
// double ex(double x)
// {
//     return x /* + 4.*/;
// }
//
// double ey(double x)
// {
//     return x /* + 5.*/;
// }
//
// double ez(double x)
// {
//     return x /* + 6.*/;
// }


const double Te = 0.12;




template<std::size_t dim>
auto makeScalarFunc(float val)
{
    if constexpr (dim == 1)
    {
        return [val](double) { return val; };
    }
    else if constexpr (dim == 2)
    {
        return [val](double, double) { return val; };
    }
}




template<int dim>
PHARE::initializer::PHAREDict createDict()
{
    using ScalarFunctionT = PHARE::initializer::ScalarFunction<dim>;

    auto density = makeScalarFunc<dim>(1.2);
    auto vx      = makeScalarFunc<dim>(1.2);
    auto vy      = makeScalarFunc<dim>(1.4);
    auto vz      = makeScalarFunc<dim>(1.6);
    auto vthx    = makeScalarFunc<dim>(0.2);
    auto vthy    = makeScalarFunc<dim>(0.4);
    auto vthz    = makeScalarFunc<dim>(0.6);
    auto bx      = makeScalarFunc<dim>(1.2);
    auto by      = makeScalarFunc<dim>(1.4);
    auto bz      = makeScalarFunc<dim>(1.6);
    auto ex      = makeScalarFunc<dim>(0.2);
    auto ey      = makeScalarFunc<dim>(0.4);
    auto ez      = makeScalarFunc<dim>(0.6);

    PHARE::initializer::PHAREDict dict;
    dict["ions"]["nbrPopulations"]                          = int{1};
    dict["ions"]["pop0"]["name"]                            = std::string{"protons"};
    dict["ions"]["pop0"]["mass"]                            = 1.;
    dict["ions"]["pop0"]["particle_initializer"]["name"]    = std::string{"maxwellian"};
    dict["ions"]["pop0"]["particle_initializer"]["density"] = static_cast<ScalarFunctionT>(density);

    dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_x"]
        = static_cast<ScalarFunctionT>(vx);

    dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_y"]
        = static_cast<ScalarFunctionT>(vy);

    dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_z"]
        = static_cast<ScalarFunctionT>(vz);


    dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_x"]
        = static_cast<ScalarFunctionT>(vthx);

    dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_y"]
        = static_cast<ScalarFunctionT>(vthy);

    dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_z"]
        = static_cast<ScalarFunctionT>(vthz);

    dict["ions"]["pop0"]["particle_initializer"]["nbr_part_per_cell"] = int{100};
    dict["ions"]["pop0"]["particle_initializer"]["charge"]            = -1.;
    dict["ions"]["pop0"]["particle_initializer"]["basis"]             = std::string{"cartesian"};

    dict["electromag"]["name"]             = std::string{"EM"};
    dict["electromag"]["electric"]["name"] = std::string{"E"};
    dict["electromag"]["magnetic"]["name"] = std::string{"B"};

    dict["electromag"]["electric"]["initializer"]["x_component"] = static_cast<ScalarFunctionT>(ex);
    dict["electromag"]["electric"]["initializer"]["y_component"] = static_cast<ScalarFunctionT>(ey);
    dict["electromag"]["electric"]["initializer"]["z_component"] = static_cast<ScalarFunctionT>(ez);

    dict["electromag"]["magnetic"]["initializer"]["x_component"] = static_cast<ScalarFunctionT>(bx);
    dict["electromag"]["magnetic"]["initializer"]["y_component"] = static_cast<ScalarFunctionT>(by);
    dict["electromag"]["magnetic"]["initializer"]["z_component"] = static_cast<ScalarFunctionT>(bz);

    dict["electrons"]["pressure_closure"]["name"] = std::string{"isothermal"};
    dict["electrons"]["pressure_closure"]["Te"]   = Te;

    return dict;
}



// https://stackoverflow.com/questions/46101569/compile-time-constructor-switch-in-c



template<int dim, int interp>
class NDlayout
{
    NDlayout() {}

    using nDL = GridLayout<GridLayoutImplYee<dim, interp>>;

public:
    static nDL create()
    {
        if constexpr (dim == 1)
        {
            return {{{0.1}}, {{50}}, {0.}};
        }
        else if constexpr (dim == 2)
        {
            return {{{0.1, 0.2}}, {{50, 40}}, {0., 0.}};
        }
    }
};



template<typename TypeInfo /*= std::pair<DimConst<1>, InterpConst<1>>*/>
struct ElectronsTest
{
    static constexpr auto dim    = typename TypeInfo::first_type{}();
    static constexpr auto interp = typename TypeInfo::second_type{}();


    using GridYee = GridLayout<GridLayoutImplYee<dim, interp>>;

    using VecFieldND      = VecField<NdArrayVector<dim>, HybridQuantity>;
    using FieldND         = typename VecFieldND::field_type;
    using ScalarFunctionT = PHARE::initializer::ScalarFunction<dim>;

    using IonPopulationND = IonPopulation<ParticleArray<dim>, VecFieldND, GridYee>;
    using IonsT           = Ions<IonPopulationND, GridYee>;
    using PartPackND      = ParticlesPack<typename IonPopulationND::particle_array_type>;
    using StandardHybridElectronFluxComputerT = StandardHybridElectronFluxComputer<IonsT>;


    GridYee layout = NDlayout<dim, interp>::create();
    // GridYee layout = createNdLayout<dim>();
    IonsT ions;
    Electromag<VecFieldND> electromag;
    VecFieldND J;
    FieldND Nibuffer;
    FieldND NiProtons;
    FieldND Vix;
    FieldND Viy;
    FieldND Viz;
    FieldND Fxi;
    FieldND Fyi;
    FieldND Fzi;
    PartPackND pack;
    FieldND Vex;
    FieldND Vey;
    FieldND Vez;
    FieldND Jx;
    FieldND Jy;
    FieldND Jz;
    Electrons<IonsT> electrons;
    FieldND Pe;

    ElectronsTest()
        : ions{createDict<dim>()["ions"]}
        , electromag{createDict<dim>()["electromag"]}
        , J{"J", HybridQuantity::Vector::J}
        , Nibuffer{ions.densityName(), HybridQuantity::Scalar::rho,
                   layout.allocSize(HybridQuantity::Scalar::rho)}
        , NiProtons{"protons_rho", HybridQuantity::Scalar::rho,
                    layout.allocSize(HybridQuantity::Scalar::rho)}
        , Vix{"bulkVel_x", HybridQuantity::Scalar::Vx, layout.allocSize(HybridQuantity::Scalar::Vx)}
        , Viy{"bulkVel_y", HybridQuantity::Scalar::Vy, layout.allocSize(HybridQuantity::Scalar::Vy)}
        , Viz{"bulkVel_z", HybridQuantity::Scalar::Vz, layout.allocSize(HybridQuantity::Scalar::Vz)}
        , Fxi{"protons_flux_x", HybridQuantity::Scalar::Vx,
              layout.allocSize(HybridQuantity::Scalar::Vx)}
        , Fyi{"protons_flux_y", HybridQuantity::Scalar::Vy,
              layout.allocSize(HybridQuantity::Scalar::Vy)}
        , Fzi{"protons_flux_z", HybridQuantity::Scalar::Vz,
              layout.allocSize(HybridQuantity::Scalar::Vz)}
        , Vex{"StandardHybridElectronFluxComputer_Ve_x", HybridQuantity::Scalar::Vx,
              layout.allocSize(HybridQuantity::Scalar::Vx)}
        , Vey{"StandardHybridElectronFluxComputer_Ve_y", HybridQuantity::Scalar::Vy,
              layout.allocSize(HybridQuantity::Scalar::Vy)}
        , Vez{"StandardHybridElectronFluxComputer_Ve_z", HybridQuantity::Scalar::Vz,
              layout.allocSize(HybridQuantity::Scalar::Vz)}
        , Jx{"J_x", HybridQuantity::Scalar::Jx, layout.allocSize(HybridQuantity::Scalar::Jx)}
        , Jy{"J_y", HybridQuantity::Scalar::Jy, layout.allocSize(HybridQuantity::Scalar::Jy)}
        , Jz{"J_z", HybridQuantity::Scalar::Jz, layout.allocSize(HybridQuantity::Scalar::Jz)}
        , electrons{createDict<dim>()["electrons"], ions, J}
        , Pe{"Pe", HybridQuantity::Scalar::P, layout.allocSize(HybridQuantity::Scalar::P)}
    {
        J.setBuffer(Jx.name(), &Jx);
        J.setBuffer(Jy.name(), &Jy);
        J.setBuffer(Jz.name(), &Jz);

        ions.setBuffer(ions.densityName(), &Nibuffer);
        ions.velocity().setBuffer(Vix.name(), &Vix);
        ions.velocity().setBuffer(Viy.name(), &Viy);
        ions.velocity().setBuffer(Viz.name(), &Viz);

        auto& pops = ions.getRunTimeResourcesUserList();

        pops[0].setBuffer(NiProtons.name(), &NiProtons);
        pops[0].flux().setBuffer(Fxi.name(), &Fxi);
        pops[0].flux().setBuffer(Fyi.name(), &Fyi);
        pops[0].flux().setBuffer(Fzi.name(), &Fzi);
        pops[0].setBuffer("protons", &pack);

        auto&& emm = std::get<0>(electrons.getCompileTimeResourcesUserList());
        auto&& fc  = std::get<0>(emm.getCompileTimeResourcesUserList());
        auto&& Ve  = std::get<0>(fc.getCompileTimeResourcesUserList());

        Ve.setBuffer(Vex.name(), &Vex);
        Ve.setBuffer(Vey.name(), &Vey);
        Ve.setBuffer(Vez.name(), &Vez);

        auto&& pc = std::get<1>(emm.getCompileTimeResourcesUserList());

        pc.setBuffer(Pe.name(), &Pe);

        if constexpr (dim == 1)
        {
            auto fill = [this](FieldND& field, auto const& filler) {
                auto gsi_X = this->layout.ghostStartIndex(field, Direction::X);
                auto gei_X = this->layout.ghostEndIndex(field, Direction::X);

                for (auto ix = gsi_X; ix <= gei_X; ++ix)
                {
                    auto point = this->layout.fieldNodeCoordinates(field, Point<double, 1>{0.}, ix);
                    field(ix)  = filler(point[0]);
                }
            };

            fill(Vix, [](double x) { return std::cosh(0.2 * x); });
            fill(Viy, [](double x) { return std::cosh(0.3 * x); });
            fill(Viz, [](double x) { return std::cosh(0.4 * x); });

            fill(Jx, [](double x) { return std::sinh(0.2 * x); });
            fill(Jy, [](double x) { return std::sinh(0.3 * x); });
            fill(Jz, [](double x) { return std::sinh(0.4 * x); });

            fill(Nibuffer, [](double x) { return std::cosh(0.1 * x); });
        }
        else if constexpr (dim == 2)
        {
            auto fill = [this](FieldND& field, auto const& filler) {
                auto gsi_X = this->layout.ghostStartIndex(field, Direction::X);
                auto gei_X = this->layout.ghostEndIndex(field, Direction::X);
                auto gsi_Y = this->layout.ghostStartIndex(field, Direction::Y);
                auto gei_Y = this->layout.ghostEndIndex(field, Direction::Y);

                for (auto ix = gsi_X; ix <= gei_X; ++ix)
                {
                    for (auto iy = gsi_Y; iy <= gei_Y; ++iy)
                    {
                        auto point = this->layout.fieldNodeCoordinates(
                            field, Point<double, 2>{0., 0.}, ix, iy);
                        field(ix, iy) = filler(point[0], point[1]);
                    }
                }
            };

            fill(Vix, [](double x, double y) { return std::cosh(0.2 * x) * std::cosh(0.2 * y); });
            fill(Viy, [](double x, double y) { return std::cosh(0.3 * x) * std::cosh(0.3 * y); });
            fill(Viy, [](double x, double y) { return std::cosh(0.4 * x) * std::cosh(0.4 * y); });

            fill(Jx, [](double x, double y) { return std::sinh(0.2 * x) * std::sinh(0.2 * y); });
            fill(Jy, [](double x, double y) { return std::sinh(0.3 * x) * std::sinh(0.3 * y); });
            fill(Jy, [](double x, double y) { return std::sinh(0.4 * x) * std::sinh(0.4 * y); });

            fill(Nibuffer,
                 [](double x, double y) { return std::cosh(0.1 * x) * std::cosh(0.1 * y); });
        }
        else if constexpr (dim == 3)
        {
        }
    }

    ~ElectronsTest()
    {
        J.setBuffer(Jx.name(), nullptr);
        J.setBuffer(Jy.name(), nullptr);
        J.setBuffer(Jz.name(), nullptr);

        ions.setBuffer(ions.densityName(), nullptr);
        ions.velocity().setBuffer(Vix.name(), nullptr);
        ions.velocity().setBuffer(Viy.name(), nullptr);
        ions.velocity().setBuffer(Viz.name(), nullptr);

        auto& pops = ions.getRunTimeResourcesUserList();

        pops[0].setBuffer(NiProtons.name(), static_cast<FieldND*>(nullptr));
        pops[0].flux().setBuffer(Fxi.name(), nullptr);
        pops[0].flux().setBuffer(Fyi.name(), nullptr);
        pops[0].flux().setBuffer(Fzi.name(), nullptr);
        pops[0].setBuffer("protons", static_cast<PartPackND*>(nullptr));

        auto&& emm = std::get<0>(electrons.getCompileTimeResourcesUserList());
        auto&& fc  = std::get<0>(emm.getCompileTimeResourcesUserList());
        auto&& Ve  = std::get<0>(fc.getCompileTimeResourcesUserList());

        Ve.setBuffer(Vex.name(), nullptr);
        Ve.setBuffer(Vey.name(), nullptr);
        Ve.setBuffer(Vez.name(), nullptr);

        auto&& pc = std::get<1>(emm.getCompileTimeResourcesUserList());

        pc.setBuffer(Pe.name(), nullptr);
    }
};


template<typename TypeInfo>
struct TElectronsTest : public ::testing::Test
{
};

using ElectronsTupleInfos = testing::Types<ElectronsTest<std::pair<DimConst<1>, InterpConst<1>>>,
                                           ElectronsTest<std::pair<DimConst<1>, InterpConst<2>>>,
                                           ElectronsTest<std::pair<DimConst<1>, InterpConst<3>>>,
                                           ElectronsTest<std::pair<DimConst<2>, InterpConst<1>>>,
                                           ElectronsTest<std::pair<DimConst<2>, InterpConst<1>>>,
                                           ElectronsTest<std::pair<DimConst<2>, InterpConst<1>>>>;

TYPED_TEST_SUITE(TElectronsTest, ElectronsTupleInfos);

TYPED_TEST(TElectronsTest, ThatElectronsHasCtor)
{
    using ElectronsTest = TypeParam;

    std::cout << __FILE__ << " " << __LINE__ << " " << ElectronsTest::dim << std::endl;
    std::cout << __FILE__ << " " << __LINE__ << " " << ElectronsTest::interp << std::endl;
}



TYPED_TEST(TElectronsTest, ThatElectronsAreUsable)
{
    TypeParam test;
    auto& electrons = test.electrons;
    EXPECT_TRUE(electrons.isUsable());
}




TYPED_TEST(TElectronsTest, ThatElectronsDensityEqualIonDensity)
{
    TypeParam test;
    auto& electrons = test.electrons;
    auto& layout    = test.layout;
    auto& ions      = test.ions;
    auto& dim       = test.dim;

    electrons.update(layout);

    auto& Ne = electrons.density();
    auto& Ni = ions.density();

    if constexpr (dim == 1)
    {
        auto psi_X = layout.physicalStartIndex(Ne, Direction::X);
        auto pei_X = layout.physicalEndIndex(Ne, Direction::X);

        for (std::uint32_t i = psi_X; i < pei_X; ++i)
        {
            EXPECT_DOUBLE_EQ(Ni(i), Ne(i));
        }
    }
    else if constexpr (dim == 2)
    {
        auto psi_X = layout.physicalStartIndex(Ne, Direction::X);
        auto pei_X = layout.physicalEndIndex(Ne, Direction::X);
        auto psi_Y = layout.physicalStartIndex(Ne, Direction::Y);
        auto pei_Y = layout.physicalEndIndex(Ne, Direction::Y);

        for (std::uint32_t i = psi_X; i < pei_X; ++i)
        {
            for (std::uint32_t j = psi_Y; j < pei_Y; ++j)
            {
                EXPECT_DOUBLE_EQ(Ni(i, j), Ne(i, j));
            }
        }
    }
    else if (dim == 3)
    {
    }
}


TYPED_TEST(TElectronsTest, ThatElectronsVelocityEqualIonVelocityMinusJ)
{
    TypeParam test;
    auto& dim       = test.dim;
    auto& interp    = test.interp;
    auto& electrons = test.electrons;
    auto& layout    = test.layout;

    using VecFieldND = VecField<NdArrayVector<dim>, HybridQuantity>;
    using FieldND    = typename VecFieldND::field_type;
    using GridYee    = GridLayout<GridLayoutImplYee<dim, interp>>;


    electrons.update(layout);

    auto& Ne = electrons.density();

    auto check = [&dim, &layout](FieldND const& Vecomp, FieldND const& Vicomp, FieldND const& Jcomp,
                                 FieldND const& Ne_, auto const& projector) {
        if constexpr (dim == 1)
        {
            auto psi_X = layout.physicalStartIndex(Vicomp, Direction::X);
            auto pei_X = layout.physicalEndIndex(Vicomp, Direction::X);

            for (std::uint32_t i = psi_X; i < pei_X; ++i)
            {
                auto const JOnV = GridYee::project(Jcomp, {i}, projector());

                EXPECT_DOUBLE_EQ(Vecomp(i), Vicomp(i) - JOnV / Ne_(i));
            }
        }
        else if constexpr (dim == 2)
        {
            auto psi_X = layout.physicalStartIndex(Vicomp, Direction::X);
            auto pei_X = layout.physicalEndIndex(Vicomp, Direction::X);
            auto psi_Y = layout.physicalStartIndex(Vicomp, Direction::Y);
            auto pei_Y = layout.physicalEndIndex(Vicomp, Direction::Y);

            for (std::uint32_t i = psi_X; i < pei_X; ++i)
            {
                for (std::uint32_t j = psi_Y; j < pei_Y; ++j)
                {
                    auto const JOnV = GridYee::project(Jcomp, {i, j}, projector());

                    EXPECT_DOUBLE_EQ(Vecomp(i, j), Vicomp(i, j) - JOnV / Ne_(i, j));
                }
            }
        }
        else if (dim == 3)
        {
        }
    };

    check(test.Vex, test.Vix, test.Jx, Ne, GridYee::JxToMoments);
    check(test.Vey, test.Viy, test.Jy, Ne, GridYee::JyToMoments);
    check(test.Vez, test.Viz, test.Jz, Ne, GridYee::JzToMoments);
}



TYPED_TEST(TElectronsTest, ThatElectronsPressureEqualsNeTe)
{
    TypeParam test;
    auto& electrons = test.electrons;
    auto& layout    = test.layout;
    auto& dim       = test.dim;

    electrons.update(layout);

    auto& Ne_ = electrons.density();
    auto& Pe_ = electrons.pressure();

    if constexpr (dim == 1)
    {
        auto psi_X = layout.physicalStartIndex(Ne_, Direction::X);
        auto pei_X = layout.physicalEndIndex(Ne_, Direction::X);

        for (std::uint32_t i = psi_X; i < pei_X; ++i)
        {
            EXPECT_DOUBLE_EQ(Pe_(i), Ne_(i) * Te);
        }
    }
    else if constexpr (dim == 2)
    {
        auto psi_X = layout.physicalStartIndex(Ne_, Direction::X);
        auto pei_X = layout.physicalEndIndex(Ne_, Direction::X);
        auto psi_Y = layout.physicalStartIndex(Ne_, Direction::Y);
        auto pei_Y = layout.physicalEndIndex(Ne_, Direction::Y);

        for (std::uint32_t i = psi_X; i < pei_X; ++i)
        {
            for (std::uint32_t j = psi_Y; j < pei_Y; ++j)
            {
                EXPECT_DOUBLE_EQ(Pe_(i, j), Ne_(i, j) * Te);
            }
        }
    }
    else if (dim == 3)
    {
    }
}



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
