#include "mono_forge_model_test/pch.h"

#include "mono_forge_model/include/mfm.h"
#include "utility_header/file_loader.h"

namespace mono_forge_model_test
{

constexpr const char* TEST_MFM_FILE_PATH = "../resources/mono_forge_model/cube.mfm";

} // namespace mono_forge_model_test

TEST(MFM, Load)
{
    // Prepare for loading MFM file
    fpos_t file_size = 0;

    // Load MFM file
    std::unique_ptr<uint8_t[]> file_data 
        = utility_header::LoadFile(mono_forge_model_test::TEST_MFM_FILE_PATH, file_size);
    ASSERT_NE(file_data, nullptr);
}

TEST(MFM, Analyze)
{
    // Prepare for loading MFM file
    fpos_t file_size = 0;

    // Load MFM file
    std::unique_ptr<uint8_t[]> file_data 
        = utility_header::LoadFile(mono_forge_model_test::TEST_MFM_FILE_PATH, file_size);
    ASSERT_NE(file_data, nullptr);

    // Create MFM object
    mono_forge_model::MFM mfm(std::move(file_data), static_cast<uint32_t>(file_size));

    // Verify MFM file header
    const mono_forge_model::MFMFileHeader* file_header = mfm.GetFileHeader();
    ASSERT_NE(file_header, nullptr);
    std::cout << "MFM File Header:" << std::endl;
    std::cout << "  File Type: 0x" << std::hex << file_header->file_type << std::dec << std::endl;
    std::cout << "  File Size: " << file_header->file_size << " bytes" << std::endl;
    std::cout << "  Content Type: 0x" << std::hex << file_header->content_type << std::dec << std::endl;

    // Verify MFM info header
    const mono_forge_model::MFMInfoHeader* info_header = mfm.GetInfoHeader();
    ASSERT_NE(info_header, nullptr);
    std::cout << "MFM Info Header:" << std::endl;
    std::cout << "  Mesh Header Offset: " << info_header->mesh_header_offset << " bytes" << std::endl;
    std::cout << "  Mesh Header Size: " << info_header->mesh_header_size << " bytes" << std::endl;
    std::cout << "  Custom Header Offset: " << info_header->custom_header_offset << " bytes" << std::endl;
    std::cout << "  Custom Header Size: " << info_header->custom_header_size << " bytes" << std::endl;

    // Verify MFM mesh header
    const mono_forge_model::MFMMeshHeader* mesh_header = mfm.GetMeshHeader();
    ASSERT_NE(mesh_header, nullptr);
    std::cout << "MFM Mesh Header:" << std::endl;
    std::cout << "  Data Offset: " << mesh_header->mesh_data_offset << " bytes" << std::endl;
    std::cout << "  Data Size: " << mesh_header->mesh_data_size << " bytes" << std::endl;
    std::cout << "  Material Count: " << mesh_header->material_count << std::endl;

    // Verify each mesh node
    for (uint32_t mesh_index = 0; mesh_index < mesh_header->material_count; ++mesh_index)
    {
        const mono_forge_model::MFMMeshNode* mesh_node = mfm.GetMeshNode(mesh_index);
        ASSERT_NE(mesh_node, nullptr);
        std::cout << "MFM Mesh Node " << mesh_index << ":" << std::endl;
        std::cout << "  Material Name: " << mfm.GetMaterialName(mesh_index) << std::endl;
        std::cout << "  Vertex Offset: " << mesh_node->vertex_offset << " bytes" << std::endl;
        std::cout << "  Vertex Size: " << mesh_node->vertex_size << " bytes" << std::endl;
        std::cout << "  Vertex Count: " << mesh_node->vertex_count << std::endl;
        std::cout << "  Index Offset: " << mesh_node->index_offset << " bytes" << std::endl;
        std::cout << "  Index Size: " << mesh_node->index_size << " bytes" << std::endl;
        std::cout << "  Index Count: " << mesh_node->index_count << std::endl;
    }
}