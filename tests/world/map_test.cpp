#include "otpch.h"
#include "map.h"
#include "tile.h"

#include <gtest/gtest.h>

namespace BlackTek::World::Tests
{
    TEST(Map, TileLookupDoesNotShareChunkCacheAcrossMaps)
    {
        Map firstMap;
        Map secondMap;
        TilePtr firstTile = std::make_shared<Tile>(100, 100, 7);
        TilePtr secondTile = std::make_shared<Tile>(100, 100, 7);
        firstMap.setTile(100, 100, 7, firstTile);
        secondMap.setTile(100, 100, 7, secondTile);
        EXPECT_EQ(firstMap.getTile(100, 100, 7), firstTile);
        EXPECT_EQ(secondMap.getTile(100, 100, 7), secondTile);
        EXPECT_EQ(firstMap.getTile(100, 100, 7), firstTile);
    }
}
