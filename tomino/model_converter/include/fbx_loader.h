#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "include/type.h"
#include "include/interfaces/file_loader.h"

namespace model_converter
{
    // FBXファイルの頂点情報を格納する構造体
    struct FBXVertex
    {
        DirectX::XMFLOAT3 position_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        DirectX::XMFLOAT2 uv_ = DirectX::XMFLOAT2(0.0f, 0.0f);
        DirectX::XMFLOAT3 normal_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        DirectX::XMFLOAT3 tangent_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    };

    // FBXファイルのメッシュ情報を格納する構造体
    struct FBXMesh
    {
        // 頂点データ
        std::vector<FBXVertex> vertices_;

        // インデックスデータ
        std::vector<u32> indices_;
    };

    // FBXファイルのデータを格納するクラス
    class FBXFileData : public IFileData
    {
    public:
        FBXFileData() = default;
        ~FBXFileData() override = default;

        /***************************************************************************************************************
         * IFileDataの純粋仮想関数の実装
        /**************************************************************************************************************/

        std::string_view GetFileExt() const override;

        /***************************************************************************************************************
         * FBXFileDataのメンバ関数
        /**************************************************************************************************************/

        const std::vector<FBXVertex>& GetVertices(int material_index = 0) const;
        u32 AddVertex(const FBXVertex& vertex, int material_index = 0);
        u32 AddVertex(FBXVertex&& vertex, int material_index = 0);

        const std::vector<u32>& GetIndices(int material_index = 0) const;
        void AddIndex(u32 index, int material_index = 0);

        const std::unordered_set<int>& GetMaterialIndices() const;
        std::string_view GetMaterialName(int material_index) const;
        void AddMaterial(int material_index, std::string_view material_name);

    private:
        // 対応するファイル形式の拡張子
        const std::string file_ext_ = ".fbx";

        // マテリアルインデックスのセット
        std::unordered_set<int> material_indices_;

        // メッシュデータ, マテリアルインデックスをキーとする
        std::unordered_map<int, FBXMesh> meshes_;

        // マテリアルインデックスとマテリアル名の対応表
        std::unordered_map<int, std::string> material_names_;
    };

    // FBXファイルを読み込むクラス
    class FBXLoader : public IFileLoader
    {
    public:
        FBXLoader();
        ~FBXLoader() override;

        /***************************************************************************************************************
         * IFileLoaderの純粋仮想関数の実装
        /**************************************************************************************************************/

        std::string_view GetSupportedFileExt() const override;
        std::unique_ptr<IFileData> Load(std::string_view file_path) const override;

    private:
        const std::string supported_file_ext_ = ".fbx";
    };

} // namespace model_converter
