#pragma once

#include <DirectXMath.h>

#include "transform_evaluator/include/dll_config.h"

namespace transform_evaluator 
{

// The structure representing Translation, Rotation, and Scale
struct TRS
{
    DirectX::XMFLOAT3 translation = {0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT4 rotation = {0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion
    DirectX::XMFLOAT3 scale = {1.0f, 1.0f, 1.0f};
};

// The Transform class encapsulating local and world TRS
class TRANSFORM_EVALUATOR_DLL Transform 
{
public:
    Transform(
        DirectX::XMFLOAT3 translation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
        DirectX::XMFLOAT4 rotation = DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f},
        DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f});

    virtual ~Transform() = default;

    // Get world TRS
    const TRS& GetWorldTRS() const;

    // Set world TRS
    void SetWorldTRS(const TRS& trs);

    // Get previous world TRS
    const TRS& GetPreviousWorldTRS() const;

    // Get local TRS
    const TRS& GetLocalTRS() const;

    // Set local TRS
    void SetLocalTRS(const TRS& trs);

    // Get previous local TRS
    const TRS& GetPreviousLocalTRS() const;

private:
    // World TRS
    TRS world_trs_ = TRS();

    // Previous World TRS
    // Used for calculating delta transformations
    TRS previous_world_trs_ = TRS();

    // Local TRS
    TRS local_trs_ = TRS();

    // Previous Local TRS
    // Used for calculating delta transformations
    TRS previous_local_trs_ = TRS();
};

} // namespace transform_evaluator