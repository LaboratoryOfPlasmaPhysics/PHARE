#ifndef HYBRID_HYBRID_STATE_H
#define HYBRID_HYBRID_STATE_H


#include "core/models/physical_state.h"
#include "initializer/data_provider.h"
#include "core/utilities/algorithm.h"
#include "core/hybrid/hybrid_quantities.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>

namespace PHARE
{
namespace core
{
    /**
     * @brief The HybridState class is a concrete implementation of a IPhysicalState.
     * It holds an Electromag, Ion and Electrons object manipulated by Hybrid concrete type of
     * ISolver
     */
    template<typename Electromag, typename Ions, typename Electrons>
    class HybridState : public IPhysicalState
    {
        using VecField = typename Electromag::vecfield_type;

    public:
        static constexpr auto dimension = Ions::dimension;

        HybridState(PHARE::initializer::PHAREDict dict)
            : electromag{dict["electromag"]}
            , ions{dict["ions"]}
            , J{"J", HybridQuantity::Vector::J}
            , electrons{dict["electrons"], ions, electromag, J}
        {
        }

        Electromag electromag;
        Ions ions;
        VecField J;
        Electrons electrons;

        std::string to_str()
        {
            std::stringstream ss;
            ss << "Hybrid State\n";
            ss << "------------------------------------\n";
            ss << core::to_str(ions);
            return ss.str();
        }


        //-------------------------------------------------------------------------
        //                  start the ResourcesUser interface
        //-------------------------------------------------------------------------

        bool isUsable() const { return electromag.isUsable() and ions.isUsable(); }



        bool isSettable() const { return electromag.isSettable() and ions.isSettable(); }


        auto getCompileTimeResourcesUserList() const
        {
            return std::forward_as_tuple(electromag, ions, electrons);
        }

        auto getCompileTimeResourcesUserList()
        {
            return std::forward_as_tuple(electromag, ions, electrons);
        }


        //-------------------------------------------------------------------------
        //                  ends the ResourcesUser interface
        //-------------------------------------------------------------------------
    };



} // namespace core
} // namespace PHARE


#endif // PHARE_HYBRID_STATE_H
