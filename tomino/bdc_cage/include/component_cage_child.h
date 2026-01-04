#pragma once
#include "bdc_cage/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_cage
{
    constexpr size_t ComponentCageChildMaxCount = 20;
    class BDC_CAGE_API ComponentCageChild
    {
    private:

    public:
        ComponentCageChild();
        ~ComponentCageChild();

        struct SetupParam
        {
        };
        void Setup(SetupParam &param);

    };

    extern BDC_CAGE_API riaecs::ComponentRegistrar
    <ComponentCageChild, ComponentCageChildMaxCount> ComponentCageChildID;

} // namespace bdc_cage