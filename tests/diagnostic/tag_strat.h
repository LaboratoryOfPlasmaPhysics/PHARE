#ifndef PHARE_TEST_DIAGNOSTIC_TAG_STRAT
#define PHARE_TEST_DIAGNOSTIC_TAG_STRAT

template<typename Field, typename GridLayout, typename Func>
void fillField(Field& field, GridLayout& layout, Func f)
{
    auto iStart = layout.physicalStartIndex(field, Direction::X);
    auto iEnd   = layout.physicalEndIndex(field, Direction::X);

    for (auto ix = iStart; ix <= iEnd; ++ix)
    {
        auto origin = layout.origin();
        auto x      = layout.fieldNodeCoordinates(field, origin, ix);
        field(ix)   = f(x[0]);
    }
}

template<typename HybridModel>
class TagStrategy : public SAMRAI::mesh::StandardTagAndInitStrategy
{
private:
    std::shared_ptr<HybridModel> model_;
    std::shared_ptr<SolverPPC<HybridModel, SAMRAI_Types>> solver_;
    std::shared_ptr<HybridMessenger<HybridModel, IPhysicalModel<SAMRAI_Types>>> messenger_;

public:
    explicit TagStrategy(
        std::shared_ptr<HybridModel> model,
        std::shared_ptr<SolverPPC<HybridModel, SAMRAI_Types>> solver,
        std::shared_ptr<HybridMessenger<HybridModel, IPhysicalModel<SAMRAI_Types>>> messenger)
        : model_{std::move(model)}
        , solver_{std::move(solver)}
        , messenger_{std::move(messenger)}
    {
        auto infoFromFiner   = messenger_->emptyInfoFromFiner();
        auto infoFromCoarser = messenger_->emptyInfoFromCoarser();

        model_->fillMessengerInfo(infoFromFiner);
        model_->fillMessengerInfo(infoFromCoarser);
        solver_->fillMessengerInfo(infoFromFiner);

        messenger_->registerQuantities(std::move(infoFromFiner), std::move(infoFromCoarser));
    }


    void initializeLevelData(std::shared_ptr<SAMRAI::hier::PatchHierarchy> const& hierarchy,
                             int const levelNumber, double const initDataTime,
                             [[maybe_unused]] bool const canBeRefined,
                             [[maybe_unused]] bool const initialTime,
                             std::shared_ptr<SAMRAI::hier::PatchLevel> const& oldLevel
                             = std::shared_ptr<SAMRAI::hier::PatchLevel>(),
                             bool const allocateData = true) override
    {
        auto level = hierarchy->getPatchLevel(levelNumber);

        if (allocateData)
        {
            for (auto patch : *level)
            {
                model_->allocate(*patch, initDataTime);
                solver_->allocate(*model_, *patch, initDataTime);
                messenger_->allocate(*patch, initDataTime);
            }
        }

        messenger_->registerLevel(hierarchy, levelNumber);

        if (oldLevel)
        {
            // in case of a regrid we need to make a bunch of temporary regriding schedules
            // using the init algorithms and actually perform the .fillData() for all of them
            messenger_->regrid(hierarchy, levelNumber, oldLevel, *model_, initDataTime);
        }
        else // we're creating a brand new finest level in the hierarchy
        {
            if (levelNumber == 0)
            {
                model_->initialize(*level);
            }
            else
            {
                messenger_->initLevel(*model_, *level, initDataTime);
            }
        }

        if (levelNumber == 0)
        {
            messenger_->fillIonGhostParticles(model_->state.ions, *level, initDataTime);
        }
    }

    void resetHierarchyConfiguration(
        [[maybe_unused]] std::shared_ptr<SAMRAI::hier::PatchHierarchy> const& hierarchy,
        [[maybe_unused]] int const coarsestLevel, [[maybe_unused]] int const finestLevel) override
    {
    }
};

#endif /* PHARE_TEST_DIAGNOSTIC_TAG_STRAT*/
