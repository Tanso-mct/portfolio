#include "mono_scene_test/pch.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mem_alloc_fixed_block.lib")

#include "mono_scene/include/component_scene.h"
#include "mono_scene/include/component_scene_tag.h"
#include "mono_scene/include/system_scene.h"
#pragma comment(lib, "mono_scene.lib")

namespace
{
    constexpr int INITIAL_A_VALUE = 7;
    class TestAComponent
    {
    public:
        int value;

        TestAComponent() : value(INITIAL_A_VALUE) 
        {
            std::cout << "TestAComponent created with value: " << value << std::endl;
        }

        ~TestAComponent() 
        { 
            std::cout << "TestAComponent destroyed with value: " << value << std::endl;
            value = 0; 
        }
    };
    riaecs::ComponentRegistrar<TestAComponent, 10> TestAComponentID;

    class TestFileData : public riaecs::IFileData
    {
    public:
        TestFileData() 
        {
            std::cout << "TestFileData created" << std::endl;
        }

        ~TestFileData() override
        { 
            std::cout << "TestFileData destroyed" << std::endl; 
        }
    };

    class TestFileLoader : public riaecs::IFileLoader
    {
    public:
        TestFileLoader() 
        {
            std::cout << "TestFileLoader created" << std::endl;
        }

        ~TestFileLoader() override
        { 
            std::cout << "TestFileLoader destroyed" << std::endl; 
        }

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override
        {
            std::cout << "Loading file data from: " << filePath << std::endl;
            return std::make_unique<TestFileData>();
        }
    };
    riaecs::FileLoaderRegistrar<TestFileLoader> TestFileLoaderID;

    constexpr int INITIAL_ASSET_VALUE = 10;
    class TestAsset : public riaecs::IAsset
    {
    public:
        TestAsset() 
        {
            std::cout << "TestAsset created" << std::endl;
        }

        ~TestAsset() override
        { 
            std::cout << "TestAsset destroyed" << std::endl; 
        }

        int value = INITIAL_ASSET_VALUE;
    };

    class TestAssetStagingArea : public riaecs::IAssetStagingArea
    {
    public:
        TestAssetStagingArea() 
        {
            std::cout << "TestAssetStagingArea created" << std::endl;
        }

        ~TestAssetStagingArea() override
        { 
            std::cout << "TestAssetStagingArea destroyed" << std::endl; 
        }
    };

    class TestAssetFactory : public riaecs::IAssetFactory
    {
    public:
        TestAssetFactory() 
        {
            std::cout << "TestAssetFactory created" << std::endl;
        }

        ~TestAssetFactory() override
        { 
            std::cout << "TestAssetFactory destroyed" << std::endl; 
        }

        std::unique_ptr<riaecs::IAssetStagingArea> Prepare() const override
        {
            std::cout << "Preparing asset staging area" << std::endl;
            return std::make_unique<TestAssetStagingArea>();
        }

        std::unique_ptr<riaecs::IAsset> Create
        (
            const riaecs::IFileData &fileData, riaecs::IAssetStagingArea &stagingArea
        ) const override
        {
            std::cout << "Creating asset from file data" << std::endl;
            return std::make_unique<TestAsset>();
        }

        void Commit(riaecs::IAssetStagingArea &stagingArea) const override
        {
            std::cout << "Committing asset staging area" << std::endl;
        }
    };
    riaecs::AssetFactoryRegistrar<TestAssetFactory> TestAssetFactoryID;

    riaecs::AssetSourceRegistrar TestAssetSourceID
    (
        "test_asset_path", 
        TestFileLoaderID(), 
        TestAssetFactoryID()
    );

    class TestSystemListEditCommand : public riaecs::ISystemLoopCommand
    {
    public:
        TestSystemListEditCommand() 
        {
            std::cout << "TestSystemListEditCommand created" << std::endl;
        }

        ~TestSystemListEditCommand() override
        { 
            std::cout << "TestSystemListEditCommand destroyed" << std::endl; 
        }

        void Execute(riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override
        {
            std::cout << "Executing TestSystemListEditCommand" << std::endl;
        }

        std::unique_ptr<riaecs::ISystemLoopCommand> Clone() const override
        {
            return std::make_unique<TestSystemListEditCommand>(*this);
        }
    };

    class TestEntitiesFactory : public mono_scene::IEntitiesFactory
    {
    public:
        TestEntitiesFactory() 
        {
            std::cout << "TestEntitiesFactory created" << std::endl;
        }

