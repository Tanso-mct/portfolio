#pragma once
#include "windows_base/windows_base.h"

#include <DirectXMath.h>

namespace wbp_transform
{
    const float MAX_CLIP_Z = 1000.0f;
    const float MIN_CLIP_Z = 0.0f;
    const DirectX::XMFLOAT3 DEFAULT_BASE_SIZE(1920.0f, 1080.0f, MAX_CLIP_Z);

    const DirectX::XMFLOAT3 ORIGINAL_SCALE(1.0f, 1.0f, 1.0f);

    class IScreenTransformComponent : public wb::IComponent
    {
    public:
        virtual ~IScreenTransformComponent() = default;

        /***************************************************************************************************************
         * Methods to get the position, rotation, and scale of a ScreenTransformComponent
        /**************************************************************************************************************/

        virtual const DirectX::XMFLOAT3 &GetLocalPosition() const = 0;
        virtual const DirectX::XMFLOAT4 &GetQuatLocalRotation() const = 0;
        virtual const DirectX::XMFLOAT3 &GetLocalScale() const = 0;

        /***************************************************************************************************************
         * Methods for changing ScreenTransform values
        /**************************************************************************************************************/

        virtual void SetLocalPosition(const DirectX::XMFLOAT3 &localPos) = 0;
        virtual void SetLocalRotation(const DirectX::XMFLOAT3 &localRot) = 0;
        virtual void SetLocalQuatRotation(const DirectX::XMFLOAT4 &localQuatRotation) = 0;
        virtual void SetLocalScale(const DirectX::XMFLOAT3 &localScale) = 0;

        virtual void Translate(const DirectX::XMFLOAT3 &translation) = 0;
        virtual void Rotate(const DirectX::XMFLOAT3 &quaternion) = 0;
        virtual void Scale(const DirectX::XMFLOAT3 &scale) = 0;

        virtual void TranslateTopLine(float translation) = 0;
        virtual void TranslateBottomLine(float translation) = 0;
        virtual void TranslateLeftLine(float translation) = 0;
        virtual void TranslateRightLine(float translation) = 0;

        /***************************************************************************************************************
         * Method to obtain the base size of the ScreenTransform
        /**************************************************************************************************************/

        virtual const DirectX::XMFLOAT3 &GetBaseSize() const = 0;
        virtual void SetBaseSize(const DirectX::XMFLOAT3 &baseSize) = 0;

        /***************************************************************************************************************
         * Method to obtain the transformation matrix of the world and local
        /**************************************************************************************************************/

        virtual DirectX::XMMATRIX GetClipMatrix() const = 0;
    };

} // namespace wbp_transform