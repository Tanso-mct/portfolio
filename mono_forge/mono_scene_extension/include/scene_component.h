#pragma once

#include <vector>
#include <DirectXMath.h>
#include <memory>

#include "ecs/include/entity.h"
#include "ecs/include/component.h"
#include "mono_scene_extension/include/dll_config.h"
#include "mono_scene_extension/include/scene_tag.h"
#include "mono_scene_extension/include/scene.h"

namespace mono_scene_extension
{

// Enum class for scene state
enum class SceneState
{
    NeedsLoad,
    Loading,
    Playing,
    NeedsRelease,
    Releasing,
    Released
};

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

// Default scene state
constexpr SceneState DEFAULT_SCENE_STATE = SceneState::NeedsLoad;

// The handle class for the component
class MONO_SCENE_EXT_DLL SceneComponentHandle : public ecs::ComponentHandle<SceneComponentHandle> {};

// The component class
class MONO_SCENE_EXT_DLL SceneComponent :
    public ecs::Component
{
public:
    SceneComponent(SceneID scene_id, std::unique_ptr<Scene> scene);
    virtual ~SceneComponent() override;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN// 
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            clear_color(DEFAULT_CLEAR_COLOR),
            max_light_count(DEFAULT_MAX_LIGHT_COUNT),
            shadow_bias(DEFAULT_SHADOW_BIAS),
            shadow_slope_scaled_bias(DEFAULT_SHADOW_SLOPE_SCALED_BIAS),
            shadow_slope_bias_exponent(DEFAULT_SHADOW_SLOPE_BIAS_EXPONENT),
            shadow_intensity(DEFAULT_SHADOW_INTENSITY),
            initial_state(DEFAULT_SCENE_STATE)
        {
        }
        virtual ~SetupParam() override = default;

        // The component IDs that the Window must have
        std::vector<ecs::ComponentID> required_window_component_ids = {};

        // The component IDs that the Camera must have
        std::vector<ecs::ComponentID> required_camera_component_ids = {};

        // The component IDs that the Renderable must have
        std::vector<ecs::ComponentID> required_renderable_component_ids = {};

        // The component IDs that the UI must have
        std::vector<ecs::ComponentID> required_ui_component_ids = {};

        // The component IDs that tha light might have
        std::vector<ecs::ComponentID> light_component_ids = {};

        // The clear color for this scene
        DirectX::XMFLOAT4 clear_color; //REFLECTED//

        // The maximum number of lights supported in this scene
        uint32_t max_light_count;

        // The shadow bias for this scene
        float shadow_bias;

        // The shadow slope scaled bias for this scene
        float shadow_slope_scaled_bias;

        // The shadow slope bias exponent for this scene
        float shadow_slope_bias_exponent;

        // The shadow intensity for this scene
        float shadow_intensity;

        // Initial state of the scene
        SceneState initial_state; //REFLECTED//

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get this scene's ID
    SceneID GetSceneID() const { return scene_id_; }

    // Get the scene instance
    Scene& GetScene() const { return *scene_; }

    // Get the window entities associated with this scene
    const ecs::Entity& GetWindowEntity() const { return window_entity_; }

    // Set the window entities associated with this scene
    void SetWindowEntity(ecs::Entity window_entities) { window_entity_ = std::move(window_entities); }

    // Get the component IDs that the Window must have
    const std::vector<ecs::ComponentID>& GetRequiredWindowComponentIDs() const { 
        return required_window_component_ids_; }

    // Get the main camera entity associated with this scene
    const ecs::Entity& GetMainCameraEntity() const { return main_camera_entity_; }

    // Set the main camera entity associated with this scene
    void SetMainCameraEntity(ecs::Entity main_camera_entity) { main_camera_entity_ = main_camera_entity; }

    // Get the component IDs that the Camera must have
    const std::vector<ecs::ComponentID>& GetRequiredCameraComponentIDs() const { 
        return required_camera_component_ids_; }

    // Get the renderable entities associated with this scene
    const std::vector<ecs::Entity>& GetRenderableEntities() const { return renderable_entities_; }

    // Set the renderable entities associated with this scene
    void SetRenderableEntities(std::vector<ecs::Entity> renderable_entities) { 
        renderable_entities_ = std::move(renderable_entities); }

    // Get the component IDs that the Renderable must have
    const std::vector<ecs::ComponentID>& GetRequiredRenderableComponentIDs() const { 
        return required_renderable_component_ids_; }

    // Get the UI entities associated with this scene
    const std::vector<ecs::Entity>& GetUIEntities() const { return ui_entities_; }

    // Set the UI entities associated with this scene
    void SetUIEntities(std::vector<ecs::Entity> ui_entities) { ui_entities_ = std::move(ui_entities); }

    // Get the component IDs that the UI must have
    const std::vector<ecs::ComponentID>& GetRequiredUIComponentIDs() const {
        return required_ui_component_ids_; }

    // Get the light entities associated with this scene
    const std::vector<std::pair<ecs::ComponentID, ecs::Entity>>& GetLightEntities() const { return light_entities_; }

    // Set the light entities associated with this scene
    void SetLightEntities(std::vector<std::pair<ecs::ComponentID, ecs::Entity>> light_entities) { light_entities_ = std::move(light_entities); }

    // Get the light component IDs that the light might have
    const std::vector<ecs::ComponentID>& GetLightComponentIDs() const { return light_component_ids_; }

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

    // Get the current state of the scene
    SceneState GetCurrentState() const { return current_state_; }

    // Set the current state of the scene to NeedsLoad
    void SetStateToNeedsLoad();

    // Set the current state of the scene to NeedsRelease
    void SetStateToNeedsRelease();

    // Set the current state of the scene
    // It can change to any state so be used with caution
    void SetCurrentState(SceneState state) { current_state_ = state; }

private:
    // This scene's ID
    const SceneID scene_id_;

    // The scene instance
    std::unique_ptr<Scene> scene_;

    // The window entity associated with this scene
    ecs::Entity window_entity_;

    // Component IDs that the Window must have
    std::vector<ecs::ComponentID> required_window_component_ids_;

    // The main camera entity associated with this scene
    ecs::Entity main_camera_entity_ = ecs::Entity();

    // Component IDs that the Camera must have
    std::vector<ecs::ComponentID> required_camera_component_ids_;

    // The renderable entities associated with this scene
    std::vector<ecs::Entity> renderable_entities_;

    // Component IDs that the Renderable must have
    std::vector<ecs::ComponentID> required_renderable_component_ids_;

    // The UI entities associated with this scene
    std::vector<ecs::Entity> ui_entities_;

    // Component IDs that the UI must have
    std::vector<ecs::ComponentID> required_ui_component_ids_;

    // The light entities associated with this scene
    std::vector<std::pair<ecs::ComponentID, ecs::Entity>> light_entities_;

    // Light component IDs that the light might have
    std::vector<ecs::ComponentID> light_component_ids_;

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

    // The current state of the scene
    SceneState current_state_ = DEFAULT_SCENE_STATE;
};

} // namespace mono_scene_extension