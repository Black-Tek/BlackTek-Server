#include "otpch.h"
#include "outfit.h"
#include "quests.h"
#include "game.h"

#include <fstream>
#include <gtest/gtest.h>

extern Game g_game;

namespace
{
class ContentFixture : public testing::Test
{
protected:
    std::filesystem::path original;
    std::filesystem::path root;

    void SetUp() override
    {
        original = std::filesystem::current_path();
        root = std::filesystem::temp_directory_path() /
            ("blacktek-content-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(root / "config");
        std::filesystem::create_directories(root / "data/quests/nested");
        std::filesystem::current_path(root);
    }

    void TearDown() override
    {
        std::filesystem::current_path(original);
        std::filesystem::remove_all(root);
    }

    void Write(const std::filesystem::path& path, std::string_view text)
    {
        std::ofstream output(root / path);
        output << text;
        ASSERT_TRUE(output.good());
    }
};
}

TEST_F(ContentFixture, QuestsInDifferentFilesHaveUniqueIds)
{
    Write("data/quests/a.toml", "[one]\nname='First'\nstartstorage=100\nstartvalue=1\n");
    Write("data/quests/nested/b.toml", "[two]\nname='Second'\nstartstorage=101\nstartvalue=1\n");
    Quests quests;
    ASSERT_TRUE(quests.loadFromToml());
    ASSERT_EQ(quests.getQuests().size(), 2);
    ASSERT_NE(quests.getQuestByID(1), nullptr);
    ASSERT_NE(quests.getQuestByID(2), nullptr);
    EXPECT_NE(quests.getQuestByID(1)->getName(), quests.getQuestByID(2)->getName());
}

TEST_F(ContentFixture, OutfitReloadReplacesAndInvalidLoadPreservesExistingData)
{
    Write("config/outfits.toml", "[test]\nsex=0\nid=128\nname='Example'\n");
    Outfits outfits;
    ASSERT_TRUE(outfits.load());
    ASSERT_TRUE(outfits.load());
    EXPECT_EQ(outfits.getOutfits(PLAYERSEX_FEMALE).size(), 1);
    Write("config/outfits.toml", "[test]\nsex=-1\nid=128\nname='Invalid'\n");
    EXPECT_FALSE(outfits.load());
    EXPECT_EQ(outfits.getOutfits(PLAYERSEX_FEMALE).size(), 1);
    EXPECT_EQ(outfits.getOutfitByLookType(static_cast<PlayerSex_t>(255), 128), nullptr);
    EXPECT_TRUE(outfits.getOutfits(static_cast<PlayerSex_t>(255)).empty());
}

TEST(QuestDescriptions, SparseStatesUseNearestDefinedValue)
{
    const auto player = g_game.MakePlayer(nullptr);
    player->addStorageValue(100, 8);
    Mission mission("Sparse", 100, 1, 10, true);
    mission.descriptions.emplace(3, "Third");
    mission.descriptions.emplace(7, "Seventh");
    EXPECT_EQ(mission.getDescription(player), "Seventh");
    Mission exact("Exact", 100, 1, 10, false);
    exact.descriptions.emplace(7, "Seventh");
    EXPECT_EQ(exact.getDescription(player), "An error has occurred, please contact a gamemaster.");
}
