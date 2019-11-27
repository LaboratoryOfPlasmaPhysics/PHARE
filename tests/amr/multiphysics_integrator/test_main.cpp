#include <SAMRAI/algs/TimeRefinementIntegrator.h>
#include <SAMRAI/algs/TimeRefinementLevelStrategy.h>
#include <SAMRAI/geom/CartesianGridGeometry.h>
#include <SAMRAI/hier/CoarsenOperator.h>
#include <SAMRAI/hier/RefineOperator.h>
#include <SAMRAI/mesh/ChopAndPackLoadBalancer.h>
#include <SAMRAI/mesh/GriddingAlgorithm.h>
#include <SAMRAI/mesh/StandardTagAndInitStrategy.h>
#include <SAMRAI/mesh/StandardTagAndInitialize.h>
#include <SAMRAI/mesh/TileClustering.h>
#include <SAMRAI/tbox/InputManager.h>
#include <SAMRAI/tbox/SAMRAIManager.h>
#include <SAMRAI/tbox/SAMRAI_MPI.h>
#include <SAMRAI/xfer/CoarsenAlgorithm.h>
#include <SAMRAI/xfer/RefineAlgorithm.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <map>
#include <memory>


#include "amr/types/amr_types.h"
#include "data/electromag/electromag.h"
#include "data/field/coarsening/field_coarsen_operator.h"
#include "data/field/refine/field_refine_operator.h"
#include "data/grid/gridlayout.h"
#include "data/grid/gridlayout_impl.h"
#include "data/ions/ions.h"
#include "data/ions/particle_initializers/maxwellian_particle_initializer.h"
#include "data/particles/particle_array.h"
#include "data/vecfield/vecfield.h"
#include "data_provider.h"
#include "messengers/messenger_factory.h"
#include "multiphysics_integrator.h"
#include "physical_models/hybrid_model.h"
#include "physical_models/mhd_model.h"
#include "resources_manager/resources_manager.h"

#include "input_config.h"

using namespace PHARE::core;
using namespace PHARE::amr;
using namespace PHARE::solver;

class ParticleSplit : public SAMRAI::hier::RefineOperator
{
public:
    ParticleSplit()
        : SAMRAI::hier::RefineOperator{"ParticlesSplit"}
    {
    }

    virtual ~ParticleSplit() = default;

    int getOperatorPriority() const override { return 0; }

    SAMRAI::hier::IntVector getStencilWidth(SAMRAI::tbox::Dimension const& dim) const override
    {
        return SAMRAI::hier::IntVector::getOne(dim);
    }

    void refine(SAMRAI::hier::Patch& destination, SAMRAI::hier::Patch const& source,
                int const destinationComponent, int const sourceComponent,
                SAMRAI::hier::BoxOverlap const& destinationOverlap,
                SAMRAI::hier::IntVector const& ratio) const override
    {
        // do nothing
    }
};

struct DisabledType
{
};

class Schedule
{
public:
};

/** \brief Algorithm purpose is to store an algorithm and the associated schedule for a
 * particular variable
 * TODO : putting refine and coarsen algorithm in the same class is not really good,
 *        given how it is used, never it will contain both a refine and a coarsen schedule.
 *        It just looks like a variant: sometimes it is a refine , and another time it is a coarsen
 *
 */
class Algorithm
{
public:
    explicit Algorithm(SAMRAI::tbox::Dimension const& dimension)
        : coarsen{dimension}
    {
    }

    std::shared_ptr<SAMRAI::hier::RefineOperator> const* refOperator{nullptr};
    std::shared_ptr<SAMRAI::hier::CoarsenOperator> const* coarseOperator{nullptr};

    SAMRAI::xfer::RefineAlgorithm refine;
    SAMRAI::xfer::CoarsenAlgorithm coarsen;

    std::vector<std::shared_ptr<SAMRAI::xfer::RefineSchedule>> refineSchedule;
    std::vector<std::shared_ptr<SAMRAI::xfer::CoarsenSchedule>> coarsenSchedule;
};

#if 0
template<typename MHDMessenger, typename MHDToHybrid, typename HybridMessenger,
         typename HybridToFullPIC, typename FullPICMessenger>
