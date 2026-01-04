#include "riaecs/src/pch.h"
#include "riaecs/include/ecs.h"

#include "riaecs/include/utilities.h"
#include "riaecs/include/global_registry.h"

size_t riaecs::ECSWorld::nextRegisterIndex_ = 0;

riaecs::ECSWorld::ECSWorld(IComponentFactoryRegistry &componentFactoryRegistry, IComponentMaxCountRegistry &componentMaxCountRegistry)
: componentFactoryRegistry_(componentFactoryRegistry), componentMaxCountRegistry_(componentMaxCountRegistry)
{
}

riaecs::ECSWorld::~ECSWorld()
{
    DestroyWorld();
}

void riaecs::ECSWorld::SetComponentFactoryRegistry(IComponentFactoryRegistry &registry)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    componentFactoryRegistry_ = registry;
}

void riaecs::ECSWorld::SetComponentMaxCountRegistry(IComponentMaxCountRegistry &registry)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    componentMaxCountRegistry_ = registry;
}

void riaecs::ECSWorld::SetPoolFactory(std::unique_ptr<IPoolFactory> poolFactory)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    poolFactory_ = std::move(poolFactory);
}

void riaecs::ECSWorld::SetAllocatorFactory(std::unique_ptr<IAllocatorFactory> allocatorFactory)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    allocatorFactory_ = std::move(allocatorFactory);
}

bool riaecs::ECSWorld::IsReady() const
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!poolFactory_)
    {
        riaecs::NotifyError({"PoolFactory is not set"}, RIAECS_LOG_LOC);
        isReady_ = false;
        return isReady_;
    }

    if (!allocatorFactory_)
    {
        riaecs::NotifyError({"AllocatorFactory is not set"}, RIAECS_LOG_LOC);
        isReady_ = false;
        return isReady_;
    }

    isReady_ = true;
    return isReady_;
}

void riaecs::ECSWorld::CreateWorld()
{
    if (!IsReady())
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    // Initialize component pools and allocators
    size_t componentCount = componentFactoryRegistry_.GetCount();
    componentPools_.resize(componentCount);
    componentAllocators_.resize(componentCount);

    for (size_t i = 0; i < componentCount; ++i)
    {
        riaecs::ROObject<IComponentFactory> factory = componentFactoryRegistry_.Get(i);
        riaecs::ROObject<size_t> maxCount = componentMaxCountRegistry_.Get(i);

        size_t blockSize = std::max(factory().GetProductSize(), riaecs::MAX_FREE_BLOCK_SIZE);
        componentPools_[i] = poolFactory_->Create(blockSize * maxCount());
        componentAllocators_[i] = allocatorFactory_->Create(*componentPools_[i], blockSize);
    }
}

void riaecs::ECSWorld::DestroyWorld()
{
    // Destroy all entities
    for (size_t index = 0; index < entityExistFlags_.size(); ++index)
        if (entityExistFlags_[index])
            DestroyEntity(entities_[index]);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    // Clear all component data
    entityComponentToData_.clear();
    entityToComponents_.clear();
    componentToEntities_.clear();

    // Destroy pools and allocators
    for (size_t i = 0; i < componentPools_.size(); ++i)
        poolFactory_->Destroy(std::move(componentPools_[i]));
    componentPools_.clear();

    for (size_t i = 0; i < componentAllocators_.size(); ++i)
        allocatorFactory_->Destroy(std::move(componentAllocators_[i]));
    componentAllocators_.clear();

    // Reset entity management
    entityExistFlags_.clear();
    freeEntities_.clear();

    // Reset ready state
    isReady_ = false;
}

riaecs::Entity riaecs::ECSWorld::CreateEntity()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (!freeEntities_.empty())
    {
        Entity entity = freeEntities_.back();
        freeEntities_.pop_back();

        entityExistFlags_[entity.GetIndex()] = true;
        entities_[entity.GetIndex()] = Entity(entity.GetIndex(), entity.GetGeneration() + 1);

        return entities_[entity.GetIndex()];
    }
    else
    {
        size_t index = entityExistFlags_.size();
        entityExistFlags_.push_back(true);
        entities_.push_back(Entity(index, riaecs::ID_DEFAULT_GENERATION));

        return entities_.back();
    }
}

