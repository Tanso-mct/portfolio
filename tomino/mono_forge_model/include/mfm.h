#pragma once

#include <stdint.h>
#include <string_view>
#include <memory>

#include "mono_forge_model/include/dll_config.h"
#include "mono_forge_model/include/mfm_layout.h"

namespace mono_forge_model
{

class MONO_FORGE_MODEL_DLL MFM
{
public:
    // Construct with mfm file data
    MFM(std::unique_ptr<uint8_t[]> data, uint32_t data_size);
    ~MFM() = default;

    // Get MFM file header
    const MFMFileHeader* GetFileHeader() const;

    // Get MFM info header
    const MFMInfoHeader* GetInfoHeader() const;

    // Get MFM mesh header
    const MFMMeshHeader* GetMeshHeader() const;

    // Get MFM custom header
    const MFMCustomHeader* GetCustomHeader() const;

    // Get mesh node by index
    const MFMMeshNode* GetMeshNode(uint32_t material_index) const;

    // Get material name
    const char* GetMaterialName(uint32_t material_index) const;

    // Get pointer to vertex data
    const uint8_t* GetVertexData(uint32_t material_index) const;

    // Get pointer to index data
    const uint8_t* GetIndexData(uint32_t material_index) const;

private:
    // MFM file data buffer
    const std::unique_ptr<uint8_t[]> data_;
    
    // MFM file data size
    const uint32_t data_size_;
};

} // namespace mono_forge_model