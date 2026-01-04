#pragma once
#include "riaecs/include/dll_config.h"

#include "riaecs/include/interfaces/ecs.h"
#include "riaecs/include/interfaces/factory.h"
#include "riaecs/include/types/stl_less.h"

#include "riaecs/include/registry.h"

#include <map>
#include <shared_mutex>
#include <queue>

namespace riaecs
{
    using ComponentFactoryRegistry = Registry<IComponentFactory>;
    using ComponentMaxCountRegistry = Registry<size_t>;

    class RIAECS_API ECSWorld : public IECSWorld
    {
    private:
        mutable std::shared_mutex mutex_;

        IComponentFactoryRegistry &componentFactoryRegistry_;
        IComponentMaxCountRegistry &componentMaxCountRegistry_;
        std::unique_ptr<IPoolFactory> poolFactory_ = nullptr;
        std::unique_ptr<IAllocatorFactory> allocatorFactory_ = nullptr;
        mutable bool isReady_ = false;

        std::vector<bool> entityExistFlags_;
        std::vector<Entity> entities_;
        std::vector<Entity> freeEntities_;
        std::set<Entity> emptyEntities_;

        static size_t nextRegisterIndex_;
        std::map<size_t, Entity> registeredEntities_;

        std::vector<std::unique_ptr<IPool>> componentPools_;
        std::vector<std::unique_ptr<IAllocator>> componentAllocators_;

        std::map<Entity, std::set<size_t>> entityToComponents_;
        std::map<size_t, std::set<Entity>> componentToEntities_;
        std::map<std::pair<Entity, size_t>, std::byte*, PairLess> entityComponentToData_;

        std::vector<Entity> stagingEntities_;

    public:
        ECSWorld(IComponentFactoryRegistry &componentFactoryRegistry, IComponentMaxCountRegistry &componentMaxCountRegistry);
        virtual ~ECSWorld() override;

        ECSWorld(const ECSWorld&) = delete;
        ECSWorld& operator=(const ECSWorld&) = delete;

        static size_t CreateRegisterIndex();

        /***************************************************************************************************************
         * IECSWorld Implementation
        /**************************************************************************************************************/

        void SetComponentFactoryRegistry(IComponentFactoryRegistry &registry) override;
        void SetComponentMaxCountRegistry(IComponentMaxCountRegistry &registry) override;
        void SetPoolFactory(std::unique_ptr<IPoolFactory> poolFactory) override;
        void SetAllocatorFactory(std::unique_ptr<IAllocatorFactory> allocatorFactory) override;
        bool IsReady() const override;

        void CreateWorld() override;
        void DestroyWorld() override;

        Entity CreateEntity() override;
        void DestroyEntity(const Entity &entity) override;
        bool CheckEntityExist(const Entity &entity) const override;

        StagingEntityArea CreateStagingArea() override;
        Entity CreateEntity(StagingEntityArea &stagingArea) override;
        void CommitEntities(StagingEntityArea &stagingArea) override;

        void RegisterEntity(size_t index, const Entity &entity) override;
        Entity GetRegisteredEntity(size_t index) const override;

        void AddComponent(const Entity &entity, size_t componentID) override;
        void RemoveComponent(const Entity &entity, size_t componentID) override;
        bool HasComponent(const Entity &entity, size_t componentID) const override;
        std::byte* GetComponent(const Entity &entity, size_t componentID) override;

        ROObject<std::set<Entity>> View(size_t componentID) const override;
    };

    template <typename T>
    class ComponentFactory : public IComponentFactory
    {
    public:
        std::byte *Create(std::byte *data) const override
        {
            if (data == nullptr)
                return nullptr;

            T* component = new(data) T();
            return reinterpret_cast<std::byte*>(component);
        }

        void Destroy(std::byte *data) const override
        {
            if (data == nullptr)
                return;

            T* component = reinterpret_cast<T*>(data);
            component->~T();
        }

        size_t GetProductSize() const override
        {
            return sizeof(T);
        }
    };

    template <typename T>
    class SystemFactory : public ISystemFactory
    {
    public:
        SystemFactory() = default;
        ~SystemFactory() override = default;

