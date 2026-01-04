#include "mono_delta_time_test/pch.h"

#include "mono_delta_time/include/delta_time_provider.h"
#pragma comment(lib, "mono_delta_time.lib")

#include <thread>

TEST(Provider, UpdateTime)
{
    mono_delta_time::DeltaTimeProvider provider;

    // Initial delta time should be 0
    EXPECT_FLOAT_EQ(provider.GetDeltaTime(), 0.0f);

    // First update should still yield 0 delta time
    provider.UpdateTime();
    EXPECT_FLOAT_EQ(provider.GetDeltaTime(), 0.0f);

    // Wait for a short duration and update time again
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    provider.UpdateTime();
    float deltaTime = provider.GetDeltaTime();
    EXPECT_GT(deltaTime, 0.0f);
    EXPECT_LT(deltaTime, 0.2f); // Should be around 0.1 seconds

    // Wait again and update
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    provider.UpdateTime();
    float newDeltaTime = provider.GetDeltaTime();
    EXPECT_GT(newDeltaTime, 0.0f);
    EXPECT_LT(newDeltaTime, 0.3f); // Should be around 0.2 seconds

    // Ensure delta times are different
    EXPECT_NE(deltaTime, newDeltaTime);
}