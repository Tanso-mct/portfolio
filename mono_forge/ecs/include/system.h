#pragma once

#include "class_template/singleton.h"
#include "utility_header/id.h"

#include "ecs/include/dll_config.h" // DLL export/import macros

namespace ecs
{

// Forward declaration
class World;

// The type used to identify system types
using SystemID = size_t;

// The singleton ID generator for system types
class ECS_DLL SystemIDGenerator :
    public class_template::Singleton<SystemIDGenerator>,
    public utility_header::IDGenerator
{
public:
    SystemIDGenerator() = default;
    virtual ~SystemIDGenerator() override = default;
};

// The interface for all systems
// It can be inherited to create specific system types.
class ECS_DLL System :
    public class_template::NonCopyable
{
public:
    System() = default;
    virtual ~System() = default;

    // Pre-update function called before the main update loop
    virtual bool PreUpdate(World& world);

    // The main update function for the system
    // It will be called every frame by the system loop
    virtual bool Update(World& world);

    // Post-update function called after the main update loop
    virtual bool PostUpdate(World& world);

    // Get the system ID
    virtual SystemID GetID() const = 0;
};

// The template class for system handles of specific types
template <typename SystemType>
class SystemHandle
{
public:
    SystemHandle() = default;
    virtual ~SystemHandle() = default;

    // Get the unique ID of the system type
    static SystemID ID()
    {
        static SystemID id = SystemIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace ecs