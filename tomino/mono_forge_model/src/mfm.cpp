#include "mono_forge_model/src/pch.h"
#include "mono_forge_model/include/mfm.h"

namespace mono_forge_model
{

MFM::MFM(std::unique_ptr<uint8_t[]> data, uint32_t data_size) :
    data_(std::move(data)),
    data_size_(data_size)
{
    assert(data_ != nullptr && "MFM file data is null.");
    assert(data_size_ > 0 && "MFM file data size is zero.");
}

const MFMFileHeader* MFM::GetFileHeader() const
{
    return reinterpret_cast<const MFMFileHeader*>(data_.get());
}

const MFMInfoHeader* MFM::GetInfoHeader() const
{
    return reinterpret_cast<const MFMInfoHeader*>(data_.get() + sizeof(MFMFileHeader));
}

const MFMMeshHeader* MFM::GetMeshHeader() const
{
    const MFMInfoHeader* info_header = GetInfoHeader();
    return reinterpret_cast<const MFMMeshHeader*>(data_.get() + info_header->mesh_header_offset);
}

const MFMCustomHeader* MFM::GetCustomHeader() const
{
    const MFMInfoHeader* info_header = GetInfoHeader();
    return reinterpret_cast<const MFMCustomHeader*>(data_.get() + info_header->custom_header_offset);
}

const MFMMeshNode* MFM::GetMeshNode(uint32_t material_index) const
{
    // Get mesh header
    const MFMMeshHeader* mesh_header = GetMeshHeader();

    // Get mesh data offset
    uint32_t offset = mesh_header->mesh_data_offset;

    // Traverse to the specified mesh node
    const MFMMeshNode* mesh_node = nullptr;
    for (uint32_t i = 0; i <= material_index; ++i)
    {
        mesh_node = reinterpret_cast<const MFMMeshNode*>(data_.get() + offset);
        offset += mesh_node->next_node_offset;
    }

    // Return the mesh node
    return mesh_node;
}

const char* MFM::GetMaterialName(uint32_t material_index) const
{
    const MFMMeshNode* mesh_node = GetMeshNode(material_index);
    return reinterpret_cast<const char*>(data_.get() + mesh_node->material_name_offset);
}

const uint8_t* MFM::GetVertexData(uint32_t material_index) const
{
    const MFMMeshNode* mesh_node = GetMeshNode(material_index);
    return data_.get() + mesh_node->vertex_offset;
}

const uint8_t* MFM::GetIndexData(uint32_t material_index) const
{
    const MFMMeshNode* mesh_node = GetMeshNode(material_index);
    return data_.get() + mesh_node->index_offset;
}

} // namespace mono_forge_model