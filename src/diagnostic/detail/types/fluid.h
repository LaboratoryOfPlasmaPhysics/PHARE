#ifndef PHARE_DIAGNOSTIC_DETAIL_TYPES_FLUID_H
#define PHARE_DIAGNOSTIC_DETAIL_TYPES_FLUID_H

#include "detail/highfive.h"
#include "detail/highfive_diag_writer.h"

namespace PHARE::diagnostic::h5
{
/*
 * It is assumed that MPI processes will have at least one patch, and each patch has equal
 *  number of populations
 * It is assumed  that all processes have at least one patch on all levels
 *
 * Possible outputs
 *
 * /#t/pl#/p#/ions/density
 * /#t/pl#/p#/ions/bulkVelocity/(x,y,z)
 * /#t/pl#/p#/ions/pop_(1,2,...)/density
 * /#t/pl#/p#/ions/pop_(1,2,...)/bulkVelocity/(x,y,z)
 */
template<typename HighFiveDiagnostic>
class FluidDiagnosticWriter : public Hi5DiagnosticWriter<HighFiveDiagnostic>
{
public:
    using Hi5DiagnosticWriter<HighFiveDiagnostic>::hi5_;
    using Attributes = typename Hi5DiagnosticWriter<HighFiveDiagnostic>::Attributes;
    FluidDiagnosticWriter(HighFiveDiagnostic& hi5)
        : Hi5DiagnosticWriter<HighFiveDiagnostic>(hi5)
    {
    }
    void write(DiagnosticDAO&) override;
    void compute(DiagnosticDAO&) override {}
    void getDataSetInfo(DiagnosticDAO& diagnostic, size_t iLevel, std::string const& patchID,
                        Attributes& patchAttributes) override;
    void initDataSets(DiagnosticDAO& diagnostic,
                      std::unordered_map<size_t, std::vector<std::string>> const& patchIDs,
                      Attributes& patchAttributes) override;

private:
    size_t levels_ = 0;
};


template<typename HighFiveDiagnostic>
void FluidDiagnosticWriter<HighFiveDiagnostic>::getDataSetInfo(DiagnosticDAO& diagnostic,
                                                               size_t iLevel,
                                                               std::string const& patchID,
                                                               Attributes& patchAttributes)
{
    auto& hi5  = this->hi5_;
    auto& ions = hi5.modelView().getIons();
    std::string lvlPatchID{std::to_string(iLevel) + "_" + patchID};

    auto checkActive
        = [&diagnostic](auto& tree, auto var) { return diagnostic.subtype == tree + var; };

    for (auto& pop : ions)
    {
        std::string popId = "fluid_" + pop.name();
        std::string tree{"/ions/pop/" + pop.name() + "/"};
        auto& popAttr = patchAttributes[lvlPatchID][popId];
        if (checkActive(tree, "density"))
            popAttr["density"] = pop.density().size();
        if (checkActive(tree, "flux"))
            for (auto& [id, type] : core::Components::componentMap)
                popAttr["flux"][id] = pop.flux().getComponent(type).size();
    }

    std::string tree{"/ions/"};
    if (checkActive(tree, "density"))
        patchAttributes[lvlPatchID]["ion"]["density"] = ions.density().size();

    if (checkActive(tree, "bulkVelocity"))
        for (auto& [id, type] : core::Components::componentMap)
            patchAttributes[lvlPatchID]["ion"]["bulkVelocity"][id]
                = ions.velocity().getComponent(type).size();

    levels_ = iLevel; // will be max level with a patch for this process
}


template<typename HighFiveDiagnostic>
void FluidDiagnosticWriter<HighFiveDiagnostic>::initDataSets(
    DiagnosticDAO& diagnostic, std::unordered_map<size_t, std::vector<std::string>> const& patchIDs,
    Attributes& patchAttributes)
{
    auto& hi5  = this->hi5_;
    auto& ions = hi5.modelView().getIons();

    auto checkActive = [&](auto& tree, auto var) { return diagnostic.subtype == tree + var; };

    auto initPatch = [&](auto& lvl, auto& attr, std::string patchID = "") {
        bool null = patchID.empty();
        std::string path{hi5.getPatchPath("time", lvl, patchID) + "/ions/"};

        for (auto& pop : ions)
        {
            std::string popId{"fluid_" + pop.name()};
            std::string tree{"/ions/pop/" + pop.name() + "/"};
            std::string popPath(path + "pop/" + pop.name() + "/");
            if (checkActive(tree, "density"))
                hi5.template createDataSet<float>(
                    popPath + "density", null ? 0 : attr[popId]["density"].template to<size_t>());

            if (checkActive(tree, "flux"))
                for (auto& [id, type] : core::Components::componentMap)
                    hi5.template createDataSet<float>(
                        popPath + "flux" + "/" + id,
                        null ? 0 : attr[popId]["flux"][id].template to<size_t>());
        }

        std::string tree{"/ions/"};
        if (checkActive(tree, "density"))
            hi5.template createDataSet<float>(
                path + "density", null ? 0 : attr["ion"]["density"].template to<size_t>());

        if (checkActive(tree, "bulkVelocity"))
            for (auto& [id, type] : core::Components::componentMap)
                hi5.template createDataSet<float>(
                    path + "bulkVelocity" + "/" + id,
                    null ? 0 : attr["ion"]["bulkVelocity"][id].template to<size_t>());
    };

    Hi5DiagnosticWriter<HighFiveDiagnostic>::initDataSets_(patchIDs, patchAttributes, levels_,
                                                           initPatch);
}


template<typename HighFiveDiagnostic>
void FluidDiagnosticWriter<HighFiveDiagnostic>::write(DiagnosticDAO& diagnostic)
{
    auto& hi5  = this->hi5_;
    auto& ions = hi5.modelView().getIons();
    std::string path{hi5.patchPath() + "/"};

    auto checkActive = [&](auto& tree, auto var) { return diagnostic.subtype == tree + var; };

    for (auto& pop : hi5.modelView().getIons())
    {
        std::string tree{"/ions/pop/" + pop.name() + "/"};
        std::string popPath{path + tree};
        if (checkActive(tree, "density"))
            hi5.writeDataSet(popPath + "density", pop.density().data());
        if (checkActive(tree, "flux"))
            hi5.writeVecFieldAsDataset(popPath + "flux", pop.flux());
    }

    std::string tree{"/ions/"};
    auto& density = ions.density();
    if (checkActive(tree, "density"))
        hi5.writeDataSet(path + tree + "density", density.data());
    if (checkActive(tree, "bulkVelocity"))
        hi5.writeVecFieldAsDataset(path + tree + "bulkVelocity", ions.velocity());
}

} // namespace PHARE::diagnostic::h5

#endif /* PHARE_DIAGNOSTIC_DETAIL_TYPES_FLUID_H */
