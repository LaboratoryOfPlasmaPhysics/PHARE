
#ifndef PHARE_TEST_DIAGNOSTIC_DEF
#define PHARE_TEST_DIAGNOSTIC_DEF

using namespace PHARE::amr;
using namespace PHARE::core;
using namespace PHARE::solver;

namespace PHARE_test
{
namespace _1d
{
    static constexpr std::size_t dim         = 1;
    static constexpr std::size_t interpOrder = 1;

    using GridImplYeeT  = GridLayoutImplYee<dim, interpOrder>;
    using GridYeeT      = GridLayout<GridImplYeeT>;
    using VecFieldT     = VecField<NdArrayVector1D<>, HybridQuantity>;
    using IonsPopT      = IonPopulation<ParticleArray<dim>, VecFieldT, GridYeeT>;
    using IonsT         = Ions<IonsPopT, GridYeeT>;
    using ElectromagT   = Electromag<VecFieldT>;
    using HybridModelT  = HybridModel<GridYeeT, ElectromagT, IonsT, SAMRAI_Types>;
    using HybridHybridT = HybridHybridMessengerStrategy<HybridModelT, IPhysicalModel<SAMRAI_Types>>;
    using HybridMessengerT  = HybridMessenger<HybridModelT, IPhysicalModel<SAMRAI_Types>>;
    using ResourcesManagerT = ResourcesManager<GridYeeT>;

    using ScalarFunctionT = PHARE::initializer::ScalarFunction<1>;
    using VectorFunctionT = PHARE::initializer::VectorFunction<1>;

    PHARE::initializer::PHAREDict<1> CREATE_IONS_DICT()
    {
        PHARE::initializer::PHAREDict<1> dict;
        dict["ions"]["name"]           = std::string{"ions"};
        dict["ions"]["nbrPopulations"] = std::size_t{2};
        dict["ions"]["pop0"]["name"]   = std::string{"protons"};
        dict["ions"]["pop0"]["mass"]   = 1.;
        dict["ions"]["pop0"]["ParticleInitializer"]["name"]
            = std::string{"MaxwellianParticleInitializer"};
        dict["ions"]["pop0"]["ParticleInitializer"]["density"]
            = static_cast<ScalarFunctionT>(density);

        dict["ions"]["pop0"]["ParticleInitializer"]["bulkVelocity"]
            = static_cast<VectorFunctionT>(bulkVelocity);

        dict["ions"]["pop0"]["ParticleInitializer"]["thermalVelocity"]
            = static_cast<VectorFunctionT>(thermalVelocity);

        dict["ions"]["pop0"]["ParticleInitializer"]["nbrPartPerCell"] = std::size_t{100};
        dict["ions"]["pop0"]["ParticleInitializer"]["charge"]         = -1.;
        dict["ions"]["pop0"]["ParticleInitializer"]["basis"]          = std::string{"Cartesian"};

        dict["ions"]["pop1"]["name"] = std::string{"alpha"};
        dict["ions"]["pop1"]["mass"] = 1.;
        dict["ions"]["pop1"]["ParticleInitializer"]["name"]
            = std::string{"MaxwellianParticleInitializer"};
        dict["ions"]["pop1"]["ParticleInitializer"]["density"]
            = static_cast<ScalarFunctionT>(density);

        dict["ions"]["pop1"]["ParticleInitializer"]["bulkVelocity"]
            = static_cast<VectorFunctionT>(bulkVelocity);

        dict["ions"]["pop1"]["ParticleInitializer"]["thermalVelocity"]
            = static_cast<VectorFunctionT>(thermalVelocity);

        dict["ions"]["pop1"]["ParticleInitializer"]["nbrPartPerCell"] = std::size_t{100};
        dict["ions"]["pop1"]["ParticleInitializer"]["charge"]         = -1.;
        dict["ions"]["pop1"]["ParticleInitializer"]["basis"]          = std::string{"Cartesian"};

        dict["electromag"]["name"]             = std::string{"EM"};
        dict["electromag"]["electric"]["name"] = std::string{"E"};
        dict["electromag"]["magnetic"]["name"] = std::string{"B"};

        dict["electromag"]["electric"]["initializer"]["x_component"]
            = static_cast<ScalarFunctionT>(ex);
        dict["electromag"]["electric"]["initializer"]["y_component"]
            = static_cast<ScalarFunctionT>(ey);
        dict["electromag"]["electric"]["initializer"]["z_component"]
            = static_cast<ScalarFunctionT>(ez);

        dict["electromag"]["magnetic"]["initializer"]["x_component"]
            = static_cast<ScalarFunctionT>(bx);
        dict["electromag"]["magnetic"]["initializer"]["y_component"]
            = static_cast<ScalarFunctionT>(by);
        dict["electromag"]["magnetic"]["initializer"]["z_component"]
            = static_cast<ScalarFunctionT>(bz);

        return dict;
    }
} // namespace _1d
} // namespace PHARE_test

#endif /*PHARE_TEST_DIAGNOSTIC_DEF*/