void riaecs::ECSWorld::DestroyEntity(const Entity &entity)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (entity.GetIndex() >= entityExistFlags_.size())
        riaecs::NotifyError({"Entity index out of range"}, RIAECS_LOG_LOC);

    if (entities_[entity.GetIndex()].GetGeneration() != entity.GetGeneration())
        riaecs::NotifyError({"Entity generation mismatch"}, RIAECS_LOG_LOC);

    if (!entityExistFlags_[entity.GetIndex()])
        return; // Already destroyed this entity
    
    // Remove all components associated with the entity
    auto it = entityToComponents_.find(entity);
    if (it != entityToComponents_.end())
    {
        for (size_t componentID : it->second)
        {
            // Remove the component from the entity
            componentToEntities_[componentID].erase(entity);

            // Get the component factory for the component ID
            riaecs::ROObject<riaecs::IComponentFactory> factory = componentFactoryRegistry_.Get(componentID);

            // Free the component data which was allocated for this entity
            std::byte *componentData = entityComponentToData_[{entity, componentID}];
            factory().Destroy(componentData);
            componentAllocators_[componentID]->Free(componentData, *componentPools_[componentID]);
            entityComponentToData_.erase({entity, componentID});
        }

        // Remove the entity from the entityToComponents map
        entityToComponents_.erase(it);
    }

    // Store the entity in freeEntities for reuse
    freeEntities_.push_back(entity);

    // Update the entityExistFlags to mark it as not existing
    entityExistFlags_[entity.GetIndex()] = false;
}

bool riaecs::ECSWorld::CheckEntityExist(const Entity &entity) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (entity.GetIndex() >= entityExistFlags_.size())
        return false;

    return entityExistFlags_[entity.GetIndex()] && entities_[entity.GetIndex()].GetGeneration() == entity.GetGeneration();
}

riaecs::StagingEntityArea riaecs::ECSWorld::CreateStagingArea()
{
    return {};
}

riaecs::Entity riaecs::ECSWorld::CreateEntity(StagingEntityArea &stagingArea)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    // if (!freeEntities_.empty())
    // {
    //     Entity entity = freeEntities_.back();
    //     freeEntities_.pop_back();

    //     entityExistFlags_[entity.GetIndex()] = true;
    //     entities_[entity.GetIndex()] = Entity(entity.GetIndex(), entity.GetGeneration() + 1);

    //     stagingEntities_.push_back(entities_[entity.GetIndex()]);
    //     stagingArea.emplace_back(entities_[entity.GetIndex()]);

    //     return entities_[entity.GetIndex()];
    // }
    // else
    // {
        size_t index = entityExistFlags_.size();
        entityExistFlags_.push_back(true);
        entities_.push_back(Entity(index, riaecs::ID_DEFAULT_GENERATION));

        stagingEntities_.push_back(entities_.back());
        stagingArea.emplace_back(entities_.back());

        return entities_.back();
    // }
}

void riaecs::ECSWorld::CommitEntities(StagingEntityArea &stagingArea)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    for (const riaecs::Entity &entity : stagingArea)
    {
        for (size_t componentID : entityToComponents_[entity])
            componentToEntities_[componentID].insert(entity);

        auto it = std::find(stagingEntities_.begin(), stagingEntities_.end(), entity);
        if (it == stagingEntities_.end())
            riaecs::NotifyError({"Entity is not a staging entity"}, RIAECS_LOG_LOC);

        stagingEntities_.erase(it);
    }

    stagingArea.clear();
}

size_t riaecs::ECSWorld::CreateRegisterIndex()
{
    return nextRegisterIndex_++;
}

void riaecs::ECSWorld::RegisterEntity(size_t index, const Entity &entity)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (registeredEntities_.find(index) != registeredEntities_.end())
        riaecs::NotifyError({"Entity already registered at index: " + std::to_string(index)}, RIAECS_LOG_LOC);

    registeredEntities_[index] = entity;
}

riaecs::Entity riaecs::ECSWorld::GetRegisteredEntity(size_t index) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    auto it = registeredEntities_.find(index);
    if (it == registeredEntities_.end())
        riaecs::NotifyError({"No entity registered at index: " + std::to_string(index)}, RIAECS_LOG_LOC);

    return it->second;
}

