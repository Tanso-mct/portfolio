#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/system_ui_button.h"

#include "balloon_shooter/include/feature/interfaces/ui_button.h"

#include "balloon_shooter/include/feature/component_ui_title.h"
#include "balloon_shooter/include/feature/component_ui_clear.h"

using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

const size_t &balloon_shooter::UIBtnSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &balloon_shooter::UIBtnSystem::GetID() const
{
    return UIBtnSystemID();
}

void balloon_shooter::UIBtnSystem::Initialize(wb::IAssetContainer &assetContainer)
{
    targetUIBtnComponentIDs_.clear();
    
    targetUIBtnComponentIDs_.push_back(balloon_shooter::TitleUIPlayBtnComponentID());
    targetUIBtnComponentIDs_.push_back(balloon_shooter::TitleUIExitBtnComponentID());
    targetUIBtnComponentIDs_.push_back(balloon_shooter::ClearUIPlayBtnComponentID());
    targetUIBtnComponentIDs_.push_back(balloon_shooter::ClearUIMenuBtnComponentID());
}

void balloon_shooter::UIBtnSystem::Update(const wb::SystemArgument &args)
{
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();
    wb::IMonitorContainer &monitorContainer = args.containerStorage_.GetContainer<wb::IMonitorContainer>();

    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);

    wb::IMouseMonitor *mouseMonitor = nullptr;
    for (const size_t &monitorID : window.GetMonitorIDs())
    {
        wb::IMonitor &monitor = monitorContainer.Get(monitorID);
        if (mouseMonitor == nullptr) mouseMonitor = wb::As<wb::IMouseMonitor>(&monitor);
    }

    float minZ = FLT_MAX;
    std::unique_ptr<wb::IOptionalValue> frontMostButtonEntityID = nullptr;
    size_t frontMostButtonComponentID = 0;

    std::vector<std::unique_ptr<wb::IOptionalValue>> processedIDs;
    std::vector<size_t> processedComponentIDs;

    for (const size_t &uiBarComponentID : targetUIBtnComponentIDs_)
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
                    {"UIBtnComponentがIIdentityComponentを持っていません。"}
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

            // 処理対象を記録
            processedIDs.emplace_back(id->Clone());
            processedComponentIDs.push_back(uiBarComponentID);

            wb::IComponent *uiBarComponent = entity->GetComponent(uiBarComponentID, args.componentContainer_);
            balloon_shooter::IUIButtonComponent *uiBar = wb::As<balloon_shooter::IUIButtonComponent>(uiBarComponent);
            if (uiBar == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"IUIButtonComponentを継承していないクラスが登録されています。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
            wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
            if (screenTransform == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {"UIButtonComponentがIScreenTransformComponentを持っていません。"}
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("BDC_UI", err);
                wb::ThrowRuntimeError(err);
            }

            // ボタンの画面上でのmin、max座標を取得
            const DirectX::XMFLOAT3 &localPosition = screenTransform->GetLocalPosition();
            DirectX::XMFLOAT2 extent = XMFLOAT2
            (
                screenTransform->GetLocalScale().x , screenTransform->GetLocalScale().y
            );

            DirectX::XMFLOAT2 minPos = XMFLOAT2(localPosition.x - extent.x, localPosition.y - extent.y);
            DirectX::XMFLOAT2 maxPos = XMFLOAT2(localPosition.x + extent.x, localPosition.y + extent.y);

            // ボタンのZ座標を取得
            float z = screenTransform->GetLocalPosition().z;

            if (mouseMonitor != nullptr)
            {
                // マウスの位置をScreenTransformの座標系に変換
                float mouseConvertedX 
                    = wbp_transform::DEFAULT_BASE_SIZE.x * ((float)mouseMonitor->GetPositionX() / (float)window.GetClientWidth()) - wbp_transform::DEFAULT_BASE_SIZE.x * 0.5f;
                float mouseConvertedY 
                    = wbp_transform::DEFAULT_BASE_SIZE.y * ((float)mouseMonitor->GetPositionY() / (float)window.GetClientHeight()) - wbp_transform::DEFAULT_BASE_SIZE.y * 0.5f;
                mouseConvertedY *= -1.0f; // Y座標は反転する

                // マウスの位置がボタンの範囲内にあるかチェック
                if 
                (
                    mouseConvertedX >= minPos.x && mouseConvertedX <= maxPos.x &&
                    mouseConvertedY >= minPos.y && mouseConvertedY <= maxPos.y
                ){
                    // マウスがボタンの範囲内にある場合、最前面のボタンを更新
                    if (z < minZ)
                    {
                        minZ = z;
                        frontMostButtonEntityID = entity->GetID().Clone();
                        frontMostButtonComponentID = uiBarComponentID;
                    }
                }
            }
            
        }
    }

    if (frontMostButtonEntityID != nullptr)
    {
        // 最前面のボタンが見つかった場合、ボタンの状態を更新
        wb::IEntity *frontMostButtonEntity = args.entityContainer_.PtrGet(*frontMostButtonEntityID);
        if (frontMostButtonEntity == nullptr) return; // エンティティが存在しない場合は終了

        wb::IComponent *uiBarComponent = frontMostButtonEntity->GetComponent(frontMostButtonComponentID, args.componentContainer_);
        balloon_shooter::IUIButtonComponent *uiButton = wb::As<balloon_shooter::IUIButtonComponent>(uiBarComponent);

        if (mouseMonitor != nullptr && mouseMonitor->GetButtonDown(wb::MouseCode::Left))
        {
            // マウスの左ボタンが押された場合、クリックイベントを発火
            uiButton->GetEvent().OnClick(args);
        }
        
        if (mouseMonitor != nullptr && !uiButton->IsHovering())
        {
            // マウスがもともとボタン上に無かった場合、ホバーリング状態にしホバーイベントを発火
            uiButton->SetIsHovering(true);
            uiButton->GetEvent().OnHover(args);
        }
    }
    else
    {
        // 最前面のボタンが見つからなかった場合、処理した全てのボタンのホバー状態を解除
        for (size_t i = 0; i < processedIDs.size(); ++i)
        {
            const std::unique_ptr<wb::IOptionalValue> &id = processedIDs[i];
            if (id == nullptr) continue;

            // Entityを取得
            wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
            if (entity == nullptr) continue;

            wb::IComponent *uiBarComponent = entity->GetComponent(processedComponentIDs[i], args.componentContainer_);
            balloon_shooter::IUIButtonComponent *uiButton = wb::As<balloon_shooter::IUIButtonComponent>(uiBarComponent);
            if (uiButton->IsHovering())
            {
                // ホバー状態を解除し、アンホバーイベントを発火
                uiButton->SetIsHovering(false);
                uiButton->GetEvent().OnUnHover(args);
            }
        }
    }
}

namespace balloon_shooter
{
    WB_REGISTER_SYSTEM(UIBtnSystem, UIBtnSystemID());

} // namespace balloon_shooter