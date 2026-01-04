#pragma once
#include "bdc_player/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>

#include "mono_input_monitor/input_monitor.h"

namespace bdc_player
{
    constexpr size_t ComponentPlayerMaxCount = 10;
    class BDC_PLAYER_API ComponentPlayer
    {
    private:
        riaecs::Entity handlerWindowEntity_ = riaecs::Entity();

        mono_input_monitor::KeyCode modeToggleKey_ = mono_input_monitor::KeyCode::R;
        mono_input_monitor::MouseCode abilityAKey = mono_input_monitor::MouseCode::Right;

        float moveSpeed_ = 6.0f;
        float enhancedMoveSpeed_ = 12.0f;

        // Blinking state
        bool isBlinking_ = false;

        // Direction to blink towards
        DirectX::XMFLOAT3 blinkDirection_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

        // Distance to blink
        float blinkDistance_ = 3.0f;

        // Distance already blinked
        float currentBlinkedDistance_ = 0.0f;

        // Units per second
        float blinkSpeed_ = 20.0f;

        // Player state
        bool isGotDot_ = false;
        bool isGotCrystal_ = false;
        bool IsGotItem_ = false;
        bool isDead_ = false;

        // Dead count
        uint32_t deadCount_ = 0;

        // Dead cooldown time
        float deadCooldownTime_ = 2.0f;

        // Collect counts
        uint32_t dotCollectCount_ = 0;
        uint32_t crystalCollectCount_ = 0;

        // Total collect counts
        uint32_t totalDotCollectCount_ = 0;
        uint32_t totalCrystalCollectCount_ = 0;

        // Material handle IDs for different states
        size_t normalMaterialHandleID_ = 0;
        size_t enhancedMaterialHandleID_ = 0;

    public:
        ComponentPlayer();
        ~ComponentPlayer();

        struct SetupParam
        {
            riaecs::Entity handlerWindowEntity = riaecs::Entity();

            mono_input_monitor::KeyCode modeToggleKey = mono_input_monitor::KeyCode::R;
            mono_input_monitor::MouseCode abilityAKey = mono_input_monitor::MouseCode::Right;

            float moveSpeed = 6.0f;
            float enhancedMoveSpeed = 12.0f;
            float blinkDistance = 3.0f;
            float blinkSpeed = 20.0f;

            size_t normalMaterialHandleID = 0;
            size_t enhancedMaterialHandleID = 0;
        };
        void Setup(SetupParam &param);

        riaecs::Entity GetHandlerWindowEntity() const { return handlerWindowEntity_; }
        void SetHandlerWindowEntity(const riaecs::Entity &entity) { handlerWindowEntity_ = entity; }

        mono_input_monitor::KeyCode GetModeToggleKey() const { return modeToggleKey_; }
        void SetModeToggleKey(mono_input_monitor::KeyCode keyCode) { modeToggleKey_ = keyCode; }

        mono_input_monitor::MouseCode GetAbilityAKey() const { return abilityAKey; }
        void SetAbilityAKey(mono_input_monitor::MouseCode keyCode) { abilityAKey = keyCode; }

        float GetMoveSpeed() const { return moveSpeed_; }
        void SetMoveSpeed(float speed) { moveSpeed_ = speed; }

        float GetEnhancedMoveSpeed() const { return enhancedMoveSpeed_; }
        void SetEnhancedMoveSpeed(float speed) { enhancedMoveSpeed_ = speed; }

        bool IsBlinking() const { return isBlinking_; }
        void SetIsBlinking(bool isBlinking) { isBlinking_ = isBlinking; }

        DirectX::XMFLOAT3 GetBlinkDirection() const { return blinkDirection_; }
        void SetBlinkDirection(const DirectX::XMFLOAT3 &direction) { blinkDirection_ = direction; }

        float GetBlinkDistance() const { return blinkDistance_; }
        void SetBlinkDistance(float distance) { blinkDistance_ = distance; }

        float GetCurrentBlinkedDistance() const { return currentBlinkedDistance_; }
        void SetCurrentBlinkedDistance(float distance) { currentBlinkedDistance_ = distance; }

        float GetBlinkSpeed() const { return blinkSpeed_; }
        void SetBlinkSpeed(float speed) { blinkSpeed_ = speed; }

        size_t GetNormalMaterialHandleID() const { return normalMaterialHandleID_; }
        void SetNormalMaterialHandleID(size_t handleID) { normalMaterialHandleID_ = handleID; }

        size_t GetEnhancedMaterialHandleID() const { return enhancedMaterialHandleID_; }
        void SetEnhancedMaterialHandleID(size_t handleID) { enhancedMaterialHandleID_ = handleID; }

        bool IsGotDot() const { return isGotDot_; }
        void SetIsGotDot(bool isGotDot) { isGotDot_ = isGotDot; }

        bool IsGotCrystal() const { return isGotCrystal_; }
        void SetIsGotCrystal(bool isGotCrystal) { isGotCrystal_ = isGotCrystal; }

        bool IsGotItem() const { return IsGotItem_; }
        void SetIsGotItem(bool isGotItem) { IsGotItem_ = isGotItem; }

        bool IsDead() const { return isDead_; }
        void SetIsDead(bool isDead) { isDead_ = isDead; }

        uint32_t GetDeadCount() const { return deadCount_; }
        void SetDeadCount(uint32_t count) { deadCount_ = count; }

        uint32_t GetDotCollectCount() const { return dotCollectCount_; }
        void SetDotCollectCount(uint32_t count) { dotCollectCount_ = count; }

        uint32_t GetCrystalCollectCount() const { return crystalCollectCount_; }
        void SetCrystalCollectCount(uint32_t count) { crystalCollectCount_ = count; }

        uint32_t GetTotalDotCollectCount() const { return totalDotCollectCount_; }
        void SetTotalDotCollectCount(uint32_t count) { totalDotCollectCount_ = count; }

        uint32_t GetTotalCrystalCollectCount() const { return totalCrystalCollectCount_; }
        void SetTotalCrystalCollectCount(uint32_t count) { totalCrystalCollectCount_ = count; }
    };

    extern BDC_PLAYER_API riaecs::ComponentRegistrar
    <ComponentPlayer, ComponentPlayerMaxCount> ComponentPlayerID;

} // namespace bdc_player