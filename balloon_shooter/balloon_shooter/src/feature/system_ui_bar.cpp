#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/system_ui_bar.h"

#include "balloon_shooter/include/feature/interfaces/ui_bar.h"

#include "balloon_shooter/include/feature/component_ui_player.h"

using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

const size_t &balloon_shooter::UIBarSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &balloon_shooter::UIBarSystem::GetID() const
{
    return UIBarSystemID();
}

void balloon_shooter::UIBarSystem::Initialize(wb::IAssetContainer &assetContainer)
{
    targetUIBarComponentIDs_.clear();
    
    targetUIBarComponentIDs_.push_back(balloon_shooter::PlayerUIBalloonLeftComponentID());
}

void balloon_shooter::UIBarSystem::Update(const wb::SystemArgument &args)
{
    for (const size_t &uiBarComponentID : targetUIBarComponentIDs_)
    {
        for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(uiBarComponentID))
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
                    {"UIBarComponentがIIdentityComponentを持っていません。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            if (!identity->IsActiveSelf())
            {
                // Entityが非アクティブな場合はスキップ
                continue;
            }

            wb::IComponent *uiBarComponent = entity->GetComponent(uiBarComponentID, args.componentContainer_);
            balloon_shooter::IUIBarComponent *uiBar = wb::As<balloon_shooter::IUIBarComponent>(uiBarComponent);

            // Frame Entityを取得
            wb::IOptionalValue &frameEntityID = uiBar->GetFrameEntityID();
            wb::IEntity *frameEntity = args.entityContainer_.PtrGet(frameEntityID);
            if (frameEntity == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"UIBarComponentのFrameEntityIDが無効です。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // Bar Entityを取得
            wb::IOptionalValue &barEntityID = uiBar->GetBarEntityID();
            wb::IEntity *barEntity = args.entityContainer_.PtrGet(barEntityID);
            if (barEntity == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"UIBarComponentのBarEntityIDが無効です。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // Frame entityのScreenTransformComponentを取得
            wb::IComponent *frameScreenTransformComponent = frameEntity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
            wbp_transform::IScreenTransformComponent *frameScreenTransform = wb::As<wbp_transform::IScreenTransformComponent>(frameScreenTransformComponent);
            if (frameScreenTransform == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Frame entityにScreenTransformComponentがありません。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // Frameの位置、幅を取得
            const XMFLOAT3 &framePosition = frameScreenTransform->GetLocalPosition();
            float frameWidth = frameScreenTransform->GetLocalScale().x * 2.0f; // Scaleの2倍が幅

            // Bar entityのScreenTransformComponentを取得
            wb::IComponent *barScreenTransformComponent = barEntity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
            wbp_transform::IScreenTransformComponent *barScreenTransform = wb::As<wbp_transform::IScreenTransformComponent>(barScreenTransformComponent);
            if (barScreenTransform == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Bar entityにScreenTransformComponentがありません。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // Barは毎回値を更新するため回転、スケールをリセット
            barScreenTransform->SetLocalScale(wbp_transform::ORIGINAL_SCALE);
            barScreenTransform->SetLocalRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

            // 位置のリセット。Frameの左端に合わせてBarの位置を設定
            barScreenTransform->SetLocalPosition
            (
                XMFLOAT3
                (
                    framePosition.x - frameWidth / 2 + balloon_shooter::UI_BAR_FRAME_BAR_OFFSET,
                    framePosition.y,
                    framePosition.z
                )
            );

            // Barの上辺、下辺をFrameの上辺、下辺からオフセット分だけずらす
            float frameHeight = frameScreenTransform->GetLocalScale().y * 2.0f; // Scaleの2倍が高さ
            barScreenTransform->TranslateTopLine(frameHeight / 2.0f - balloon_shooter::UI_BAR_FRAME_BAR_OFFSET);
            barScreenTransform->TranslateBottomLine(-frameHeight / 2.0f + balloon_shooter::UI_BAR_FRAME_BAR_OFFSET);

            // MaxとMinの値から現在の値の割合を計算
            float currentRatio = uiBar->GetCurrentValue() / (uiBar->GetMaxValue() - uiBar->GetMinValue());

            // Frameの幅からオフセットを考慮したBarの長さを計算
            float barMaxLength = frameWidth - 2.0f * balloon_shooter::UI_BAR_FRAME_BAR_OFFSET;
            float barLength = barMaxLength * currentRatio;

            // Barの右辺をBarの長さ分だけずらす
            barScreenTransform->TranslateRightLine(barLength);

            // FrameのSpriteRendererComponentを取得
            wb::IComponent *frameSpriteRendererComponent = frameEntity->GetComponent(wbp_render::SpriteRendererComponentID(), args.componentContainer_);
            wbp_render::ISpriteRendererComponent *frameSpriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(frameSpriteRendererComponent);
            if (frameSpriteRenderer == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Frame entityにSpriteRendererComponentがありません。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // BarのSpriteRendererComponentを取得
            wb::IComponent *barSpriteRendererComponent = barEntity->GetComponent(wbp_render::SpriteRendererComponentID(), args.componentContainer_);
            wbp_render::ISpriteRendererComponent *barSpriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(barSpriteRendererComponent);
            if (barSpriteRenderer == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"Bar entityにSpriteRendererComponentがありません。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // Frameの色を設定
            {
                wbp_render::SpriteColorConfig frameColorConfig;
                frameColorConfig.color = uiBar->GetFrameColor();
                frameColorConfig.isColorEnabled = true; // 色を有効にする
                frameSpriteRenderer->SetColorConfig(frameColorConfig);
            }

            // Barの色を設定
            {
                wbp_render::SpriteColorConfig barColorConfig;
                barColorConfig.color = uiBar->GetBarColor();
                barColorConfig.isColorEnabled = true; // 色を有効にする
                barSpriteRenderer->SetColorConfig(barColorConfig);
            }
        }
    }
}

namespace balloon_shooter
{
    WB_REGISTER_SYSTEM(UIBarSystem, UIBarSystemID());

} // namespace balloon_shooter