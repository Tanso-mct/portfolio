#pragma once

#include <memory>
#include <cstdint>

#include "window_provider/include/dll_config.h"

namespace window_provider
{

// The base window class
class WINDOW_PROVIDER_DLL Window
{
public:
    Window() = default;
    virtual ~Window() = default;

    // The descriptor for creating a window
    class CreateDesc
    {
    public:
        CreateDesc() = default;
        virtual ~CreateDesc() = default;
    };

    // Create a window with the specified descriptor
    virtual bool Create(std::unique_ptr<CreateDesc> desc) = 0;

    // Destroy the window
    virtual void Destroy() = 0;

    // Show the window
    virtual bool Show() = 0;

    // Hide the window
    virtual bool Hide() = 0;

    // Resize the window
    virtual bool Resize(uint32_t width, uint32_t height) = 0;

    // Minimize the window
    virtual bool Maximize() = 0;

    // Minimize the window
    virtual bool Minimize() = 0;

    // Restore the window
    virtual bool Restore() = 0;

    // Fullscreen the window
    virtual bool Fullscreen() = 0;

    // Get the width of the window
    virtual uint32_t GetWidth() const = 0;

    // Get the height of the window
    virtual uint32_t GetHeight() const = 0;

    // Get the width of the client area
    virtual uint32_t GetClientWidth() const = 0;

    // Get the height of the client area
    virtual uint32_t GetClientHeight() const = 0;

    // Get the X position of the window
    virtual uint32_t GetPositionX() const = 0;

    // Get the Y position of the window
    virtual uint32_t GetPositionY() const = 0;
};

} // namespace window_provider