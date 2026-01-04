#pragma once

#include <functional>
#include <vector>

#include "class_template/singleton.h"
#include "class_template/instance.h"
#include "utility_header/id.h"
#include "directx12_util/include/wrapper.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/resource_manager.h"
#include "render_graph/include/render_pass_context.h"

namespace render_graph
{

// Forward declaration
class RenderGraph;
class RenderPass;

// The type used to identify render pass handles
using RenderPassHandleID = utility_header::ID;

// The ID generator for render pass handles
class RENDER_GRAPH_DLL RenderPassIDGenerator :
    public class_template::Singleton<RenderPassIDGenerator>,
    public utility_header::IDGenerator
{
public:
    RenderPassIDGenerator() = default;
    ~RenderPassIDGenerator() = default;
};

// The template class for render pass handles of specific types
// It must only be used on the EXE side
template <typename HandleTag>
class RenderPassHandle
{
public:
    RenderPassHandle() = default;
    virtual ~RenderPassHandle() = default;

    // Get the unique ID of the render pass handle type
    static RenderPassHandleID ID()
    {
        static const RenderPassHandleID id = RenderPassIDGenerator::GetInstance().Generate();
        return id;
    }
};

// Builder for setting up a RenderPass
class RENDER_GRAPH_DLL RenderPassBuilder
{
public:
    RenderPassBuilder(RenderPass& pass) : 
        render_pass_(pass) 
    {
    }

    ~RenderPassBuilder() = default;

    // Declare that the render pass will read from a resource
    void Read(const ResourceHandle* handle);

    // Declare that the render pass will write to a resource
    void Write(const ResourceHandle* handle);

private:
    RenderPass& render_pass_;
};

// A render pass in the render graph
// Each render pass has a unique handle ID, setup function, and execute function
class RENDER_GRAPH_DLL RenderPass :
    public class_template::NonCopyable
{
public:
    using SetupFunc = std::function<bool(RenderPassBuilder&)>;
    using ExecuteFunc = std::function<bool(RenderPass&, RenderPassContext&)>;

    RenderPass(RenderPassHandleID handleID, SetupFunc setup, ExecuteFunc execute) : 
        handle_id_(handleID),
        setup_func_(std::move(setup)), 
        execute_func_(std::move(execute))
    {
    }

    ~RenderPass() = default;

    // Get the unique handle ID of the render pass
    RenderPassHandleID GetHandleID() const { return handle_id_; }

    // Setup the render pass
    bool Setup(RenderPassBuilder& builder);

    // Execute the render pass
    bool Execute(RenderPassContext& context);

    // Get read access token
    ResourceAccessToken& GetReadToken() { return read_token_; }

    // Get write access token
    ResourceAccessToken& GetWriteToken() { return write_token_; }

private:
    // The unique handle ID of the render pass
    const RenderPassHandleID handle_id_;

    // Function to setup the render pass
    const SetupFunc setup_func_;

    // Function to execute the render pass
    const ExecuteFunc execute_func_;

    // Tokens for resource read access
    ResourceAccessToken read_token_;

    // Tokens for resource write access
    ResourceAccessToken write_token_;
};

// Base class for render passes defined as classes
// Inherit from this class and implement AddToGraph to add the pass to a RenderGraph
class RenderPassBase
{
public:
    RenderPassBase() = default;
    virtual ~RenderPassBase() = default;

    // Add self to the render graph
    virtual bool AddToGraph(RenderGraph& render_graph) = 0;
};

} // namespace render_graph