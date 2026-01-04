#include "render_graph_test/pch.h"

#include "render_graph/include/render_graph.h"

namespace render_graph_test
{

// Tag a render pass type
class TestPass1Handle : public render_graph::RenderPassHandle<TestPass1Handle> {};
class TestPass2Handle : public render_graph::RenderPassHandle<TestPass2Handle> {};
class TestPass3Handle : public render_graph::RenderPassHandle<TestPass3Handle> {};
class TestPass4Handle : public render_graph::RenderPassHandle<TestPass4Handle> {};

} // namespace render_graph_test

TEST(RenderGraph, AddPass)
{
    // Create singleton instance of RenderPassIDGenerator
    std::unique_ptr<render_graph::RenderPassIDGenerator> render_pass_id_generator 
        = std::make_unique<render_graph::RenderPassIDGenerator>();

    // Create a render graph
    render_graph::RenderGraph render_graph;

    // The flag to check if the execute function was called
    bool test_pass1_executed = false;
    bool test_pass2_executed = false;
    bool test_pass3_executed = false;

    // Resource handles
    render_graph::ResourceHandle resA = render_graph::ResourceHandle(0, 0);
    render_graph::ResourceHandle resB = render_graph::ResourceHandle(1, 0);
    render_graph::ResourceHandle resC = render_graph::ResourceHandle(2, 0);
    render_graph::ResourceHandle resD = render_graph::ResourceHandle(3, 0);

    // Add test pass 1
    render_graph.AddPass
    (
        // Render pass handle ID
        render_graph_test::TestPass1Handle::ID(),

        // Setup function
        [&](render_graph::RenderPassBuilder& builder) 
        {
            builder.Read(&resA); // Read resource A
            builder.Write(&resB); // Write resource B

            return true; // Setup successful
        },

        // Execute function
        [&](render_graph::RenderPass& self_pass, render_graph::RenderPassContext& context) 
        {
            test_pass1_executed = true;

            // Get read access token
            const render_graph::ResourceAccessToken& read_token = self_pass.GetReadToken();
            EXPECT_TRUE(read_token.HasAccess(&resA)); // Should have access to resource A
            EXPECT_FALSE(read_token.HasAccess(&resB)); // Should not have access to resource B
            EXPECT_FALSE(read_token.HasAccess(&resC)); // Should not have access to resource C
            EXPECT_FALSE(read_token.HasAccess(&resD)); // Should not have access to resource D

            // Get write access token
            const render_graph::ResourceAccessToken& write_token = self_pass.GetWriteToken();
            EXPECT_FALSE(write_token.HasAccess(&resA)); // Should not have access to resource A
            EXPECT_TRUE(write_token.HasAccess(&resB)); // Should have access to resource B
            EXPECT_FALSE(write_token.HasAccess(&resC)); // Should not have access to resource C
            EXPECT_FALSE(write_token.HasAccess(&resD)); // Should not have access to resource D

            return true; // Execution successful
        }
    );

    // Add test pass 2
    render_graph.AddPass
    (
        // Render pass handle ID
        render_graph_test::TestPass2Handle::ID(),

        // Setup function
        [&](render_graph::RenderPassBuilder& builder) 
        {
            builder.Read(&resB); // Read resource B
            builder.Write(&resC); // Write resource C

            return true; // Setup successful
        },

        // Execute function
        [&](render_graph::RenderPass& self_pass, render_graph::RenderPassContext& context) 
        {
            EXPECT_TRUE(test_pass1_executed); // Ensure TestPass1Handle executed first
            test_pass2_executed = true;

            // Get read access token
            const render_graph::ResourceAccessToken& read_token = self_pass.GetReadToken();
            EXPECT_FALSE(read_token.HasAccess(&resA)); // Should not have access to resource A
            EXPECT_TRUE(read_token.HasAccess(&resB)); // Should have access to resource B
            EXPECT_FALSE(read_token.HasAccess(&resC)); // Should not have access to resource C
            EXPECT_FALSE(read_token.HasAccess(&resD)); // Should not have access to resource D

            // Get write access token
            const render_graph::ResourceAccessToken& write_token = self_pass.GetWriteToken();
            EXPECT_FALSE(write_token.HasAccess(&resA)); // Should not have access to resource A
            EXPECT_FALSE(write_token.HasAccess(&resB)); // Should not have access to resource B
            EXPECT_TRUE(write_token.HasAccess(&resC)); // Should have access to resource C
            EXPECT_FALSE(write_token.HasAccess(&resD)); // Should not have access to resource D

            return true; // Execution successful
        }
    );

    // Add test pass 3
    render_graph.AddPass
    (
        // Render pass handle ID
        render_graph_test::TestPass3Handle::ID(),

        // Setup function
        [&](render_graph::RenderPassBuilder& builder) 
        {
            builder.Read(&resB); // Read resource A
            builder.Read(&resC); // Read resource B

            builder.Write(&resD); // Write resource C

            return true; // Setup successful
        },

        // Execute function
        [&](render_graph::RenderPass& self_pass, render_graph::RenderPassContext& context) 
        {
            test_pass3_executed = true;

            // Get read access token
            const render_graph::ResourceAccessToken& read_token = self_pass.GetReadToken();
            EXPECT_FALSE(read_token.HasAccess(&resA)); // Should have access to resource A
            EXPECT_TRUE(read_token.HasAccess(&resB)); // Should not have access to resource B
            EXPECT_TRUE(read_token.HasAccess(&resC)); // Should not have access to resource C
            EXPECT_FALSE(read_token.HasAccess(&resD)); // Should not have access to resource D

            // Get write access token
            const render_graph::ResourceAccessToken& write_token = self_pass.GetWriteToken();
            EXPECT_FALSE(write_token.HasAccess(&resA)); // Should not have access to resource A
            EXPECT_FALSE(write_token.HasAccess(&resB)); // Should not have access to resource B
            EXPECT_FALSE(write_token.HasAccess(&resC)); // Should have access to resource C
            EXPECT_TRUE(write_token.HasAccess(&resD)); // Should have access to resource D

            return true; // Execution successful
        }
    );

    // Compile the render graph
    EXPECT_TRUE(render_graph.Compile());

    // Mock command allocator for context
    dx12_util::CommandAllocator mock_command_allocator;

    // Mock command list for context
    dx12_util::CommandList mock_command_list(mock_command_allocator);

    // Create a render pass context
    std::unique_ptr<render_graph::RenderPassContext> context 
        = std::make_unique<render_graph::RenderPassContext>(mock_command_list);

    // Execute the render graph
    EXPECT_TRUE(render_graph.Execute(*context));

    // Ensure all passes were executed
    EXPECT_TRUE(test_pass1_executed);
    EXPECT_TRUE(test_pass2_executed);
    EXPECT_TRUE(test_pass3_executed);

    // Clear the render graph
    render_graph.Clear();
}

