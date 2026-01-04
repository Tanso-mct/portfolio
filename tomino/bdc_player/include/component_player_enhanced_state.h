#pragma once
#include "bdc_player/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_player
{
    constexpr size_t ComponentPlayerEnhancedStateMaxCount = 10;
    class BDC_PLAYER_API ComponentPlayerEnhancedState
    {
    private:

    public:
        ComponentPlayerEnhancedState();
        ~ComponentPlayerEnhancedState();

        struct SetupParam
        {
        };
        void Setup(SetupParam &param);

    };

    extern BDC_PLAYER_API riaecs::ComponentRegistrar
    <ComponentPlayerEnhancedState, ComponentPlayerEnhancedStateMaxCount> ComponentPlayerEnhancedStateID;

} // namespace bdc_player