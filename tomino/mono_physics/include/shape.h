#pragma once
#include "mono_physics/include/dll_config.h"

#include <DirectXMath.h>

namespace mono_physics
{
    MONO_PHYSICS_API size_t CreateShapeTypeID();

    class IShape
    {
    public:
        virtual ~IShape() = default;
        virtual size_t GetTypeID() const = 0;
    };

    class MONO_PHYSICS_API ShapeBox : public IShape
    {
    private:
        DirectX::XMFLOAT3 min_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 max_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 center_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 extents_ = { 0.0f, 0.0f, 0.0f };
        
    public:
        ShapeBox();
        ShapeBox(const DirectX::XMFLOAT3 &center, const DirectX::XMFLOAT3 &extents);
        ~ShapeBox() override;

        /***************************************************************************************************************
         * IShape Implementation
        /**************************************************************************************************************/

        size_t GetTypeID() const override;

        /***************************************************************************************************************
         * ShapeBox Implementation
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3& GetMin() const { return min_; }
        void SetMin(const DirectX::XMFLOAT3& min);

        const DirectX::XMFLOAT3& GetMax() const { return max_; }
        void SetMax(const DirectX::XMFLOAT3& max);

        const DirectX::XMFLOAT3& GetCenter() const { return center_; }
        void SetCenter(const DirectX::XMFLOAT3& center);

        const DirectX::XMFLOAT3& GetExtents() const { return extents_; }
        void SetExtents(const DirectX::XMFLOAT3& extents);
    };

    class MONO_PHYSICS_API ShapeSphere : public IShape
    {
    private:
        DirectX::XMFLOAT3 center_ = { 0.0f, 0.0f, 0.0f };
        float radius_ = 0.0f;

    public:
        ShapeSphere();
        ShapeSphere(const DirectX::XMFLOAT3 &center, float radius);
        ~ShapeSphere() override;

        /***************************************************************************************************************
         * IShape Implementation
        /**************************************************************************************************************/

        size_t GetTypeID() const override;

        /***************************************************************************************************************
         * ShapeSphere Implementation
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3& GetCenter() const { return center_; }
        void SetCenter(const DirectX::XMFLOAT3& center);

        float GetRadius() const { return radius_; }
        void SetRadius(float radius);
    };

    class MONO_PHYSICS_API ShapeRay : public IShape
    {
    private:
        DirectX::XMFLOAT3 origin_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 direction_ = { 0.0f, 0.0f, 1.0f };
        float length_ = 1.0f;

    public:
        ShapeRay();
        ShapeRay(const DirectX::XMFLOAT3 &origin, const DirectX::XMFLOAT3 &direction, float length);
        ~ShapeRay() override;

        /***************************************************************************************************************
         * IShape Implementation
        /**************************************************************************************************************/

        size_t GetTypeID() const override;

        /***************************************************************************************************************
         * ShapeRay Implementation
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3& GetOrigin() const { return origin_; }
        void SetOrigin(const DirectX::XMFLOAT3& origin);

        const DirectX::XMFLOAT3& GetDirection() const { return direction_; }
        void SetDirection(const DirectX::XMFLOAT3& direction);

        float GetLength() const { return length_; }
        void SetLength(float length);
    };
}