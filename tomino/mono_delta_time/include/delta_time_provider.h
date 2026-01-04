#pragma once
#include "mono_delta_time/include/dll_config.h"

#include <chrono>

namespace mono_delta_time
{
    class MONO_DELTA_TIME_API DeltaTimeProvider
    {
    private:
        std::chrono::high_resolution_clock::time_point lastUpdateTime_;
        std::chrono::high_resolution_clock::time_point currentUpdateTime_;

        float deltaTime_ = 0.0; // In seconds
        bool isFirstUpdate_ = true;

    public:
        DeltaTimeProvider();
        ~DeltaTimeProvider();

        void UpdateTime();
        float GetDeltaTime() const;
    };
    

} // namespace mono_delta_time