class MessengerManager
{
public:
    template<typename Variable>
    void registerVariable(Variable const &variable)
    {
        std::cout << "register variable for future messenger\n";
        // this one will consist at creating the Algorithm for each
        // component.
        // As well as tracking which variable need which algorithm
        // Note that at this moment we do not know the temporal nature
        // of the messenger : so we have two possibilites :
        // create one algorithm for each or adding a parameter to determine
        // if we will need temporal interpolation
    }

private:
    std::vector<MHDMessenger> mhdMessengers_;
    std::vector<HybridToFullPIC> hybridMessengers_;
    std::vector<FullPICMessenger> fullPICMessenger_;

    MHDToHybrid mhdToHybridMessenger_;
    HybridToFullPIC hybridToFullPICMessenger_;
};
#endif




using VecField1D   = VecField<NdArrayVector1D<>, HybridQuantity>;
using Field1D      = Field<NdArrayVector1D<>, HybridQuantity::Scalar>;
using Electromag1D = Electromag<VecField1D>;

static constexpr std::size_t dim         = 1;
static constexpr std::size_t interpOrder = 1;

using GridImplYee1D = GridLayoutImplYee<dim, interpOrder>;
using GridYee1D     = GridLayout<GridImplYee1D>;
using MaxwellianParticleInitializer1D
    = MaxwellianParticleInitializer<ParticleArray<dim>, GridYee1D>;
using IonsPop1D = IonPopulation<ParticleArray<dim>, VecField1D, GridYee1D>;
using Ions1D    = Ions<IonsPop1D, GridYee1D>;




double density(double x)
{
    return x * x + 2.;
}

double vx(double x)
{
    (void)x;
    return 1.;
}


double vy(double x)
{
    (void)x;
    return 1.;
}


double vz(double x)
{
    (void)x;
    return 1.;
}


double vthx(double x)
{
    (void)x;
    return 1.;
}


double vthy(double x)
{
    (void)x;
    return 1.;
}



double vthz(double x)
{
    (void)x;
    return 1.;
}



double bx(double x)
{
    return x * x + 2.;
}

double by(double x)
{
    return x * x + 2.;
}

double bz(double x)
{
    return x * x + 2.;
}

double ex(double x)
{
    return x * x + 2.;
}

double ey(double x)
{
    return x * x + 2.;
}

double ez(double x)
{
    return x * x + 2.;
}



// -----------------------------------------------------------------------------
//                          MULTIPHYSICS INTEGRATOR
// -----------------------------------------------------------------------------



using ScalarFunctionT = PHARE::initializer::ScalarFunction<1>;