void riaecs::ECSWorld::AddComponent(const Entity &entity, size_t componentID)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (entity.GetIndex() >= entityExistFlags_.size())
        riaecs::NotifyError({"Entity index out of range"}, RIAECS_LOG_LOC);

    if (!entityExistFlags_[entity.GetIndex()])
        riaecs::NotifyError({"Entity does not exist"}, RIAECS_LOG_LOC);

    if (entities_[entity.GetIndex()].GetGeneration() != entity.GetGeneration())
        riaecs::NotifyError({"Entity generation mismatch"}, RIAECS_LOG_LOC);

    if (componentID >= componentPools_.size())
        riaecs::NotifyError({"Component ID out of range"}, RIAECS_LOG_LOC);

    if (componentPools_[componentID] == nullptr || componentAllocators_[componentID] == nullptr)
        riaecs::NotifyError({"Component pool or allocator not initialized for component ID"}, RIAECS_LOG_LOC);

    if (entityToComponents_[entity].find(componentID) != entityToComponents_[entity].end())
        riaecs::NotifyError({"Entity already has this component"}, RIAECS_LOG_LOC);

    // Get the component factory for the component ID
    riaecs::ROObject<riaecs::IComponentFactory> factory = componentFactoryRegistry_.Get(componentID);

    // Allocate memory for the component using the allocator
    size_t blockSize = std::max(factory().GetProductSize(), riaecs::MAX_FREE_BLOCK_SIZE);
    std::byte *componentPtr = componentAllocators_[componentID]->Malloc(blockSize, *componentPools_[componentID]);

    if (!componentPtr)
        riaecs::NotifyError({"Failed to allocate memory for component"}, RIAECS_LOG_LOC);

    // Initialize the component using the factory
    componentPtr = factory().Create(componentPtr);

    // Store to the maps
    entityToComponents_[entity].insert(componentID);
    entityComponentToData_[{entity, componentID}] = componentPtr;

    // If the entity is not a staging entity, add it to the componentToEntities_ map immediately
    if (stagingEntities_.end() == std::find(stagingEntities_.begin(), stagingEntities_.end(), entity))
        componentToEntities_[componentID].insert(entity);
}

void riaecs::ECSWorld::RemoveComponent(const Entity &entity, size_t componentID)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (entity.GetIndex() >= entityExistFlags_.size())
        riaecs::NotifyError({"Entity index out of range"}, RIAECS_LOG_LOC);

    if (!entityExistFlags_[entity.GetIndex()])
        riaecs::NotifyError({"Entity does not exist"}, RIAECS_LOG_LOC);

    if (entities_[entity.GetIndex()].GetGeneration() != entity.GetGeneration())
        riaecs::NotifyError({"Entity generation mismatch"}, RIAECS_LOG_LOC);

    if (componentID >= componentPools_.size())
        riaecs::NotifyError({"Component ID out of range"}, RIAECS_LOG_LOC);

    if (stagingEntities_.end() != std::find(stagingEntities_.begin(), stagingEntities_.end(), entity))
        riaecs::NotifyError({"Cannot remove component from a staging entity"}, RIAECS_LOG_LOC);

    // Check if the entity has the component
    auto it = entityToComponents_.find(entity);
    if (it != entityToComponents_.end() && it->second.find(componentID) != it->second.end())
    {
        // Remove the component from the entity
        it->second.erase(componentID);
        componentToEntities_[componentID].erase(entity);

        // Get the component factory for the component ID
        riaecs::ROObject<riaecs::IComponentFactory> factory = componentFactoryRegistry_.Get(componentID);

        // Free the component data which was allocated for this entity
        std::byte *componentData = entityComponentToData_[{entity, componentID}];
        factory().Destroy(componentData);
        componentAllocators_[componentID]->Free(componentData, *componentPools_[componentID]);
        entityComponentToData_.erase({entity, componentID});
    }
}

bool riaecs::ECSWorld::HasComponent(const Entity &entity, size_t componentID) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (entity.GetIndex() >= entityExistFlags_.size())
        riaecs::NotifyError({"Entity index out of range"}, RIAECS_LOG_LOC);

    if (!entityExistFlags_[entity.GetIndex()])
        riaecs::NotifyError({"Entity does not exist"}, RIAECS_LOG_LOC);

    if (entities_[entity.GetIndex()].GetGeneration() != entity.GetGeneration())
        riaecs::NotifyError({"Entity generation mismatch"}, RIAECS_LOG_LOC);

    if (componentID >= componentPools_.size())
        riaecs::NotifyError({"Component ID out of range"}, RIAECS_LOG_LOC);

    auto it = entityToComponents_.find(entity);
    return it != entityToComponents_.end() && it->second.find(componentID) != it->second.end();
}

