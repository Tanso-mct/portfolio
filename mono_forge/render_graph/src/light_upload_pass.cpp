#include "render_graph/src/pch.h"
#include "render_graph/include/light_upload_pass.h"

#include "render_graph/include/resource_manager.h"
#include "render_graph/include/light_manager.h"

namespace render_graph
{

LightUploadPass::LightUploadPass()
{
}

LightUploadPass::~LightUploadPass()
{
}

bool LightUploadPass::Setup()
{
    // No special setup needed for buffer update pass
    return true;
}

bool LightUploadPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        LightUploadPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            builder.Write(lights_upload_buffer_handle_); // Declare write access for lights upload buffer
            builder.Write(light_config_buffer_handle_); // Declare write access for light config buffer
            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            // Get write access token
            const ResourceAccessToken& write_token = self_pass.GetWriteToken();

            // Check the light count does not exceed maximum
            assert(light_handles_.size() <= light_config_.max_lights && "Exceeded maximum light count");

            // Check that buffer handles are valid
            assert(lights_upload_buffer_handle_->IsValid() && "Invalid lights upload buffer handle");
            assert(light_config_buffer_handle_->IsValid() && "Invalid light config buffer handle");

            // Set number of lights in light config
            light_config_.num_lights = static_cast<uint>(light_handles_.size());

            ResourceManager::GetInstance().WithLock([&](ResourceManager& manager)
            {
                // Get light config buffer for writing
                dx12_util::Resource& light_config_resource 
                    = manager.GetWriteResource(light_config_buffer_handle_, write_token);
                dx12_util::Buffer* light_config_buffer 
                    = dynamic_cast<dx12_util::Buffer*>(&light_config_resource);
                assert(light_config_buffer != nullptr && "Failed to cast to dx12_util::Buffer");

                // Update light config buffer data
                light_config_buffer->UpdateData(&light_config_, sizeof(Light::LightConfigBuffer));
            });

            LightManager::GetInstance().WithLock([&](LightManager& light_manager)
            {
                ResourceManager::GetInstance().WithLock([&](ResourceManager& resource_manager)
                {
                    // Get lights upload buffer for writing
                    dx12_util::Resource& lights_upload_resource 
                        = resource_manager.GetWriteResource(lights_upload_buffer_handle_, write_token);
                    dx12_util::Buffer* lights_buffer 
                        = dynamic_cast<dx12_util::Buffer*>(&lights_upload_resource);
                    assert(lights_buffer != nullptr && "Failed to cast to dx12_util::Buffer");

                    // For each light handle, get light data and upload to buffer
                    for (size_t i = 0; i < light_handles_.size(); ++i)
                    {
                        const LightHandle* light_handle = light_handles_[i];
                        Light& light = light_manager.GetLight(light_handle);

                        // Get light buffer data
                        const Light::LightBuffer* light_buffer = light.GetBuffer();

                        // Update lights upload buffer at the correct offset
                        size_t offset = i * sizeof(Light::LightBuffer);
                        lights_buffer->UpdateData(light_buffer, sizeof(Light::LightBuffer), offset);
                    }
                });
            });

            // Clear light handles after upload
            light_handles_.clear();

            // Reset buffer handles
            lights_upload_buffer_handle_ = nullptr;
            light_config_buffer_handle_ = nullptr;

            // Clear light config
            light_config_ = Light::LightConfigBuffer();

            return true; // Execution successful
        }
    );
}

void LightUploadPass::AddUploadLight(const LightHandle* light_handle)
{
    assert(IsSetup() && "Instance is not setup");
    light_handles_.emplace_back(light_handle);
}

void LightUploadPass::SetLightsUploadBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    lights_upload_buffer_handle_ = buffer_handle;
}

void LightUploadPass::SetLightConfigBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    light_config_buffer_handle_ = buffer_handle;
}

void LightUploadPass::SetLightConfig(Light::LightConfigBuffer config)
{
    assert(IsSetup() && "Instance is not setup");
    light_config_ = std::move(config);
}

} // namespace render_graph