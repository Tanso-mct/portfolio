#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "utility_header/registry.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_pass.h"

namespace render_graph
{

// Node structure for render graph topological sorting
struct RenderGraphNode
{
    RenderPassHandleID passID;
    std::vector<RenderGraphNode*> dependencies;
};

// The render graph class
// Manages render passes and their execution order based on dependencies
class RENDER_GRAPH_DLL RenderGraph :
    public class_template::NonCopyable
{
public:
    RenderGraph() = default;
    ~RenderGraph() = default;

    // Add a render pass to the graph
    bool AddPass(
        RenderPassHandleID handleID, RenderPass::SetupFunc setup, RenderPass::ExecuteFunc execute);

    // Compile the render graph (resolve dependencies, etc.)
    bool Compile();

    // Execute the render graph
    bool Execute(RenderPassContext& context);

    // Clear the render graph
    void Clear();

private:
    // List of render passes in the graph
    std::unordered_map<RenderPassHandleID, std::unique_ptr<RenderPass>> pass_map_;

    // List of added render pass handle IDs
    // It sames the order in which passes were added
    std::vector<RenderPassHandleID> added_pass_order_;

    // Sorted list of render pass handle IDs after compilation
    std::vector<RenderPassHandleID> sorted_passes_;
};

// Function to create nodes from the pass map
RENDER_GRAPH_DLL std::unordered_map<RenderPassHandleID, RenderGraphNode> CreateNodes(
    const std::unordered_map<RenderPassHandleID, std::unique_ptr<RenderPass>>& pass_map,
    const std::vector<RenderPassHandleID>& added_pass_order);

// Function to perform stable topological sort using Kahn's algorithm
RENDER_GRAPH_DLL bool StableTopologicalSortKahn(
    const std::unordered_map<RenderPassHandleID, RenderGraphNode>& nodeMap,
    std::vector<RenderPassHandleID>& sorted);

} // namespace render_graph