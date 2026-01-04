#include "render_graph/src/pch.h"
#include "render_graph/include/lambert_material.h"

#include "render_graph/include/render_pass.h"

namespace render_graph
{

LambertMaterial::LambertMaterial(ResourceAdder& resource_adder, ResourceEraser& resource_eraser) :
    resource_adder_(resource_adder),
    resource_eraser_(resource_eraser)
{
}

LambertMaterial::~LambertMaterial()
{
    // Erase material buffer resource
    if (material_buffer_handle_.IsValid())
        resource_eraser_.EraseResource(&material_buffer_handle_);
}

bool LambertMaterial::Setup(Material::SetupParam& param)
{
    // Cast to LambertMaterial::SetupParam
    SetupParam* lambert_param = dynamic_cast<SetupParam*>(&param);
    assert(lambert_param != nullptr && "Invalid SetupParam type for LambertMaterial.");

    // Store material buffer data
    buffer_.material_id = GetMaterialTypeHandleID();
    buffer_.base_color = lambert_param->base_color;
    buffer_.albedo_source = lambert_param->albedo_source;
    buffer_.normal_source = lambert_param->normal_source;
    buffer_.ao_source = lambert_param->ao_source;
	buffer_.emission_source = lambert_param->emission_source;
    buffer_.emission_color = lambert_param->emission_color;

    // Store texture handle
    assert(lambert_param->albedo_texture_handle->IsValid() && "Albedo texture handle is invalid.");
    assert(lambert_param->normal_texture_handle->IsValid() && "Normal texture handle is invalid.");
    assert(lambert_param->ao_texture_handle->IsValid() && "AO texture handle is invalid.");
    albedo_texture_handle_ = lambert_param->albedo_texture_handle;
    normal_texture_handle_ = lambert_param->normal_texture_handle;
    ao_texture_handle_ = lambert_param->ao_texture_handle;
	emission_texture_handle_ = lambert_param->emission_texture_handle;

    // Create material buffer resource
    std::unique_ptr<dx12_util::Buffer> buffer
        = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
            sizeof(MaterialBuffer), D3D12_HEAP_TYPE_UPLOAD,
            lambert_param->debug_name_prefix + L" Lambert Material Buffer",
            dx12_util::Device::GetInstance().Get(), nullptr);
    if (!buffer)
        return false; // Failure

    // Upload material buffer data
    if (!buffer->UpdateData(&buffer_, sizeof(MaterialBuffer)))
        return false; // Failure

    // Add material buffer to resource container
    material_buffer_handle_ = resource_adder_.AddResource(std::move(buffer));
    if (!material_buffer_handle_.IsValid())
        return false; // Failure

    return true; // Setup successful
}

bool LambertMaterial::Apply(const Material::SetupParam& param)
{
    // Cast to LambertMaterial::SetupParam
    const SetupParam* lambert_param = dynamic_cast<const SetupParam*>(&param);
    assert(lambert_param != nullptr && "Invalid SetupParam type for LambertMaterial.");

    // Update material buffer data
    buffer_.material_id = GetMaterialTypeHandleID();
    buffer_.base_color = lambert_param->base_color;
    buffer_.albedo_source = lambert_param->albedo_source;
    buffer_.normal_source = lambert_param->normal_source;
    buffer_.ao_source = lambert_param->ao_source;
	buffer_.emission_source = lambert_param->emission_source;
    buffer_.emission_color = lambert_param->emission_color;

    // Update material buffer resource
    {
        render_graph::ResourceManager& resource_manager = render_graph::ResourceManager::GetInstance();
        resource_manager.WithLock([&](render_graph::ResourceManager& manager)
        {
            // Create access token
            render_graph::ResourceAccessToken write_token;
            write_token.PermitAccess(&material_buffer_handle_);

            // Get buffer for writing
            dx12_util::Resource& resource = manager.GetWriteResource(&material_buffer_handle_, write_token);
            dx12_util::Buffer* buffer = dynamic_cast<dx12_util::Buffer*>(&resource);
            assert(buffer && "Failed to cast to Buffer for material buffer.");

            // Update buffer data
            buffer->UpdateData(&buffer_, sizeof(MaterialBuffer));
        });
    }

    // Update albedo texture handle
    albedo_texture_handle_ = lambert_param->albedo_texture_handle;
    normal_texture_handle_ = lambert_param->normal_texture_handle;
    ao_texture_handle_ = lambert_param->ao_texture_handle;
	emission_texture_handle_ = lambert_param->emission_texture_handle;

    return true; // Apply successful
}

const ResourceHandle* LambertMaterial::GetBufferHandle() const
{
    return &material_buffer_handle_;
}

MaterialTypeHandleID LambertMaterial::GetMaterialTypeHandleID() const
{
    return LambertMaterialTypeHandle::ID();
}

void LambertMaterial::DeclareResources(RenderPassBuilder& builder) const
{
    builder.Read(&material_buffer_handle_); // Declare material buffer as read resource
    builder.Read(albedo_texture_handle_); // Declare albedo texture as read resource
    builder.Read(normal_texture_handle_); // Declare normal texture as read resource
    builder.Read(ao_texture_handle_); // Declare ao texture as read resource
    builder.Read(emission_texture_handle_); // Declare emission texture as read resource
}

const void* LambertMaterial::GetBufferData(uint32_t& size) const
{
    size = sizeof(MaterialBuffer);
    return &buffer_;
}

void LambertMaterial::SetAlbedoTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Albedo texture handle is invalid.");
    albedo_texture_handle_ = handle;
}

void LambertMaterial::SetNormalTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Normal texture handle is invalid.");
    normal_texture_handle_ = handle;
}

void LambertMaterial::SetAOTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "AO texture handle is invalid.");
    ao_texture_handle_ = handle;
}

void LambertMaterial::SetEmissionTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Emission texture handle is invalid.");
    emission_texture_handle_ = handle;
}

} // namespace render_graph