std::byte* riaecs::ECSWorld::GetComponent(const Entity &entity, size_t componentID)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (entity.GetIndex() >= entityExistFlags_.size())
        riaecs::NotifyError({"Entity index out of range"}, RIAECS_LOG_LOC);

    if (!entityExistFlags_[entity.GetIndex()])
        riaecs::NotifyError({"Entity does not exist"}, RIAECS_LOG_LOC);

    if (entities_[entity.GetIndex()].GetGeneration() != entity.GetGeneration())
        riaecs::NotifyError({"Entity generation mismatch"}, RIAECS_LOG_LOC);

    if (componentID >= componentPools_.size())
        riaecs::NotifyError({"Component ID out of range"}, RIAECS_LOG_LOC);

    auto it = entityComponentToData_.find({entity, componentID});
    if (it != entityComponentToData_.end())
        return it->second;
    
    return nullptr;
}

riaecs::ROObject<std::set<riaecs::Entity>> riaecs::ECSWorld::View(size_t componentID) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);

    if (!isReady_)
        riaecs::NotifyError({"ECSWorld is not ready"}, RIAECS_LOG_LOC);

    if (componentID >= componentPools_.size())
        riaecs::NotifyError({"Component ID out of range"}, RIAECS_LOG_LOC);

    auto it = componentToEntities_.find(componentID);
    if (it != componentToEntities_.end())
        return riaecs::ROObject<std::set<riaecs::Entity>>(std::move(lock), it->second);

    return riaecs::ROObject<std::set<riaecs::Entity>>(std::move(lock), emptyEntities_);
}

riaecs::SystemList::~SystemList()
{
    DestroySystems();
}

void riaecs::SystemList::CreateSystem(size_t systemID)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (systemMap_.find(systemID) != systemMap_.end())
        return; // System already exists

    riaecs::ROObject<riaecs::ISystemFactory> factory = riaecs::gSystemFactoryRegistry->Get(systemID);
    std::unique_ptr<riaecs::ISystem> system = factory().Create();

    if (!system)
        riaecs::NotifyError({"Failed to create system with ID: " + std::to_string(systemID)}, RIAECS_LOG_LOC);

    systemMap_[systemID] = std::move(system);
}

void riaecs::SystemList::DestroySystem(size_t systemID)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = systemMap_.find(systemID);
    if (it == systemMap_.end())
        return; // System does not exist

    riaecs::ROObject<riaecs::ISystemFactory> factory = riaecs::gSystemFactoryRegistry->Get(systemID);
    factory().Destroy(std::move(it->second));

    systemMap_.erase(it);
}

void riaecs::SystemList::DestroySystems()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    for (auto &pair : systemMap_)
    {
        riaecs::ROObject<riaecs::ISystemFactory> factory = riaecs::gSystemFactoryRegistry->Get(pair.first);
        factory().Destroy(std::move(pair.second));
    }

    systemMap_.clear();
    order_.clear();
}

bool riaecs::SystemList::HasSystem(size_t systemID) const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return systemMap_.find(systemID) != systemMap_.end();
}

void riaecs::SystemList::SetOrder(std::vector<size_t> order)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    order_ = std::move(order);
}

std::vector<size_t> riaecs::SystemList::GetOrder() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return order_;
}

void riaecs::SystemList::ClearOrder()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    order_.clear();
}

riaecs::RWObject<riaecs::ISystem> riaecs::SystemList::Get(size_t index)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (index >= order_.size())
        riaecs::NotifyError({"Index out of range: " + std::to_string(index)}, RIAECS_LOG_LOC);

    size_t systemID = order_[index];
    if (systemMap_.find(systemID) == systemMap_.end())
        riaecs::NotifyError({"System with ID " + std::to_string(systemID) + " does not exist"}, RIAECS_LOG_LOC);

    return riaecs::RWObject<riaecs::ISystem>(std::move(lock), *systemMap_[systemID]);
}

size_t riaecs::SystemList::GetCount() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return order_.size();
}

std::unique_ptr<riaecs::ISystemList> riaecs::DefaultSystemListFactory::Create() const
{
    return std::make_unique<riaecs::SystemList>();
}

void riaecs::DefaultSystemListFactory::Destroy(std::unique_ptr<ISystemList> product) const
{
    product.reset();
}

