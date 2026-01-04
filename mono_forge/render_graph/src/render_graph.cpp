#include "render_graph/src/pch.h"
#include "render_graph/include/render_graph.h"

#include "utility_header/logger.h"

namespace render_graph
{

bool RenderGraph::AddPass(
    RenderPassHandleID handleID, RenderPass::SetupFunc setup, RenderPass::ExecuteFunc execute)
{
    assert(pass_map_.find(handleID) == pass_map_.end() && "Render pass with this HandleID already exists.");

    // Create a new RenderPass
    std::unique_ptr<RenderPass> newPass = std::make_unique<RenderPass>(handleID, setup, execute);

    // Create a RenderPassBuilder for setup
    RenderPassBuilder builder(*newPass);

    // Setup the render pass
    if (!newPass->Setup(builder))
    {
        utility_header::ConsoleLogErr(
            { "Failed to setup render pass with HandleID: " + std::to_string(handleID) },
            __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Add the new pass to the map
    pass_map_[handleID] = std::move(newPass);

    // Record the addition order
    added_pass_order_.push_back(handleID);

    return true; // Successfully added
}

bool RenderGraph::Compile()
{
    // Create nodes for all passes
    std::unordered_map<RenderPassHandleID, RenderGraphNode> node_map = CreateNodes(pass_map_, added_pass_order_);

    // Sort nodes topologically using Kahn's algorithm
    sorted_passes_.clear();
    if (!StableTopologicalSortKahn(node_map, sorted_passes_))
    {
        utility_header::ConsoleLogErr(
            { "Failed to compile render graph due to cyclic dependencies." },
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Compilation failed due to cycles
    }

    return true; // Successfully compiled
}

bool RenderGraph::Execute(RenderPassContext& context)
{
    // Execute all root nodes
    for (const RenderPassHandleID& handleID : sorted_passes_)
    {
        auto it = pass_map_.find(handleID);
        assert(it != pass_map_.end() && "Render pass not found in pass map.");

        // Get the render pass
        RenderPass& pass = *it->second;

        // Execute the render pass
        if (!pass.Execute(context))
        {
            utility_header::ConsoleLogErr(
                { "Failed to execute render pass with HandleID: " + std::to_string(handleID) },
                __FILE__, __LINE__, __FUNCTION__);
            return false; // Execution failed
        }
    }

    // Clear passes after execution
    pass_map_.clear();
    sorted_passes_.clear();
    added_pass_order_.clear();

    return true; // Successfully executed the render graph
}

void render_graph::RenderGraph::Clear()
{
    pass_map_.clear();
    sorted_passes_.clear();
}

RENDER_GRAPH_DLL std::unordered_map<RenderPassHandleID, RenderGraphNode> CreateNodes(
    const std::unordered_map<RenderPassHandleID, std::unique_ptr<RenderPass>>& pass_map,
    const std::vector<RenderPassHandleID>& added_pass_order)
{
    // Create a map to hold the nodes
    std::unordered_map<RenderPassHandleID, RenderGraphNode> node_map;

    // Create nodes for each render pass
    for (const auto& [handleID, pass] : pass_map)
    {
        // Create a node for this pass
        RenderGraphNode node = { handleID, {} };

        // Add the node to the map
        node_map[handleID] = node;
    }

    // Map of resource writers
    // Writer must come before readers, and only one writer per resource is allowed
    std::unordered_map<ResourceHandle, RenderPassHandleID> resource_writers;

    // Map of resource readers
    // Multiple readers per resource are allowed
    std::unordered_map<ResourceHandle, std::vector<RenderPassHandleID>> resource_readers;

    // Populate resource writers and readers
    for (const RenderPassHandleID& handleID : added_pass_order)
    {
        const std::unique_ptr<RenderPass>& pass = pass_map.at(handleID);
        
        // Get write access tokens
        const ResourceAccessToken& write_token = pass->GetWriteToken();

        // Register writers
        for (const ResourceHandle* resource_handle : write_token.GetAccessibleResourceHandles())
        {
            if (resource_writers.find(*resource_handle) != resource_writers.end())
            {
				int debug_breakpoint = 0;
            }

            assert(
                resource_writers.find(*resource_handle) == resource_writers.end() && 
                "Multiple writers for the same resource detected.");

            // Add writer
            resource_writers[*resource_handle] = handleID;
        }

        // Get read access tokens
        const ResourceAccessToken& read_token = pass->GetReadToken();

        // Register readers
        for (const ResourceHandle* resource_handle : read_token.GetAccessibleResourceHandles())
            resource_readers[*resource_handle].push_back(handleID); // Add reader
    }

    // Establish dependencies based on resource access
    for (const auto& [resource_handle, writerID] : resource_writers)
    {
        // Get the node for the writer
        RenderGraphNode& writer_node = node_map[writerID];

        // Check if there are readers for this resource
        if (resource_readers.find(resource_handle) == resource_readers.end())
            continue; // No readers for this resource

        // Add dependencies for all readers of this resource
        for (const RenderPassHandleID& readerID : resource_readers[resource_handle])
        {
            assert(readerID != writerID && "Writer cannot be a reader of the same resource.");

            // Get the node for the reader
            RenderGraphNode& reader_node = node_map[readerID];

            // Add dependency: reader depends on writer
            reader_node.dependencies.push_back(&writer_node);
        }
    }

    return node_map; // Return the created node map
}

RENDER_GRAPH_DLL bool StableTopologicalSortKahn(
    const std::unordered_map<RenderPassHandleID, RenderGraphNode>& node_map,
    std::vector<RenderPassHandleID>& sorted)
{
    // Compute in-degrees
    std::map<RenderPassHandleID, int> inDegree;
    for (const auto& [id, node] : node_map) inDegree[id] = 0;

    // Calculate in-degrees
    for (const auto& [id, node] : node_map)
        for (auto* dep : node.dependencies)
            inDegree[id]++;

    // Initialize zero in-degree set
    std::set<RenderPassHandleID> zeroInDegree;
    for (const auto& [id, deg] : inDegree)
        if (deg == 0) zeroInDegree.insert(id);

    // Map for reverse edges
    std::unordered_map<RenderPassHandleID, std::vector<RenderPassHandleID>> reverseEdges;
    for (const auto& [id, node] : node_map)
        for (auto* dep : node.dependencies)
            reverseEdges[dep->passID].push_back(id);

    while (!zeroInDegree.empty()) 
    {
        RenderPassHandleID id = *zeroInDegree.begin();
        zeroInDegree.erase(zeroInDegree.begin());
        sorted.push_back(id);

        // Decrease in-degrees of neighbors
        for (auto neighbor : reverseEdges[id])
            if (--inDegree[neighbor] == 0)
                zeroInDegree.insert(neighbor);
    }

    // Check for cycles
    if (sorted.size() != node_map.size())
    {
        utility_header::ConsoleLogErr(
            { "Cycle detected in render graph during stable topological sort." },
            __FILE__, __LINE__, __FUNCTION__);

        sorted.clear();
        return false;
    }
    return true;
}

} // namespace render_graph