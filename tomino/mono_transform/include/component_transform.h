#pragma once
#include "mono_transform/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>

namespace mono_transform
{
    constexpr DirectX::XMFLOAT2 DEFAULT_ANCHOR_SIZE = DirectX::XMFLOAT2(1920.0f, 1080.0f);
    constexpr DirectX::XMFLOAT2 DEFAULT_PIVOT = DirectX::XMFLOAT2(0.5f, 0.5f); // Center (0.0f ~ 1.0f)

    class Anchor
    {
    public:
        Anchor() = default;
        ~Anchor() = default;

        DirectX::XMFLOAT2 size_ = DEFAULT_ANCHOR_SIZE;
        DirectX::XMFLOAT2 pivot_ = DEFAULT_PIVOT;
    };

    constexpr Anchor DEFAULT_ANCHOR = Anchor();

    constexpr size_t ComponentTransformMaxCount = 10000;
    class MONO_TRANSFORM_API ComponentTransform
    {
    private:
        bool isInitialized_ = false;

        DirectX::XMFLOAT3 pos_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT4 rot_ = { 0.0f, 0.0f, 0.0f, 1.0f }; // Quaternion
        DirectX::XMFLOAT3 scale_ = { 1.0f, 1.0f, 1.0f };
        
        DirectX::XMFLOAT3 localPos_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT4 localRot_ = { 0.0f, 0.0f, 0.0f, 1.0f }; // Quaternion
        DirectX::XMFLOAT3 localScale_ = { 1.0f, 1.0f, 1.0f };

        DirectX::XMFLOAT3 lastPos_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT4 lastRot_ = { 0.0f, 0.0f, 0.0f, 1.0f }; // Quaternion
        DirectX::XMFLOAT3 lastScale_ = { 1.0f, 1.0f, 1.0f };

        riaecs::Entity parent_ = riaecs::Entity();
        std::vector<riaecs::Entity> childs_ = std::vector<riaecs::Entity>();

    public:
        ComponentTransform();
        ~ComponentTransform();

        struct SetupParam
        {
            DirectX::XMFLOAT3 pos_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            float pitch_ = 0.0f;
            float yaw_ = 0.0f;
            float roll_ = 0.0f;
            DirectX::XMFLOAT3 scale_ = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        };
        void Setup(SetupParam &param);

        struct SetupParamWithAnchor
        {
            Anchor anchor_ = DEFAULT_ANCHOR;

            DirectX::XMFLOAT2 anchoredPos_ = DirectX::XMFLOAT2(0.0f, 0.0f);
            float pitch_ = 0.0f;
            float yaw_ = 0.0f;
            float roll_ = 0.0f;
            DirectX::XMFLOAT2 anchoredSize_ = DirectX::XMFLOAT2(0.0f, 0.0f);
            float depth_ = 0.0f;
        };
        void Setup(const SetupParamWithAnchor &param);

        /***************************************************************************************************************
         * Initialization
        /**************************************************************************************************************/

        bool IsInitialized() const { return isInitialized_; }

        /***************************************************************************************************************
         * World Transform
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3& GetPos() const { return pos_; }
        void SetPos(const DirectX::XMFLOAT3 &pos, riaecs::IECSWorld &ecsWorld);

        const DirectX::XMFLOAT4& GetRotByQuat() const { return rot_; }
        void SetRotFromQuat(const DirectX::XMFLOAT4 &rot, riaecs::IECSWorld &ecsWorld);
        
        DirectX::XMFLOAT3 GetRotByEuler() const;
        void SetRotFromEuler(float pitch, float yaw, float roll, riaecs::IECSWorld &ecsWorld);

        const DirectX::XMFLOAT3& GetScale() const { return scale_; }
        void SetScale(const DirectX::XMFLOAT3 &scale, riaecs::IECSWorld &ecsWorld);

        /***************************************************************************************************************
         * Local Transform
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3& GetLocalPos() const { return localPos_; }
        void SetLocalPos(const DirectX::XMFLOAT3 &localPos, riaecs::IECSWorld &ecsWorld);

        const DirectX::XMFLOAT4& GetLocalRotByQuat() const { return localRot_; }
        void SetLocalRotFromQuat(const DirectX::XMFLOAT4 &localRot, riaecs::IECSWorld &ecsWorld);

        DirectX::XMFLOAT3 GetLocalRotByEuler() const;
        void SetLocalRotFromEuler(float pitch, float yaw, float roll, riaecs::IECSWorld &ecsWorld);

        const DirectX::XMFLOAT3& GetLocalScale() const { return localScale_; }
        void SetLocalScale(const DirectX::XMFLOAT3 &localScale, riaecs::IECSWorld &ecsWorld);

        /***************************************************************************************************************
         * Last Transform
        /**************************************************************************************************************/

        const DirectX::XMFLOAT3& GetLastPos() const { return lastPos_; }
        void SetLastPos(const DirectX::XMFLOAT3 &lastPos) { lastPos_ = lastPos; }

        const DirectX::XMFLOAT4& GetLastRotByQuat() const { return lastRot_; }
        void SetLastRotFromQuat(const DirectX::XMFLOAT4 &lastRot) { lastRot_ = lastRot; }