PHARE::initializer::PHAREDict createIonsDict()
{
    PHARE::initializer::PHAREDict dict;
    dict["ions"]["name"]           = std::string{"ions"};
    dict["ions"]["nbrPopulations"] = int{2};
    dict["ions"]["pop0"]["name"]   = std::string{"protons"};
    dict["ions"]["pop0"]["mass"]   = 1.;
    dict["ions"]["pop0"]["ParticleInitializer"]["name"]
        = std::string{"MaxwellianParticleInitializer"};
    dict["ions"]["pop0"]["ParticleInitializer"]["density"] = static_cast<ScalarFunctionT>(density);

    dict["ions"]["pop0"]["ParticleInitializer"]["bulk_velocity_x"]
        = static_cast<ScalarFunctionT>(vx);

    dict["ions"]["pop0"]["ParticleInitializer"]["bulk_velocity_y"]
        = static_cast<ScalarFunctionT>(vy);

    dict["ions"]["pop0"]["ParticleInitializer"]["bulk_velocity_z"]
        = static_cast<ScalarFunctionT>(vz);


    dict["ions"]["pop0"]["ParticleInitializer"]["thermal_velocity_x"]
        = static_cast<ScalarFunctionT>(vthx);

    dict["ions"]["pop0"]["ParticleInitializer"]["thermal_velocity_y"]
        = static_cast<ScalarFunctionT>(vthy);

    dict["ions"]["pop0"]["ParticleInitializer"]["thermal_velocity_z"]
        = static_cast<ScalarFunctionT>(vthz);


    dict["ions"]["pop0"]["ParticleInitializer"]["nbrPartPerCell"] = int{100};
    dict["ions"]["pop0"]["ParticleInitializer"]["charge"]         = -1.;
    dict["ions"]["pop0"]["ParticleInitializer"]["basis"]          = std::string{"Cartesian"};

    dict["ions"]["pop1"]["name"] = std::string{"alpha"};
    dict["ions"]["pop1"]["mass"] = 1.;
    dict["ions"]["pop1"]["ParticleInitializer"]["name"]
        = std::string{"MaxwellianParticleInitializer"};
    dict["ions"]["pop1"]["ParticleInitializer"]["density"] = static_cast<ScalarFunctionT>(density);

    dict["ions"]["pop1"]["ParticleInitializer"]["bulk_velocity_x"]
        = static_cast<ScalarFunctionT>(vx);

    dict["ions"]["pop1"]["ParticleInitializer"]["bulk_velocity_y"]
        = static_cast<ScalarFunctionT>(vy);

    dict["ions"]["pop1"]["ParticleInitializer"]["bulk_velocity_z"]
        = static_cast<ScalarFunctionT>(vz);


    dict["ions"]["pop1"]["ParticleInitializer"]["thermal_velocity_x"]
        = static_cast<ScalarFunctionT>(vthx);

    dict["ions"]["pop1"]["ParticleInitializer"]["thermal_velocity_y"]
        = static_cast<ScalarFunctionT>(vthy);

    dict["ions"]["pop1"]["ParticleInitializer"]["thermal_velocity_z"]
        = static_cast<ScalarFunctionT>(vthz);


    dict["ions"]["pop1"]["ParticleInitializer"]["nbrPartPerCell"] = int{100};
    dict["ions"]["pop1"]["ParticleInitializer"]["charge"]         = -1.;
    dict["ions"]["pop1"]["ParticleInitializer"]["basis"]          = std::string{"Cartesian"};

    dict["electromag"]["name"]             = std::string{"EM"};
    dict["electromag"]["electric"]["name"] = std::string{"E"};
    dict["electromag"]["magnetic"]["name"] = std::string{"B"};

    dict["electromag"]["electric"]["initializer"]["x_component"] = static_cast<ScalarFunctionT>(ex);
    dict["electromag"]["electric"]["initializer"]["y_component"] = static_cast<ScalarFunctionT>(ey);
    dict["electromag"]["electric"]["initializer"]["z_component"] = static_cast<ScalarFunctionT>(ez);

    dict["electromag"]["magnetic"]["initializer"]["x_component"] = static_cast<ScalarFunctionT>(bx);
    dict["electromag"]["magnetic"]["initializer"]["y_component"] = static_cast<ScalarFunctionT>(by);
    dict["electromag"]["magnetic"]["initializer"]["z_component"] = static_cast<ScalarFunctionT>(bz);

    return dict;
}




class aMultiPhysicsIntegrator : public ::testing::Test
{
private:
public:
    int levelNumber   = 0;
    bool initialTime  = true;
    bool canBeRefined = true;

    // IonsInit1D ionsInit;

    using HybridModelT = HybridModel<GridYee1D, Electromag1D, Ions1D, SAMRAI_Types>;
    using MHDModelT    = MHDModel<GridYee1D, VecField1D, SAMRAI_Types>;
    using SolverMHDT   = SolverMHD<MHDModelT, SAMRAI_Types>;
    using SolverPPCT   = SolverPPC<HybridModelT, SAMRAI_Types>;


    // physical models that can be used
    std::shared_ptr<HybridModelT> hybridModel;
    std::shared_ptr<MHDModelT> mhdModel;



    // (starts at 3rd level)
    int hybridStartLevel = 2; // levels : mhd mhd hybrid hybrid
    int maxLevelNbr      = 4;

    bool isInHybridRange(int iLevel) { return iLevel >= hybridStartLevel && iLevel < maxLevelNbr; }
    bool isInMHDdRange(int iLevel) { return iLevel >= 0 && iLevel < hybridStartLevel; }

    std::shared_ptr<SAMRAI::hier::PatchHierarchy> hierarchy;
    std::shared_ptr<SAMRAI::algs::TimeRefinementIntegrator> timeRefIntegrator;

    using MultiPhysicsIntegratorT = MultiPhysicsIntegrator<
        MessengerFactory<MHDModelT, HybridModelT, IPhysicalModel<SAMRAI_Types>>, SAMRAI_Types>;

    std::shared_ptr<MultiPhysicsIntegratorT> multiphysInteg;



