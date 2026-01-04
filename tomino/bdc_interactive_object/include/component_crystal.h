#pragma once
#include "bdc_interactive_object/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_interactive_object
{
    constexpr size_t ComponentCrystalMaxCount = 1000;
    class BDC_INTERACTIVE_OBJECT_API ComponentCrystal
    {
    private:
        // Whether the crystal has been taken
        bool isTaken_ = false;

    public:
        ComponentCrystal();
        ~ComponentCrystal();

        struct SetupParam
        {
        };
        void Setup(SetupParam &param);

        bool IsTaken() const { return isTaken_; }
        void SetIsTaken(bool isTaken) { isTaken_ = isTaken; }

    };

    extern BDC_INTERACTIVE_OBJECT_API riaecs::ComponentRegistrar
    <ComponentCrystal, ComponentCrystalMaxCount> ComponentCrystalID;

} // namespace bdc_interactive_object