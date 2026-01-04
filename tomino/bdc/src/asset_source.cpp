#include "bdc/src/pch.h"
#include "bdc/include/asset_source.h"

#pragma comment(lib, "mono_file.lib")
#pragma comment(lib, "mono_asset.lib")

riaecs::AssetSourceRegistrar bdc::EmptyTextureAssetSourceID
(
    "../resources/bdc/empty_texture.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::PlayerModelAssetSourceID
(
    "../resources/bdc/player/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::PlayerLocatorAssetSourceID
(
    "../resources/bdc/player/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::PlayerBoundingBoxAssetSourceID
(
    "../resources/bdc/player/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::PlayerAlbedoTextureAssetSourceID
(
    "../resources/bdc/player/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::EnhancedPlayerAlbedoTextureAssetSourceID
(
    "../resources/bdc/player_enhanced/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::FloorModelAssetSourceID
(
    "../resources/bdc/map/floor/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::FloorAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/floor/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::FloorLocatorAssetSourceID
(
    "../resources/bdc/map/floor/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::BookShelfModelAssetSourceID
(
    "../resources/bdc/map/book_shelf/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::BookShelfAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/book_shelf/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::BookShelfLocatorAssetSourceID
(
    "../resources/bdc/map/book_shelf/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::BookShelfBoundingBoxAssetSourceID
(
    "../resources/bdc/map/book_shelf/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::CatScratchModelAssetSourceID
(
    "../resources/bdc/map/cat_scratch/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::CatScratchAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/cat_scratch/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::CatScratchLocatorAssetSourceID
(
    "../resources/bdc/map/cat_scratch/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::CatScratchBoundingBoxAssetSourceID
(
    "../resources/bdc/map/cat_scratch/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf01ModelAssetSourceID
(
    "../resources/bdc/map/food_shelf_01/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf01AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/food_shelf_01/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf01LocatorAssetSourceID
(
    "../resources/bdc/map/food_shelf_01/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf01BoundingBoxAssetSourceID
(
    "../resources/bdc/map/food_shelf_01/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf02ModelAssetSourceID
(
    "../resources/bdc/map/food_shelf_02/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf02AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/food_shelf_02/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf02LocatorAssetSourceID
(
    "../resources/bdc/map/food_shelf_02/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::FoodShelf02BoundingBoxAssetSourceID
(
    "../resources/bdc/map/food_shelf_02/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Table4x20ModelAssetSourceID
(
    "../resources/bdc/map/table_4x20/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Table4x20AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/table_4x20/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Table4x20LocatorAssetSourceID
(
    "../resources/bdc/map/table_4x20/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Table4x20BoundingBoxAssetSourceID
(
    "../resources/bdc/map/table_4x20/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Table4x6ModelAssetSourceID
(
    "../resources/bdc/map/table_4x6/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Table4x6AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/table_4x6/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Table4x6LocatorAssetSourceID
(
    "../resources/bdc/map/table_4x6/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Table4x6BoundingBoxAssetSourceID
(
    "../resources/bdc/map/table_4x6/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Table6x8ModelAssetSourceID
(
    "../resources/bdc/map/table_6x8/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Table6x8AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/table_6x8/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Table6x8LocatorAssetSourceID
(
    "../resources/bdc/map/table_6x8/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Table6x8BoundingBoxAssetSourceID
(
    "../resources/bdc/map/table_6x8/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Table4x28ModelAssetSourceID
(
    "../resources/bdc/map/table_4x28/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Table4x28AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/table_4x28/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Table4x28LocatorAssetSourceID
(
    "../resources/bdc/map/table_4x28/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Table4x28BoundingBoxAssetSourceID
(
    "../resources/bdc/map/table_4x28/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Wall1ModelAssetSourceID
(
    "../resources/bdc/map/wall_1/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Wall1AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/wall_1/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Wall1LocatorAssetSourceID
(
    "../resources/bdc/map/wall_1/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Wall1BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wall_1/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Wall2ModelAssetSourceID
(
    "../resources/bdc/map/wall_2/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Wall2AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/wall_2/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Wall2LocatorAssetSourceID
(
    "../resources/bdc/map/wall_2/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Wall2BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wall_2/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::RefrigeratorModelAssetSourceID
(
    "../resources/bdc/map/refrigerator/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::RefrigeratorLocatorAssetSourceID
(
    "../resources/bdc/map/refrigerator/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::RefrigeratorBoundingBoxAssetSourceID
(
    "../resources/bdc/map/refrigerator/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::KitchenModelAssetSourceID
(
    "../resources/bdc/map/kitchen/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::KitchenAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/kitchen/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::KitchenLocatorAssetSourceID
(
    "../resources/bdc/map/kitchen/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::KitchenBoundingBoxAssetSourceID
(
    "../resources/bdc/map/kitchen/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x2ModelAssetSourceID
(
    "../resources/bdc/map/chair_2x2/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x2AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/chair_2x2/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x2LocatorAssetSourceID
(
    "../resources/bdc/map/chair_2x2/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x2BoundingBoxAssetSourceID
(
    "../resources/bdc/map/chair_2x2/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x4ModelAssetSourceID
(
    "../resources/bdc/map/chair_2x4/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x4AlbedoTextureAssetSourceID
(
    "../resources/bdc/map/chair_2x4/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x4LocatorAssetSourceID
(
    "../resources/bdc/map/chair_2x4/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::Chair2x4BoundingBoxAssetSourceID
(
    "../resources/bdc/map/chair_2x4/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::SofaModelAssetSourceID
(
    "../resources/bdc/map/sofa/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::SofaAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/sofa/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::SofaLocatorAssetSourceID
(
    "../resources/bdc/map/sofa/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::SofaBoundingBoxAssetSourceID
(
    "../resources/bdc/map/sofa/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::MouseTrapModelAssetSourceID
(
    "../resources/bdc/map/mouse_trap/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::MouseTrapAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/mouse_trap/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::MouseTrapLocatorAssetSourceID
(
    "../resources/bdc/map/mouse_trap/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::MouseTrapBoundingBoxAssetSourceID
(
    "../resources/bdc/map/mouse_trap/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::SofaTableModelAssetSourceID
(
    "../resources/bdc/map/sofa_table/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::SofaTableAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/sofa_table/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::SofaTableLocatorAssetSourceID
(
    "../resources/bdc/map/sofa_table/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::SofaTableBoundingBoxAssetSourceID
(
    "../resources/bdc/map/sofa_table/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBoxAlbedoTextureAssetSourceID
(
    "../resources/bdc/map/wood_box/texture/wooden box_Base_color.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox01ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/01/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox01LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/01/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox01BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/01/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox02ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/02/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox02LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/02/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox02BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/02/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox03ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/03/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox03LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/03/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox03BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/03/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox04ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/04/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox04LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/04/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox04BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/04/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox05ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/05/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox05LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/05/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox05BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/05/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox06ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/06/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox06LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/06/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox06BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/06/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox07ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/07/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox07LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/07/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox07BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/07/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox08ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/08/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox08LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/08/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox08BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/08/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox09ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/09/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox09LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/09/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox09BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/09/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox10ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/10/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox10LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/10/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox10BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/10/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox11ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/11/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox11LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/11/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox11BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/11/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox12ModelAssetSourceID
(
    "../resources/bdc/map/wood_box/12/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox12LocatorAssetSourceID
(
    "../resources/bdc/map/wood_box/12/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::WoodBox12BoundingBoxAssetSourceID
(
    "../resources/bdc/map/wood_box/12/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::DotLocatorAssetSourceID
(
    "../resources/bdc/dot/locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::DotBoundingBoxAssetSourceID
(
    "../resources/bdc/dot/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::DotModelAssetSourceID
(
    "../resources/bdc/dot/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

// riaecs::AssetSourceRegistrar bdc::DotAlbedoTextureAssetSourceID
// (
//     "../resources/bdc/dot/albedo.png",
//     mono_file::FileLoaderPNGID(),
//     mono_asset::AssetFactoryTextureID()
// );

riaecs::AssetSourceRegistrar bdc::LargeEnemyModelAssetSourceID
(
    "../resources/bdc/large_enemy/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::LargeEnemyBoundingBoxAssetSourceID
(
    "../resources/bdc/large_enemy/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::SmallEnemyModelAssetSourceID
(
    "../resources/bdc/small_enemy/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::SmallEnemyBoundingBoxAssetSourceID
(
    "../resources/bdc/small_enemy/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::CageModelAssetSourceID
(
    "../resources/bdc/cage/cage_1.5.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::CageLocatorAssetSourceID
(
    "../resources/bdc/cage/cage_locator.fbx",
    mono_file::FileLoaderFBXTransformOnlyID(),
    mono_asset::AssetFactoryLocatorID()
);

riaecs::AssetSourceRegistrar bdc::SouthCageBoundingBoxAssetSourceID
(
    "../resources/bdc/cage/cage_collider_south.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::NorthCageBoundingBoxAssetSourceID
(
    "../resources/bdc/cage/cage_collider_north.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::WestCageBoundingBoxAssetSourceID
(
    "../resources/bdc/cage/cage_collider_west.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::EastCageBoundingBoxAssetSourceID
(
    "../resources/bdc/cage/cage_collider_east.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::CrystalModelAssetSourceID
(
    "../resources/bdc/crystal/model.mfm",
    mono_file::FileLoaderFBXID(),
    mono_asset::AssetFactoryModelID()
);

riaecs::AssetSourceRegistrar bdc::CrystalBoundingBoxAssetSourceID
(
    "../resources/bdc/crystal/bounding_box.fbx",
    mono_file::FileLoaderFbxMinMaxOnlyID(),
    mono_asset::AssetFactoryBoundingBoxID()
);

riaecs::AssetSourceRegistrar bdc::CrystalAlbedoTextureAssetSourceID
(
    "../resources/bdc/crystal/albedo.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::MenuBackgroundTextureAssetSourceID
(
    "../resources/bdc/menu/background.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::LoadingBackgroundTextureAssetSourceID
(
    "../resources/bdc/loading.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::PlayerBarTextureAssetSourceID
(
    "../resources/bdc/playing/player_bar.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);

riaecs::AssetSourceRegistrar bdc::GameOverBackgroundTextureAssetSourceID
(
    "../resources/bdc/playing/game_over_background.png",
    mono_file::FileLoaderPNGID(),
    mono_asset::AssetFactoryTextureID()
);
