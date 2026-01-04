#pragma once

#include "imgui/include/imgui_impl_win32.h"

#include "ecs/include/world.h"
#include "mono_service/include/service_proxy_manager.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace mono_forge
{

// Create initial entities in the ECS world
bool CreateInitialEntities(
    ecs::World& ecs_world, mono_service::ServiceProxyManager& service_proxy_manager);

// Initial window constants
constexpr const char* WINDOW_ENTITY_NAME = "Main Window";
constexpr const wchar_t* WINDOW_NAME = L"Mono Forge";
constexpr const wchar_t* WINDOW_CLASS_NAME = L"MonoForgeWindowClass";
constexpr uint32_t WINDOW_WIDTH = 1920;
constexpr uint32_t WINDOW_HEIGHT = 1080;
constexpr const char* WINDOW_FONT_PATH = "C:/Windows/Fonts/meiryo.ttc";
constexpr float WINDOW_FONT_SIZE = 18.0f;

// Window procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Create ImGui context
ImGuiContext* CreateImGuiContext(HWND hwnd);

// Destroy ImGui context
void DestroyImguiContext(ImGuiContext* context);

ecs::Entity CreateInitialWindowEntity(
    ecs::World& ecs_world, mono_service::ServiceProxyManager& service_proxy_manager);

// Initial scne constants
constexpr const char* SCENE_ENTITY_NAME = "Initial Scene";
constexpr uint32_t MAX_LIGHT_COUNT = 10;

ecs::Entity CreateInitialSceneEntity(
    ecs::World& ecs_world, mono_service::ServiceProxyManager& service_proxy_manager, 
    const ecs::Entity& window_entity);

} // namespace mono_forge