#pragma once

#include "windows_base/include/interfaces/factory.h"
#include "windows_base/include/interfaces/file.h"

#include <unordered_map>
#include <memory>

namespace wb
{
    template <typename FACTORY>
    class IFactoryRegistry
    {
    public:
        virtual ~IFactoryRegistry() = default;

        virtual void AddFactory(size_t id, std::unique_ptr<FACTORY> factory) = 0;
        virtual FACTORY &GetFactory(size_t id) = 0;

        virtual size_t GetMaxID() const = 0;
        virtual const std::vector<size_t> &GetKeys() const = 0;
    };

    using IComponentRegistry = IFactoryRegistry<IComponentFactory>;
    using ISystemRegistry = IFactoryRegistry<ISystemFactory>;
    using IAssetFactoryRegistry = IFactoryRegistry<IAssetFactory>;
    using ISceneFacadeRegistry = IFactoryRegistry<ISceneFacadeFactory>;
    using IMonitorFactoryRegistry = IFactoryRegistry<IMonitorFactory>;
    using ISharedFacadeRegistry = IFactoryRegistry<ISharedFacadeFactory>;
    
    class IAssetRegistry
    {
    public:
        virtual ~IAssetRegistry() = default;

        virtual void Add(size_t id, size_t factoryID, size_t fileLoaderID, std::string_view filePath) = 0;
        virtual const size_t &GetFactoryID(size_t id) const = 0;
        virtual const size_t &GetFileLoaderID(size_t id) const = 0;
        virtual std::string_view GetFilePath(size_t id) const = 0;

        virtual size_t GetMaxID() const = 0;
        virtual const std::vector<size_t> &GetKeys() const = 0;
    };

    class IMonitorRegistry
    {
    public:
        virtual ~IMonitorRegistry() = default;

        virtual void Add(size_t id, size_t factoryID) = 0;
        virtual const size_t &GetFactoryID(size_t id) const = 0;

        virtual size_t GetMaxID() const = 0;
        virtual const std::vector<size_t> &GetKeys() const = 0;
    };

    class IWindowRegistry
    {
    public:
        virtual ~IWindowRegistry() = default;

        virtual void AddFactories
        (
            size_t id, 
            std::unique_ptr<IWindowFacadeFactory> facadeFactory, std::unique_ptr<IWindowEventFactory> eventFactory
        )= 0;
        virtual IWindowFacadeFactory &GetFacadeFactory(size_t id) = 0;
        virtual IWindowEventFactory &GetEventFactory(size_t id) = 0;

        virtual size_t GetMaxID() const = 0;
        virtual const std::vector<size_t> &GetKeys() const = 0;
    };

    template <typename LOADER>
    class ILoaderRegistry
    {
    public:
        virtual ~ILoaderRegistry() = default;

        virtual void AddLoader(size_t id, std::unique_ptr<LOADER> loader) = 0;
        virtual LOADER &GetLoader(size_t id) = 0;

        virtual size_t GetMaxID() const = 0;
        virtual const std::vector<size_t> &GetKeys() const = 0;
    };

    using IFileLoaderRegistry = ILoaderRegistry<IFileLoader>;

    
    

} // namespace wb