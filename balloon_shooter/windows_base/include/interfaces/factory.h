#pragma once

#include "windows_base/include/interfaces/container.h"

#include <memory>

namespace wb
{
    template <typename PRODUCT, typename... ARGS>
    class IFactory
    {
    public:
        virtual ~IFactory() = default;
        virtual PRODUCT Create(ARGS...) const = 0;
    };

    class IComponent;
    using IComponentFactory = IFactory<std::unique_ptr<IComponent>>;

    using ISystemFactory = IFactory<std::unique_ptr<ISystem>, IAssetContainer&>;
    using ISystemsFactory = IFactory<std::unique_ptr<ISystemContainer>, IAssetContainer&>;

    class IEntityIDView;
    using IEntitiesFactory = IFactory<void, IAssetContainer&, IEntityContainer&, IComponentContainer&, IEntityIDView&>;

    class IEntityIDView;
    using IEntityIDViewFactory = IFactory<std::unique_ptr<IEntityIDView>>;

    class IFileData;
    class IAssetFactory : public IFactory<std::unique_ptr<IAsset>, IFileData&>
    {
    public:
        virtual ~IAssetFactory() = default;
        virtual void CreateAfter() {} // This method can be overridden to perform actions after asset creation
    };

    class ISceneFacade;
    using ISceneFacadeFactory = IFactory<std::unique_ptr<ISceneFacade>>;

    class IMonitor;
    using IMonitorFactory = IFactory<std::unique_ptr<IMonitor>>;

    class IWindowFacade;
    using IWindowFacadeFactory = IFactory<std::unique_ptr<IWindowFacade>>;

    class IWindowEvent;
    using IWindowEventFactory = IFactory<std::unique_ptr<IWindowEvent>>;

    class ISharedFacade;
    using ISharedFacadeFactory = IFactory<std::unique_ptr<ISharedFacade>>;
}