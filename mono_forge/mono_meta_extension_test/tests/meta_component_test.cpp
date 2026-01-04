#include "mono_meta_extension_test/pch.h"

#include "mono_meta_extension/include/meta_component.h"

TEST(MetaComponent, CreateMetaComponent)
{
    // Create setup parameters for MetaComponent
    std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> meta_param 
        = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();

    // Example tags
    const uint64_t example_tag_a = 3; // Represents tag bit 3
    const uint64_t example_tag_b = 7; // Represents tag bit 7
    const uint64_t example_tag_c = 10; // Represents tag bit 10

    // Example layers
    const uint64_t example_layer_a = 2; // Represents layer bit 2
    const uint64_t example_layer_b = 5; // Represents layer bit 5
    const uint64_t example_layer_c = 8; // Represents layer bit 8

    // Set setup parameters
    meta_param->name = "TestEntity";
    meta_param->active_self = true;
    meta_param->tag = (uint64_t(1) << example_tag_a) | (uint64_t(1) << example_tag_b);
    meta_param->layer = (uint64_t(1) << example_layer_a) | (uint64_t(1) << example_layer_b);

    // Create MetaComponent
    std::unique_ptr<mono_meta_extension::MetaComponent> meta_component 
        = std::make_unique<mono_meta_extension::MetaComponent>();
    bool setup_result = meta_component->Setup(*meta_param);
    ASSERT_TRUE(setup_result);

    // Verify MetaComponent properties
    ASSERT_EQ(meta_component->GetName(), "TestEntity");
    ASSERT_TRUE(meta_component->IsActiveSelf());
    ASSERT_TRUE(meta_component->HasTag(example_tag_a));
    ASSERT_TRUE(meta_component->HasTag(example_tag_b));
    ASSERT_FALSE(meta_component->HasTag(example_tag_c)); // Tag not set
    ASSERT_TRUE(meta_component->IsInLayer(example_layer_a));
    ASSERT_TRUE(meta_component->IsInLayer(example_layer_b));
    ASSERT_FALSE(meta_component->IsInLayer(example_layer_c)); // Layer not set
}