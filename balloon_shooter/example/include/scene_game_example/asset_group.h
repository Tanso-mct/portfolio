#pragma once
#include "windows_base/windows_base.h"

#include "example/include/mode.h"

namespace example
{

#if defined(EXAMPLE_MODE_FBX_LOADER) || defined(EXAMPLE_MODE_PNG_LOADER) || defined(EXAMPLE_MODE_TEXTURE)

    const size_t &MockAssetFactoryID();

    class MockAssetFactory : public wb::IAssetFactory
    {
    public:
        MockAssetFactory() = default;
        ~MockAssetFactory() override = default;

        std::unique_ptr<wb::IAsset> Create(wb::IFileData &fileData) const override;
    };

    const size_t &MockAssetID();

#elif defined(EXAMPLE_MODE_MODEL) || defined(EXAMPLE_MODE_RENDER)

    const size_t &CharacterModelAssetID();
    const size_t &CharacterDiffuseTextureAssetID();

    const size_t &SpriteTextureAssetID();

#elif defined(EXAMPLE_MODE_COLLISION) || defined(EXAMPLE_MODE_PHYSICS)

    const size_t &CharacterModelAssetID();
    const size_t &CharacterColliderShapeAssetID();
    const size_t &FieldModelAssetID();
    const size_t &FieldColliderShapeAssetID();

#elif defined(EXAMPLE_MODE_LOCATOR)

    const size_t &CharacterModelAssetID();
    const size_t &FieldModelAssetID();

    const size_t &CharacterLocatorAssetID();

#endif

    class GameExampleAssetGroup : public wb::AssetGroup
    {
    public:
        GameExampleAssetGroup();
        ~GameExampleAssetGroup() override = default;
    };

} // namespace example