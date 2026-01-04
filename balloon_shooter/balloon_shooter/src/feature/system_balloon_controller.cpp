#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/system_balloon_controller.h"

#include "balloon_shooter/include/feature/component_balloon.h"

using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

const size_t &balloon_shooter::BalloonControllerSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &balloon_shooter::BalloonControllerSystem::GetID() const
{
    return BalloonControllerSystemID();
}

void balloon_shooter::BalloonControllerSystem::Initialize(wb::IAssetContainer &assetContainer)
{
}

void balloon_shooter::BalloonControllerSystem::Update(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::BalloonComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        if (identity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "BalloonComponentを追加する場合、IdentityComponentも追加する必要があります。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        if (!identity->IsActiveSelf())
        {
            // Entityが非アクティブな場合はスキップ
            continue;
        }

        wb::IComponent *balloonComponent = entity->GetComponent(balloon_shooter::BalloonComponentID(), args.componentContainer_);
        balloon_shooter::IBalloonComponent *balloon = wb::As<balloon_shooter::IBalloonComponent>(balloonComponent);
        if (balloon == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "BalloonControllerSystemを使用する場合、BalloonComponentも追加する必要があります。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        if (transform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Balloon entityにTransformComponentがありません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        if (std::abs(balloon->GetCurrentMoveValue()) >= balloon->GetMoveLimit())
        {
            // MoveLimitを超えた場合は反対方向に移動
            balloon->SetMoveSide(-balloon->GetMoveSide());
        }

        // 基準となる中心点から現在地の差分を計算
        XMVECTOR baseCenterVec = XMLoadFloat3(&balloon->GetBaseCenter());
        XMVECTOR currentPositionVec = XMLoadFloat3(&transform->GetLocalPosition());
        XMVECTOR currentOffsetVec = currentPositionVec - baseCenterVec;
        XMFLOAT3 currentOffset;
        XMStoreFloat3(&currentOffset, currentOffsetVec);

        // 原点からの距離を計算
        float distance = sqrtf(currentOffset.x * currentOffset.x + currentOffset.z * currentOffset.z);

        // 現在のΘを計算
        float theta = atan2f(currentOffset.z, currentOffset.x);

        // Θを度数法に変換
        float thetaDegrees = XMConvertToDegrees(theta);

        // 移動後のΘを計算
        float newDegrees = thetaDegrees + balloon->GetMoveSide() * balloon->GetMovingSpeed() * args.deltaTime_;
        float newTheta = XMConvertToRadians(newDegrees);

        // 新しい位置を計算
        transform->SetLocalPosition
        (
            XMFLOAT3
            (
                balloon->GetBaseCenter().x + distance * cosf(newTheta),
                transform->GetLocalPosition().y, // Y座標はそのまま
                balloon->GetBaseCenter().z + distance * sinf(newTheta)
            )
        );

        // 現在の移動値を更新
        balloon->SetCurrentMoveValue(balloon->GetCurrentMoveValue() + balloon->GetMovingSpeed() * balloon->GetMoveSide() * args.deltaTime_);
    }
}

namespace balloon_shooter
{
    WB_REGISTER_SYSTEM(BalloonControllerSystem, BalloonControllerSystemID());

} // namespace balloon_shooter