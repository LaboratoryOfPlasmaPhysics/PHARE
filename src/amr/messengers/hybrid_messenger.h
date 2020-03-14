

#ifndef PHARE_HYBRID_MESSENGER_H
#define PHARE_HYBRID_MESSENGER_H




#include "core/hybrid/hybrid_quantities.h"
#include "amr/messengers/hybrid_messenger_strategy.h"
#include "amr/messengers/messenger.h"
#include "amr/messengers/messenger_info.h"
#include "amr/messengers/mhd_messenger.h"




namespace PHARE
{
namespace amr
{
    enum class HybridMessengerStrategyType { HybridHybrid, MHDHybrid };



    /** @brief HybridMessenger is a concrete implementation of the IMessenger interface which
     * represents all data messengers towards a Hybrid level.
     *
     * Most of the operations needed to communicate data cannot however be defined here since it is
     * not known whether the origin level is Hybrid or not. The class therefore relies on a pointer
     * to a strategy that performs the operations.
     *
     * Possible HybridMessengerStrategy are:
     *
     * - HybridHybridMessengerStrategy
     * - MHDHybridMessengerStrategy
     *
     *
     * In addition to implement the interface of a IMessenger to be used by the
     * MultiPhysicsIntegrator, HybridMessenger also provides Hybrid ISolver objects with an
     * interface specific to Hybrid systems. Those methods are:
     *
     *
     * Ghost filling methods tuned for Hybrid quantities:
     *
     * - fillMagneticGhosts()
     * - fillElectricGhosts()
     * - fillIonGhostParticles()
     * - fillIonMomentGhosts()
     *
     * Synchronization methods of Hybrid quantities:
     *
     * - syncMagnetic()
     * - syncElectric()
     * - syncIonMOments()
     *
     */
    template<typename HybridModel, typename IPhysicalModel>
    class HybridMessenger : public IMessenger<IPhysicalModel>
    {
    private:
        using IonsT     = decltype(std::declval<HybridModel>().state.ions);
        using VecFieldT = decltype(std::declval<HybridModel>().state.electromag.E);

        using stratT = HybridMessengerStrategy<HybridModel, IPhysicalModel>;

    public:
        explicit HybridMessenger(std::unique_ptr<stratT> strat)
            : strat_{std::move(strat)}
        {
        }


        /* -------------------------------------------------------------------------
                                Start IMessenger interface
           -------------------------------------------------------------------------*/



        /**
         * @brief see IMessenger::allocate. Allocate calls the abstract HybridMessengerStrategy to
         * perform the allocation of its internal resources
         */
        virtual void allocate(SAMRAI::hier::Patch& patch, double const allocateTime) const override
        {
            strat_->allocate(patch, allocateTime);
        }



        /**
         * @brief see IMessenger::registerQuantities. Prepares a HybridMessenger to communicates
         * variables given in the two information structures. Since the HybridMessenger does not
         * know which concrete strategy it is using, it cannot directly use these informations. It
         * thus passes them to its strategy.
         *
         * @param fromCoarserInfo see IMessenger
         * @param fromFinerInfo see IMessenger
         */
        virtual void registerQuantities(std::unique_ptr<IMessengerInfo> fromCoarserInfo,
                                        std::unique_ptr<IMessengerInfo> fromFinerInfo) override
        {
            strat_->registerQuantities(std::move(fromCoarserInfo), std::move(fromFinerInfo));
        }




        /**
         * @brief see IMessenger::registerLevel
         */
        virtual void registerLevel(std::shared_ptr<SAMRAI::hier::PatchHierarchy> const& hierarchy,
                                   int const levelNumber) override
        {
            strat_->registerLevel(hierarchy, levelNumber);
        }



        /**
         * @brief see IMessenger::registerLevel
         */
        virtual void regrid(std::shared_ptr<SAMRAI::hier::PatchHierarchy> const& hierarchy,
                            const int levelNumber,
                            std::shared_ptr<SAMRAI::hier::PatchLevel> const& oldLevel,
                            IPhysicalModel& model, double const initDataTime) final
        {
            strat_->regrid(hierarchy, levelNumber, oldLevel, model, initDataTime);
        }




        /**
         * @brief see IMessenger::initLevel
         * @param levelNumber
         * @param initDataTime
         */
        virtual void initLevel(IPhysicalModel& model, SAMRAI::hier::PatchLevel& level,
                               double const initDataTime) override
        {
            strat_->initLevel(model, level, initDataTime);
        }


        /**
         * @brief see IMessenger::firstStep
         * @param model
         */
        virtual void firstStep(IPhysicalModel& model, SAMRAI::hier::PatchLevel& level,
                               const std::shared_ptr<SAMRAI::hier::PatchHierarchy>& hierarchy,
                               double time) final
        {
            strat_->firstStep(model, level, hierarchy, time);
        }



