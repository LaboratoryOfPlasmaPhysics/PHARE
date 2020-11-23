

#include "src/simulator/simulator.h"
#include "src/simulator/phare_types.h"
#include "src/phare/phare.h"

#include "test_messenger_basichierarchy.h"
#include "test_integrator_strat.h"
#include "test_messenger_tag_strategy.h"
#include "tests/initializer/init_functions.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"


using namespace PHARE::core;
using namespace PHARE::amr;
using namespace PHARE::solver;

template<uint8_t dim>
using InitFunctionT = PHARE::initializer::InitFunction<dim>;



template<uint8_t dimension>
struct DimDict
{
};

template<>
struct DimDict<1>
{
    static constexpr uint8_t dim = 1;
    static void set(PHARE::initializer::PHAREDict& dict)
    {
        using namespace PHARE::initializer::test_fn::func_1d; // density/etc are here

        dict["ions"]["pop0"]["particle_initializer"]["density"]
            = static_cast<InitFunctionT<dim>>(density);

        dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vx);

        dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vy);

        dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vz);


        dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vthx);

        dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vthy);

        dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vthz);

        dict["ions"]["pop1"]["particle_initializer"]["density"]
            = static_cast<InitFunctionT<dim>>(density);

        dict["ions"]["pop1"]["particle_initializer"]["bulk_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vx);

        dict["ions"]["pop1"]["particle_initializer"]["bulk_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vy);

        dict["ions"]["pop1"]["particle_initializer"]["bulk_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vz);


        dict["ions"]["pop1"]["particle_initializer"]["thermal_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vthx);

        dict["ions"]["pop1"]["particle_initializer"]["thermal_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vthy);

        dict["ions"]["pop1"]["particle_initializer"]["thermal_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vthz);

        dict["electromag"]["magnetic"]["initializer"]["x_component"]
            = static_cast<InitFunctionT<dim>>(bx);
        dict["electromag"]["magnetic"]["initializer"]["y_component"]
            = static_cast<InitFunctionT<dim>>(by);
        dict["electromag"]["magnetic"]["initializer"]["z_component"]
            = static_cast<InitFunctionT<dim>>(bz);

        dict["simulation"]["algo"]["ion_updater"]["pusher"]["name"] = std::string{"modified_boris"};
    }
};




template<>
struct DimDict<2>
{
    static constexpr uint8_t dim = 2;
    static void set(PHARE::initializer::PHAREDict& dict)
    {
        using namespace PHARE::initializer::test_fn::func_2d; // density/etc are here
        dict["simulation"]["algo"]["pusher"]["name"] = std::string{"modified_boris"};

        dict["ions"]["pop0"]["particle_initializer"]["density"]
            = static_cast<InitFunctionT<dim>>(density);

        dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vx);

        dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vy);

        dict["ions"]["pop0"]["particle_initializer"]["bulk_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vz);

        dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vthx);

        dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vthy);

        dict["ions"]["pop0"]["particle_initializer"]["thermal_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vthz);

        dict["ions"]["pop1"]["particle_initializer"]["density"]
            = static_cast<InitFunctionT<dim>>(density);

        dict["ions"]["pop1"]["particle_initializer"]["bulk_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vx);

        dict["ions"]["pop1"]["particle_initializer"]["bulk_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vy);

        dict["ions"]["pop1"]["particle_initializer"]["bulk_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vz);

        dict["ions"]["pop1"]["particle_initializer"]["thermal_velocity_x"]
            = static_cast<InitFunctionT<dim>>(vthx);

        dict["ions"]["pop1"]["particle_initializer"]["thermal_velocity_y"]
            = static_cast<InitFunctionT<dim>>(vthy);

        dict["ions"]["pop1"]["particle_initializer"]["thermal_velocity_z"]
            = static_cast<InitFunctionT<dim>>(vthz);

        dict["electromag"]["magnetic"]["initializer"]["x_component"]
            = static_cast<InitFunctionT<dim>>(bx);
        dict["electromag"]["magnetic"]["initializer"]["y_component"]
            = static_cast<InitFunctionT<dim>>(by);
        dict["electromag"]["magnetic"]["initializer"]["z_component"]
            = static_cast<InitFunctionT<dim>>(bz);

        dict["simulation"]["algo"]["ion_updater"]["pusher"]["name"] = std::string{"modified_boris"};
    }
};

template<uint8_t dimension = 1>
PHARE::initializer::PHAREDict createDict()
{
    PHARE::initializer::PHAREDict dict;

    dict["simulation"]["algo"]["pusher"]["name"] = std::string{"modified_boris"};

    dict["ions"]["nbrPopulations"]                       = int{2};
    dict["ions"]["pop0"]["name"]                         = std::string{"protons"};
    dict["ions"]["pop0"]["mass"]                         = 1.;
    dict["ions"]["pop0"]["particle_initializer"]["name"] = std::string{"maxwellian"};

    dict["ions"]["pop0"]["particle_initializer"]["nbr_part_per_cell"] = int{100};
    dict["ions"]["pop0"]["particle_initializer"]["charge"]            = -1.;
    dict["ions"]["pop0"]["particle_initializer"]["basis"]             = std::string{"cartesian"};

    dict["ions"]["pop1"]["name"]                         = std::string{"alpha"};
    dict["ions"]["pop1"]["mass"]                         = 1.;
    dict["ions"]["pop1"]["particle_initializer"]["name"] = std::string{"maxwellian"};

    dict["ions"]["pop1"]["particle_initializer"]["nbr_part_per_cell"] = int{100};
    dict["ions"]["pop1"]["particle_initializer"]["charge"]            = -1.;
    dict["ions"]["pop1"]["particle_initializer"]["basis"]             = std::string{"cartesian"};

    dict["electromag"]["name"]             = std::string{"EM"};
    dict["electromag"]["electric"]["name"] = std::string{"E"};
    dict["electromag"]["magnetic"]["name"] = std::string{"B"};

    dict["electrons"]["pressure_closure"]["name"] = std::string{"isothermal"};
    dict["electrons"]["pressure_closure"]["Te"]   = 0.12;


    DimDict<dimension>::set(dict);

    return dict;
}


namespace test_1d
{
static constexpr std::size_t dim          = 1;
static constexpr std::size_t interpOrder  = 1;
static constexpr std::size_t nbRefinePart = 2;

using Simulator         = PHARE::Simulator<dim, interpOrder, nbRefinePart>;
using HybridModelT      = Simulator::HybridModel;
using MHDModelT         = Simulator::MHDModel;
using ResourcesManagerT = typename HybridModelT::resources_manager_type;
using Phare_Types       = PHARE::PHARE_Types<dim, interpOrder, nbRefinePart>;



TEST(MessengerDescriptors, areObtainedFromAModelList)
{
    auto modelList   = std::vector<std::string>{"MHDModel", "HybridModel"};
    auto descriptors = makeDescriptors(modelList);

    EXPECT_EQ(3, descriptors.size());
    EXPECT_EQ("MHDModel", descriptors[0].coarseModel);
    EXPECT_EQ("MHDModel", descriptors[0].fineModel);

    EXPECT_EQ("MHDModel", descriptors[1].coarseModel);
    EXPECT_EQ("HybridModel", descriptors[1].fineModel);

    EXPECT_EQ("HybridModel", descriptors[2].coarseModel);
    EXPECT_EQ("HybridModel", descriptors[2].fineModel);

    modelList   = std::vector<std::string>{"HybridModel"};
    descriptors = makeDescriptors(modelList);

    EXPECT_EQ(1, descriptors.size());
    EXPECT_EQ("HybridModel", descriptors[0].coarseModel);
    EXPECT_EQ("HybridModel", descriptors[0].fineModel);
}



// ----------------------------------------------------------------------------
// The tests below test that hybrid messengers (with either MHDHybrid or HybridHybrid
// strategies) can take quantities to communicate from models and solvers
// ----------------------------------------------------------------------------


class HybridMessengers : public ::testing::Test
{
    std::vector<MessengerDescriptor> descriptors{
        {"MHDModel", "MHDModel"}, {"MHDModel", "HybridModel"}, {"HybridModel", "HybridModel"}};
    Phare_Types::MessengerFactory messengerFactory{descriptors};


public:
    std::vector<std::unique_ptr<IMessenger<IPhysicalModel<SAMRAI_Types>>>> messengers;
    std::vector<std::unique_ptr<IPhysicalModel<SAMRAI_Types>>> models;

    HybridMessengers()
    {
        auto resourcesManagerHybrid = std::make_shared<ResourcesManagerT>();
        auto resourcesManagerMHD
            = std::make_shared<ResourcesManager<typename Phare_Types::GridLayout_t>>();

        auto hybridModel = std::make_unique<HybridModelT>(createDict(), resourcesManagerHybrid);
        auto mhdModel    = std::make_unique<MHDModelT>(resourcesManagerMHD);

        hybridModel->resourcesManager->registerResources(hybridModel->state.electromag);
        hybridModel->resourcesManager->registerResources(hybridModel->state.ions);

        mhdModel->resourcesManager->registerResources(mhdModel->state.B);
        mhdModel->resourcesManager->registerResources(mhdModel->state.V);

        models.push_back(std::move(mhdModel));
        models.push_back(std::move(hybridModel));


        auto mhdmhdMessenger{
            messengerFactory.create("MHDModel-MHDModel", *models[0], *models[0], 0)};
        auto mhdHybridMessenger{
            messengerFactory.create("MHDModel-HybridModel", *models[0], *models[1], 2)};
        auto hybridHybridMessenger{
            messengerFactory.create("HybridModel-HybridModel", *models[1], *models[1], 3)};

        messengers.push_back(std::move(mhdmhdMessenger));
        messengers.push_back(std::move(mhdHybridMessenger));
        messengers.push_back(std::move(hybridHybridMessenger));
    }
};



TEST_F(HybridMessengers, receiveQuantitiesFromMHDHybridModelsAndHybridSolver)
{
    auto hybridSolver = std::make_unique<SolverPPC<HybridModelT, SAMRAI_Types>>(
        createDict()["simulation"]["algo"]);

    MessengerRegistration::registerQuantities(*messengers[1], *models[0], *models[1],
                                              *hybridSolver);
}



TEST_F(HybridMessengers, receiveQuantitiesFromMHDHybridModelsAndMHDSolver)
{
    auto mhdSolver = std::make_unique<SolverMHD<MHDModelT, SAMRAI_Types>>();
    MessengerRegistration::registerQuantities(*messengers[0], *models[0], *models[0], *mhdSolver);
}



TEST_F(HybridMessengers, receiveQuantitiesFromHybridModelsOnlyAndHybridSolver)
{
    auto hybridSolver = std::make_unique<SolverPPC<HybridModelT, SAMRAI_Types>>(
        createDict()["simulation"]["algo"]);
    MessengerRegistration::registerQuantities(*messengers[2], *models[1], *models[1],
                                              *hybridSolver);
}



TEST_F(HybridMessengers, throwsIfGivenAnIncompatibleFineModel)
{
    auto hybridSolver = std::make_unique<SolverPPC<HybridModelT, SAMRAI_Types>>(
        createDict()["simulation"]["algo"]);

    auto& hybridhybridMessenger = *messengers[2];
    auto& mhdModel              = *models[0];
    auto& hybridModel           = *models[1];
    EXPECT_ANY_THROW(MessengerRegistration::registerQuantities(hybridhybridMessenger, mhdModel,
                                                               hybridModel, *hybridSolver));
}


TEST_F(HybridMessengers, throwsIfGivenAnIncompatibleCoarseModel)
{
    auto hybridSolver = std::make_unique<SolverPPC<HybridModelT, SAMRAI_Types>>(
        createDict()["simulation"]["algo"]);

    auto& hybridhybridMessenger = *messengers[2];
    auto& mhdModel              = *models[0];
    auto& hybridModel           = *models[1];
    EXPECT_ANY_THROW(MessengerRegistration::registerQuantities(hybridhybridMessenger, hybridModel,
                                                               mhdModel, *hybridSolver));
}




TEST_F(HybridMessengers, areNamedByTheirStrategyName)
{
    EXPECT_EQ(std::string{"MHDModel-MHDModel"}, messengers[0]->name());
    EXPECT_EQ(std::string{"MHDModel-HybridModel"}, messengers[1]->name());
    EXPECT_EQ(std::string{"HybridModel-HybridModel"}, messengers[2]->name());
}



// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------


} // namespace test_1d

#if 0
TEST_F(HybridHybridMessenger, initializesNewLevelDuringRegrid)
{
    auto tagStrat   = std::make_shared<TagStrategy<HybridModelT>>(hybridModel, solver, messenger);
    int const ratio = 2;
    short unsigned const dimension = 1;

    auto integratorStrat = std::make_shared<TestIntegratorStrat>();


    BasicHierarchy basicHierarchy{ratio, dimension, tagStrat.get(), integratorStrat};

    auto& integrator = basicHierarchy.integrator;

    // regrid all > 0

    double rootDt = 0.1;
    integrator->advanceHierarchy(rootDt);

    auto& hierarchy = basicHierarchy.getHierarchy();

    for (auto iLevel = 0; iLevel < hierarchy.getNumberOfLevels(); ++iLevel)
    {
        auto const& level = hierarchy.getPatchLevel(iLevel);

        std::cout << "iLevel = " << iLevel << "\n";

        for (auto& patch : *level)
        {
            auto _
                = hybridModel->resourcesManager->setOnPatch(*patch, hybridModel->state.electromag);

            auto layout = PHARE::layoutFromPatch<typename HybridModelT::gridLayout_type>(*patch);

            auto& Ex = hybridModel->state.electromag.E.getComponent(PHARE::Component::X);
            auto& Ey = hybridModel->state.electromag.E.getComponent(PHARE::Component::Y);
            auto& Ez = hybridModel->state.electromag.E.getComponent(PHARE::Component::Z);

            auto& Bx = hybridModel->state.electromag.B.getComponent(PHARE::Component::X);
            auto& By = hybridModel->state.electromag.B.getComponent(PHARE::Component::Y);
            auto& Bz = hybridModel->state.electromag.B.getComponent(PHARE::Component::Z);


            auto checkMyField = [&layout](auto const& field, auto const& func) //
            {
                auto iStart = layout.physicalStartIndex(field, PHARE::Direction::X);
                auto iEnd   = layout.physicalEndIndex(field, PHARE::Direction::X);

                for (auto ix = iStart; ix <= iEnd; ++ix)
                {
                    auto origin   = layout.origin();
                    auto x        = layout.fieldNodeCoordinates(field, origin, ix);
                    auto expected = func(x[0]);
                    EXPECT_DOUBLE_EQ(expected, field(ix));
                }
            };


            checkMyField(Ex, TagStrategy<HybridModelT>::fillEx);
            checkMyField(Ey, TagStrategy<HybridModelT>::fillEy);
            checkMyField(Ez, TagStrategy<HybridModelT>::fillEz);

            checkMyField(Bx, TagStrategy<HybridModelT>::fillBx);
            checkMyField(By, TagStrategy<HybridModelT>::fillBy);
            checkMyField(Bz, TagStrategy<HybridModelT>::fillBz);
        }
    }
}



TEST_F(HybridHybridMessenger, initializesNewFinestLevelAfterRegrid)
{
    auto tagStrat   = std::make_shared<TagStrategy<HybridModelT>>(hybridModel, solver, messenger);
    int const ratio = 2;
    short unsigned const dimension = 1;

    auto integratorStrat = std::make_shared<TestIntegratorStrat>();


    //   BasicHierarchy hierarchy{ratio, dimension, tagStrat.get(),integratorStrat};
}
#endif

template<uint8_t dimension, std::size_t nbRefinePart>
struct AfullHybridBasicHierarchy
{
    static constexpr std::size_t interpOrder = 1;

    using Simulator         = typename PHARE::Simulator<dimension, interpOrder, nbRefinePart>;
    using HybridModelT      = typename Simulator::HybridModel;
    using MHDModelT         = typename Simulator::MHDModel;
    using ResourcesManagerT = typename HybridModelT::resources_manager_type;
    using Phare_Types       = PHARE::PHARE_Types<dimension, interpOrder, nbRefinePart>;

    int const firstHybLevel{0};
    int const ratio{2};

    using HybridHybridT
        = HybridHybridMessengerStrategy<HybridModelT, typename Phare_Types::RefinementParams>;

    SAMRAI::tbox::SAMRAI_MPI mpi{MPI_COMM_WORLD};

    PHARE::initializer::PHAREDict dict{createDict<dimension>()};

    std::shared_ptr<ResourcesManagerT> resourcesManagerHybrid{
        std::make_shared<ResourcesManagerT>()};

    std::shared_ptr<HybridModelT> hybridModel{
        std::make_shared<HybridModelT>(dict, resourcesManagerHybrid)};


    std::unique_ptr<HybridMessengerStrategy<HybridModelT>> hybhybStrat{
        std::make_unique<HybridHybridT>(resourcesManagerHybrid, firstHybLevel)};

    std::shared_ptr<HybridMessenger<HybridModelT>> messenger{
        std::make_shared<HybridMessenger<HybridModelT>>(std::move(hybhybStrat))};

    std::shared_ptr<SolverPPC<HybridModelT, SAMRAI_Types>> solver{

        std::make_shared<SolverPPC<HybridModelT, SAMRAI_Types>>(
            createDict()["simulation"]["algo"])};


    std::shared_ptr<TagStrategy<HybridModelT>> tagStrat;


    std::shared_ptr<TestIntegratorStrat> integrator;

    std::shared_ptr<BasicHierarchy> basicHierarchy;

    AfullHybridBasicHierarchy()
    {
        hybridModel->resourcesManager->registerResources(hybridModel->state);

        solver->registerResources(*hybridModel);

        tagStrat   = std::make_shared<TagStrategy<HybridModelT>>(hybridModel, solver, messenger);
        integrator = std::make_shared<TestIntegratorStrat>();
        basicHierarchy
            = std::make_shared<BasicHierarchy>(ratio, dimension, tagStrat.get(), integrator);
    }

    inline void fillsRefinedLevelFieldGhosts();
};


template<uint8_t dimension, std::size_t nbRefinePart> // keeps the test "this"
void AfullHybridBasicHierarchy<dimension, nbRefinePart>::fillsRefinedLevelFieldGhosts()
{
    if (mpi.getSize() > 1)
    {
        GTEST_SKIP() << "Test Broken for // execution, SHOULD BE FIXED";
    }

    auto newTime       = 1.;
    auto& hierarchy    = basicHierarchy->getHierarchy();
    auto const& level0 = hierarchy.getPatchLevel(0);
    auto const& level1 = hierarchy.getPatchLevel(1);
    auto& rm           = hybridModel->resourcesManager;


    // this prepareStep copies the current model EM into messenger EM
    messenger->prepareStep(*hybridModel, *level0, 0);


    // here we set the level 0 at t=1, this simulates the advanceLevel
    for (auto& patch : *level0)
    {
        auto dataOnPatch = rm->setOnPatch(*patch, hybridModel->state.electromag);
        rm->setTime(hybridModel->state.electromag, *patch, newTime);
    }


    // this simulates a substep of level 1 to an intermediate time t=0.5
    for (auto& patch : *level1)
    {
        rm->setTime(hybridModel->state.electromag, *patch, 0.5);
    }


    // now we want to fill ghosts on level 1
    // this will need the space/time interpolation of level0 EM fields between
    // t=0 and t=1. The Model on level0 is at t=1 (above set time) and thanks
    // to the call to prepareStep() the messenger holds the copy of level 0 EM fields
    // at t=0. So at this point the ghosts should be filled OK at t=0.5.
    messenger->fillMagneticGhosts(hybridModel->state.electromag.B, 1, 0.5);
    messenger->fillElectricGhosts(hybridModel->state.electromag.E, 1, 0.5);



    std::size_t total_eq = 0;
    std::size_t iPatch   = 0;
    for (auto patch : *level1)
    {
        auto exOldId = hybridModel->resourcesManager->getID("HybridModel-HybridModel_EM_old_E_x");
        auto exId    = hybridModel->resourcesManager->getID("EM_E_x");

        ASSERT_TRUE(exOldId);
        ASSERT_TRUE(exId);

        EXPECT_TRUE(patch->checkAllocated(*exOldId));
        EXPECT_TRUE(patch->checkAllocated(*exId));

        auto exOldData = patch->getPatchData(*exOldId);
        auto exData    = patch->getPatchData(*exId);

        auto dataOnPatch = rm->setOnPatch(*patch, hybridModel->state.electromag);


        EXPECT_DOUBLE_EQ(0., patch->getPatchData(*exOldId)->getTime());
        EXPECT_DOUBLE_EQ(0.5, patch->getPatchData(*exId)->getTime());

        auto layout = layoutFromPatch<typename HybridModelT::gridlayout_type>(*patch);

        auto& Bx = hybridModel->state.electromag.B.getComponent(Component::X);
        auto& By = hybridModel->state.electromag.B.getComponent(Component::Y);
        auto& Bz = hybridModel->state.electromag.B.getComponent(Component::Z);



        // since we have not changed the fields on level0 between time t=0 and t=1
        // but just changed the time, the time interpolation at t=0.5 on level 1
        // should be 0.5*(FieldAtT0 + FieldAtT1) = 0.5*(2*FieldAtT0) = FieldAtT0
        // moreoever, since the level0 fields are linear function of space
        // the spatial interpolation on level 1 should be equal to the result of the function
        // that defined the field on level0.
        // As a consequence, if the space/time interpolation worked the field on level1
        // should be equal to the outcome of the function used on level0

        if constexpr (dimension == 1)
        {
            using namespace PHARE::initializer::test_fn::func_1d; // density/etc are here

            auto checkMyField = [&](auto const& field, auto const& func) //
            {
                auto check = [&](auto const& start, auto const& end) {
                    auto origin = layout.origin();

                    std::vector<double> x;
                    std::vector<std::size_t> ixes;

                    for (auto ix = start; ix < end; ++ix)
                    {
                        ixes.emplace_back(ix);
                        x.emplace_back(layout.fieldNodeCoordinates(field, origin, ix)[0]);
                    }

                    EXPECT_GT(x.size(), 0);
                    EXPECT_EQ(x.size(), end - start);
                    EXPECT_EQ(ixes.size(), end - start);

                    auto gridPtr = func(x);
                    auto& grid   = *gridPtr;

                    EXPECT_EQ(x.size(), grid.size());

                    for (std::size_t i = 0; i < x.size(); i++)
                    {
                        auto ix       = ixes[i];
                        auto expected = grid[i];
                        std::cout << iPatch << " " << ix << " " << expected << " " << field(ix)
                                  << " " << expected - field(ix) << "\n";
                        EXPECT_DOUBLE_EQ(expected, field(ix));
                        total_eq++;
                    }
                };

                auto iGhostStart = layout.ghostStartIndex(field, Direction::X);
                auto iStart      = layout.physicalStartIndex(field, Direction::X);
                auto iEnd        = layout.physicalEndIndex(field, Direction::X);
                auto iGhostEnd   = layout.ghostEndIndex(field, Direction::X);

                check(iGhostStart, iStart);
                check(iEnd, iGhostEnd);
            };

            checkMyField(Bx, bx);
            checkMyField(By, by);
            checkMyField(Bz, bz);
        }


        if constexpr (dimension == 2)
        {
            using namespace PHARE::initializer::test_fn::func_2d;        // density/etc are here
            auto checkMyField = [&](auto const& field, auto const& func) //
            {
                auto check = [&](auto startX, auto startY, auto endX, auto endY) {
                    auto origin = layout.origin();
                    std::vector<double> x, y;
                    std::vector<std::size_t> ixes, iyes;
                    for (auto ix = startX; ix < endX; ++ix)
                        for (auto iy = startY; iy < endY; ++iy)
                        {
                            ixes.emplace_back(ix);
                            iyes.emplace_back(iy);
                            auto xy = layout.fieldNodeCoordinates(field, origin, ix, iy);
                            x.emplace_back(xy[0]);
                            y.emplace_back(xy[1]);
                        }

                    auto gridPtr = func(x, y);
                    auto& grid   = *gridPtr;

                    std::size_t cell_idx = 0;
                    for (std::size_t i = 0; i < ixes.size(); i++)
                    {
                        auto expected = grid[cell_idx];
                        EXPECT_DOUBLE_EQ(expected, field(ixes[cell_idx], iyes[cell_idx]));
                        cell_idx++;
                        total_eq++;
                    }
                };

                auto [iXGhostStart, iXGhostEnd] = layout.ghostStartToEnd(field, Direction::X);
                auto [iXStart, iXEnd]           = layout.physicalStartToEnd(field, Direction::X);
                auto [iYGhostStart, iYGhostEnd] = layout.ghostStartToEnd(field, Direction::Y);
                auto [iYStart, iYEnd]           = layout.physicalStartToEnd(field, Direction::Y);

                check(iXGhostStart, iYGhostStart, iXStart, iYStart);
                check(iXEnd, iYEnd, iXGhostEnd, iYGhostEnd);
            };

            checkMyField(Bx, bx);
            checkMyField(By, by);
            checkMyField(Bz, bz);
        }

        iPatch++;
    }

    ASSERT_TRUE(total_eq > iPatch);
}

template<typename Simulator>
struct HybridBasicHierarchyTest : public ::testing::Test
{
};

using HybridBasicHierarchies
    = testing::Types<AfullHybridBasicHierarchy<1, 2>, AfullHybridBasicHierarchy<2, 4>>;

TYPED_TEST_SUITE(HybridBasicHierarchyTest, HybridBasicHierarchies);

TYPED_TEST(HybridBasicHierarchyTest, fillsRefinedLevelFieldGhosts)
{
    TypeParam{}.fillsRefinedLevelFieldGhosts();
}




#if 0
TEST_F(AfullHybridBasicHierarchy, fillsRefinedLevelGhostsAfterRegrid)
{
    auto tagStrat = std::make_shared<TagStrategy<HybridModelT>>(hybridModel, solver, messenger);

    int const ratio                = 2;
    short unsigned const dimension = 1;

    auto integratorStrat = std::make_shared<TestIntegratorStrat>();


    //   BasicHierarchy hierarchy{ratio, dimension, tagStrat.get(), integratorStrat};
}
#endif


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    PHARE::SamraiLifeCycle samsam(argc, argv);
    return RUN_ALL_TESTS();
}