    aMultiPhysicsIntegrator()
        : hybridModel{std::make_shared<HybridModelT>(
            createIonsDict(), std::make_shared<typename HybridModelT::resources_manager_type>())}
        , mhdModel{std::make_shared<MHDModelT>(
              std::make_shared<typename MHDModelT::resources_manager_type>())}
        , multiphysInteg{std::make_shared<MultiPhysicsIntegratorT>(maxLevelNbr)}
    {
        hybridModel->resourcesManager->registerResources(hybridModel->state);
        mhdModel->resourcesManager->registerResources(mhdModel->state);

        auto dimension = SAMRAI::tbox::Dimension{1};

        std::vector<MessengerDescriptor> descriptors;
        descriptors.push_back({"MHDModel", "MHDModel"});
        descriptors.push_back({"MHDModel", "HybridModel"});
        descriptors.push_back({"HybridModel", "HybridModel"});

        MessengerFactory<MHDModelT, HybridModelT, IPhysicalModel<SAMRAI_Types>> messengerFactory{
            descriptors};

        // hierarchy
        auto inputDatabase = SAMRAI::tbox::InputManager::getManager()->parseInputFile(
            inputBase + "input/input_1d_ratio_2.txt");
        auto patchHierarchyDatabase = inputDatabase->getDatabase("PatchHierarchy");

        auto gridGeometry = std::make_shared<SAMRAI::geom::CartesianGridGeometry>(
            dimension, "cartesian", inputDatabase->getDatabase("CartesianGridGeometry"));

        hierarchy = std::make_shared<SAMRAI::hier::PatchHierarchy>("PatchHierarchy", gridGeometry,
                                                                   patchHierarchyDatabase);


        auto loadBalancer = std::make_shared<SAMRAI::mesh::ChopAndPackLoadBalancer>(
            dimension, "ChopAndPackLoadBalancer",
            inputDatabase->getDatabase("ChopAndPackLoadBalancer"));




        multiphysInteg->registerModel(0, hybridStartLevel - 1, mhdModel);
        multiphysInteg->registerModel(hybridStartLevel, maxLevelNbr - 1, hybridModel);


        std::unique_ptr<SolverMHDT> mhdSolver{std::make_unique<SolverMHDT>()};
        std::unique_ptr<SolverPPCT> hybridSolver{std::make_unique<SolverPPCT>()};

        multiphysInteg->registerAndInitSolver(0, hybridStartLevel - 1, std::move(mhdSolver));
        multiphysInteg->registerAndInitSolver(hybridStartLevel, maxLevelNbr - 1,
                                              std::move(hybridSolver));


        // models and solvers must be registered to SAMRAI system before
        multiphysInteg->registerAndSetupMessengers(messengerFactory);


        auto standardTag = std::make_shared<SAMRAI::mesh::StandardTagAndInitialize>(
            "StandardTagAndInitialize", multiphysInteg.get(),
            inputDatabase->getDatabase("StandardTagAndInitialize"));

        auto clustering = std::make_shared<SAMRAI::mesh::TileClustering>(
            dimension, inputDatabase->getDatabase("TileClustering"));

        auto gridding = std::make_shared<SAMRAI::mesh::GriddingAlgorithm>(
            hierarchy, "GriddingAlgorithm", inputDatabase->getDatabase("GriddingAlgorithm"),
            standardTag, clustering, loadBalancer);


        timeRefIntegrator = std::make_shared<SAMRAI::algs::TimeRefinementIntegrator>(
            "TimeRefinementIntegrator", inputDatabase->getDatabase("TimeRefinementIntegrator"),
            hierarchy, multiphysInteg, gridding);


        // timeRefIntegrator->initializeHierarchy();
    }
};




TEST_F(aMultiPhysicsIntegrator, knowsWhichSolverisOnAGivenLevel)
{
    for (int iLevel = 0; iLevel < hierarchy->getNumberOfLevels(); ++iLevel)
    {
        if (isInHybridRange(iLevel))
        {
            EXPECT_EQ(std::string{"PPC"}, multiphysInteg->solverName(iLevel));
        }
        else if (isInMHDdRange(iLevel))
        {
            EXPECT_EQ(std::string{"MHDSolver"}, multiphysInteg->solverName(iLevel));
        }
    }
}



