#include "windows_base/src/pch.h"
#include "windows_base/include/type_cast.h"

WINDOWS_BASE_API void wb::SpecifiedAs(HRESULT src, std::string &dst)
{
    char* msgBuf = nullptr;
    DWORD size = FormatMessageA
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        src,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPSTR)&msgBuf,
        0, nullptr
    );

    dst = (size && msgBuf) ? msgBuf : "Unknown error";
    if (msgBuf) LocalFree(msgBuf);
}