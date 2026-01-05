#pragma once

#include "render_graph/include/material_handle.h"
#include "render_graph/include/resource_handle.h"

namespace render_graph
{

// Forward declaration
class RenderPassBuilder;

// The interface for materials
class Material
{
public:
    Material() = default;
    virtual ~Material() = default;

    // Parameter class for setup
    class SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() = default;

        // Clone the setup param
        virtual std::unique_ptr<SetupParam> Clone() const { return nullptr; }

        // Get material type handle ID
        virtual MaterialTypeHandleID GetMaterialTypeHandleID() const = 0;
    };

    // Setup the material with given parameters
    virtual bool Setup(SetupParam& param) = 0;

    // Apply the material with given parameters
    virtual bool Apply(const SetupParam& param) = 0;

    // Get material buffer handle
    virtual const ResourceHandle* GetBufferHandle() const = 0;

    // Get material type handle ID
    virtual MaterialTypeHandleID GetMaterialTypeHandleID() const = 0;

    // Declare resources used by the material in the render pass
    virtual void DeclareResources(RenderPassBuilder& builder) const = 0;

    // Get buffer data for uploading
    virtual const void* GetBufferData(uint32_t& size) const = 0;
};

} // namespace render_graph