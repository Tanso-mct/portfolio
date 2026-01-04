#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/entity.h"
#include "windows_base/include/interfaces/factory.h"

#include <vector>

namespace wb
{
    class WINDOWS_BASE_API Entity : public IEntity
    {
    private:
        std::unique_ptr<IOptionalValue> id_ = nullptr;

        std::unique_ptr<IStaticContainer<IOptionalValue>> componentIndicesInCont_ = nullptr;
        std::vector<size_t> componentIDs_;

    public:
        Entity();
        ~Entity() override = default;

        /***************************************************************************************************************
         * IEntity implementation
        /**************************************************************************************************************/

        void Destroy(IComponentContainer &componentCont) override;

        void SetID(std::unique_ptr<IOptionalValue> id) override;
        const IOptionalValue &GetID() const override;

        void AddComponent(size_t componentID, IComponentContainer &componentCont) override;
        IComponent *GetComponent(size_t componentID, IComponentContainer &componentCont) override;

        const std::vector<size_t> &GetComponentIDs() const override { return componentIDs_; }
    };

    class WINDOWS_BASE_API EntityIDView : public IEntityIDView
    { 
    private:
        // Container for entity IDs, indexed by component ID.
        // Outside vector's index is the component ID, and inside vector contains entity IDs.
        std::vector<std::vector<std::unique_ptr<IOptionalValue>>> entityIDsPerComponent_;
        
    public:
        EntityIDView();
        ~EntityIDView() = default;

        EntityIDView(const EntityIDView &) = delete;
        EntityIDView &operator=(const EntityIDView &) = delete;

        void RegisterEntity(IEntity &entity) override;
        void UnregisterEntity(IEntity &entity) override;

        const std::vector<std::unique_ptr<IOptionalValue>> &operator()(size_t componentID) const override;
    };

    template <typename ENTITY_ID_VIEW>
    class EntityIDViewFactory : public IEntityIDViewFactory
    {
    public:
        std::unique_ptr<IEntityIDView> Create() const override
        {
            return std::make_unique<ENTITY_ID_VIEW>();
        }
    };

    class WINDOWS_BASE_API CreatingEntity
    {
    private:
        IEntity & entity_;
        IEntityIDView &entityIDView_;
    
    public:
        CreatingEntity(IEntity &entity, IEntityIDView &entityIDView);
        ~CreatingEntity();

        CreatingEntity(const CreatingEntity &) = delete;
        CreatingEntity &operator=(const CreatingEntity &) = delete;

        IEntity &operator()();
    };

    WINDOWS_BASE_API CreatingEntity CreateEntity(IEntityContainer &entityCont, IEntityIDView &entityIDView);

    WINDOWS_BASE_API void DestroyEntity
    (
        IEntity *entity, 
        IEntityIDView &entityIDView, IEntityContainer &entityCont, IComponentContainer &componentCont
    );

} // namespace wb