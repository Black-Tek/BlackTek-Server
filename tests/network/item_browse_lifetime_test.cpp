#include "otpch.h"
#include "game.h"
#include "item.h"
#include "tile.h"

#include <gtest/gtest.h>

extern Game g_game;

namespace BlackTek::Net::Tests
{
    static ItemPtr CreateBrowseField(const TilePtr& tile)
    {
        auto item = std::make_shared<Item>(ITEM_BROWSEFIELD);
        item->attachContainer(30, false, true);
        item->setTileParent(tile);
        return item;
    }

    TEST(BrowseFieldLifetime, RegistryRemovalReleasesOwner)
    {
        const auto tile = std::make_shared<Tile>(50000, 50000, 7);
        auto item = CreateBrowseField(tile);
        std::weak_ptr<Item> weakItem = item;
        g_game.browseFields[tile] = item->getContainer();
        item.reset();
        EXPECT_FALSE(weakItem.expired());
        g_game.browseFields.erase(tile);
        EXPECT_TRUE(weakItem.expired());
        EXPECT_FALSE(g_game.browseFields.contains(tile));
    }

    TEST(BrowseFieldLifetime, PreviousOwnerCannotEraseReplacement)
    {
        const auto tile = std::make_shared<Tile>(50001, 50000, 7);
        auto previous = CreateBrowseField(tile);
        g_game.browseFields[tile] = previous->getContainer();
        auto replacement = CreateBrowseField(tile);
        const auto replacementContainer = replacement->getContainer();
        g_game.browseFields[tile] = replacementContainer;
        previous.reset();
        const auto found = g_game.browseFields.find(tile);
        ASSERT_NE(found, g_game.browseFields.end());
        EXPECT_EQ(found->second, replacementContainer);
        g_game.browseFields.erase(tile);
    }
}