        ~TestEntitiesFactory() override
        { 
            std::cout << "TestEntitiesFactory destroyed" << std::endl; 
        }

        riaecs::StagingEntityArea CreateEntities(
            riaecs::Entity sceneEntity ,riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override
        {
            std::cout << "Creating entities in TestEntitiesFactory" << std::endl;

            riaecs::StagingEntityArea stagingArea = ecsWorld.CreateStagingArea();

            riaecs::Entity entity = ecsWorld.CreateEntity(stagingArea);
            ecsWorld.AddComponent(entity, TestAComponentID());
            ecsWorld.AddComponent(entity, mono_scene::ComponentSceneTagID());

            mono_scene::ComponentSceneTag* tag 
            = riaecs::GetComponent<mono_scene::ComponentSceneTag>(ecsWorld, entity, mono_scene::ComponentSceneTagID());
            {
                mono_scene::ComponentSceneTag::SetupParam param;
                param.sceneEntity = sceneEntity;
                tag->Setup(param);
            }

            return stagingArea;
        }
    };

}

TEST(Scene, LifeCycle)
{
    std::unique_ptr<riaecs::IECSWorld> ecsWorld = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    EXPECT_TRUE(ecsWorld->IsReady());
    ecsWorld->CreateWorld();

    std::unique_ptr<riaecs::IAssetContainer> assetCont = std::make_unique<riaecs::AssetContainer>();
    assetCont->Create(riaecs::gAssetSourceRegistry->GetCount());

    std::unique_ptr<riaecs::ISystemLoopCommandQueue> systemLoopCmdQueue = std::make_unique<riaecs::SystemLoopCommandQueue>();

    riaecs::Entity sceneEntity = ecsWorld->CreateEntity();
    ecsWorld->AddComponent(sceneEntity, mono_scene::ComponentSceneID());

    mono_scene::ComponentScene* scene
    = riaecs::GetComponent<mono_scene::ComponentScene>(*(ecsWorld), sceneEntity, mono_scene::ComponentSceneID());
    mono_scene::ComponentScene::SetupParam sceneParam;
    sceneParam.entitiesFactory_ = std::make_unique<TestEntitiesFactory>();
    sceneParam.assetSourceIDs_ = { TestAssetSourceID() };

    std::vector<std::unique_ptr<riaecs::ISystemLoopCommand>> cmds;
    cmds.emplace_back(std::make_unique<TestSystemListEditCommand>());
    sceneParam.systemListEditCmds_ = std::move(cmds);

    sceneParam.targetEditCmdIndex_ = 0;
    sceneParam.needsLoad_ = true;
    sceneParam.needsEditSystemList_ = true;
    scene->Setup(sceneParam);

    bool isRunning = true;
    std::future<void> mainLoop = std::async
    (
        std::launch::async, [&isRunning, &ecsWorld, &assetCont, &systemLoopCmdQueue]()
        {
            std::future<void> loadFuture;
            while (isRunning)
            {
                for (const riaecs::Entity &entity : ecsWorld->View(mono_scene::ComponentSceneID())())
                {
                    mono_scene::ComponentScene* scene
                    = riaecs::GetComponent<mono_scene::ComponentScene>(*(ecsWorld), entity, mono_scene::ComponentSceneID());
                    EXPECT_NE(scene, nullptr);

                    if (scene->NeedsLoad() && riaecs::CheckFutureIsReady(loadFuture))
                    {
                        loadFuture = std::async
                        (
                            std::launch::async, &mono_scene::LoadScene, 
                            entity, scene, std::ref(*ecsWorld), std::ref(*assetCont)
                        );
                    }

                    if (scene->IsLoaded(), !scene->StagingEntityAreaRO()().empty())
                    {
                        ecsWorld->CommitEntities(scene->StagingEntityAreaRW()());
                        scene->NeedsReleaseRW()() = true;
                    }
                    
                    if (scene->NeedsRelease())
                        mono_scene::ReleaseScene(entity, scene, *ecsWorld, *assetCont);

                    if (scene->IsReleased())
                        isRunning = false;

                    if (scene->NeedsEditSystemList())
                        mono_scene::AddSystemListEditCommand(scene, *systemLoopCmdQueue);
                }
            }

            if (loadFuture.valid())
                loadFuture.get();
        }
    );
}