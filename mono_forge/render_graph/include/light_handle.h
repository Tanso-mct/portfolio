#pragma once

#include "utility_header/id.h"
#include "class_template/singleton.h"

#include "render_graph/include/dll_config.h"

namespace render_graph
{

// The type used to identify material handles
using LightHandle = utility_header::GenerationID;

// The type used to identify light types
using LightTypeHandleID = utility_header::ID;

// The ID generator for light handle types
class RENDER_GRAPH_DLL LightTypeHandleIDGenerator :
    public class_template::Singleton<LightTypeHandleIDGenerator>
{
public:
    LightTypeHandleIDGenerator() = default;
    virtual ~LightTypeHandleIDGenerator() override = default;

    // Generate a new unique LightTypeHandleID
    LightTypeHandleID Generate() { return next_id_++; }

private:
    LightTypeHandleID next_id_ = 1; // Start from 1 to avoid using 0 as a valid ID
};

template <typename HandleTag>
class LightTypeHandle
{
public:
    LightTypeHandle() = default;
    virtual ~LightTypeHandle() = default;

    // Get the unique ID of the light handle type
    static LightTypeHandleID ID()
    {
        static const LightTypeHandleID id = LightTypeHandleIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace render_graph