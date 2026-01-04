#pragma once
#include "mono_scene/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_scene/include/entities_factory.h"

#include <memory>
#include <vector>
#include <shared_mutex>
#include <directxmath.h>

namespace mono_scene
{
    // Default clear color
    const DirectX::XMFLOAT4 DEFAULT_CLEAR_COLOR = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Default maximum number of lights
    constexpr uint32_t DEFAULT_MAX_LIGHT_COUNT = 10;

    // Default shadow bias
    constexpr float DEFAULT_SHADOW_BIAS = 0.005f;

    // Default shadow slope scaled bias
    constexpr float DEFAULT_SHADOW_SLOPE_SCALED_BIAS = 0.001f;

    // Default shadow slope bias exponent
    constexpr float DEFAULT_SHADOW_SLOPE_BIAS_EXPONENT = 100.0f;

    // Default shadow intensity
    constexpr float DEFAULT_SHADOW_INTENSITY = 0.2f;

    constexpr size_t ComponentSceneMaxCount = 50;
    class MONO_SCENE_API ComponentScene
    {
    private:
        mutable std::shared_mutex mutex_;

        std::unique_ptr<IEntitiesFactory> entitiesFactory_ = nullptr;
        riaecs::StagingEntityArea stagingEntityArea_;

        std::vector<size_t> assetSourceIDs_;
        std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>> systemListEditCmds_;

        bool isLoaded_ = false;
        bool isReleased_ = true;
        bool isSystemListEdited_ = false;

        bool needsLoad_ = false;
        bool needsRelease_ = false;

        bool needsEditSystemList_ = false;
        size_t targetEditCmdIndex_ = 0;

        // The clear color for this scene
        DirectX::XMFLOAT4 clear_color_ = DEFAULT_CLEAR_COLOR;

        // The maximum number of lights supported in this scene
        uint32_t max_light_count_ = DEFAULT_MAX_LIGHT_COUNT;

        // The shadow bias for this scene
        float shadow_bias_ = DEFAULT_SHADOW_BIAS;

        // The shadow slope scaled bias for this scene
        float shadow_slope_scaled_bias_ = DEFAULT_SHADOW_SLOPE_SCALED_BIAS;

        // The shadow slope bias exponent for this scene
        float shadow_slope_bias_exponent_ = DEFAULT_SHADOW_SLOPE_BIAS_EXPONENT;

        // The shadow intensity for this scene
        float shadow_intensity_ = DEFAULT_SHADOW_INTENSITY;

        // Cached file data for asset sources
        std::unordered_map<std::string_view, std::unique_ptr<riaecs::IFileData>> fileDatas_;

    public:
        ComponentScene();
        ~ComponentScene();

        ComponentScene(const ComponentScene&) = delete;
        ComponentScene& operator=(const ComponentScene&) = delete;

        struct SetupParam
        {
            std::unique_ptr<IEntitiesFactory> entitiesFactory_ = nullptr;
            std::vector<size_t> assetSourceIDs_;
            std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>> systemListEditCmds_;
            size_t targetEditCmdIndex_ = 0;

            bool needsLoad_ = true;
            bool needsEditSystemList_ = true;
            DirectX::XMFLOAT4 clear_color_ = DEFAULT_CLEAR_COLOR;
        };
        void Setup(SetupParam &param);

        template<typename F>
        void WithLock(F&& func)
        {
            std::unique_lock lock(mutex_);
            func(*this);
        }

        riaecs::ROObject<std::unique_ptr<IEntitiesFactory>> EntitiesFactoryRO() const { 
            return riaecs::ROObject<std::unique_ptr<IEntitiesFactory>>(std::shared_lock<std::shared_mutex>(mutex_), entitiesFactory_); }
        riaecs::RWObject<std::unique_ptr<IEntitiesFactory>> EntitiesFactoryRW() { 
            return riaecs::RWObject<std::unique_ptr<IEntitiesFactory>>(std::unique_lock<std::shared_mutex>(mutex_), entitiesFactory_); }

        riaecs::ROObject<riaecs::StagingEntityArea> StagingEntityAreaRO() const { 
            return riaecs::ROObject<riaecs::StagingEntityArea>(std::shared_lock<std::shared_mutex>(mutex_), stagingEntityArea_); }
        riaecs::RWObject<riaecs::StagingEntityArea> StagingEntityAreaRW() { 
            return riaecs::RWObject<riaecs::StagingEntityArea>(std::unique_lock<std::shared_mutex>(mutex_), stagingEntityArea_); }

        riaecs::ROObject<std::vector<size_t>> AssetSourceIDsRO() const { 
            return riaecs::ROObject<std::vector<size_t>>(std::shared_lock<std::shared_mutex>(mutex_), assetSourceIDs_); }
        riaecs::RWObject<std::vector<size_t>> AssetSourceIDsRW() { 
            return riaecs::RWObject<std::vector<size_t>>(std::unique_lock<std::shared_mutex>(mutex_), assetSourceIDs_); }

