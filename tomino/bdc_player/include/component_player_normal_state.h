#pragma once
#include "bdc_player/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_player
{
    constexpr size_t ComponentPlayerNormalStateMaxCount = 10;
    class BDC_PLAYER_API ComponentPlayerNormalState
    {
    private:
    public:
        ComponentPlayerNormalState();
        ~ComponentPlayerNormalState();
        
        struct SetupParam
        {
        };
        void Setup(SetupParam &param);

    };

    extern BDC_PLAYER_API riaecs::ComponentRegistrar
    <ComponentPlayerNormalState, ComponentPlayerNormalStateMaxCount> ComponentPlayerNormalStateID;

} // namespace bdc_player