size_t riaecs::DefaultSystemListFactory::GetProductSize() const
{
    return sizeof(SystemList);
}

void riaecs::SystemLoopCommandQueue::Enqueue(std::unique_ptr<ISystemLoopCommand> cmd)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    commandQueue_.emplace(std::move(cmd));
}

std::unique_ptr<riaecs::ISystemLoopCommand> riaecs::SystemLoopCommandQueue::Dequeue()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    if (commandQueue_.empty())
        return nullptr;

    std::unique_ptr<ISystemLoopCommand> cmd = std::move(commandQueue_.front());
    commandQueue_.pop();
    return cmd;
}

bool riaecs::SystemLoopCommandQueue::IsEmpty() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return commandQueue_.empty();
}

std::unique_ptr<riaecs::ISystemLoopCommandQueue> riaecs::DefaultSystemLoopCommandQueueFactory::Create() const
{
    return std::make_unique<SystemLoopCommandQueue>();
}

void riaecs::DefaultSystemLoopCommandQueueFactory::Destroy(std::unique_ptr<ISystemLoopCommandQueue> product) const
{
    product.reset();
}

size_t riaecs::DefaultSystemLoopCommandQueueFactory::GetProductSize() const
{
    return sizeof(SystemLoopCommandQueue);
}

riaecs::SystemLoop::~SystemLoop()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);

    assert(listFactory_ && "SystemListFactory is not set");
    assert(loopCommandQueueFactory_ && "SystemLoopCommandQueueFactory is not set");

    listFactory_->Destroy(std::move(systemList_));
    loopCommandQueueFactory_->Destroy(std::move(commandQueue_));
}

void riaecs::SystemLoop::SetSystemListFactory(std::unique_ptr<ISystemListFactory> factory)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    listFactory_ = std::move(factory);
}

void riaecs::SystemLoop::SetSystemLoopCommandQueueFactory(std::unique_ptr<ISystemLoopCommandQueueFactory> factory)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    loopCommandQueueFactory_ = std::move(factory);
}

bool riaecs::SystemLoop::IsReady() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    if (!listFactory_)
    {
        riaecs::NotifyError({"SystemListFactory is not set"}, RIAECS_LOG_LOC);
        isReady_ = false;
        return isReady_;
    }

    if (!loopCommandQueueFactory_)
    {
        riaecs::NotifyError({"SystemLoopCommandQueueFactory is not set"}, RIAECS_LOG_LOC);
        isReady_ = false;
        return isReady_;
    }

    isReady_ = true;
    return isReady_;
}

void riaecs::SystemLoop::Initialize()
{
    if (!IsReady())
        riaecs::NotifyError({"SystemLoop is not ready"}, RIAECS_LOG_LOC);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    systemList_ = listFactory_->Create();
    commandQueue_ = loopCommandQueueFactory_->Create();

    if (!systemList_)
        riaecs::NotifyError({"Failed to create System List"}, RIAECS_LOG_LOC);

    if (!commandQueue_)
        riaecs::NotifyError({"Failed to create System Loop Command Queue"}, RIAECS_LOG_LOC);
}

void riaecs::SystemLoop::Run(IECSWorld &ecsWorld, IAssetContainer &assetCont)
{
    if (!isReady_)
        riaecs::NotifyError({"SystemLoop is not ready"}, RIAECS_LOG_LOC);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    while (true)
    {
        // Process commands in the command queue
        while (!commandQueue_->IsEmpty())
        {
            std::unique_ptr<ISystemLoopCommand> cmd = commandQueue_->Dequeue();
            if (cmd)
                cmd->Execute(*systemList_, ecsWorld, assetCont);
            else
                riaecs::NotifyError({"Invalid command in System Loop Command Queue"}, RIAECS_LOG_LOC);
        }

        if (systemList_->GetCount() == 0)
            break; // Exit the loop if no systems are available

        // Update systems
        bool continueLoop = true;
        for (size_t i = 0; i < systemList_->GetCount(); ++i)
        {
            riaecs::RWObject<ISystem> system = systemList_->Get(i);
            continueLoop = system().Update(ecsWorld, assetCont, *commandQueue_);
            if (!continueLoop)
                break; // Stop the system update if any system returns false
        }

        if (!continueLoop)
            break; // Stop the system loop if any system returns false
    }
}