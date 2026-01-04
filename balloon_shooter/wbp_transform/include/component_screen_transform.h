#pragma once
#include "wbp_transform/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_transform/include/interfaces/component_screen_transform.h"

namespace wbp_transform
{
    const WBP_TRANSFORM_API size_t &ScreenTransformComponentID();

    class WBP_TRANSFORM_API ScreenTransformComponent : public IScreenTransformComponent
    {
    private:
        DirectX::XMFLOAT3 localPosition_;
        DirectX::XMFLOAT4 quatLocalRotation_;
        DirectX::XMFLOAT3 localScale_;
        DirectX::XMFLOAT3 baseSize_;

    public:
        ScreenTransformComponent();
        virtual ~ScreenTransformComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;

        /***************************************************************************************************************
         * IScreenTransformComponent implementation
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3 &GetLocalPosition() const override { return localPosition_; }
        const DirectX::XMFLOAT4 &GetQuatLocalRotation() const override { return quatLocalRotation_; }
        const DirectX::XMFLOAT3 &GetLocalScale() const override { return localScale_; }

        void SetLocalPosition(const DirectX::XMFLOAT3 &localPos) override { localPosition_ = localPos; }
        void SetLocalRotation(const DirectX::XMFLOAT3 &localRot) override;
        void SetLocalQuatRotation(const DirectX::XMFLOAT4 &localQuatRotation) override { quatLocalRotation_ = localQuatRotation; }
        void SetLocalScale(const DirectX::XMFLOAT3 &localScale) override { localScale_ = localScale; }

        void Translate(const DirectX::XMFLOAT3 &translation) override;
        void Rotate(const DirectX::XMFLOAT3 &rotation) override;
        void Scale(const DirectX::XMFLOAT3 &scale) override;

        void TranslateTopLine(float translation) override;
        void TranslateBottomLine(float translation) override;
        void TranslateLeftLine(float translation) override;
        void TranslateRightLine(float translation) override;

        const DirectX::XMFLOAT3 &GetBaseSize() const override { return baseSize_; }
        void SetBaseSize(const DirectX::XMFLOAT3 &baseSize) override { baseSize_ = baseSize; }

        DirectX::XMMATRIX GetClipMatrix() const override;

    };

} // namespace wbp_transform