        /***************************************************************************************************************
         * IFactory Implementation
        /**************************************************************************************************************/

        std::unique_ptr<ISystem> Create() const override
        {
            return std::make_unique<T>();
        }

        void Destroy(std::unique_ptr<ISystem> product) const override
        {
            product.reset();
        }

        size_t GetProductSize() const override
        {
            return sizeof(T);
        }
    };

    using SystemFactoryRegistry = Registry<ISystemFactory>;

    class RIAECS_API SystemList : public ISystemList
    {
    private:
        mutable std::shared_mutex mutex_;
        std::map<size_t, std::unique_ptr<ISystem>> systemMap_;
        std::vector<size_t> order_;

    public:
        SystemList() = default;
        virtual ~SystemList() override;

        SystemList(const SystemList&) = delete;
        SystemList& operator=(const SystemList&) = delete;

        /***************************************************************************************************************
         * ISystemList Implementation
        /**************************************************************************************************************/

        void CreateSystem(size_t systemID) override;
        void DestroySystem(size_t systemID) override;
        void DestroySystems() override;
        bool HasSystem(size_t systemID) const override;

        void SetOrder(std::vector<size_t> order) override;
        std::vector<size_t> GetOrder() const override;
        void ClearOrder() override;

        RWObject<ISystem> Get(size_t index) override;
        size_t GetCount() const override;
    };

    class RIAECS_API DefaultSystemListFactory : public ISystemListFactory
    {
    public:
        DefaultSystemListFactory() = default;
        ~DefaultSystemListFactory() override = default;

        /***************************************************************************************************************
         * IFactory Implementation
        /**************************************************************************************************************/

        virtual std::unique_ptr<ISystemList> Create() const override;
        virtual void Destroy(std::unique_ptr<ISystemList> product) const override;
        virtual size_t GetProductSize() const override;
    };

    class RIAECS_API SystemLoopCommandQueue : public ISystemLoopCommandQueue
    {
    private:
        std::queue<std::unique_ptr<ISystemLoopCommand>> commandQueue_;
        mutable std::shared_mutex mutex_;

    public:
        SystemLoopCommandQueue() = default;
        virtual ~SystemLoopCommandQueue() override = default;

        /***************************************************************************************************************
         * ISystemLoopCommandQueue Implementation
        /**************************************************************************************************************/

        void Enqueue(std::unique_ptr<ISystemLoopCommand> cmd) override;
        std::unique_ptr<ISystemLoopCommand> Dequeue() override;
        bool IsEmpty() const override;
    };

    class RIAECS_API DefaultSystemLoopCommandQueueFactory : public ISystemLoopCommandQueueFactory
    {
    public:
        DefaultSystemLoopCommandQueueFactory() = default;
        ~DefaultSystemLoopCommandQueueFactory() override = default;

        /***************************************************************************************************************
         * IFactory Implementation
        /**************************************************************************************************************/

        virtual std::unique_ptr<ISystemLoopCommandQueue> Create() const override;
        virtual void Destroy(std::unique_ptr<ISystemLoopCommandQueue> product) const override;
        virtual size_t GetProductSize() const override;
    };

    class RIAECS_API SystemLoop : public ISystemLoop
    {
    private:
        mutable std::shared_mutex mutex_;

        std::unique_ptr<ISystemListFactory> listFactory_;
        std::unique_ptr<ISystemLoopCommandQueueFactory> loopCommandQueueFactory_;
        mutable bool isReady_ = false;

        std::unique_ptr<ISystemList> systemList_;
        std::unique_ptr<ISystemLoopCommandQueue> commandQueue_;

    public:
        SystemLoop() = default;
        virtual ~SystemLoop() override;

        /***************************************************************************************************************
         * ISystemLoop Implementation
        /**************************************************************************************************************/

        void SetSystemListFactory(std::unique_ptr<ISystemListFactory> factory) override;
        void SetSystemLoopCommandQueueFactory(std::unique_ptr<ISystemLoopCommandQueueFactory> factory) override;
        bool IsReady() const override;

        void Initialize() override;
        void Run(IECSWorld &ecsWorld, IAssetContainer &assetCont) override;
    };

} // namespace riaecs