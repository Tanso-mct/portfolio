#pragma once
#include "windows_base/include/dll_config.h"

namespace wb
{
    class WINDOWS_BASE_API IDFactory
    {
    public:
        IDFactory() = default;
        ~IDFactory() = default;

        static size_t CreateComponentID();
        static size_t CreateSystemID();

        static size_t CreateFileLoaderID();
        static size_t CreateAssetID();
        static size_t CreateAssetFactoryID();

        static size_t CreateSceneFacadeID();

        static size_t CreateMonitorID();
        static size_t CreateMonitorFactoryID();

        static size_t CreateWindowID();

        static size_t CreateSharedFacadeID();

    };

} // namespace wb