#include "otpch.h"
#include "tools.h"
#include "position.h"
#include "configmanager.h"

#include <gtest/gtest.h>

TEST(ToolsParsing, IntegerConversionHonorsStringViewBoundary)
{
    const std::string input = "12345";
    EXPECT_EQ(vectorAtoi({std::string_view(input.data(), 2)}), (IntegerVector{12}));
    EXPECT_THROW(vectorAtoi({std::string_view(input.data(), 0)}), std::invalid_argument);
}

TEST(ToolsParsing, SplitHandlesEmptySeparatorAndLimits)
{
    EXPECT_EQ(explodeString("a:b:c", ":", 1), (std::vector<std::string_view>{"a", "b:c"}));
    EXPECT_EQ(explodeString("a:b:c", ":", 0), (std::vector<std::string_view>{"a:b:c"}));
    EXPECT_EQ(explodeString("a:b:c", ":"), (std::vector<std::string_view>{"a", "b", "c"}));
    EXPECT_EQ(explodeString("abc", ""), (std::vector<std::string_view>{"abc"}));
    EXPECT_EQ(explodeString("a:b", ":", std::numeric_limits<int32_t>::min()),
        (std::vector<std::string_view>{"a", "b"}));
}

TEST(ToolsParsing, EmptySearchDoesNotGrowReplacementForever)
{
    std::string text = "unchanged";
    replaceString(text, "", "more");
    EXPECT_EQ(text, "unchanged");
}

TEST(Authenticator, StandardHotpVectorsAndLongSecret)
{
    const std::array<const char*, 10> expected{
        "755224", "287082", "359152", "969429", "338314",
        "254676", "287922", "162583", "399871", "520489"};
    for (uint32_t counter = 0; counter < expected.size(); ++counter)
    {
        EXPECT_EQ(generateToken("12345678901234567890", counter), expected[counter]);
    }
    EXPECT_EQ(generateToken(std::string(100, 'x'), 1), "971424");
}

TEST(PositionComparison, GreaterIsStrictAndOppositeOfLess)
{
    const Position first(100, 200, 7), second(101, 200, 7);
    EXPECT_FALSE(first > first);
    EXPECT_TRUE(second > first);
    EXPECT_FALSE(first > second);
}

TEST(ConfigurationBounds, InvalidKeysCannotIndexArrays)
{
    ConfigManager config;
    EXPECT_FALSE(config.SetNumber(static_cast<ConfigManager::IntegerConfig>(-1), 42));
    EXPECT_EQ(config.GetNumber(static_cast<ConfigManager::IntegerConfig>(-1)), 0);
    EXPECT_FALSE(config.SetString(static_cast<ConfigManager::StringConfig>(-1), "bad"));
    EXPECT_TRUE(config.GetString(static_cast<ConfigManager::StringConfig>(-1)).empty());
    EXPECT_FALSE(config.SetBoolean(static_cast<ConfigManager::BooleanConfig>(-1), true));
    EXPECT_FALSE(config.GetBoolean(static_cast<ConfigManager::BooleanConfig>(-1)));
    EXPECT_FALSE(config.SetFloat(static_cast<ConfigManager::FloatConfig>(-1), 1));
    EXPECT_EQ(config.GetFloat(static_cast<ConfigManager::FloatConfig>(-1)), 0);
}