        DirectX::XMFLOAT3 GetLastRotByEuler() const;
        void SetLastRotFromEuler(float pitch, float yaw, float roll);

        const DirectX::XMFLOAT3& GetLastScale() const { return lastScale_; }
        void SetLastScale(const DirectX::XMFLOAT3 &lastScale) { lastScale_ = lastScale; }

        /***************************************************************************************************************
         * Anchor Based Transform
        /**************************************************************************************************************/

        void SetPosWithAnchor(
            const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredPos, float depth, riaecs::IECSWorld &ecsWorld);

        void SetLocalPosWithAnchor(
            const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredLocalPos, float depth, riaecs::IECSWorld &ecsWorld);

        void SetScaleWithAnchor(
            const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredSize, riaecs::IECSWorld &ecsWorld);

        void SetLocalScaleWithAnchor(
            const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredLocalSize, riaecs::IECSWorld &ecsWorld);

        /***************************************************************************************************************
         * Hierarchy
        /**************************************************************************************************************/

        const riaecs::Entity& GetParent() const { return parent_; }
        void SetParent(const riaecs::Entity &self, const riaecs::Entity &parent, riaecs::IECSWorld &ecsWorld);

        const std::vector<riaecs::Entity>& GetChilds() const { return childs_; }
        void RemoveChild(const riaecs::Entity &self, const riaecs::Entity &child, riaecs::IECSWorld &ecsWorld);

        /***************************************************************************************************************
         * Matrix
        /**************************************************************************************************************/

        DirectX::XMMATRIX GetWorldMatrix();
        DirectX::XMMATRIX GetWorldMatrixNoRot();
        DirectX::XMMATRIX GetWorldMatrixNoScale();

        DirectX::XMMATRIX GetLocalMatrix();
        DirectX::XMMATRIX GetLocalMatrixNoRot();
        DirectX::XMMATRIX GetLocalMatrixNoScale();

        DirectX::XMMATRIX GetLastWorldMatrix();
        DirectX::XMMATRIX GetLastWorldMatrixNoRot();
        DirectX::XMMATRIX GetLastWorldMatrixNoScale();
    };
    extern MONO_TRANSFORM_API riaecs::ComponentRegistrar<ComponentTransform, ComponentTransformMaxCount> ComponentTransformID;

    // Create new transform from changed world/local transform
    // If parentWorldTransform is not changed, use oldLocal/WorldTransform to calculate new local/world transform
    MONO_TRANSFORM_API DirectX::XMFLOAT3 CreateNewLocalPosFromWorldPos(
        const DirectX::XMFLOAT3 &worldPos, const DirectX::XMFLOAT3 &oldLocalPos, const DirectX::XMFLOAT3 &parentWorldPos);
    MONO_TRANSFORM_API DirectX::XMFLOAT3 CreateNewWorldPosFromLocalPos(
        const DirectX::XMFLOAT3 &localPos, const DirectX::XMFLOAT3 &oldWorldPos, const DirectX::XMFLOAT3 &parentWorldPos);

    MONO_TRANSFORM_API DirectX::XMFLOAT4 CreateNewLocalRotFromWorldRot(
        const DirectX::XMFLOAT4 &worldRot, const DirectX::XMFLOAT4 &oldLocalRot, const DirectX::XMFLOAT4 &parentWorldRot);
    MONO_TRANSFORM_API DirectX::XMFLOAT4 CreateNewWorldRotFromLocalRot(
        const DirectX::XMFLOAT4 &localRot, const DirectX::XMFLOAT4 &oldWorldRot, const DirectX::XMFLOAT4 &parentWorldRot);

    MONO_TRANSFORM_API DirectX::XMFLOAT3 CreateNewLocalScaleFromWorldScale(
        const DirectX::XMFLOAT3 &worldScale, const DirectX::XMFLOAT3 &oldLocalScale, const DirectX::XMFLOAT3 &parentWorldScale);
    MONO_TRANSFORM_API DirectX::XMFLOAT3 CreateNewWorldScaleFromLocalScale(
        const DirectX::XMFLOAT3 &localScale, const DirectX::XMFLOAT3 &oldWorldScale, const DirectX::XMFLOAT3 &parentWorldScale);

    // Update transforms
    MONO_TRANSFORM_API void UpdateRootTransform(ComponentTransform* component);
    MONO_TRANSFORM_API void UpdateChildTransform(
        ComponentTransform* self, ComponentTransform* parent, riaecs::IECSWorld &ecsWorld);
    MONO_TRANSFORM_API void UpdateChildsTransform(const riaecs::Entity &entity, riaecs::IECSWorld &ecsWorld);

    // Update transforms without save last transform
    MONO_TRANSFORM_API void UpdateRootTransformNoLastTransform(ComponentTransform* component);
    MONO_TRANSFORM_API void UpdateChildTransformNoLastTransform(
        ComponentTransform* self, ComponentTransform* parent, riaecs::IECSWorld &ecsWorld);
    MONO_TRANSFORM_API void UpdateChildsTransformNoLastTransform(const riaecs::Entity &entity, riaecs::IECSWorld &ecsWorld);


} // namespace mono_transform