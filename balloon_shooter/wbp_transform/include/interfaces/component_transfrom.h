#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace wbp_transform
{
    constexpr DirectX::XMFLOAT3 XM_UP(0.0f, 1.0f, 0.0f);
    constexpr DirectX::XMFLOAT3 XM_FORWARD(0.0f, 0.0f, 1.0f);
    constexpr DirectX::XMFLOAT3 XM_RIGHT(1.0f, 0.0f, 0.0f);

    class ITransformComponent : public wb::IComponent
    {
    public:
        virtual ~ITransformComponent() = default;

        /***************************************************************************************************************
         * Methods to get the position, rotation, and scale of a TransformComponent
        /**************************************************************************************************************/

        virtual const DirectX::XMFLOAT3 &GetPosition() const = 0;
        virtual const DirectX::XMFLOAT3 &GetPreviousPosition() const = 0;
        virtual const DirectX::XMFLOAT3 &GetLocalPosition() const = 0;

        virtual const DirectX::XMFLOAT4 &GetQuatRotation() const = 0;
        virtual const DirectX::XMFLOAT4 &GetPreviousQuatRotation() const = 0;
        virtual const DirectX::XMFLOAT4 &GetQuatLocalRotation() const = 0;
        
        virtual const DirectX::XMFLOAT3 &GetScale() const = 0;
        virtual const DirectX::XMFLOAT3 &GetPreviousScale() const = 0;
        virtual const DirectX::XMFLOAT3 &GetLocalScale() const = 0;

        /***************************************************************************************************************
         * Methods to get the current top, front, and right vectors
        /**************************************************************************************************************/

        virtual DirectX::XMVECTOR GetUp() const = 0;
        virtual DirectX::XMVECTOR GetForward() const = 0;
        virtual DirectX::XMVECTOR GetRight() const = 0;

        /***************************************************************************************************************
         * Methods for changing Transform values
        /**************************************************************************************************************/
        virtual void SetLocalPosition(const DirectX::XMFLOAT3 &localPos) = 0;
        virtual void SetLocalRotation(const DirectX::XMFLOAT3 &localRot) = 0;
        virtual void SetLocalQuatRotation(const DirectX::XMFLOAT4 &localQuatRotation) = 0;
        virtual void SetLocalScale(const DirectX::XMFLOAT3 &localScale) = 0;

        virtual void Translate(const DirectX::XMFLOAT3 &translation) = 0;
        virtual void Rotate(const DirectX::XMFLOAT3 &quaternion) = 0;
        virtual void Scale(const DirectX::XMFLOAT3 &scale) = 0;

        /***************************************************************************************************************
         * Method to retrieve the flag indicating whether the value has changed or not
        /**************************************************************************************************************/
        
        virtual bool IsDirty() const = 0;
        virtual bool WasDirty() const = 0;

        /***************************************************************************************************************
         * Methods for managing Transform parent-child relationships
        /**************************************************************************************************************/

        virtual wb::IEntity *GetParent(wb::IEntityContainer &entityCont) const = 0;

        // It can be used to set the parent of a Transform, 
        // The parent-child relationship can be removed by setting parent to nullptr
        virtual void SetParent
        (
            wb::IEntity *self, wb::IEntity *parent, 
            wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
        ) = 0;

        virtual size_t GetChildCount() const = 0;
        virtual wb::IEntity *GetChild(const size_t &index, wb::IEntityContainer &entityCont) const = 0;
        virtual wb::IEntity &GetRoot(wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont) const = 0;

        /***************************************************************************************************************
         * Method to obtain the transformation matrix of the world and local
        /**************************************************************************************************************/

        virtual DirectX::XMMATRIX GetWorldMatrix() const = 0;
        virtual DirectX::XMMATRIX GetWorldMatrixWithoutRot() const = 0;

        virtual DirectX::XMMATRIX GetPreviousWorldMatrix() const = 0;
        virtual DirectX::XMMATRIX GetPreviousWorldMatrixWithoutRot() const = 0;

    private:
        /***************************************************************************************************************
         * Methods for setting world coordinates
         * The user side is set only locally, so the world setting is set to private.
        /**************************************************************************************************************/

        virtual void SetPosition(const DirectX::XMFLOAT3 &pos) = 0;
        virtual void SetRotation(const DirectX::XMFLOAT3 &rot) = 0;
        virtual void SetQuatRotation(const DirectX::XMFLOAT4 &quatRotation) = 0;
        virtual void SetScale(const DirectX::XMFLOAT3 &scale) = 0;

        /***************************************************************************************************************
         * Methods for setting the flag to indicate if the value has changed or not
        /**************************************************************************************************************/

        virtual void SetDirty(bool isDirty) = 0;

        // WasDirty() to true and IsDirty() to false when IsDirty() is true
        virtual void UpdateDirtyFlags() = 0;

        /***************************************************************************************************************
         * Cleanup process if child Entity is destroyed before processing
        /**************************************************************************************************************/

        virtual void CleanNotExistEntities(wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont) = 0;

        friend class TransformSystem;
    };

} // namespace wbp_transform