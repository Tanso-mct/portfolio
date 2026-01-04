#pragma once

#include <memory>
#include <vector>

#include "windows_base/include/interfaces/container.h"
#include "windows_base/include/interfaces/component.h"
#include "windows_base/include/interfaces/optional_value.h"

namespace wb
{
    class IEntity
    {
    public:
        virtual ~IEntity() = default;

        virtual void Destroy(IComponentContainer &componentCont) = 0;

        virtual void SetID(std::unique_ptr<IOptionalValue> id) = 0;
        virtual const IOptionalValue &GetID() const = 0;

        virtual void AddComponent(size_t componentID, IComponentContainer &componentCont) = 0;
        virtual IComponent *GetComponent(size_t componentID, IComponentContainer &componentCont) = 0;

        virtual const std::vector<size_t> &GetComponentIDs() const = 0;
    };

    class IEntityIDView
    {
    public:
        virtual ~IEntityIDView() = default;

        virtual void RegisterEntity(IEntity &entity) = 0;
        virtual void UnregisterEntity(IEntity &entity) = 0;

        virtual const std::vector<std::unique_ptr<IOptionalValue>> &operator()(size_t componentID) const = 0;
    };

} // namespace wb