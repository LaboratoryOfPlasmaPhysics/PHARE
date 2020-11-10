
#ifndef PHARE_TAGGER_H
#define PHARE_TAGGER_H

#include "physical_models/physical_model.h"
#include "amr/types/amr_types.h"

#include <memory>

namespace PHARE::amr
{
class Tagger
{
protected:
    using patch_t = PHARE::amr::SAMRAI_Types::patch_t;
    using amr_t   = PHARE::amr::SAMRAI_Types;

public:
    virtual void tag(PHARE::solver::IPhysicalModel<amr_t>& model, patch_t& patch, int tag_index)
        = 0;
    virtual ~Tagger() = 0;
};

Tagger::~Tagger() {}

} // namespace PHARE::amr

#endif
