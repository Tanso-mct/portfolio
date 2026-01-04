#pragma once

#include "utility_header/id.h"
#include "class_template/singleton.h"

#include "render_graph/include/dll_config.h"

namespace render_graph
{

// The type used to identify material handles
using MaterialHandle = utility_header::GenerationID;

// The type used to identify material types
using MaterialTypeHandleID = utility_header::ID;

// The ID generator for material handle types
class RENDER_GRAPH_DLL MaterialTypeHandleIDGenerator :
    public class_template::Singleton<MaterialTypeHandleIDGenerator>
{
public:
    MaterialTypeHandleIDGenerator() = default;
    virtual ~MaterialTypeHandleIDGenerator() override = default;

    // Generate a new unique MaterialTypeHandleID
    MaterialTypeHandleID Generate() { return next_id_++; }

private:
    MaterialTypeHandleID next_id_ = 1; // Start from 1 to avoid using 0 as a valid ID
};

template <typename HandleTag>
class MaterialTypeHandle
{
public:
    MaterialTypeHandle() = default;
    virtual ~MaterialTypeHandle() = default;

    // Get the unique ID of the material handle type
    static MaterialTypeHandleID ID()
    {
        static const MaterialTypeHandleID id = MaterialTypeHandleIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace render_graph