        riaecs::ROObject<std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>>> SystemListEditCommandsRO() const { 
            return riaecs::ROObject<std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>>>(std::shared_lock<std::shared_mutex>(mutex_), systemListEditCmds_); }
        riaecs::RWObject<std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>>> SystemListEditCommandsRW() { 
            return riaecs::RWObject<std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>>>(std::unique_lock<std::shared_mutex>(mutex_), systemListEditCmds_); }

        bool IsLoaded() const;
        riaecs::RWObject<bool> IsLoadedRW() { 
            return riaecs::RWObject<bool>(std::unique_lock<std::shared_mutex>(mutex_), isLoaded_); }

        bool IsReleased() const;
        riaecs::RWObject<bool> IsReleasedRW() { 
            return riaecs::RWObject<bool>(std::unique_lock<std::shared_mutex>(mutex_), isReleased_); }

        bool IsSystemListEdited() const;
        riaecs::RWObject<bool> IsSystemListEditedRW() { 
            return riaecs::RWObject<bool>(std::unique_lock<std::shared_mutex>(mutex_), isSystemListEdited_); }

        bool NeedsLoad() const;
        riaecs::RWObject<bool> NeedsLoadRW() { 
            return riaecs::RWObject<bool>(std::unique_lock<std::shared_mutex>(mutex_), needsLoad_); }

        bool NeedsRelease() const;
        riaecs::RWObject<bool> NeedsReleaseRW() { 
            return riaecs::RWObject<bool>(std::unique_lock<std::shared_mutex>(mutex_), needsRelease_); }

        bool NeedsEditSystemList() const;
        riaecs::RWObject<bool> NeedsEditSystemListRW() { 
            return riaecs::RWObject<bool>(std::unique_lock<std::shared_mutex>(mutex_), needsEditSystemList_); }

        size_t TargetEditCommandIndex() const;
        riaecs::RWObject<size_t> TargetEditCommandIndexRW() { 
            return riaecs::RWObject<size_t>(std::unique_lock<std::shared_mutex>(mutex_), targetEditCmdIndex_); }

        // Get the clear color for this scene
        const DirectX::XMFLOAT4& GetClearColor() const { return clear_color_; }

        // Set the clear color for this scene
        void SetClearColor(const DirectX::XMFLOAT4& clear_color) { clear_color_ = clear_color; }

        // Get the maximum number of lights supported in this scene
        const uint32_t GetMaxLightCount() const { return max_light_count_; }

        // Set the maximum number of lights supported in this scene
        void SetMaxLightCount(uint32_t max_light_count) { max_light_count_ = max_light_count; }

        // Get the shadow intensity for this scene
        float GetShadowBias() const { return shadow_bias_; }

        // Set the shadow intensity for this scene
        void SetShadowBias(float shadow_bias) { shadow_bias_ = shadow_bias; }

        // Get the shadow slope scaled bias for this scene
        float GetShadowSlopeScaledBias() const { return shadow_slope_scaled_bias_; }

        // Set the shadow slope scaled bias for this scene
        void SetShadowSlopeScaledBias(float shadow_slope_scaled_bias) { 
            shadow_slope_scaled_bias_ = shadow_slope_scaled_bias; }

        // Get the shadow slope bias exponent for this scene
        float GetShadowSlopeBiasExponent() const { return shadow_slope_bias_exponent_; }

        // Set the shadow slope bias exponent for this scene
        void SetShadowSlopeBiasExponent(float shadow_slope_bias_exponent) { 
            shadow_slope_bias_exponent_ = shadow_slope_bias_exponent; }

        // Get the shadow intensity for this scene
        float GetShadowIntensity() const { return shadow_intensity_; }

        // Set the shadow intensity for this scene
        void SetShadowIntensity(float shadow_intensity) { shadow_intensity_ = shadow_intensity; }

        // Get cached file data for asset sources
        std::unordered_map<std::string_view, std::unique_ptr<riaecs::IFileData>>& GetFileDatas() { return fileDatas_; }
    };
    extern MONO_SCENE_API riaecs::ComponentRegistrar<ComponentScene, ComponentSceneMaxCount> ComponentSceneID;

    MONO_SCENE_API void LoadScene
    (
        riaecs::Entity sceneEntity, ComponentScene *component, 
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont
    );
    
    MONO_SCENE_API void ReleaseScene
    (
        const riaecs::Entity &sceneEntity, ComponentScene *component, 
        riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont
    );

    MONO_SCENE_API void AddSystemListEditCommand(ComponentScene *component, riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue);

}