#include "include/pch.h"
#include "include/mfm_converter.h"

#include "include/fbx_loader.h"

namespace model_converter
{

MFMConverter::MFMConverter()
{
}

MFMConverter::~MFMConverter()
{
}

std::string_view MFMConverter::GetInputFileExt() const
{
    return input_file_ext_;
}

std::string_view MFMConverter::GetConvertedFileExt() const
{
    return converted_file_ext_;
}

std::unique_ptr<u8[]> model_converter::MFMConverter::Convert(const IFileData *file_data, u32 &rt_data_size) const
{
    const FBXFileData* fbx_data = dynamic_cast<const FBXFileData*>(file_data);
    if (!fbx_data)
    {
        std::cerr << "MFM形式への変換に失敗: 入力データがFBXFileData型ではありません。" << std::endl;
        return nullptr;
    }

    // メッシュデータチャンクのサイズ
    u32 mesh_data_size = 0;
    for (const int material_index : fbx_data->GetMaterialIndices())
    {
        const u32 vertex_count = static_cast<u32>(fbx_data->GetVertices(material_index).size());
        const u32 index_count = static_cast<u32>(fbx_data->GetIndices(material_index).size());
        mesh_data_size += sizeof(MFMMeshNode); // メッシュノードサイズ
        mesh_data_size += fbx_data->GetMaterialName(material_index).size(); // マテリアル名サイズ
        mesh_data_size += vertex_count * sizeof(FBXVertex); // 頂点データサイズ
        mesh_data_size += index_count * sizeof(u32); // インデックスデータサイズ
    }

    // カスタムデータチャンクのサイズ
    const u32 custom_data_size = 0; // 今回はカスタムデータ無し

    // ファイルヘッダーの設定
    MFMFileHeader file_header{};
    file_header.file_size 
        = sizeof(MFMFileHeader) + sizeof(MFMInfoHeader) + sizeof(MFMMeshHeader) + sizeof(MFMCustomHeader) 
            + custom_data_size + mesh_data_size;

    // サイズを返す
    rt_data_size = file_header.file_size;

    // 情報ヘッダーの設定
    MFMInfoHeader info_header{};
    info_header.mesh_header_offset =  sizeof(MFMFileHeader) + sizeof(MFMInfoHeader);
    info_header.mesh_header_size = sizeof(MFMMeshHeader);
    info_header.custom_header_offset = sizeof(MFMFileHeader) + sizeof(MFMInfoHeader) + sizeof(MFMMeshHeader);
    info_header.custom_header_size = sizeof(MFMCustomHeader);

    // メッシュヘッダーの設定
    MFMMeshHeader mesh_header{};
    mesh_header.mesh_data_offset 
        = sizeof(MFMFileHeader) + sizeof(MFMInfoHeader) + sizeof(MFMMeshHeader) + sizeof(MFMCustomHeader);
    mesh_header.mesh_data_size = mesh_data_size;
    mesh_header.material_count = static_cast<u32>(fbx_data->GetMaterialIndices().size());

    // カスタムヘッダーの設定
    MFMCustomHeader custom_header{};
    custom_header.custom_data_offset 
        = sizeof(MFMFileHeader) + sizeof(MFMInfoHeader) + sizeof(MFMMeshHeader) + sizeof(MFMCustomHeader) + mesh_data_size;
    custom_header.custom_data_size = custom_data_size;
    custom_header.coord_system = 0; // 右手系
    custom_header.up_axis = 1; // Y軸が上方向

    // バッファの確保
    std::unique_ptr<u8[]> buffer = std::make_unique<u8[]>(file_header.file_size);

    // ファイルヘッダーを書き込む
    u32 offset = 0;
    for (u32 i = 0; i < sizeof(MFMFileHeader); ++i)
        buffer[i] = reinterpret_cast<u8*>(&file_header)[i];
    offset += sizeof(MFMFileHeader);

    // 情報ヘッダーを書き込む
    for (u32 i = 0; i < sizeof(MFMInfoHeader); ++i)
        buffer[offset + i] = reinterpret_cast<u8*>(&info_header)[i];
    offset += sizeof(MFMInfoHeader);

    // メッシュヘッダーを書き込む
    for (u32 i = 0; i < sizeof(MFMMeshHeader); ++i)
        buffer[offset + i] = reinterpret_cast<u8*>(&mesh_header)[i];
    offset += sizeof(MFMMeshHeader);

    // カスタムヘッダーを書き込む
    for (u32 i = 0; i < sizeof(MFMCustomHeader); ++i)
        buffer[offset + i] = reinterpret_cast<u8*>(&custom_header)[i];
    offset += sizeof(MFMCustomHeader);

    // メッシュデータチャンクを書き込む
    for (const int material_index : fbx_data->GetMaterialIndices())
    {
        // メッシュノードの設定
        MFMMeshNode mesh_node{};
        mesh_node.material_name_offset = offset + sizeof(MFMMeshNode);
        mesh_node.material_name_size = static_cast<u32>(fbx_data->GetMaterialName(material_index).size());

        mesh_node.vertex_offset = offset + sizeof(MFMMeshNode) + mesh_node.material_name_size;
        mesh_node.vertex_size = sizeof(FBXVertex);
        mesh_node.vertex_count = static_cast<u32>(fbx_data->GetVertices(material_index).size());

        mesh_node.index_offset 
            = offset + sizeof(MFMMeshNode) + mesh_node.material_name_size + mesh_node.vertex_count * mesh_node.vertex_size;
        mesh_node.index_size = sizeof(u32);
        mesh_node.index_count = static_cast<u32>(fbx_data->GetIndices(material_index).size());

        // メッシュノードを書き込む
        for (u32 i = 0; i < sizeof(MFMMeshNode); ++i)
            buffer[offset + i] = reinterpret_cast<u8*>(&mesh_node)[i];
        offset += sizeof(MFMMeshNode);

        // マテリアル名を書き込む
        std::string_view material_name = fbx_data->GetMaterialName(material_index);
        for (u32 i = 0; i < material_name.size(); ++i)
            buffer[offset + i] = static_cast<u8>(material_name[i]);
        offset += mesh_node.material_name_size;

        // 頂点データを書き込む
        for (size_t vertex_count = 0; vertex_count < fbx_data->GetVertices(material_index).size(); ++vertex_count)
        {
            const FBXVertex& vertex = fbx_data->GetVertices(material_index)[vertex_count];
            for (u32 j = 0; j < sizeof(FBXVertex); ++j)
                buffer[offset + vertex_count * sizeof(FBXVertex) + j] = reinterpret_cast<const u8*>(&vertex)[j];
        }
        offset += mesh_node.vertex_count * mesh_node.vertex_size;

        // インデックスデータを書き込む
        for (size_t index_count = 0; index_count < fbx_data->GetIndices(material_index).size(); ++index_count)
        {
            const u32& index = fbx_data->GetIndices(material_index)[index_count];
            for (u32 j = 0; j < sizeof(u32); ++j)
                buffer[offset + index_count * sizeof(u32) + j] = reinterpret_cast<const u8*>(&index)[j];
        }
        offset += mesh_node.index_count * mesh_node.index_size;
    }

    // カスタムデータチャンクを書き込む
    // 今回はカスタムデータ無し

    return buffer;
}

} // namespace model_converter