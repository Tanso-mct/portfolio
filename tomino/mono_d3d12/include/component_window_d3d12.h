#pragma once
#include "mono_d3d12/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_d3d12/include/window_message_state.h"
#include "mono_input_monitor/input_monitor.h"

#include <Windows.h>
#include <shared_mutex>
#include <vector>
#include <DirectXMath.h>
#include <string_view>

namespace mono_d3d12
{
    constexpr UINT DEFAULT_WINDOW_WIDTH = 960;
    constexpr UINT DEFAULT_WINDOW_HEIGHT = 540;

    constexpr UINT64 INITIAL_FENCE_VALUE = 0;

    struct MONO_D3D12_API WindowD3D12Info
    {
    private:
        HWND handle_ = nullptr;
        HINSTANCE instance_ = nullptr;
        HWND parentHandle_ = nullptr;

        DWORD style_ = WS_OVERLAPPEDWINDOW;
        std::wstring name_ = L"Window";
        std::wstring className_ = L"WindowClass";
        UINT posX_ = CW_USEDEFAULT;
        UINT posY_ = CW_USEDEFAULT;
        UINT width_ = DEFAULT_WINDOW_WIDTH;
        UINT height_ = DEFAULT_WINDOW_HEIGHT;
        UINT clientWidth_ = 0;
        UINT clientHeight_ = 0;
        float dpiScale_ = 1.0f;

    public:
        WindowD3D12Info() = default;
        ~WindowD3D12Info() = default;

        HWND GetHandle() const { return handle_; }
        void SetHandle(HWND handle) { handle_ = handle; }

        HINSTANCE GetInstance() const { return instance_; }
        void SetInstance(HINSTANCE instance) { instance_ = instance; }

        HWND GetParentHandle() const { return parentHandle_; }
        void SetParentHandle(HWND parentHandle) { parentHandle_ = parentHandle; }

        DWORD GetStyle() const { return style_; }
        void SetStyle(DWORD style) { style_ = style; }

        const std::wstring &GetName() const { return name_; }
        void SetName(const std::wstring &name) { name_ = name; }

        const std::wstring &GetItClassName() const { return className_; }
        void SetItClassName(const std::wstring &className) { className_ = className; }

        UINT GetPosX() const { return posX_; }
        void SetPosX(UINT posX) { posX_ = posX; }

        UINT GetPosY() const { return posY_; }
        void SetPosY(UINT posY) { posY_ = posY; }

        UINT GetWidth() const { return width_; }
        void SetWidth(UINT width) { width_ = width; }

        UINT GetHeight() const { return height_; }
        void SetHeight(UINT height) { height_ = height; }

        UINT GetClientWidth() const { return clientWidth_; }
        void SetClientWidth(UINT clientWidth) { clientWidth_ = clientWidth; }

        UINT GetClientHeight() const { return clientHeight_; }
        void SetClientHeight(UINT clientHeight) { clientHeight_ = clientHeight; }

        float GetDpiScale() const { return dpiScale_; }
        void SetDpiScale(float dpiScale) { dpiScale_ = dpiScale; }
    };

    class MONO_D3D12_API WindowD3D12State
    {
    private:
        bool isCreated_ = false;
        bool isFocused_ = false;
        bool isUnFocused_ = false;
        bool isMaximized_ = false;
        bool isMinimized_ = false;
        bool isFullScreened_ = false;

    public:
        WindowD3D12State();
        ~WindowD3D12State();

        bool IsCreated() const { return isCreated_; }
        void SetCreated(bool created) { isCreated_ = created; }

        bool IsFocused() const { return isFocused_; }
        void SetFocused(bool focused) { isFocused_ = focused; }

        bool IsUnFocused() const { return isUnFocused_; }
        void SetUnFocused(bool unFocused) { isUnFocused_ = unFocused; }

        bool IsMaximized() const { return isMaximized_; }
        void SetMaximized(bool maximized) { isMaximized_ = maximized; }

        bool IsMinimized() const { return isMinimized_; }
        void SetMinimized(bool minimized) { isMinimized_ = minimized; }

        bool IsFullScreened() const { return isFullScreened_; }
        void SetFullScreened(bool fullScreened) { isFullScreened_ = fullScreened; }
    };

    class MONO_D3D12_API WindowD3D12Action
    {
    private:
        bool needsQuitAppWhenDestroyed_ = false;

    public:
        WindowD3D12Action();
        ~WindowD3D12Action();

        bool NeedsQuitAppWhenDestroyed() const { return needsQuitAppWhenDestroyed_; }
        void SetNeedsQuitAppWhenDestroyed(bool needsQuitAppWhenDestroyed) { needsQuitAppWhenDestroyed_ = needsQuitAppWhenDestroyed; }
    };

    constexpr size_t ComponentWindowD3D12MaxCount = 10;
    class MONO_D3D12_API ComponentWindowD3D12
    {
    private:
        WindowD3D12Info info_;
        WindowD3D12State state_;
        WindowD3D12Action action_;

        mono_input_monitor::KeyboardInputState keyboardState_;
        mono_input_monitor::MouseInputState mouseState_;

        riaecs::Entity sceneEntity_ = riaecs::Entity();

    public:
        ComponentWindowD3D12();
        ~ComponentWindowD3D12();
        
        struct SetupParam
        {
            std::wstring windowName_ = L"Default Window";
            std::wstring windowClassName_ = L"Default WindowClass";
            UINT windowWidth_ = DEFAULT_WINDOW_WIDTH;
            UINT windowHeight_ = DEFAULT_WINDOW_HEIGHT;
            UINT windowPosX_ = CW_USEDEFAULT;
            UINT windowPosY_ = CW_USEDEFAULT;
            riaecs::Entity sceneEntity_ = riaecs::Entity();
            bool needsQuitAppWhenDestroyed_ = true;
        };
        void Setup(const ComponentWindowD3D12::SetupParam &params);

        /***************************************************************************************************************
         * Other systems accessible functions
        /**************************************************************************************************************/

        void Create();
        void Destroy();
        void Resize(UINT width, UINT height);
        void Show();
        void Hide();
        void Focus();
        void UnFocus();
        void Maximize();
        void Minimize();
        void FullScreen();
        void Restore();
        void SetPosition(UINT x, UINT y);

        void SetSceneEntity(riaecs::Entity sceneEntity) { sceneEntity_ = sceneEntity; }

        /***************************************************************************************************************
         * Getters
        /**************************************************************************************************************/

        const WindowD3D12Info &GetInfo() const { return info_; }
        const WindowD3D12State &GetState() const { return state_; }
        const WindowD3D12Action &GetAction() const { return action_; }

        const mono_input_monitor::KeyboardInputState &GetKeyboardState() const { return keyboardState_; }
        const mono_input_monitor::MouseInputState &GetMouseState() const { return mouseState_; }
        riaecs::Entity GetSceneEntity() const { return sceneEntity_; }

        /***************************************************************************************************************
         * Corresponding system accessible functions
        /**************************************************************************************************************/
    private:
        friend class SystemWindowD3D12; // Corresponding system

        void OnDestroyed();
        void OnResized();
        void OnFocused();
        void OnUnFocused();
        void OnMaximized();
        void OnMinimized();
        void OnFullScreened();
        void OnRestored();
        void OnMoved();
    };
    extern MONO_D3D12_API riaecs::ComponentRegistrar<ComponentWindowD3D12, ComponentWindowD3D12MaxCount> ComponentWindowD3D12ID;

} // namespace mono_d3d12