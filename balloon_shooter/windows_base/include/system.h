#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/system.h"
#include "windows_base/include/interfaces/container.h"
#include "windows_base/include/interfaces/entity.h"
#include "windows_base/include/interfaces/scene.h"
#include "windows_base/include/container_storage.h"

namespace wb
{

    class WINDOWS_BASE_API SystemArgument
    {
    public:
        SystemArgument
        (
            IEntityContainer &entityContainer,
            IComponentContainer &componentContainer,
            IEntityIDView &entityIDView,
            ISystemContainer &systemContainer,
            ContainerStorage &containerStorage,
            const double &deltaTime, const size_t &belongWindowID, size_t &nextSceneID
        );
        ~SystemArgument() = default;

        IEntityContainer &entityContainer_;
        IComponentContainer &componentContainer_;
        IEntityIDView &entityIDView_;
        ISystemContainer &systemContainer_;

        ContainerStorage &containerStorage_;
        SceneState state_ = SceneState::Updating;
        size_t &nextSceneID_;
        const double &deltaTime_;
        const size_t &belongWindowID_;
    };

    class WINDOWS_BASE_API SystemsFactory : public ISystemsFactory
    {
    public:
        std::unique_ptr<ISystemContainer> Create(IAssetContainer &assetCont) const override;
    };

} // namespace wb