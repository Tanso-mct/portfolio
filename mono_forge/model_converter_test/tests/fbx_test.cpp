#include "pch.h"

#include "include/fbx_loader.h"

TEST(FBX, Load)
{
    model_converter::FBXLoader loader;
    EXPECT_EQ(loader.GetSupportedFileExt(), ".fbx");

    std::unique_ptr<model_converter::IFileData> data = loader.Load("../resources/model_converter/bot.fbx");
    EXPECT_NE(data, nullptr);

    const model_converter::FBXFileData& fbx_data = static_cast<const model_converter::FBXFileData&>(*data);

    // マテリアルインデックスらを取得
	std::unordered_set<int> material_indices = fbx_data.GetMaterialIndices();

	// マテリアルごとの情報の確認
    for (int material_index : material_indices)
    {
        std::string_view material_name = fbx_data.GetMaterialName(material_index);
		std::cout << "Material Index: " << material_index << ", Name: " << material_name << std::endl;

		// マテリアルごとの頂点数とインデックス数の確認
		const std::vector<model_converter::FBXVertex>& material_vertices = fbx_data.GetVertices(material_index);
		std::cout << "  Vertex Count: " << material_vertices.size() << std::endl;

		const std::vector<uint32_t>& material_indices = fbx_data.GetIndices(material_index);
		std::cout << "  Index Count: " << material_indices.size() << std::endl;

        // 頂点バッファに上げられる状態にできるか確認
		EXPECT_GT(material_vertices.size(), 0u);
		const model_converter::FBXVertex* vertex_ptr = material_vertices.data();
		EXPECT_NE(vertex_ptr, nullptr);

		// インデックスバッファに上げられる状態にできるか確認
		EXPECT_GT(material_indices.size(), 0u);
		const uint32_t* index_ptr = material_indices.data();
		EXPECT_NE(index_ptr, nullptr);
    }
}