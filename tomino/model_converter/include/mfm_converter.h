#pragma once

#include <memory>
#include <string_view>
#include <string>

#include "include/type.h"
#include "include/interfaces/converter.h"

namespace model_converter
{
    #pragma pack(push, 1)

    // ファイルヘッダー構造体
    struct MFMFileHeader
    {
        u16 file_type = 0x4D46; // ファイルタイプ識別子('MF'のASCIIコード)
        u32 file_size = 0; // ファイルサイズ(ファイルヘッダー + 情報ヘッダー + メッシュデータチャンクの合計)
        u16 content_type = 0x0001; // コンテンツタイプ(0x0001:メッシュのみ)
    };

    // 情報ヘッダー構造体
    struct MFMInfoHeader
    {
        u32 mesh_header_offset = 0; // メッシュヘッダーのオフセット
        u32 mesh_header_size = 0; // メッシュヘッダーのサイズ

        u32 custom_header_offset = 0; // カスタムヘッダーのオフセット
        u32 custom_header_size = 0; // カスタムヘッダーのサイズ
    };

    struct MFMMeshHeader
    {
        u32 mesh_data_offset = 0; // メッシュデータチャンクのオフセット
        u32 mesh_data_size = 0; // メッシュデータチャンクのサイズ

        u32 material_count = 0; // マテリアル数(メッシュノード数)
    };

    struct MFMCustomHeader
    {
        u32 custom_data_offset = 0; // カスタムデータチャンクのオフセット
        u32 custom_data_size = 0; // カスタムデータチャンクのサイズ

        u32 coord_system = 0; // 座標系(0:右手系, 1:左手系)
        u32 up_axis = 1; // 上方向の軸(0:X軸, 1:Y軸, 2:Z軸)
    };

    struct MFMMeshNode
    {
        u32 next_node_offset = 0; // 次のメッシュノードへのオフセット

        u32 material_name_offset = 0; // マテリアル名のオフセット
        u32 material_name_size = 0; // マテリアル名のサイズ

        u32 vertex_offset = 0; // 頂点データのオフセット
        u32 vertex_size = 0; // 頂点サイズ
        u32 vertex_count = 0; // 頂点数

        u32 index_offset = 0; // インデックスデータのオフセット
        u32 index_size = 0; // インデックスサイズ
        u32 index_count = 0; // インデックス数
    };

    #pragma pack(pop)

    // MFM形式(Mono Forge Model Format)への変換を行うクラス
    class MFMConverter : public IConverter
    {
    public:
        MFMConverter();
        ~MFMConverter() override;

        /***************************************************************************************************************
         * IConverterの純粋仮想関数の実装
        /**************************************************************************************************************/

        std::string_view GetInputFileExt() const override;
        std::string_view GetConvertedFileExt() const override;
        std::unique_ptr<u8[]> Convert(const IFileData* file_data, u32& rt_data_size) const override;

    private:
        const std::string input_file_ext_ = ".fbx";
        const std::string converted_file_ext_ = ".mfm";
    };
    

} // namespace model_converter