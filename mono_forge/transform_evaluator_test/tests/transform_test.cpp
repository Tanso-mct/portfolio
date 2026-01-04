#include "transform_evaluator_test/pch.h"

#include "transform_evaluator/include/transform_manager.h"

TEST(Transform, Create)
{
    // Create a TransformContainer
    std::unique_ptr<transform_evaluator::TransformContainer> transform_container =
        std::make_unique<transform_evaluator::TransformContainer>();

    // Create a TransformManager
    std::unique_ptr<transform_evaluator::TransformManager> transform_manager =
        std::make_unique<transform_evaluator::TransformManager>(*transform_container);

    // Create a TransformAdder
    std::unique_ptr<transform_evaluator::TransformAdder> transform_adder =
        std::make_unique<transform_evaluator::TransformAdder>(*transform_container);

    // Create a TransformEraser
    std::unique_ptr<transform_evaluator::TransformEraser> transform_eraser =
        std::make_unique<transform_evaluator::TransformEraser>(*transform_container);

    // Create new Transform
    std::unique_ptr<transform_evaluator::Transform> transform =
        std::make_unique<transform_evaluator::Transform>(
            DirectX::XMFLOAT3{1.0f, 2.0f, 3.0f},
            DirectX::XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f},
            DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f});

    // Add Transform to the transform_container
    transform_evaluator::TransformHandle handle = transform_adder->AddTransform(std::move(transform));

    // Verify the Transform was added correctly
    transform_manager->WithLock([&](transform_evaluator::TransformManager& manager)
    {
        const transform_evaluator::Transform& retrieved_transform = manager.GetTransform(handle);
        const transform_evaluator::TRS& trs = retrieved_transform.GetWorldTRS();

        EXPECT_EQ(trs.translation.x, 1.0f);
        EXPECT_EQ(trs.translation.y, 2.0f);
        EXPECT_EQ(trs.translation.z, 3.0f);
        EXPECT_EQ(trs.rotation.x, 0.0f);
        EXPECT_EQ(trs.rotation.y, 0.0f);
        EXPECT_EQ(trs.rotation.z, 0.0f);
        EXPECT_EQ(trs.rotation.w, 1.0f);
        EXPECT_EQ(trs.scale.x, 1.0f);
        EXPECT_EQ(trs.scale.y, 1.0f);
        EXPECT_EQ(trs.scale.z, 1.0f);
    });

    // Erase the Transform
    transform_eraser->EraseTransform(handle);

    // Clean up
    transform_eraser.reset();
    transform_adder.reset();
    transform_manager.reset();
    transform_container.reset();
}