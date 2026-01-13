## Balloon Shooter
C++、DirectX12を使用したシンプルなFPSゲームです。
バルーンをすべて撃ち落とすとクリアとなります。

## プロジェクト構成
以下が、プロジェクトの主要なプロジェクト構成です。
- balloon_shooter
  - ゲーム本体のプロジェクト
- directx12_helpers
  - DirectX12の補助機能を提供、公式のもの
- directxtex
  - DirectXTexライブラリ
- directxtk12
  - DirectXTK12ライブラリ
- example
  - ゲームプロジェクトのテンプレート
- wbp_collision
  - 衝突判定の機能を提供
- wbp_d3d12
  - DirectX12を使用し描画する機能を提供
- wbp_fbx_loader
  - FBXファイルを各形式で読み込む機能を提供
- wbp_identity
  - エンティティに固有情報を付加する機能を提供
- wbp_locator
  - エンティティを特定の位置にファイル情報から配置する機能を提供
- wbp_model
  - モデルデータの作成を行う機能を提供
- wbp_physics
  - エンティティを衝突対象に加え、物理的な情報を持たせる機能を提供
- wbp_png_loader
  - DirectXTexを使用しPNGファイルを読み込む機能を提供
- wbp_primitive
  - 形状データを使用して行う処理のヘルパーを提供
- wbp_primitive_test
  - wbp_primitiveの単体テストを行うプロジェクト
- wbp_render
  - DirectX12を使用しレンダリングを行う機能を提供
- wbp_texture
  - テクスチャデータの作成を行う機能を提供
- wbp_transform
  - エンティティの位置、回転、スケールを親子関係を考慮し管理する機能を提供
- windows_base
  - Windowsアプリケーションの基盤機能を提供
- windows_base_app_template
  - アプリケーションのテンプレートプロジェクト
- windows_base_plugin_template
  - プラグインのテンプレートプロジェクト