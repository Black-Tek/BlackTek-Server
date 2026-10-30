#include "otpch.h"
#include "items.h"
#include "condition.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

namespace BlackTek::Tests
{
    class ItemLoaderTest : public ::testing::Test
    {
    protected:
        std::filesystem::path original;
        std::filesystem::path directory;

        void SetUp() override
        {
            original = std::filesystem::current_path();
            directory = std::filesystem::temp_directory_path() /
                ("blacktek-items-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            ASSERT_TRUE(std::filesystem::create_directory(directory));
            std::filesystem::create_directories(directory / "data/items");
            std::filesystem::current_path(directory);
        }

        void TearDown() override
        {
            std::filesystem::current_path(original);
            std::filesystem::remove_all(directory);
        }

        void WriteDat(uint16_t count, const std::string& data)
        {
            std::ofstream stream("assets.dat", std::ios::binary);
            const uint32_t signature = 0;
            const std::array<uint16_t, 3> otherCounts{};
            stream.write(reinterpret_cast<const char*>(&signature), sizeof(signature));
            stream.write(reinterpret_cast<const char*>(&count), sizeof(count));
            stream.write(reinterpret_cast<const char*>(otherCounts.data()), sizeof(otherCounts));
            stream.write(data.data(), data.size());
        }

        void WriteToml(std::string_view text)
        {
            std::ofstream stream("data/items/test.toml");
            stream << text;
        }

        static std::string SimpleItem()
        {
            return std::string("\xff\x01\x01\x01\x01\x01\x01\x01\0\0\0\0", 12);
        }
    };

    TEST_F(ItemLoaderTest, RejectsShortHeader)
    {
        std::ofstream("assets.dat", std::ios::binary).put('\0');
        Items items;
        EXPECT_FALSE(items.loadFromDat("assets.dat"));
    }

    TEST_F(ItemLoaderTest, AcceptsShippedAssetDefinitions)
    {
        const auto assets = original / "data/items/assets.dat";
        if (not std::filesystem::exists(assets))
            GTEST_SKIP() << "Run from the repository root to validate the shipped asset definitions.";
        Items items;
        EXPECT_TRUE(items.loadFromDat(assets.string()));
        EXPECT_GT(items.size(), 100u);
    }

    TEST_F(ItemLoaderTest, RejectsMissingItemAndUnknownFlag)
    {
        Items items;
        WriteDat(100, "");
        EXPECT_FALSE(items.loadFromDat("assets.dat"));
        WriteDat(100, std::string("\xfe\xfa", 2));
        EXPECT_FALSE(items.loadFromDat("assets.dat"));
    }

    TEST_F(ItemLoaderTest, RejectsTruncatedSpritePayloadAndHugeDimensions)
    {
        Items items;
        WriteDat(100, SimpleItem().substr(0, 11));
        EXPECT_FALSE(items.loadFromDat("assets.dat"));
        WriteDat(100, std::string(9, '\xff'));
        EXPECT_FALSE(items.loadFromDat("assets.dat"));
    }

    TEST_F(ItemLoaderTest, AcceptsMaximumItemIdWithoutCounterWrap)
    {
        std::string data;
        data.reserve((65536 - 100) * 12);
        for (uint32_t id = 100; id <= UINT16_MAX; ++id)
            data += SimpleItem();
        WriteDat(UINT16_MAX, data);
        Items items;
        ASSERT_TRUE(items.loadFromDat("assets.dat"));
        EXPECT_EQ(items.getItemType(UINT16_MAX).getID(), UINT16_MAX);
    }

    TEST_F(ItemLoaderTest, FailedDatLoadPreservesPreviousDefinitions)
    {
        WriteDat(100, SimpleItem());
        Items items;
        ASSERT_TRUE(items.loadFromDat("assets.dat"));
        WriteDat(101, "");
        EXPECT_FALSE(items.loadFromDat("assets.dat"));
        EXPECT_EQ(items.getItemType(100).getID(), 100);
        EXPECT_EQ(items.size(), 101u);
    }

    TEST_F(ItemLoaderTest, EmptyAndClearedRegistryHaveAnInvalidFallbackItem)
    {
        Items items;
        EXPECT_EQ(items.getItemType(UINT16_MAX).getID(), 0);
        items.clear();
        EXPECT_EQ(items.getItemType(UINT16_MAX).getID(), 0);
    }

    TEST_F(ItemLoaderTest, MaximumTomlRangeDoesNotWrap)
    {
        WriteToml("[[items]]\nfromid=65535\ntoid=65535\nname='boundary'\n");
        Items items;
        ASSERT_TRUE(items.loadFromToml());
        EXPECT_EQ(items.getItemType(UINT16_MAX).name, "boundary");
    }

    TEST_F(ItemLoaderTest, RejectsWrongTypeAndOutOfRangeIds)
    {
        Items items;
        WriteToml("[[items]]\nid='invalid'\n");
        EXPECT_FALSE(items.loadFromToml());
        WriteToml("[[items]]\nid=65536\n");
        EXPECT_FALSE(items.loadFromToml());
        WriteToml("[[items]]\nfromid=1\ntoid='invalid'\n");
        EXPECT_FALSE(items.loadFromToml());
    }

    TEST_F(ItemLoaderTest, OptionalNestedFieldsUseTypedDefaults)
    {
        WriteToml("[[items]]\nid=100\nname='typed defaults'\n"
                  "skills=[{name=1,level='invalid',multiplier=1}]\n"
                  "buffs=[{name='test',value=1}]\n"
                  "field={type=1,ticks='invalid'}\n");
        Items items;
        EXPECT_TRUE(items.loadFromToml());
        EXPECT_EQ(items.getItemType(100).name, "typed defaults");
    }

    TEST_F(ItemLoaderTest, CurrencyWorthMustBePositive)
    {
        WriteToml("[[items]]\nid=100\nworth=0\n[[items]]\nid=101\nworth=-1\n");
        Items items;
        ASSERT_TRUE(items.loadFromToml());
        EXPECT_TRUE(items.currencyItems.empty());
        EXPECT_EQ(items.getItemType(100).worth, 0u);
        EXPECT_EQ(items.getItemType(101).worth, 0u);

        WriteToml("[[items]]\nid=102\nworth=4294967296\n");
        ASSERT_TRUE(items.loadFromToml());
        ASSERT_EQ(items.currencyItems.size(), 1u);
        EXPECT_EQ(items.currencyItems.begin()->first, uint64_t{1} << 32);
        EXPECT_EQ(items.getItemType(102).worth, uint64_t{1} << 32);
    }
}