namespace render_graph_test
{

class ExamplePass
{
public:
    ExamplePass(render_graph::ResourceHandle a, render_graph::ResourceHandle b) :
        res_a_(a), res_b_(b) 
    {
    }

    ~ExamplePass() = default;

    bool AddToGraph(render_graph::RenderGraph& graph)
    {
        return graph.AddPass
        (
            // Render pass handle ID
            render_graph_test::TestPass2Handle::ID(),

            // Setup function
            [&](render_graph::RenderPassBuilder& builder) 
            {
                builder.Read(&res_a_); // Read resource A
                builder.Write(&res_b_); // Write resource B

                return true; // Setup successful
            },

            // Execute function
            [&](render_graph::RenderPass& self_pass, render_graph::RenderPassContext& context) 
            {
                // Get read access token
                const render_graph::ResourceAccessToken& read_token = self_pass.GetReadToken();
                EXPECT_TRUE(read_token.HasAccess(&res_a_)); // Should have access to resource A
                EXPECT_FALSE(read_token.HasAccess(&res_b_)); // Should not have access to resource B

                // Get write access token
                const render_graph::ResourceAccessToken& write_token = self_pass.GetWriteToken();
                EXPECT_FALSE(write_token.HasAccess(&res_a_)); // Should not have access to resource A
                EXPECT_TRUE(write_token.HasAccess(&res_b_)); // Should have access to resource B

                return true; // Execution successful
            }
        );
    }

private:
    render_graph::ResourceHandle res_a_;
    render_graph::ResourceHandle res_b_;
};

} // namespace render_graph_test

TEST(RenderGraph, RenderPassByClass)
{
    // Create singleton instance of RenderPassIDGenerator
    std::unique_ptr<render_graph::RenderPassIDGenerator> render_pass_id_generator 
        = std::make_unique<render_graph::RenderPassIDGenerator>();

    // Create a render graph
    render_graph::RenderGraph render_graph;

    // Resource handles
    render_graph::ResourceHandle resA = render_graph::ResourceHandle(0, 0);
    render_graph::ResourceHandle resB = render_graph::ResourceHandle(1, 0);

    // Create an example pass
    render_graph_test::ExamplePass example_pass(resA, resB);

    // Add the example pass to the render graph
    EXPECT_TRUE(example_pass.AddToGraph(render_graph));

    // Compile the render graph
    EXPECT_TRUE(render_graph.Compile());

    // Mock command allocator for context
    dx12_util::CommandAllocator mock_command_allocator;

    // Mock command list for context
    dx12_util::CommandList mock_command_list(mock_command_allocator);

    // Create a render pass context
    std::unique_ptr<render_graph::RenderPassContext> context 
        = std::make_unique<render_graph::RenderPassContext>(mock_command_list);

    // Execute the render graph
    EXPECT_TRUE(render_graph.Execute(*context));

    // Clear the render graph
    render_graph.Clear();
}