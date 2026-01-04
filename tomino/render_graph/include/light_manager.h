#pragma once

#include <memory>

#include "class_template/singleton.h"
#include "utility_header/container.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/light_handle.h"
#include "render_graph/include/light.h"

namespace render_graph
{

// The container type used to contain lights
using LightContainer = utility_header::Container<Light>;

// The resource manager class that manages lights and their handles
class RENDER_GRAPH_DLL LightManager :
    public class_template::Singleton<LightManager>
{
public:
    LightManager(LightContainer& container) : container_(container) {}
    ~LightManager() override = default;

    // Execute a function with a unique lock on the resource container
    template <typename Func>
    void WithLock(Func&& func) 
    {
        container_.WithUniqueLock([&](LightContainer& container) 
        {
            func(*this);
        });
    }

    // Get a light using its handle
    Light& GetLight(const LightHandle* handle);

    // Get a const light using its handle
    const Light& GetLight(const LightHandle* handle) const;

    // Wrap the Contains method of the container
    // It must be called in WithLock lambda function
    bool Contains(const LightHandle* handle) const;

private:
    // The resource container reference
    LightContainer& container_;
};

// Adder class for lights
class RENDER_GRAPH_DLL LightAdder
{
public:
    LightAdder(LightContainer& container) : container_(container) {}
    ~LightAdder() = default;

    // Adds a new resource and returns its associated LightHandle
    LightHandle AddLight(std::unique_ptr<Light> resource);

private:
    // The resource container reference
    LightContainer& container_;
};

// Eraser class for lights
class RENDER_GRAPH_DLL LightEraser
{
public:
    LightEraser(LightContainer& container) : container_(container) {}
    ~LightEraser() = default;

    // Erases a resource associated with the given LightHandle
    void EraseLight(const LightHandle* handle);

private:
    // The resource container reference
    LightContainer& container_;
};

} // namespace render_graph
