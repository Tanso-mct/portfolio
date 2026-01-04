#include "mono_delta_time/src/pch.h"
#include "mono_delta_time/include/delta_time_provider.h"

mono_delta_time::DeltaTimeProvider::DeltaTimeProvider()
{
    lastUpdateTime_ = std::chrono::high_resolution_clock::now();
    currentUpdateTime_ = lastUpdateTime_;

    deltaTime_ = 0.0f;
}

mono_delta_time::DeltaTimeProvider::~DeltaTimeProvider()
{
}

void mono_delta_time::DeltaTimeProvider::UpdateTime()
{
    if (isFirstUpdate_)
    {
        lastUpdateTime_ = std::chrono::high_resolution_clock::now();
        currentUpdateTime_ = lastUpdateTime_;
        isFirstUpdate_ = false;
        deltaTime_ = 0.0f;
        return;
    }

    // Get current time and calculate delta time
    currentUpdateTime_ = std::chrono::high_resolution_clock::now();
    deltaTime_ = std::chrono::duration<float>(currentUpdateTime_ - lastUpdateTime_).count();

    // Store current time as last update time for next frame
    lastUpdateTime_ = currentUpdateTime_;
}

float mono_delta_time::DeltaTimeProvider::GetDeltaTime() const
{
    return deltaTime_;
}
