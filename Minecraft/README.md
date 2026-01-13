## Unity版 Minecraft
Unityを用いて、Minecraftを部分的に再現しました。

## プロジェクト構成
- Assets/: Unityのアセットフォルダ
  - Animation : アニメーション関連のアセット
  - Fonts : フォント関連のアセット
  - Materials : 各種マテリアル
  - Prefabs : プレハブら
  - Resources : FBXやサウンド、テクスチャなどのリソース
  - Scenes : シーンファイル
  - Scripts
    - Element : 各UI要素
    - Option : 各オプション画面
    - Parts : 各要素のベースクラス
    - System : 各要素をゲーム上で管理するものら
    - Window : 各シーンで表示するものを管理するものら
    - World : ワールド生成やブロック管理など
  - Shader : ブロックらを描画するために使用するComputeShader