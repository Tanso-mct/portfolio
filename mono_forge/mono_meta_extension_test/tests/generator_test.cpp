#include "mono_meta_extension_test/pch.h"

#include "mono_meta_extension/include/meta_tag.h"
#include "mono_meta_extension/include/meta_tag_generator.h"
#include "mono_meta_extension/include/meta_layer.h"
#include "mono_meta_extension/include/meta_layer_generator.h"

namespace mono_meta_extension_test
{

class ExampleATag : public mono_meta_extension::MetaTag<ExampleATag> {};
class ExampleBTag : public mono_meta_extension::MetaTag<ExampleBTag> {};

class ExampleALayer : public mono_meta_extension::MetaLayer<ExampleALayer> {};
class ExampleBLayer : public mono_meta_extension::MetaLayer<ExampleBLayer> {};

} // namespace mono_meta_extension_test

TEST(MetaTagGenerator, GenerateTag)
{
    // Create singleton meta tag generator
    std::unique_ptr<mono_meta_extension::MetaTagGenerator> tag_generator 
        = std::make_unique<mono_meta_extension::MetaTagGenerator>();

    // Get example A tag
    uint64_t tag1 = mono_meta_extension_test::ExampleATag::Get();
    ASSERT_EQ(tag1, 0);

    // Get example B tag
    uint64_t tag2 = mono_meta_extension_test::ExampleBTag::Get();
    ASSERT_EQ(tag2, 1);
}

TEST(MetaLayerGenerator, GenerateLayer)
{
    // Create singleton meta layer generator
    std::unique_ptr<mono_meta_extension::MetaLayerGenerator> layer_generator 
        = std::make_unique<mono_meta_extension::MetaLayerGenerator>();

    // Get example A layer
    uint64_t layer1 = mono_meta_extension_test::ExampleALayer::Get();
    ASSERT_EQ(layer1, 0);

    // Get example B layer
    uint64_t layer2 = mono_meta_extension_test::ExampleBLayer::Get();
    ASSERT_EQ(layer2, 1);
}