#ifdef DISABLE_TEST
// this test is disabled because one cannot for now initialize a
// multiphysics hierarchy.
TEST_F(aMultiPhysicsIntegrator, allocatesModelDataOnAppropriateLevels)
{
    for (int iLevel = 0; iLevel < hierarchy->getNumberOfLevels(); ++iLevel)
    {
        if (isInMHDdRange(iLevel))
        {
            auto Bid = mhdModel->resourcesManager->getIDs(mhdModel->state.B);
            auto Vid = mhdModel->resourcesManager->getIDs(mhdModel->state.V);

            std::array<std::vector<int> const*, 2> allIDs{{&Bid, &Vid}};

            for (auto& idVec : allIDs)
            {
                for (auto& id : *idVec)
                {
                    auto level = hierarchy->getPatchLevel(iLevel);
                    auto patch = level->begin();
                    EXPECT_TRUE(patch->checkAllocated(id));
                }
            }
        }
        else if (isInHybridRange(iLevel))
        {
            auto Bid   = hybridModel->resourcesManager->getIDs(hybridModel->state.electromag.B);
            auto Eid   = hybridModel->resourcesManager->getIDs(hybridModel->state.electromag.E);
            auto IonId = hybridModel->resourcesManager->getIDs(hybridModel->state.ions);

            std::array<std::vector<int> const*, 3> allIDs{{&Bid, &Eid, &IonId}};

            for (auto& idVec : allIDs)
            {
                for (auto& id : *idVec)
                {
                    auto level = hierarchy->getPatchLevel(iLevel);
                    auto patch = level->begin();
                    EXPECT_TRUE(patch->checkAllocated(id));
                }
            }
        }
    }
}
#endif


TEST_F(aMultiPhysicsIntegrator, knowsWhichModelIsSolvedAtAGivenLevel)
{
    auto nbrOfLevels = hierarchy->getNumberOfLevels();
    for (int iLevel = 0; iLevel < nbrOfLevels; ++iLevel)
    {
        if (isInMHDdRange(iLevel))
        {
            EXPECT_EQ(std::string{"MHDModel"}, multiphysInteg->modelName(iLevel));
        }
        else if (isInHybridRange(iLevel))
        {
            EXPECT_EQ(std::string{"HybridModel"}, multiphysInteg->modelName(iLevel));
        }
    }
}




TEST_F(aMultiPhysicsIntegrator, returnsCorrecMessengerForEachLevel)
{
    EXPECT_EQ(std::string{"MHDModel-MHDModel"}, multiphysInteg->messengerName(0));
    EXPECT_EQ(std::string{"MHDModel-MHDModel"}, multiphysInteg->messengerName(1));
    EXPECT_EQ(std::string{"MHDModel-HybridModel"}, multiphysInteg->messengerName(2));
    EXPECT_EQ(std::string{"HybridModel-HybridModel"}, multiphysInteg->messengerName(3));
}




/*
#endif

*/

