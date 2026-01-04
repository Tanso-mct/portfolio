#include "mono_physics/src/pch.h"
#include "mono_physics/include/collision_detector.h"

void mono_physics::CollisionDetectorRegistry::Register(
    ColliderPair pair, std::unique_ptr<CollisionDetector> detector)
{
    assert(detector != nullptr); // Ensure detector is not null
    assert(detectors_.find(pair) == detectors_.end()); // Ensure no duplicate registration

    // Register the detector
    detectors_[pair] = std::move(detector);
}

mono_physics::CollisionDetector& mono_physics::CollisionDetectorRegistry::Get(
    const ColliderPair &pair) const
{
    assert(detectors_.find(pair) != detectors_.end()); // Ensure the detector exists
    return *detectors_.at(pair);
}