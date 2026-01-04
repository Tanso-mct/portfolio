#pragma once
#include "bdc_dot/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_dot
{
    constexpr size_t ComponentDotMaxCount = 2000;
    class BDC_DOT_API ComponentDot
    {
    private:

    public:
        ComponentDot();
        ~ComponentDot();

        struct SetupParam
        {
        };
        void Setup(SetupParam &param);

    };

    extern BDC_DOT_API riaecs::ComponentRegistrar
    <ComponentDot, ComponentDotMaxCount> ComponentDotID;

} // namespace bdc_dot