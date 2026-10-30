#include "otpch.h"
#include "chunk.h"
#include "chunkgrid.h"
#include "test_creature.h"

#include <gtest/gtest.h>

namespace BlackTek::World::Tests
{
    TEST(Chunk, PlayerInsertionPreservesAllTypePartitionsAndSlots)
    {
        Chunk chunk;
        const auto npc = std::make_shared<TestCreature>(CreatureSubType::Npc, Position(1, 2, 7));
        const auto monster = std::make_shared<TestCreature>(CreatureSubType::Monster, Position(3, 4, 7));
        const auto player = std::make_shared<TestCreature>(CreatureSubType::Player, Position(5, 6, 7));
        chunk.AddCreature(npc);
        chunk.AddCreature(monster);
        chunk.AddCreature(player);

        ASSERT_EQ(chunk.creature_ptrs.size(), 3u);
        EXPECT_EQ(chunk.player_count, 1u);
        EXPECT_EQ(chunk.monster_count, 1u);
        EXPECT_EQ(chunk.creature_ptrs[0], player);
        EXPECT_EQ(chunk.creature_ptrs[1], monster);
        EXPECT_EQ(chunk.creature_ptrs[2], npc);
        for (size_t index = 0; index < chunk.creature_ptrs.size(); ++index)
        {
            const auto& creature = chunk.creature_ptrs[index];
            EXPECT_EQ(creature->getChunkSlot(), index);
            EXPECT_EQ(chunk.creature_x[index], creature->getPosition().x);
            EXPECT_EQ(chunk.creature_y[index], creature->getPosition().y);
        }

        chunk.RemoveCreature(monster);
        EXPECT_EQ(chunk.player_count, 1u);
        EXPECT_EQ(chunk.monster_count, 0u);
        EXPECT_EQ(chunk.creature_ptrs[0], player);
        EXPECT_EQ(chunk.creature_ptrs[1], npc);
        chunk.RemoveCreature(player);
        EXPECT_EQ(chunk.creature_ptrs[0], npc);
        EXPECT_EQ(npc->getChunkSlot(), 0u);
        chunk.RemoveCreature(npc);
        EXPECT_TRUE(chunk.creature_ptrs.empty());
        EXPECT_EQ(chunk.distinct_floor_count, 0u);
    }

    TEST(ChunkGrid, RepeatedReservePreservesExistingChunks)
    {
        ChunkGrid grid;
        grid.Reserve(64, 64);
        const auto inside = grid.GetOrCreateChunk({1, 1});
        const auto outside = grid.GetOrCreateChunk({10, 10});
        grid.Reserve(128, 128);
        EXPECT_EQ(grid.FindChunk({1, 1}), inside);
        EXPECT_EQ(grid.FindChunk({10, 10}), outside);
        EXPECT_EQ(grid.GetOrCreateChunk({1, 1}), inside);
    }

    TEST(CreatureCustomStats, MissingStatIsRejectedAfterRegistryCreation)
    {
        const auto creature = std::make_shared<TestCreature>();
        creature->getCustomStats();
        EXPECT_FALSE(creature->increaseCustomStat(99, 1));
        EXPECT_FALSE(creature->decreaseCustomStat(99, 1));
    }
}
