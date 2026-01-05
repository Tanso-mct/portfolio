#pragma once

#include <string>
#include <windows.h>
#include <commdlg.h>
#include <filesystem>

#pragma comment(lib, "Comdlg32.lib")

namespace utility_header
{

// Opens a Win32 file open dialog and returns the selected file path as a wide string.
inline std::wstring OpenFileDialogWin32(HWND owner = nullptr)
{
    wchar_t file_path[MAX_PATH] = L"";

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file_path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"All Files\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
        return file_path;

    return L"";
}

// Gets the directory of the current executable.
inline std::wstring GetExeDir()
{
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    std::filesystem::path exe_path = buf;
    return exe_path.parent_path().wstring();
}

// Gets the current working directory.
inline std::wstring GetWorkingDir()
{
    DWORD size = GetCurrentDirectoryW(0, nullptr);
    if (size == 0)
        return L"";

    std::wstring buffer(size, L'\0');
    DWORD len = GetCurrentDirectoryW(size, buffer.data());
    if (len == 0 || len >= size)
        return L"";

    buffer.resize(len);
    return buffer;
}

// Gets the relative path from base_path to full_path.
inline std::wstring GetRelativePath(const std::wstring& full_path, const std::wstring& base_path)
{
    std::filesystem::path full(full_path);
    std::filesystem::path base(base_path);
    std::filesystem::path relative = std::filesystem::relative(full, base);
    return relative.wstring();
}

inline std::wstring StringToWstring(const std::string& str)
{
    if (str.empty()) return std::wstring();

    int len = MultiByteToWideChar(
        CP_UTF8, 
        0,
        str.c_str(),
        -1,
        nullptr,
        0
    );
    if (len <= 0)
        return std::wstring();

    std::wstring result(len - 1, L'\0');

    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        -1,
        &result[0],
        len
    );

    return result;
}

} // namespace utility_header