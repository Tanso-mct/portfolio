#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/window.h"
#include "windows_base/include/interfaces/scene.h"
#include "windows_base/include/container_storage.h"

#include <unordered_map>

namespace wb
{
    class WindowContext : public IWindowContext
    {
    private:
        HWND handle_ = nullptr;
        HINSTANCE instance_ = nullptr;
        HWND parentHandle_ = nullptr;

        DWORD style_ = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        std::wstring name_ = L"";
        UINT posX_ = CW_USEDEFAULT;
        UINT posY_ = CW_USEDEFAULT;
        UINT width_ = DEFAULT_WINDOW_WIDTH;
        UINT height_ = DEFAULT_WINDOW_HEIGHT;
        UINT clientWidth_ = 0;
        UINT clientHeight_ = 0;

        bool isCreated_ = false;
        bool isFocus_ = true;
        bool isFocused_ = true;
        bool isUnFocused_ = false;
        bool isMaximized_ = false;
        bool isMinimized_ = false;
        bool isFullScreen_ = false;

        bool needsResize_ = false;
        bool needsToQuitApp_ = true;
        bool needsShowWhenCreated_ = true;

    public:
        WindowContext() = default;
        virtual ~WindowContext() override = default;

        HWND &Handle() override { return handle_; }
        HINSTANCE &Instance() override { return instance_; }
        HWND &ParentHandle() override { return parentHandle_; }

        DWORD &Style() override { return style_; }
        std::wstring &Name() override { return name_; }
        UINT &PosX() override { return posX_; }
        UINT &PosY() override { return posY_; }
        UINT &Width() override { return width_; }
        UINT &Height() override { return height_; }
        UINT &ClientWidth() override { return clientWidth_; }
        UINT &ClientHeight() override { return clientHeight_; }

        bool &IsCreated() override { return isCreated_; }
        bool &IsFocus() override { return isFocus_; }
        bool &IsFocused() override { return isFocused_; }
        bool &IsUnFocused() override { return isUnFocused_; }
        bool &IsMaximized() override { return isMaximized_; }
        bool &IsMinimized() override { return isMinimized_; }
        bool &IsFullScreen() override { return isFullScreen_; }

        bool &NeedsResize() override { return needsResize_; }
        bool &NeedsToQuitApp() override { return needsToQuitApp_; }
        bool &NeedsShowWhenCreated() { return needsShowWhenCreated_; }
    };

    class WINDOWS_BASE_API DefaultWindowFacade : public IWindowFacade
    {
    protected:
        std::unique_ptr<IWindowContext> context_;

        std::vector<size_t> monitorIDs_;
        std::unordered_map<size_t, size_t> monitorTypeToIDMap_;

    public:
        DefaultWindowFacade() = default;
        virtual ~DefaultWindowFacade() override = default;

        DefaultWindowFacade(const DefaultWindowFacade &) = delete;
        DefaultWindowFacade &operator=(const DefaultWindowFacade &) = delete;

        /***************************************************************************************************************
         * IFacade implementation
        /**************************************************************************************************************/

        virtual void SetContext(std::unique_ptr<IContext> context) override;
        virtual bool CheckIsReady() const override;

        /***************************************************************************************************************
         * IWindowFacade implementation
        /**************************************************************************************************************/
        
        virtual const HWND &GetHandle() const override;
        virtual std::wstring_view GetName() const override;

        virtual const UINT &GetClientWidth() const override;
        virtual const UINT &GetClientHeight() const override;

        virtual bool IsCreated() const override;
        virtual bool NeedsResize() const override;
        virtual bool NeedsToQuitApp() const override;
        virtual bool NeedsShowWhenCreated() const override;

        virtual bool IsFocusing() const override;
        virtual bool &IsFocused() override;
        virtual bool &IsUnFocused() override;

        virtual bool IsMaximizing() const override;
        virtual bool IsMinimizing() const override;
        virtual bool IsFullScreen() const override;

        virtual void AddMonitorID(size_t monitorID) override;
        virtual const std::vector<size_t> &GetMonitorIDs() const override;

        virtual void Create(WNDCLASSEX& wc) override;

        virtual void Show() override;
        virtual void Hide() override;

        virtual void Destroy() override;
        virtual void Destroyed() override;

        virtual void Resize(UINT width, UINT height) override;
        virtual void Resized() override;

        virtual void Focus() override;
        virtual void Focused() override;
        virtual void UnFocused() override;

        virtual void Maximize() override;
        virtual void Maximized() override;

        virtual void Minimize() override;
        virtual void Minimized() override;

        virtual void FullScreen() override;
        virtual void FullScreened() override;

        virtual void Restore() override;
        virtual void Restored() override;

        virtual void Move(UINT posX, UINT posY) override;
        virtual void Moved() override;
    };

    class WINDOWS_BASE_API DefaultWindowEvent : public IWindowEvent
    {
    protected:
        size_t windowID_ = 0;
        bool isWindowIDSet_ = false;

        size_t keyboardMonitorID_ = 0;
        bool isKeyboardMonitorIDSet_ = false;

        size_t mouseMonitorID_ = 0;
        bool isMouseMonitorIDSet_ = false;

        std::unique_ptr<ISceneUpdator> sceneUpdator_ = nullptr;

    public:
        DefaultWindowEvent() = default;
        virtual ~DefaultWindowEvent() override = default;

        DefaultWindowEvent(const DefaultWindowEvent &) = delete;
        DefaultWindowEvent &operator=(const DefaultWindowEvent &) = delete;

        /***************************************************************************************************************
         * IWindowEvent implementation
        /**************************************************************************************************************/

        virtual void SetWindowID(size_t windowID) override;
        virtual void SetSceneUpdator(std::unique_ptr<ISceneUpdator> sceneUpdator) override;
        virtual void SetKeyboardMonitorID(size_t keyboardMonitorID) override;
        virtual void SetMouseMonitorID(size_t mouseMonitorID) override;
        virtual bool CheckIsReady() const override;

        virtual void OnEvent(ContainerStorage &contStorage, UINT msg, WPARAM wParam, LPARAM lParam) override;
    };

} // namespace wb