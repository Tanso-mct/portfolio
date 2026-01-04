#pragma once
#include "bdc_interactive_object/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_interactive_object
{
    constexpr size_t ComponentAttackItemMaxCount = 1000;
    class BDC_INTERACTIVE_OBJECT_API ComponentAttackItem
    {
    private:

    public:
        ComponentAttackItem();
        ~ComponentAttackItem();

        struct SetupParam
        {
        };
        void Setup(SetupParam &param);

    };

    extern BDC_INTERACTIVE_OBJECT_API riaecs::ComponentRegistrar
    <ComponentAttackItem, ComponentAttackItemMaxCount> ComponentAttackItemID;

} // namespace bdc_interactive_object