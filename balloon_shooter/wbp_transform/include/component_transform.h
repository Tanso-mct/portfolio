#pragma once
#include "wbp_transform/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_transform/include/interfaces/component_transfrom.h"

namespace wbp_transform
{
    const WBP_TRANSFORM_API size_t &TransformComponentID();

    class WBP_TRANSFORM_API TransformComponent : public ITransformComponent
    {
    private:
        bool previousPosInitialized_ = false;
		DirectX::XMFLOAT3 position_ = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 previousPosition_ = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 localPosition_ = { 0.0f, 0.0f, 0.0f };

        bool previousQuatLocalRotInitialized_ = false;
		DirectX::XMFLOAT4 quatRotation_ = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT4 previousRotation_ = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT4 quatLocalRotation_ = { 0.0f, 0.0f, 0.0f, 1.0f };

        bool previousLocalScaleInitialized_ = false;
		DirectX::XMFLOAT3 scale_ = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 previousLocalScale_ = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 localScale_ = { 1.0f, 1.0f, 1.0f };

        bool isDirty_ = false;
        bool wasDirty_ = false;

        std::unique_ptr<wb::IOptionalValue> parentEntityID_ = nullptr;
        std::vector<std::unique_ptr<wb::IOptionalValue>> childEntityIDs_;

    public:
        TransformComponent();
        virtual ~TransformComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;

        /***************************************************************************************************************
         * ITransformComponent implementation
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3 &GetPosition() const override { return position_; }
        const DirectX::XMFLOAT3 &GetPreviousPosition() const override { return previousPosition_; }
        const DirectX::XMFLOAT3 &GetLocalPosition() const override { return localPosition_; }

        const DirectX::XMFLOAT4 &GetQuatRotation() const override { return quatRotation_; }
        const DirectX::XMFLOAT4 &GetPreviousQuatRotation() const override { return previousRotation_; }
        const DirectX::XMFLOAT4 &GetQuatLocalRotation() const override { return quatLocalRotation_; }

        const DirectX::XMFLOAT3 &GetScale() const override { return scale_; }
        const DirectX::XMFLOAT3 &GetPreviousScale() const override { return previousLocalScale_; }
        const DirectX::XMFLOAT3 &GetLocalScale() const override { return localScale_; }

        DirectX::XMVECTOR GetUp() const override;
        DirectX::XMVECTOR GetForward() const override;
        DirectX::XMVECTOR GetRight() const override;

        void SetLocalPosition(const DirectX::XMFLOAT3 &localPos);
        void SetLocalRotation(const DirectX::XMFLOAT3 &localRot);
        void SetLocalQuatRotation(const DirectX::XMFLOAT4 &localQuatRotation);
        void SetLocalScale(const DirectX::XMFLOAT3 &localScale);

        void Translate(const DirectX::XMFLOAT3 &translation) override;
        void Rotate(const DirectX::XMFLOAT3 &rotation) override;
        void Scale(const DirectX::XMFLOAT3 &scale) override;

        bool IsDirty() const override { return isDirty_; }
        bool WasDirty() const override { return wasDirty_; }

        wb::IEntity *GetParent(wb::IEntityContainer &entityCont) const override;
        void SetParent
        (
            wb::IEntity *self, wb::IEntity *parent, 
            wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
        ) override;

        size_t GetChildCount() const override;
        wb::IEntity *GetChild(const size_t &index, wb::IEntityContainer &entityCont) const override;
        wb::IEntity &GetRoot(wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont) const override;

        DirectX::XMMATRIX GetWorldMatrix() const override;
        DirectX::XMMATRIX GetWorldMatrixWithoutRot() const override;

        DirectX::XMMATRIX GetPreviousWorldMatrix() const override;
        DirectX::XMMATRIX GetPreviousWorldMatrixWithoutRot() const override;

    private:
        void SetPosition(const DirectX::XMFLOAT3 &pos) override;
        void SetRotation(const DirectX::XMFLOAT3 &rot) override;
        void SetQuatRotation(const DirectX::XMFLOAT4 &quatRotation) override;
        void SetScale(const DirectX::XMFLOAT3 &scale) override;

        void SetDirty(bool isDirty) override { isDirty_ = isDirty; }
        void UpdateDirtyFlags() override;

        void CleanNotExistEntities(wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont) override;
    };

} // namespace wbp_transform