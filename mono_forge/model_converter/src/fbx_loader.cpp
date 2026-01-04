#include "include/pch.h"
#include "include/fbx_loader.h"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettingspath.h>
#pragma comment(lib, "libfbxsdk.lib")

namespace model_converter
{

std::string_view FBXFileData::GetFileExt() const
{
    return file_ext_;
}

const std::vector<FBXVertex>& FBXFileData::GetVertices(int material_index) const
{
    auto it = meshes_.find(material_index);
    assert(it != meshes_.end() && "Material index not found in meshes_");
    return it->second.vertices_;
}

u32 FBXFileData::AddVertex(const FBXVertex& vertex, int material_index)
{
    // 指定されたマテリアルインデックスに対応するメッシュを取得または作成
    FBXMesh& mesh = meshes_[material_index];

    // 頂点を追加
    mesh.vertices_.emplace_back(vertex);

    // 追加した頂点のインデックスを返す
    return static_cast<u32>(mesh.vertices_.size() - 1);
}

u32 FBXFileData::AddVertex(FBXVertex&& vertex, int material_index)
{
    // 指定されたマテリアルインデックスに対応するメッシュを取得または作成
    FBXMesh& mesh = meshes_[material_index];

    // 頂点を追加
    mesh.vertices_.emplace_back(std::move(vertex));

    // 追加した頂点のインデックスを返す
    return static_cast<u32>(mesh.vertices_.size() - 1);
}

const std::vector<u32>& FBXFileData::GetIndices(int material_index) const
{
    auto it = meshes_.find(material_index);
    assert(it != meshes_.end() && "Material index not found in meshes_");
    return it->second.indices_;
}

void FBXFileData::AddIndex(u32 index, int material_index)
{
    // 指定されたマテリアルインデックスに対応するメッシュを取得または作成
    FBXMesh& mesh = meshes_[material_index];

    // インデックスを追加
    mesh.indices_.emplace_back(index);
}

const std::unordered_set<int>& FBXFileData::GetMaterialIndices() const
{
    return material_indices_;
}

std::string_view FBXFileData::GetMaterialName(int material_index) const
{
    // マテリアルインデックスに対応するマテリアル名を取得
    auto it = material_names_.find(material_index);
    assert(it != material_names_.end() && "Material index not found in material_names_");

    // マテリアル名を返す
    return it->second;
}

void FBXFileData::AddMaterial(int material_index, std::string_view material_name)
{
    // 既に登録されていないか確認
    auto it = material_indices_.find(material_index);

    if (it != material_indices_.end())
        return; // 既に登録されている場合は何もしない

    // マテリアルインデックスをセットに追加
    material_indices_.insert(material_index);

    // マテリアル名を登録
    material_names_[material_index] = std::string(material_name);
}

FBXLoader::FBXLoader()
{
}

FBXLoader::~FBXLoader()
{
}

std::string_view FBXLoader::GetSupportedFileExt() const
{
    return supported_file_ext_;
}

// メッシュノード1つ分を処理する関数
void ProcessMeshNode(
    fbxsdk::FbxNode* node,
    FBXFileData& fbx_data
)
{
    if (!node) return;

    fbxsdk::FbxNodeAttribute* node_attr = node->GetNodeAttribute();
    if (!node_attr) return;
    if (node_attr->GetAttributeType() != fbxsdk::FbxNodeAttribute::eMesh) return;

    fbxsdk::FbxMesh* mesh = node->GetMesh();
    if (!mesh) return;

    // マテリアル要素を取得
    const fbxsdk::FbxGeometryElementMaterial* material_element = mesh->GetElementMaterial();

    // 各要素の使用するインデックスを0に設定
    const int element_uv_index = 0;
    const int element_normal_index = 0;
    const int element_tangent_index = 0;

    // 各要素を取得
    const fbxsdk::FbxGeometryElementUV* element_uv = mesh->GetElementUV(element_uv_index);
    if (!element_uv) return; // UVが存在しない場合はスキップ

    const fbxsdk::FbxGeometryElementNormal* element_normal = mesh->GetElementNormal(element_normal_index);
    if (!element_normal) return; // 法線が存在しない場合はスキップ

    const fbxsdk::FbxGeometryElementTangent* element_tangent = mesh->GetElementTangent(element_tangent_index);
    if (!element_tangent) return; // 接線が存在しない場合はスキップ

    // グローバル変換行列を取得
    fbxsdk::FbxAMatrix global_matrix = node->EvaluateGlobalTransform();

    // ポリゴン数を取得
    const int polygon_count = mesh->GetPolygonCount();

    // ポリゴン頂点インデックス
    int polygon_vertex_index = 0;

    for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
    {
        // マテリアルインデックスを取得（material_element が無い場合は 0）
        int material_index = 0;
        if (material_element && material_element->GetIndexArray().GetCount() > polygon_index)
        {
            material_index = material_element->GetIndexArray().GetAt(polygon_index);
        }

        // マテリアル名を取得
        fbxsdk::FbxSurfaceMaterial* material = node->GetMaterial(material_index);
        std::string material_name = material ? material->GetName() : "";

        // マテリアル情報をFBXFileDataに追加
        fbx_data.AddMaterial(material_index, material_name);

        // ポリゴンの頂点数を取得
        const int polygon_size = mesh->GetPolygonSize(polygon_index);
        for (int vertex_index = 0; vertex_index < polygon_size; ++vertex_index)
        {
            // 頂点データ格納先の構造体を作成
            FBXVertex vertex{};

            // ポリゴンの頂点インデックスからコントロールポイントインデックスを取得
            const int control_point_index = mesh->GetPolygonVertex(polygon_index, vertex_index);
            if (control_point_index < 0) continue;

            // 頂点座標を取得
            fbxsdk::FbxVector4 pos = global_matrix.MultT(mesh->GetControlPointAt(control_point_index));
            vertex.position_ = DirectX::XMFLOAT3(
                static_cast<float>(pos[0]),
                static_cast<float>(pos[1]),
                static_cast<float>(pos[2])
            );

            // UVを取得
            switch (element_uv->GetMappingMode())
            {
            case fbxsdk::FbxGeometryElement::eByControlPoint:
                switch (element_uv->GetReferenceMode())
                {
                case fbxsdk::FbxGeometryElement::eDirect:
                {
                    fbxsdk::FbxVector2 uv = element_uv->GetDirectArray().GetAt(control_point_index);
                    vertex.uv_ = DirectX::XMFLOAT2(
                        static_cast<float>(uv[0]),
                        static_cast<float>(uv[1])
                    );
                }
                break;

                case fbxsdk::FbxGeometryElement::eIndexToDirect:
                {
                    int id = element_uv->GetIndexArray().GetAt(control_point_index);
                    fbxsdk::FbxVector2 uv = element_uv->GetDirectArray().GetAt(id);
                    vertex.uv_ = DirectX::XMFLOAT2(
                        static_cast<float>(uv[0]),
                        static_cast<float>(uv[1])
                    );
                }
                break;

                default:
                    break; // 他の参照モードはここでは扱わない
                }
                break;

            case fbxsdk::FbxGeometryElement::eByPolygonVertex:
            {
                int texture_uv_index = mesh->GetTextureUVIndex(polygon_index, vertex_index);
                switch (element_uv->GetReferenceMode())
                {
                case fbxsdk::FbxGeometryElement::eDirect:
                case fbxsdk::FbxGeometryElement::eIndexToDirect:
                {
                    fbxsdk::FbxVector2 uv = element_uv->GetDirectArray().GetAt(texture_uv_index);
                    vertex.uv_ = DirectX::XMFLOAT2(
                        static_cast<float>(uv[0]),
                        static_cast<float>(uv[1])
                    );
                }
                break;

                default:
                    break; // 他の参照モードはここでは扱わない
                }
            }
            break;

            default:
                break; // 他のマッピングモードはここでは扱わない
            }

            // UVのY軸を反転
            vertex.uv_.y = 1.0f - vertex.uv_.y;

            // 法線を取得
            {
                fbxsdk::FbxVector4 normal(0, 0, 1, 0);
                switch (element_normal->GetMappingMode())
                {
                case fbxsdk::FbxGeometryElement::eByControlPoint:
                    switch (element_normal->GetReferenceMode())
                    {
                    case fbxsdk::FbxGeometryElement::eDirect:
                        normal = element_normal->GetDirectArray().GetAt(control_point_index);
                        break;
                    case fbxsdk::FbxGeometryElement::eIndexToDirect:
                    {
                        int id = element_normal->GetIndexArray().GetAt(control_point_index);
                        normal = element_normal->GetDirectArray().GetAt(id);
                    }
                    break;
                    default:
                        break;
                    }
                    break;

                case fbxsdk::FbxGeometryElement::eByPolygonVertex:
                    switch (element_normal->GetReferenceMode())
                    {
                    case fbxsdk::FbxGeometryElement::eDirect:
                        normal = element_normal->GetDirectArray().GetAt(polygon_vertex_index);
                        break;
                    case fbxsdk::FbxGeometryElement::eIndexToDirect:
                    {
                        int id = element_normal->GetIndexArray().GetAt(polygon_vertex_index);
                        normal = element_normal->GetDirectArray().GetAt(id);
                    }
                    break;
                    default:
                        break;
                    }
                    break;

                default:
                    // 他のマッピングモードはここでは扱わない
                    break;
                }

                // global_matrixの回転成分のみ抽出（スケール=1、移動=0）
                fbxsdk::FbxAMatrix normal_matrix = global_matrix;
                normal_matrix.SetS(fbxsdk::FbxVector4(1, 1, 1, 1)); // スケールを1に
                normal_matrix.SetT(fbxsdk::FbxVector4(0, 0, 0, 0)); // 移動を0に
                fbxsdk::FbxVector4 transformed_normal = normal_matrix.MultT(normal);

                // 正規化してfloat3へ
                DirectX::XMFLOAT3 n(
                    static_cast<float>(transformed_normal[0]),
                    static_cast<float>(transformed_normal[1]),
                    static_cast<float>(transformed_normal[2])
                );
                float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
                if (len > 0.00001f)
                {
                    n.x /= len;
                    n.y /= len;
                    n.z /= len;
                }
                vertex.normal_ = n;
            }

            // 接線を取得
            if (element_tangent->GetMappingMode() == fbxsdk::FbxGeometryElement::eByPolygonVertex)
            {
                switch (element_tangent->GetReferenceMode())
                {
                case fbxsdk::FbxGeometryElement::eDirect:
                {
                    fbxsdk::FbxVector4 tangent = element_tangent->GetDirectArray().GetAt(polygon_vertex_index);
                    vertex.tangent_ = {
                        static_cast<float>(tangent[0]),
                        static_cast<float>(tangent[1]),
                        static_cast<float>(tangent[2])
                    };
                }
                break;

                case fbxsdk::FbxGeometryElement::eIndexToDirect:
                {
                    int id = element_tangent->GetIndexArray().GetAt(polygon_vertex_index);
                    fbxsdk::FbxVector4 tangent = element_tangent->GetDirectArray().GetAt(id);
                    vertex.tangent_ = {
                        static_cast<float>(tangent[0]),
                        static_cast<float>(tangent[1]),
                        static_cast<float>(tangent[2])
                    };
                }
                break;

                default:
                    break; // 他の参照モードはここでは扱わない
                }
            }

            // 頂点データを追加
            u32 index = fbx_data.AddVertex(std::move(vertex), material_index);

            // インデックスを追加
            fbx_data.AddIndex(index, material_index);

            // ポリゴン頂点インデックスをインクリメント
            ++polygon_vertex_index;

        } // vertex_index
    }     // polygon_index
}

// ノードツリーを再帰的に走査する関数
void TraverseNode(
    fbxsdk::FbxNode* node,
    FBXFileData& fbx_data
)
{
    if (!node) return;

    // このノードのメッシュを処理
    ProcessMeshNode(node, fbx_data);

    // 子ノードを再帰的に処理
    const int child_count = node->GetChildCount();
    for (int i = 0; i < child_count; ++i)
    {
        fbxsdk::FbxNode* child = node->GetChild(i);
        TraverseNode(child, fbx_data);
    }
}

std::unique_ptr<IFileData> FBXLoader::Load(std::string_view file_path) const
{
    // FBX SDK Managerの生成
    fbxsdk::FbxManager* manager = fbxsdk::FbxManager::Create();
    if (!manager)
    {
        std::cerr << "FBX Load Error: Failed to create FbxManager." << std::endl;
        return nullptr;
    }

    // IOSettingsの生成と設定
    fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(manager, IOSROOT);
    manager->SetIOSettings(ios);

    // Importerの生成
    fbxsdk::FbxImporter* importer = fbxsdk::FbxImporter::Create(manager, "");
    if (!importer)
    {
        std::cerr << "FBX Load Error: Failed to create FbxImporter." << std::endl;
        manager->Destroy();
        return nullptr;
    }

    // Importerの初期化
    bool import_status = importer->Initialize(file_path.data(), -1, manager->GetIOSettings());
    if (!import_status)
    {
        const char* err = importer->GetStatus().GetErrorString();
        std::cerr << "FBX Importer Initialize Error: " << err << std::endl;

        importer->Destroy();
        manager->Destroy();
        return nullptr;
    }

    // Sceneの生成
    fbxsdk::FbxScene* scene = fbxsdk::FbxScene::Create(manager, file_path.data());
    if (!scene)
    {
        std::cerr << "FBX Load Error: Failed to create FbxScene." << std::endl;
        importer->Destroy();
        manager->Destroy();
        return nullptr;
    }

    // Sceneへインポート
    if (!importer->Import(scene))
    {
        std::cerr << "FBX Load Error: Import failed." << std::endl;
        scene->Destroy();
        importer->Destroy();
        manager->Destroy();
        return nullptr;
    }

    // 座標系をDirectXに変換
    fbxsdk::FbxAxisSystem dx(fbxsdk::FbxAxisSystem::eDirectX);
    if (scene->GetGlobalSettings().GetAxisSystem() != dx)
    {
        dx.DeepConvertScene(scene);
    }

    // RootNodeの取得
    fbxsdk::FbxNode* root_node = scene->GetRootNode();
    if (!root_node)
    {
        std::cerr << "FBX Load Error: RootNode is nullptr." << std::endl;
        scene->Destroy();
        importer->Destroy();
        manager->Destroy();
        return nullptr;
    }

    // FBXFileDataの生成
    std::unique_ptr<FBXFileData> fbx_data = std::make_unique<FBXFileData>();

    // ルートから再帰的に全ノードを走査
    TraverseNode(root_node, *fbx_data);

    importer->Destroy();
    scene->Destroy();
    manager->Destroy();

    return fbx_data;
}

}
 // namespace model_converter