#if 0
TEST(aMessenger, isCreated)
{
    SAMRAI::tbox::Dimension dimension{dim};
    ResourcesManager<GridYee1D> resourcesManager{dimension};


    IonsInit1D ionsInit;
    ionsInit.name   = "Ions";
    ionsInit.masses = {{0.1, 0.3}};

    ionsInit.names.emplace_back("specie1");
    ionsInit.names.emplace_back("specie2");

    ionsInit.nbrPopulations = 2;

    ionsInit.particleInitializers.push_back(std::make_unique<FluidParticleInitializer1D>(
        std::make_unique<ScalarFunction<dim>>(density),
        std::make_unique<VectorFunction<dim>>(bulkVelocity),
        std::make_unique<VectorFunction<dim>>(thermalVelocity), -1., 10));

    ionsInit.particleInitializers.push_back(std::make_unique<FluidParticleInitializer1D>(
        std::make_unique<ScalarFunction<dim>>(density),
        std::make_unique<VectorFunction<dim>>(bulkVelocity),
        std::make_unique<VectorFunction<dim>>(thermalVelocity), -1., 10));


    // HybridState need an ions initializers to create an ions
    HybridState<Electromag1D, Ions1D, IonsInit1D> hybridModel{std::move(ionsInit)};

    SolverPPC<Electromag1D, decltype(hybridModel)> solverppc{hybridModel};

    // we have to register the hybridModel for variable instanciation on  a patch
    resourcesManager.registerResources(hybridModel.electromag.E);
    resourcesManager.registerResources(hybridModel.electromag.B);
    resourcesManager.registerResources(hybridModel.ions);



    // our solver may also have data that need to  be instanciate on a patch
    solverppc.init(resourcesManager);


    auto fieldRefineOp  = std::make_shared<FieldDataLinearRefine<GridImplYee1D, Field1D>>();
    auto fieldCoarsenOp = std::make_shared<FieldDataCoarsen<GridImplYee1D, Field1D>>();

    auto particlesRefineOp = std::make_shared<ParticleSplit>();

    int const hybridStartLevel = 0;

    // then we init the messenger needed
    HybridMessenger hybridMessenger{hybridModel,    resourcesManager,  fieldRefineOp,
                                        fieldCoarsenOp, particlesRefineOp, hybridStartLevel};

    solverppc.initMessenger(hybridMessenger);


    // hierarchy
    auto inputDatabase = SAMRAI::tbox::InputManager::getManager()->parseInputFile(
        inputBase + "input/input_1d_ratio_2.txt");
    auto patchHierarchyDatabase = inputDatabase->getDatabase("PatchHierarchy");

    auto gridGeometry = std::make_shared<SAMRAI::geom::CartesianGridGeometry>(
        dimension, "cartesian", inputDatabase->getDatabase("CartesianGridGeometry"));

    auto hierarchy = std::make_shared<SAMRAI::hier::PatchHierarchy>("PatchHierarchy", gridGeometry,
                                                                    patchHierarchyDatabase);

    auto loadBalancer = std::make_shared<SAMRAI::mesh::ChopAndPackLoadBalancer>(
        dimension, "ChopAndPackLoadBalancer",
        inputDatabase->getDatabase("ChopAndPackLoadBalancer"));


    auto integratorStrategy = std::make_shared<
        MultiPhysicsIntegrator<decltype(hybridModel), decltype(resourcesManager)>>(
        hybridModel, hybridStartLevel, resourcesManager);

    auto standardTag = std::make_shared<SAMRAI::mesh::StandardTagAndInitialize>(
        "StandardTagAndInitialize", integratorStrategy.get(),
        inputDatabase->getDatabase("StandardTagAndInitialize"));

    auto clustering = std::make_shared<SAMRAI::mesh::TileClustering>(
        dimension, inputDatabase->getDatabase("TileClustering"));

    auto gridding = std::make_shared<SAMRAI::mesh::GriddingAlgorithm>(
        hierarchy, "GriddingAlgorithm", inputDatabase->getDatabase("GriddingAlgorithm"),
        standardTag, clustering, loadBalancer);


    auto integrator = std::make_shared<SAMRAI::algs::TimeRefinementIntegrator>(
        "TimeRefinementIntegrator", inputDatabase->getDatabase("TimeRefinementIntegrator"),
        hierarchy, integratorStrategy, gridding);



    // First step is to make the CoarsestLevel, and then to refine
    // as much as needed
    gridding->makeCoarsestLevel(0.0);
    // we refine for tag 0
    // For that we try to refine until we get no more refine boxes or that
    // we have reach the maximum level
    for (int iLevel = 0, hierarchyMaxLevelAllowed = hierarchy->getMaxNumberOfLevels();
         iLevel < hierarchyMaxLevelAllowed - 1; ++iLevel)
    {
        int const cycle   = 0;
        double const time = 0.0;

        SAMRAI::hier::BoxContainer boxes{};
        auto reset = standardTag->getUserSuppliedRefineBoxes(boxes, iLevel, cycle, time);
        NULL_USE(reset);
        if (!boxes.empty())
        {
            gridding->makeFinerLevel(0, true, cycle, time, 0.0);
        }
        else
        {
            break;
        }
    }


    auto level0 = hierarchy->getPatchLevel(0);
    hybridMessenger.setStatus(&hierarchy, &level0, nullptr);

    // then for each level, we give an hybridMessenger that know which level is the current one
    solverppc.advanceLevel(hybridMessenger, hybridMessenger);

    // after advancing each substep of a leaf
    solverppc.syncLevel(hybridMessenger);
}
#endif



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    SAMRAI::tbox::SAMRAI_MPI::init(&argc, &argv);
    SAMRAI::tbox::SAMRAIManager::initialize();
    SAMRAI::tbox::SAMRAIManager::startup();


    int testResult = RUN_ALL_TESTS();

    // Finalize
    SAMRAI::tbox::SAMRAIManager::shutdown();
    SAMRAI::tbox::SAMRAIManager::finalize();
    SAMRAI::tbox::SAMRAI_MPI::finalize();


    return testResult;
}
