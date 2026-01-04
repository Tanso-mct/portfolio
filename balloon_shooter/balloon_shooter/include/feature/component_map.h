#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/component_map.h"

namespace balloon_shooter
{
    const size_t &MapComponentID();

    class MapComponent : public IMapComponent
    {
    public:
        MapComponent() = default;
        virtual ~MapComponent() = default;

        MapComponent(const MapComponent &) = delete;
        MapComponent &operator=(const MapComponent &) = delete;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

    const size_t &MapBottomComponentID();

    class MapBottomComponent : public IMapComponent
    {
    public:
        MapBottomComponent() = default;
        virtual ~MapBottomComponent() = default;

        MapBottomComponent(const MapBottomComponent &) = delete;
        MapBottomComponent &operator=(const MapBottomComponent &) = delete;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
    };

} // namespace balloon_shooter