        /**
         * @brief see IMessenger::lastStep
         * @param model
         */
        virtual void lastStep(IPhysicalModel& model, SAMRAI::hier::PatchLevel& level) final
        {
            strat_->lastStep(model, level);
        }



        /**
         * @brief prepareStep see IMessenger::prepareStep
         */
        virtual void prepareStep(IPhysicalModel& model, SAMRAI::hier::PatchLevel& level) final
        {
            strat_->prepareStep(model, level);
        }



        virtual void fillRootGhosts(IPhysicalModel& model, SAMRAI::hier::PatchLevel& level,
                                    double const initDataTime) final
        {
            strat_->fillRootGhosts(model, level, initDataTime);
        }



        virtual void synchronize(SAMRAI::hier::PatchLevel& level) final
        {
            strat_->synchronize(level);
        }



        /**
         * @brief IMessenger::fineModelName
         * @return
         */
        virtual std::string fineModelName() const override { return strat_->fineModelName(); }




        /**
         * @brief see IMessenger::coarseModelName
         * @return
         */
        virtual std::string coarseModelName() const override { return strat_->coarseModelName(); }




        /**
         * @brief see IMessenger::emptyInfoFromCoarser
         */
        virtual std::unique_ptr<IMessengerInfo> emptyInfoFromCoarser() override
        {
            return strat_->emptyInfoFromCoarser();
        }



        /**
         * @brief see IMessenger::emptyInfoFromFiner
         */
        virtual std::unique_ptr<IMessengerInfo> emptyInfoFromFiner() override
        {
            return strat_->emptyInfoFromFiner();
        }



        /**
         * @brief returns the name of the concrete IMessenger, which in the case of a
         * HybridMessenger is just the name of its strategy.
         */
        virtual std::string name() override
        {
            if (strat_ != nullptr)
            {
                return strat_->name();
            }
            else
            {
                throw std::runtime_error("invalid strat");
            }
        }

        /* -------------------------------------------------------------------------
                                End IMessenger interface

                            Start HybridMessenger Interface
           -------------------------------------------------------------------------*/




        /**
         * @brief fillMagneticGhosts is called by a ISolver solving hybrid equations to fill
         * the ghost nodes of the magnetic field
         * @param B is the magnetic field for which ghost nodes will be filled
         * @param levelNumber
         * @param fillTime
         */
        void fillMagneticGhosts(VecFieldT& B, int const levelNumber, double const fillTime)
        {
            strat_->fillMagneticGhosts(B, levelNumber, fillTime);
        }


        /**
         * @brief fillElectricGhosts is called by a ISolver solving a hybrid equatons to fill
         * the ghost nodes of the electric field
         * @param E is the electric field for which ghost nodes will be filled
         * @param levelNumber
         * @param fillTime
         */
        void fillElectricGhosts(VecFieldT& E, int const levelNumber, double const fillTime)
        {
            strat_->fillElectricGhosts(E, levelNumber, fillTime);
        }



        /**
         * @brief fillCurrentGhosts is called by a ISolver solving a hybrid equatons to fill
         * the ghost nodes of the electric current density field
         * @param J is the electric current densityfor which ghost nodes will be filled
         * @param levelNumber
         * @param fillTime
         */
        void fillCurrentGhosts(VecFieldT& J, int const levelNumber, double const fillTime)
        {
            strat_->fillCurrentGhosts(J, levelNumber, fillTime);
        }




        /**
         * @brief fillIonGhostParticles is called by a ISolver solving hybrid equations to fill the
         * ghosts particles
         * @param ions for which ghost particles will be filled
         * @param levelNumber
         * @param fillTime
         */
        void fillIonGhostParticles(IonsT& ions, SAMRAI::hier::PatchLevel& level,
                                   double const fillTime)
        {
            strat_->fillIonGhostParticles(ions, level, fillTime);
        }



        /**
         * @brief fillIonMomentGhosts is called by a ISolver solving hybrid equations to fill the
         * ion moments
         * @param ions
         * @param levelNumber
         * @param fillTime
         */
        void fillIonMomentGhosts(IonsT& ions, SAMRAI::hier::PatchLevel& level,
                                 double const currentTime, double const fillTime)
        {
            strat_->fillIonMomentGhosts(ions, level, currentTime, fillTime);
        }



        // synchronization/coarsening methods

        void syncMagnetic(VecFieldT& B) { strat_->syncMagnetic(B); }
        void syncElectric(VecFieldT& E) { strat_->syncElectric(E); }
        void syncIonMoments(IonsT& ions) { strat_->syncIonMoments(ions); }



        /* -------------------------------------------------------------------------
                            End HybridMessenger Interface
           -------------------------------------------------------------------------*/




        virtual ~HybridMessenger() = default;

    private:
        const std::unique_ptr<stratT> strat_;
    };


} // namespace amr

} // namespace PHARE
#endif
