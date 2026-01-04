#include "example/src/pch.h"
#include "example/include/scene_game_example/asset_group.h"

#include "wbp_fbx_loader/plugin.h"
#pragma comment(lib, "wbp_fbx_loader.lib")

#include "wbp_model/plugin.h"
#pragma comment(lib, "wbp_model.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

#include "wbp_locator/plugin.h"
#pragma comment(lib, "wbp_locator.lib")

#include "wbp_png_loader/plugin.h"
#pragma comment(lib, "wbp_png_loader.lib")

#include "wbp_texture/plugin.h"
#pragma comment(lib, "wbp_texture.lib")

#if defined(EXAMPLE_MODE_FBX_LOADER)

const size_t &example::MockAssetFactoryID()
{
    static size_t id = wb::IDFactory::CreateAssetFactoryID();
    return id;
}

std::unique_ptr<wb::IAsset> example::MockAssetFactory::Create(wb::IFileData &fileData) const
{
    return nullptr;
}

const size_t &example::MockAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace example
{
    WB_REGISTER_ASSET(MockAssetID, MockAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(), "../resources/example/character.fbx");

} // namespace example

#elif defined(EXAMPLE_MODE_MODEL) || defined(EXAMPLE_MODE_RENDER)

const size_t &example::CharacterModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::CharacterDiffuseTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::SpriteTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace example
{
    WB_REGISTER_ASSET
    (
        CharacterModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(), 
        "../resources/example/isu_2x2.fbx"
    );

    WB_REGISTER_ASSET
    (
        CharacterDiffuseTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/example/isu_k_Base_color.png"
    );

    WB_REGISTER_ASSET
    (
        SpriteTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/example/lena_std.png"
    );

} // namespace example

#elif defined(EXAMPLE_MODE_COLLISION) || defined(EXAMPLE_MODE_PHYSICS)

const size_t &example::CharacterModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::CharacterColliderShapeAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::FieldModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::FieldColliderShapeAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace example
{
    WB_REGISTER_ASSET
    (
        CharacterModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/character.fbx"
    );

    WB_REGISTER_ASSET
    (
        CharacterColliderShapeAssetID, wbp_collision::ColliderShapeAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/character_collider_shape.fbx"
    );

    WB_REGISTER_ASSET
    (
        FieldModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/field.fbx"
    );

    WB_REGISTER_ASSET
    (
        FieldColliderShapeAssetID, wbp_collision::ColliderShapeAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/field_collider_shape.fbx"
    );

} // namespace example

#elif defined(EXAMPLE_MODE_LOCATOR)

const size_t &example::CharacterModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::FieldModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &example::CharacterLocatorAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace example
{
    WB_REGISTER_ASSET
    (
        CharacterModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/character.fbx"
    );

    WB_REGISTER_ASSET
    (
        FieldModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/field.fbx"
    );

    WB_REGISTER_ASSET
    (
        CharacterLocatorAssetID, wbp_locator::LocatorAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/example/character_locator.fbx"
    );

} // namespace example

#elif defined(EXAMPLE_MODE_PNG_LOADER) 

const size_t &example::MockAssetFactoryID()
{
    static size_t id = wb::IDFactory::CreateAssetFactoryID();
    return id;
}

std::unique_ptr<wb::IAsset> example::MockAssetFactory::Create(wb::IFileData &fileData) const
{
    return nullptr;
}

const size_t &example::MockAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace example
{
    WB_REGISTER_ASSET_FACTORY(MockAssetFactoryID(), MockAssetFactory);
    WB_REGISTER_ASSET(MockAssetID, MockAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(), "../resources/example/lena_std.png");

} // namespace example

#elif defined(EXAMPLE_MODE_TEXTURE)

const size_t &example::MockAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace example
{
    WB_REGISTER_ASSET(MockAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(), "../resources/example/lena_std.png");

} // namespace example

#endif



example::GameExampleAssetGroup::GameExampleAssetGroup()
{
#if defined(EXAMPLE_MODE_FBX_LOADER) || defined(EXAMPLE_MODE_PNG_LOADER) || defined(EXAMPLE_MODE_TEXTURE)

    AddAssetID(example::MockAssetID());

#elif defined(EXAMPLE_MODE_MODEL) || defined(EXAMPLE_MODE_RENDER)

    AddAssetID(example::CharacterModelAssetID());
    AddAssetID(example::CharacterDiffuseTextureAssetID());

    AddAssetID(example::SpriteTextureAssetID());

#elif defined(EXAMPLE_MODE_COLLISION) || defined(EXAMPLE_MODE_PHYSICS)

    AddAssetID(example::CharacterModelAssetID());
    AddAssetID(example::CharacterColliderShapeAssetID());
    AddAssetID(example::FieldModelAssetID());
    AddAssetID(example::FieldColliderShapeAssetID());

#elif defined(EXAMPLE_MODE_LOCATOR)

    AddAssetID(example::CharacterModelAssetID());
    AddAssetID(example::FieldModelAssetID());

    AddAssetID(example::CharacterLocatorAssetID());

#endif
}