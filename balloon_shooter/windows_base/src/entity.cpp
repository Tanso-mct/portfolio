#include "windows_base/src/pch.h"
#include "windows_base/include/entity.h"

#include "windows_base/include/component_registry.h"
#include "windows_base/include/container_static.h"
#include "windows_base/include/optional_value.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

wb::Entity::Entity()
{
    componentIndicesInCont_ = std::make_unique<wb::StaticContainer<wb::IOptionalValue>>();
    componentIndicesInCont_->Create(gComponentRegistry.GetMaxID() + 1);
}

void wb::Entity::Destroy(IComponentContainer &componentCont)
{
    for (const size_t &componentID : componentIDs_)
    {
        wb::IOptionalValue &index = componentIndicesInCont_->Get(componentID);
        componentCont.Erase(index);
    }

    componentIndicesInCont_->Clear();
    componentIDs_.clear();
}

void wb::Entity::SetID(std::unique_ptr<IOptionalValue> id)
{
    if (id_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"ID is already set for this entity."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    id_ = std::move(id);
}

const wb::IOptionalValue &wb::Entity::GetID() const
{
    if (id_ == nullptr || !id_->IsValid())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"ID is not set for this entity."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *id_;
}

void wb::Entity::AddComponent(size_t componentID, IComponentContainer &componentCont)
{
    wb::IComponentFactory &componentFactory = gComponentRegistry.GetFactory(componentID);

    // Create a new component using the factory
    std::unique_ptr<wb::IComponent> component = componentFactory.Create();

    // Add the component to the container
    std::unique_ptr<wb::IOptionalValue> index = componentCont.Add(std::move(component));

    // Store the component ID and index in the entity
    componentIndicesInCont_->Set(componentID, std::move(index));
    componentIDs_.emplace_back(componentID);
}

wb::IComponent *wb::Entity::GetComponent(size_t componentID, IComponentContainer &componentCont)
{
    // Get the index of the component in the container
    wb::IOptionalValue *index = componentIndicesInCont_->PtrGet(componentID);
    if (index == nullptr || !index->IsValid())
    {
        return nullptr; // Component not found or index is invalid
    }

    // Get the component from the container using the index
    wb::IComponent *component = componentCont.PtrGet(*index);

    return component;
}

wb::EntityIDView::EntityIDView()
{
    // Resize the vector with a size based on the maximum component ID available.
    entityIDsPerComponent_.resize(gComponentRegistry.GetMaxID() + 1);
}

void wb::EntityIDView::RegisterEntity(IEntity &entity)
{
    const std::vector<size_t> &componentIDs = entity.GetComponentIDs();

    // Get the entity's ID.
    const IOptionalValue &entityID = entity.GetID();

    // Register the entity's ID for each component it has.
    for (const size_t &componentID : componentIDs)
    {
        // Get the entity vector for the component ID.
        std::vector<std::unique_ptr<IOptionalValue>> &entityIDs = entityIDsPerComponent_[componentID];

        // Add the entity's ID
        entityIDs.emplace_back(entityID.Clone());
    }
}

void wb::EntityIDView::UnregisterEntity(IEntity &entity)
{
    const std::vector<size_t> &componentIDs = entity.GetComponentIDs();

    // Get the entity's ID.
    const IOptionalValue &entityID = entity.GetID();

    // Unregister the entity's ID for each component it has.
    for (const size_t &componentID : componentIDs)
    {
        // Get the entity vector for the component ID.
        std::vector<std::unique_ptr<IOptionalValue>> &entityIDs = entityIDsPerComponent_[componentID];

        // Find the entity ID in the vector and remove it.
        for (size_t i = 0; i < entityIDs.size(); i++)
        {
            if ((*entityIDs[i])() == entityID())
            {
                // Remove the entity ID from the vector.
                entityIDs.erase(entityIDs.begin() + i);

                // Exit the loop after removing the entity ID.
                break;
            }
        }
    }
}

const std::vector<std::unique_ptr<wb::IOptionalValue>> &wb::EntityIDView::operator()(size_t componentID) const
{
    // Check if the component ID is valid.
    if (componentID >= entityIDsPerComponent_.size())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid component ID: " + std::to_string(componentID)}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Return the entity IDs for the specified component ID.
    return entityIDsPerComponent_[componentID];
}

wb::CreatingEntity::CreatingEntity(IEntity &entity, IEntityIDView &entityIDView) :
    entity_(entity), entityIDView_(entityIDView)
{
}

wb::CreatingEntity::~CreatingEntity()
{
    // Register the entity in the ID view when the CreatingEntity goes out of scope.
    entityIDView_.RegisterEntity(entity_);
}

wb::IEntity &wb::CreatingEntity::operator()()
{
    return entity_;
}

WINDOWS_BASE_API wb::CreatingEntity wb::CreateEntity(IEntityContainer &entityCont, IEntityIDView &entityIDView)
{
    std::unique_ptr<wb::IOptionalValue> id = nullptr;
    {
        std::unique_ptr<wb::IEntity> entity = std::make_unique<wb::Entity>();

        // Add to the entity container
        id = entityCont.Add(std::move(entity));
    }

    // Get the entity reference from the container
    wb::IEntity &entity = entityCont.Get(*id);

    // Set the ID for the entity
    entity.SetID(id->Clone());

    return wb::CreatingEntity(entity, entityIDView);
}

WINDOWS_BASE_API void wb::DestroyEntity
(
    IEntity *entity, 
    IEntityIDView &entityIDView, IEntityContainer &entityCont, IComponentContainer &componentCont
){
    // Unregister the entity from the ID view
    entityIDView.UnregisterEntity(*entity);

    // Destroy the entity
    entity->Destroy(componentCont);

    // Remove the entity from the container
    entityCont.Erase(entity->GetID());
}