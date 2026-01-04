#pragma once

#include <stdint.h>

namespace mono_forge_model
{

#pragma pack(push, 1)

// File header structure
struct MFMFileHeader
{
    uint16_t file_type = 0x4D46; // File type identifier (ASCII code of 'MF')
    uint32_t file_size = 0; // File size (total of file header + info header + mesh data chunks)
    uint16_t content_type = 0x0001; // Content type (0x0001: mesh only)
};

// Information header structure
struct MFMInfoHeader
{
    uint32_t mesh_header_offset = 0; // Offset to the mesh header
    uint32_t mesh_header_size = 0; // Size of the mesh header

    uint32_t custom_header_offset = 0; // Offset to the custom header
    uint32_t custom_header_size = 0; // Size of the custom header
};

struct MFMMeshHeader
{
    uint32_t mesh_data_offset = 0; // Offset to the mesh data chunk
    uint32_t mesh_data_size = 0; // Size of the mesh data chunk

    uint32_t material_count = 0; // Number of materials (mesh nodes)
};

struct MFMCustomHeader
{
    uint32_t custom_data_offset = 0; // Offset to the custom data chunk
    uint32_t custom_data_size = 0; // Size of the custom data chunk

    uint32_t coord_system = 0; // Coordinate system (0: right-handed, 1: left-handed)
    uint32_t up_axis = 1; // Up axis (0: X-axis, 1: Y-axis, 2: Z-axis)
};

struct MFMMeshNode
{
    uint32_t next_node_offset = 0; // Offset to the next mesh node

    uint32_t material_name_offset = 0; // Offset to the material name
    uint32_t material_name_size = 0; // Size of the material name

    uint32_t vertex_offset = 0; // Offset to vertex data
    uint32_t vertex_size = 0; // Size of a vertex
    uint32_t vertex_count = 0; // Number of vertices

    uint32_t index_offset = 0; // Offset to index data
    uint32_t index_size = 0; // Size of an index
    uint32_t index_count = 0; // Number of indices
};


#pragma pack(pop)

constexpr const char* MFM_FILE_EXT = ".mfm";

} // namespace mono_forge_model