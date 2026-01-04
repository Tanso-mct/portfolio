#pragma once

#include "windows_base/include/dll_config.h"

#include <Windows.h>
#include <memory>
#include <string_view>

namespace wb
{
    WINDOWS_BASE_API void CreateWindowWB
    (
        HWND& hWnd, DWORD windowStyle, LPCWSTR windowName, 
        const UINT& posX, const UINT& posY, const UINT& width, const UINT& height,
        HWND hWndParent, WNDCLASSEX& wc
    );
    WINDOWS_BASE_API void ShowWindowWB(HWND& hWnd);
    WINDOWS_BASE_API void CreateStandardConsole();

    WINDOWS_BASE_API void GetMonitorSize(HWND hWnd, UINT& width, UINT& height);

    WINDOWS_BASE_API std::unique_ptr<unsigned char[]> LoadFileData(std::string_view filePath, fpos_t &size);

}