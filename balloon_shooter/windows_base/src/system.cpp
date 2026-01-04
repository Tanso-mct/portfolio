#include "windows_base/src/pch.h"
#include "windows_base/include/system.h"

#include "windows_base/include/interfaces/system.h"
#include "windows_base/include/interfaces/container.h"
#include "windows_base/include/interfaces/entity.h"

#include "windows_base/include/system_registry.h"
#include "windows_base/include/container_impl.h"

wb::SystemArgument::SystemArgument
(
    IEntityContainer &entityContainer, IComponentContainer &componentContainer, 
    IEntityIDView &entityIDView, ISystemContainer &systemContainer,
    ContainerStorage &containerStorage,
    const double &deltaTime, const size_t &belongWindowID, size_t &nextSceneID
) :
    entityContainer_(entityContainer),
    componentContainer_(componentContainer),
    entityIDView_(entityIDView),
    systemContainer_(systemContainer),
    containerStorage_(containerStorage),
    deltaTime_(deltaTime),
    belongWindowID_(belongWindowID),
    nextSceneID_(nextSceneID)
{
}

std::unique_ptr<wb::ISystemContainer> wb::SystemsFactory::Create(IAssetContainer &assetCont) const
{
    std::unique_ptr<ISystemContainer> systemContainer = std::make_unique<SystemContainer>();
    systemContainer->Create(wb::gSystemRegistry.GetMaxID() + 1);

    // Create registered systems
    for (const size_t &systemID : wb::gSystemRegistry.GetKeys())
    {
        ISystemFactory &systemFactory = wb::gSystemRegistry.GetFactory(systemID);
        std::unique_ptr<ISystem> system = systemFactory.Create(assetCont);

        // Add the system to the container
        systemContainer->Set(systemID, std::move(system));
    }

    return systemContainer;
}

