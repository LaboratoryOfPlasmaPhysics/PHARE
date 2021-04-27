
#ifndef PHARE_AMR_INCLUDE_H
#define PHARE_AMR_INCLUDE_H

#include "phare_core.h"
#include "amr/amr_constants.h"
#include "amr/types/amr_types.h"
#include "amr/wrappers/hierarchy.h"
#include "amr/wrappers/integrator.h"
#include "amr/messengers/messenger_factory.h"

namespace PHARE::amr
{
template<std::size_t dimension_, std::size_t interp_order_, std::size_t nbRefinedPart_>
struct PHARE_Types
{
    static auto constexpr dimension     = dimension_;
    static auto constexpr interp_order  = interp_order_;
    static auto constexpr nbRefinedPart = nbRefinedPart_;

    using hierarchy_t = PHARE::amr::Hierarchy;

    using Splitter = PHARE::amr::Splitter<PHARE::core::DimConst<dimension>,
                                          PHARE::core::InterpConst<interp_order>,
                                          PHARE::core::RefinedParticlesConst<nbRefinedPart>>;

    using core_types = PHARE::core::PHARE_Types<dimension, interp_order>;

    using RefinementParams
        = PHARE::amr::RefinementParams<typename core_types::ParticleArray_t, Splitter>;
};

} // namespace PHARE::amr


#endif // PHARE_AMR_INCLUDE_H
