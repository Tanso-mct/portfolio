#pragma once

#include "windows_base/include/interfaces/context.h"
#include "windows_base/include/interfaces/facade.h"

#include "windows_base/include/interfaces/scene.h"

#include <Windows.h>
#include <memory>
#include <string>
#include <vector>

namespace wb
{
    constexpr const wchar_t* TASK_BAR_CLASS_NAME = L"Shell_TrayWnd";

    constexpr UINT MINIMUM_WINDOW_WIDTH = 1;
    constexpr UINT MINIMUM_WINDOW_HEIGHT = 1;

    class IWindowContext : public IContext
    {
    public:
        virtual ~IWindowContext() = default;

        virtual HWND &Handle() = 0;
        virtual HINSTANCE &Instance() = 0;
        virtual HWND &ParentHandle() = 0;

        virtual DWORD &Style() = 0;
        virtual std::wstring &Name() = 0;
        virtual UINT &PosX() = 0;
        virtual UINT &PosY() = 0;
        virtual UINT &Width() = 0;
        virtual UINT &Height() = 0;
        virtual UINT &ClientWidth() = 0;
        virtual UINT &ClientHeight() = 0;

        virtual bool &IsCreated() = 0;
        virtual bool &IsFocus() = 0;
        virtual bool &IsFocused() = 0;
        virtual bool &IsUnFocused() = 0;
        virtual bool &IsMaximized() = 0;
        virtual bool &IsMinimized() = 0;
        virtual bool &IsFullScreen() = 0;

        virtual bool &NeedsResize() = 0;
        virtual bool &NeedsToQuitApp() = 0;
        virtual bool &NeedsShowWhenCreated() = 0;
    };

    constexpr UINT DEFAULT_WINDOW_WIDTH = 960;
    constexpr UINT DEFAULT_WINDOW_HEIGHT = 540;

    class IWindowFacade : public IFacade
    {
    public:
        virtual ~IWindowFacade() = default;

        /*******************************************************************************************************************
         * Get window's information
        /******************************************************************************************************************/

        virtual const HWND &GetHandle() const = 0;
        virtual std::wstring_view GetName() const = 0;

        virtual const UINT &GetClientWidth() const = 0;
        virtual const UINT &GetClientHeight() const = 0;

        virtual bool IsCreated() const = 0;
        virtual bool NeedsResize() const = 0;
        virtual bool NeedsToQuitApp() const = 0;
        virtual bool NeedsShowWhenCreated() const = 0;

        virtual bool IsFocusing() const = 0;
        virtual bool &IsFocused() = 0; // Remains true until False is substituted
        virtual bool &IsUnFocused() = 0; // Remains true until False is substituted

        virtual bool IsMaximizing() const = 0;
        virtual bool IsMinimizing() const = 0;
        virtual bool IsFullScreen() const = 0;

        /***************************************************************************************************************
         * Monitor related
        /**************************************************************************************************************/

        virtual void AddMonitorID(size_t monitorID) = 0;
        virtual const std::vector<size_t> &GetMonitorIDs() const = 0;

        /***************************************************************************************************************
         * The processing to be done to each window and the processing 
         * that needs to be done after that processing is actually done, etc.
         * 
         * Present tense methods are methods for changing the state of the window
         * Past tense methods are methods for confirming the state of the window after the change
        /**************************************************************************************************************/

        virtual void Create(WNDCLASSEX& wc) = 0;
        
        virtual void Show() = 0;
        virtual void Hide() = 0;

        virtual void Destroy() = 0;
        virtual void Destroyed() = 0;

        virtual void Resize(UINT width, UINT height) = 0;
        virtual void Resized() = 0;

        virtual void Focus() = 0;
        virtual void Focused() = 0;
        virtual void UnFocused() = 0;

        virtual void Maximize() = 0;
        virtual void Maximized() = 0;

        virtual void Minimize() = 0;
        virtual void Minimized() = 0;

        virtual void FullScreen() = 0;
        virtual void FullScreened() = 0;

        virtual void Restore() = 0;
        virtual void Restored() = 0;

        virtual void Move(UINT posX, UINT posY) = 0;
        virtual void Moved() = 0;
    };

    class ContainerStorage;

    class IWindowEvent
    {
    public:
        virtual ~IWindowEvent() = default;

        virtual void SetWindowID(size_t windowID) = 0;
        virtual void SetSceneUpdator(std::unique_ptr<ISceneUpdator> sceneUpdator) = 0;
        virtual void SetKeyboardMonitorID(size_t keyboardMonitorID) = 0;
        virtual void SetMouseMonitorID(size_t mouseMonitorID) = 0;
        virtual bool CheckIsReady() const = 0;

        virtual void OnEvent(ContainerStorage &contStorage, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
    };

} // namespace wb