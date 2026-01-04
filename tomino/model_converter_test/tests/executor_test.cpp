#include "pch.h"

#include "include/interfaces/file_loader.h"
#include "include/interfaces/converter.h"
#include "include/conversion_executor.h"

TEST(Executor, PairEquals)
{
    model_converter::ConversionPair pair1("input", "output");
    model_converter::ConversionPair pair2("input", "output");
    model_converter::ConversionPair pair3("input_diff", "output");
    model_converter::ConversionPair pair4("input", "output_diff");

    EXPECT_TRUE(pair1 == pair2);
    EXPECT_FALSE(pair1 == pair3);
    EXPECT_FALSE(pair1 == pair4);
}

TEST(Executor, Execute)
{
    // Mock Data
    class MockData : public model_converter::IFileData
    {
    public:
        std::string_view GetFileExt() const override { return ".mock"; }
    };

    // Mock Loader
    class MockLoader : public model_converter::IFileLoader
    {
    public:
        std::string_view GetSupportedFileExt() const override { return ".mock"; }
        std::unique_ptr<model_converter::IFileData> Load(std::string_view file_path) const override { return std::make_unique<MockData>(); }
    };

    // Mock Converter
    class MockConverter : public model_converter::IConverter
    {
    public:
        std::string_view GetInputFileExt() const override { return ".mock"; }
        std::string_view GetConvertedFileExt() const override { return ".test"; }
        std::unique_ptr<u8[]> Convert(const model_converter::IFileData* file_data, u32& rt_data_size) const override 
        { 
            rt_data_size = 4; 
            std::unique_ptr<u8[]> data = std::make_unique<u8[]>(rt_data_size);
            data[0] = 1; data[1] = 2; data[2] = 3; data[3] = 4; 
            return data; 
        }
    };

    std::unique_ptr<model_converter::IFileLoader> loader = std::make_unique<MockLoader>();
    std::unique_ptr<model_converter::IConverter> converter = std::make_unique<MockConverter>();
    model_converter::ConversionExecutor executor;
    EXPECT_TRUE(executor.Setup(std::move(loader), std::move(converter)));
    EXPECT_TRUE(executor.Execute("input.mock", "../output/test_output.bin"));
}