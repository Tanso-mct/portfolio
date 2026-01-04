#pragma once

#include "riaecs/include/types/id.h"
#include "riaecs/include/types/object.h"
#include "riaecs/include/interfaces/registry.h"
#include "riaecs/include/interfaces/factory.h"
#include "riaecs/include/interfaces/memory.h"
#include "riaecs/include/interfaces/asset.h"
#include "riaecs/include/utilities.h"

#include <memory>
#include <set>
#include <vector>
#include <string_view>

namespace riaecs
{
    using Entity = ID;

    using IComponentFactory = IFactory<std::byte*, std::byte*>;
    using IComponentFactoryRegistry = IRegistry<IComponentFactory>;

    using IComponentMaxCountRegistry = IRegistry<size_t>;

    using IPoolFactory = IFactory<std::unique_ptr<IPool>, size_t>;
    using IAllocatorFactory = IFactory<std::unique_ptr<IAllocator>, IPool&, size_t>;

    using StagingEntityArea = std::vector<Entity>;
    class IECSWorld
    {
    public:
        virtual ~IECSWorld() = default;

        virtual void SetComponentFactoryRegistry(IComponentFactoryRegistry &registry) = 0;
        virtual void SetComponentMaxCountRegistry(IComponentMaxCountRegistry &registry) = 0;
        virtual void SetPoolFactory(std::unique_ptr<IPoolFactory> poolFactory) = 0;
        virtual void SetAllocatorFactory(std::unique_ptr<IAllocatorFactory> allocatorFactory) = 0;
        virtual bool IsReady() const = 0;

        virtual void CreateWorld() = 0;
        virtual void DestroyWorld() = 0;

        virtual Entity CreateEntity() = 0;
        virtual void DestroyEntity(const Entity &entity) = 0;
        virtual bool CheckEntityExist(const Entity &entity) const = 0;

        virtual StagingEntityArea CreateStagingArea() = 0;
        virtual Entity CreateEntity(StagingEntityArea &stagingArea) = 0;
        virtual void CommitEntities(StagingEntityArea &stagingArea) = 0;

        virtual void RegisterEntity(size_t index, const Entity &entity) = 0;
        virtual Entity GetRegisteredEntity(size_t index) const = 0;

        virtual void AddComponent(const Entity &entity, size_t componentID) = 0;
        virtual void RemoveComponent(const Entity &entity, size_t componentID) = 0;
        virtual bool HasComponent(const Entity &entity, size_t componentID) const = 0;
        virtual std::byte* GetComponent(const Entity &entity, size_t componentID) = 0;

        virtual ROObject<std::set<Entity>> View(size_t componentID) const = 0;
    };

    template <typename T>
    T* GetComponent(IECSWorld &ecsWorld, const Entity &entity, size_t componentID)
    {
        std::byte* componentData = ecsWorld.GetComponent(entity, componentID);
        if (componentData == nullptr)
            return nullptr;

        T* data = reinterpret_cast<T*>(componentData);
        return data;
    }

    template <typename T>
    T* GetComponentWithCheck
    (
        IECSWorld &ecsWorld, const Entity &entity, size_t componentID, std::string_view componentName,
        const std::string &file, int line, const std::string &function
    ){
        std::byte* componentData = ecsWorld.GetComponent(entity, componentID);
        if (componentData == nullptr)
        {
            NotifyError({
                "Entity does not have " + std::string(componentName) + " component.",
                "This error occurs when an Entity does not have a component that it must have.",
                "Or it can also occur when a specific Component is used and the required Component is not added in the set.",
                "Entity index: " + std::to_string(entity.GetIndex()),
                "Entity generation: " + std::to_string(entity.GetGeneration())
            }, file, line, function);
            return nullptr;
        }

        T* data = reinterpret_cast<T*>(componentData);
        return data;
    }

    class IPrefab
    {
    public:
        virtual ~IPrefab() = default;
        virtual Entity Instantiate(IECSWorld &ecsWorld, IAssetContainer &assetCont) = 0;
    };

    class ISystemLoopCommandQueue;

    class ISystem
    {
    public:
        virtual ~ISystem() = default;

        // If returns true, the system loop will continue to run
        // If returns false, the system loop will stop
        virtual bool Update
        (
            IECSWorld &ecsWorld, IAssetContainer &assetCont, 
            ISystemLoopCommandQueue &systemLoopCmdQueue
        ) = 0;
    };
    using ISystemFactory = IFactory<std::unique_ptr<ISystem>>;
    using ISystemFactoryRegistry = IRegistry<ISystemFactory>;

    class ISystemList
    {
    public:
        virtual ~ISystemList() = default;

        virtual void CreateSystem(size_t systemID) = 0;
        virtual void DestroySystem(size_t systemID) = 0;
        virtual void DestroySystems() = 0;
        virtual bool HasSystem(size_t systemID) const = 0;

        virtual void SetOrder(std::vector<size_t> order) = 0;
        virtual std::vector<size_t> GetOrder() const = 0;
        virtual void ClearOrder() = 0;

        virtual RWObject<ISystem> Get(size_t index) = 0;
        virtual size_t GetCount() const = 0;
    };
    using ISystemListFactory = IFactory<std::unique_ptr<ISystemList>>;

    class ISystemLoopCommand
    {
    public:
        virtual ~ISystemLoopCommand() = default;
        virtual void Execute(ISystemList &systemList, IECSWorld &ecsWorld, IAssetContainer &assetCont) const = 0;
        virtual std::unique_ptr<riaecs::ISystemLoopCommand> Clone() const = 0;
    };

    class ISystemLoopCommandQueue
    {
    public:
        virtual ~ISystemLoopCommandQueue() = default;

        virtual void Enqueue(std::unique_ptr<ISystemLoopCommand> cmd) = 0;
        virtual std::unique_ptr<ISystemLoopCommand> Dequeue() = 0;
        virtual bool IsEmpty() const = 0;
    };
    using ISystemLoopCommandQueueFactory = IFactory<std::unique_ptr<ISystemLoopCommandQueue>>;

    class ISystemLoop
    {
    public:
        virtual ~ISystemLoop() = default;

        virtual void SetSystemListFactory(std::unique_ptr<ISystemListFactory> factory) = 0;
        virtual void SetSystemLoopCommandQueueFactory(std::unique_ptr<ISystemLoopCommandQueueFactory> factory) = 0;
        virtual bool IsReady() const = 0;

        virtual void Initialize() = 0;
        virtual void Run(IECSWorld &ecsWorld, IAssetContainer &assetCont) = 0;
    };

} // namespace riaecs