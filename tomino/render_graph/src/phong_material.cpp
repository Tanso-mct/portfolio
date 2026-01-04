#include "render_graph/src/pch.h"
#include "render_graph/include/phong_material.h"

#include "render_graph/include/render_pass.h"

namespace render_graph
{

PhongMaterial::PhongMaterial(ResourceAdder& resource_adder, ResourceEraser& resource_eraser) :
    resource_adder_(resource_adder),
    resource_eraser_(resource_eraser)
{
}

PhongMaterial::~PhongMaterial()
{
    // Erase material buffer resource
    if (material_buffer_handle_.IsValid())
        resource_eraser_.EraseResource(&material_buffer_handle_);
}

bool PhongMaterial::Setup(Material::SetupParam& param)
{
    // Cast to PhongMaterial::SetupParam
    SetupParam* phong_material_param = dynamic_cast<SetupParam*>(&param);
    assert(phong_material_param != nullptr && "Invalid SetupParam type for PhongMaterial.");

    // Store material buffer data
    buffer_.material_id = PhongMaterialTypeHandle::ID();
    buffer_.base_color = phong_material_param->base_color;
    buffer_.albedo_source = phong_material_param->albedo_source;
    buffer_.normal_source = phong_material_param->normal_source;
    buffer_.ao_source = phong_material_param->ao_source;
    buffer_.specular_source = phong_material_param->specular_source;
    buffer_.roughness_source = phong_material_param->roughness_source;
    buffer_.roughness_value = phong_material_param->roughness_value;
    buffer_.metalness_source = phong_material_param->metalness_source;
    buffer_.metalness_value = phong_material_param->metalness_value;
    buffer_.emission_source = phong_material_param->emission_source;
    buffer_.emission_color = phong_material_param->emission_color;

    // Store texture handles
    assert(phong_material_param->albedo_texture_handle->IsValid() && "Albedo texture handle is invalid.");
    assert(phong_material_param->normal_texture_handle->IsValid() && "Normal texture handle is invalid.");
    assert(phong_material_param->ao_texture_handle->IsValid() && "AO texture handle is invalid.");
    assert(phong_material_param->specular_texture_handle->IsValid() && "Specular texture handle is invalid.");
    assert(phong_material_param->roughness_texture_handle->IsValid() && "Roughness texture handle is invalid.");
    assert(phong_material_param->metalness_texture_handle->IsValid() && "Metalness texture handle is invalid.");
    assert(phong_material_param->emission_texture_handle->IsValid() && "Emission texture handle is invalid.");
    albedo_texture_handle_ = phong_material_param->albedo_texture_handle;
    normal_texture_handle_ = phong_material_param->normal_texture_handle;
    ao_texture_handle_ = phong_material_param->ao_texture_handle;
    specular_texture_handle_ = phong_material_param->specular_texture_handle;
    roughness_texture_handle_ = phong_material_param->roughness_texture_handle;
    metalness_texture_handle_ = phong_material_param->metalness_texture_handle;
    emission_texture_handle_ = phong_material_param->emission_texture_handle;

    // Create material buffer resource
    std::unique_ptr<dx12_util::Buffer> buffer
        = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
            sizeof(MaterialBuffer), D3D12_HEAP_TYPE_UPLOAD,
            phong_material_param->debug_name_prefix + L" Phong Material Buffer",
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

bool PhongMaterial::Apply(const Material::SetupParam& param)
{
    // Cast to PhongMaterial::SetupParam
    const SetupParam* phong_material_param = dynamic_cast<const SetupParam*>(&param);
    assert(phong_material_param != nullptr && "Invalid SetupParam type for PhongMaterial.");

    // Update material buffer data
    buffer_.material_id = PhongMaterialTypeHandle::ID();
    buffer_.base_color = phong_material_param->base_color;
    buffer_.albedo_source = phong_material_param->albedo_source;
    buffer_.normal_source = phong_material_param->normal_source;
    buffer_.ao_source = phong_material_param->ao_source;
    buffer_.specular_source = phong_material_param->specular_source;
    buffer_.roughness_source = phong_material_param->roughness_source;
    buffer_.roughness_value = phong_material_param->roughness_value;
    buffer_.metalness_source = phong_material_param->metalness_source;
    buffer_.metalness_value = phong_material_param->metalness_value;
    buffer_.emission_source = phong_material_param->emission_source;
    buffer_.emission_color = phong_material_param->emission_color;

    // Update material buffer resource
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

    // Update texture handles
    assert(phong_material_param->albedo_texture_handle->IsValid() && "Albedo texture handle is invalid.");
    assert(phong_material_param->normal_texture_handle->IsValid() && "Normal texture handle is invalid.");
    assert(phong_material_param->ao_texture_handle->IsValid() && "AO texture handle is invalid.");
    assert(phong_material_param->specular_texture_handle->IsValid() && "Specular texture handle is invalid.");
    assert(phong_material_param->roughness_texture_handle->IsValid() && "Roughness texture handle is invalid.");
    assert(phong_material_param->metalness_texture_handle->IsValid() && "Metalness texture handle is invalid.");
    assert(phong_material_param->emission_texture_handle->IsValid() && "Emission texture handle is invalid.");
    albedo_texture_handle_ = phong_material_param->albedo_texture_handle;
    normal_texture_handle_ = phong_material_param->normal_texture_handle;
    ao_texture_handle_ = phong_material_param->ao_texture_handle;
    specular_texture_handle_ = phong_material_param->specular_texture_handle;
    roughness_texture_handle_ = phong_material_param->roughness_texture_handle;
    metalness_texture_handle_ = phong_material_param->metalness_texture_handle;
    emission_texture_handle_ = phong_material_param->emission_texture_handle;

    return true; // Apply successful
}

const ResourceHandle* PhongMaterial::GetBufferHandle() const
{
    return &material_buffer_handle_;
}

MaterialTypeHandleID PhongMaterial::GetMaterialTypeHandleID() const
{
    return PhongMaterialTypeHandle::ID();
}

void PhongMaterial::DeclareResources(RenderPassBuilder& builder) const
{
    builder.Read(&material_buffer_handle_); // Declare material buffer as read resource
    builder.Read(albedo_texture_handle_); // Declare albedo texture as read resource
    builder.Read(normal_texture_handle_); // Declare normal texture as read resource
    builder.Read(ao_texture_handle_); // Declare ao texture as read resource
    builder.Read(specular_texture_handle_); // Declare specular texture as read resource
    builder.Read(roughness_texture_handle_); // Declare roughness texture as read resource
    builder.Read(metalness_texture_handle_); // Declare metalness texture as read resource
    builder.Read(emission_texture_handle_); // Declare emission texture as read resource
}

const void* PhongMaterial::GetBufferData(uint32_t& size) const
{
    size = sizeof(MaterialBuffer);
    return &buffer_;
}

void PhongMaterial::SetAlbedoTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Albedo texture handle is invalid.");
    albedo_texture_handle_ = handle;
}

void PhongMaterial::SetNormalTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Normal texture handle is invalid.");
    normal_texture_handle_ = handle;
}

void PhongMaterial::SetAOTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "AO texture handle is invalid.");
    ao_texture_handle_ = handle;
}

void PhongMaterial::SetSpecularTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Specular texture handle is invalid.");
    specular_texture_handle_ = handle;
}

void PhongMaterial::SetRoughnessTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Roughness texture handle is invalid.");
    roughness_texture_handle_ = handle;
}

void PhongMaterial::SetMetalnessTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Metalness texture handle is invalid.");
    metalness_texture_handle_ = handle;
}

void PhongMaterial::SetEmissionTextureHandle(const render_graph::ResourceHandle* handle)
{
    assert(handle->IsValid() && "Emission texture handle is invalid.");
    emission_texture_handle_ = handle;
}

} // namespace render_graph