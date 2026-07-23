// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "chunk.h"

#include "creature.h"

#include <cassert>
#include <utility>

namespace BlackTek::World
{
    void Chunk::AddCreature(const CreaturePtr& creature)
    {
        const Position& position = creature->getPosition();

        creature_ptrs.push_back(creature);
        creature_x.push_back(position.x);
        creature_y.push_back(position.y);
        creature_z.push_back(position.z);

        if (floor_creature_counts[position.z]++ == 0)
            ++distinct_floor_count;

        uint32_t slot = static_cast<uint32_t>(creature_ptrs.size() - 1);
        const uint32_t lastIndex = slot;

        if (creature->is_player())
        {
            std::swap(creature_ptrs[lastIndex], creature_ptrs[player_count]);
            std::swap(creature_x[lastIndex], creature_x[player_count]);
            std::swap(creature_y[lastIndex], creature_y[player_count]);
            std::swap(creature_z[lastIndex], creature_z[player_count]);

            slot = player_count;
            creature_ptrs[lastIndex]->setChunkSlot(lastIndex);
            ++player_count;
        }
        else if (creature->is_monster())
        {
            const uint32_t boundary = player_count + monster_count;
            std::swap(creature_ptrs[lastIndex], creature_ptrs[boundary]);
            std::swap(creature_x[lastIndex], creature_x[boundary]);
            std::swap(creature_y[lastIndex], creature_y[boundary]);
            std::swap(creature_z[lastIndex], creature_z[boundary]);

            slot = boundary;
            creature_ptrs[lastIndex]->setChunkSlot(lastIndex);
            ++monster_count;
        }

        creature->setChunkSlot(slot);
    }

    void Chunk::RemoveCreature(const CreaturePtr& creature)
    {
        const uint32_t removeIndex = creature->getChunkSlot();
        assert(removeIndex < creature_ptrs.size() and creature_ptrs[removeIndex] == creature);

        if (--floor_creature_counts[creature_z[removeIndex]] == 0)
            --distinct_floor_count;

        const uint32_t lastIndex = static_cast<uint32_t>(creature_ptrs.size() - 1);
        const uint32_t monsterEnd = static_cast<uint32_t>(player_count) + monster_count;

        const auto moveInto = [this](const uint32_t dst, const uint32_t src)
        {
            if (dst == src)
                return;

            creature_ptrs[dst] = creature_ptrs[src];
            creature_x[dst] = creature_x[src];
            creature_y[dst] = creature_y[src];
            creature_z[dst] = creature_z[src];
            creature_ptrs[dst]->setChunkSlot(dst);
        };

        if (removeIndex < player_count)
        {
            moveInto(removeIndex, player_count - 1u);
            moveInto(player_count - 1u, monsterEnd - 1u);
            moveInto(monsterEnd - 1u, lastIndex);
            --player_count;
        }
        else if (removeIndex < monsterEnd)
        {
            moveInto(removeIndex, monsterEnd - 1u);
            moveInto(monsterEnd - 1u, lastIndex);
            --monster_count;
        }
        else
        {
            moveInto(removeIndex, lastIndex);
        }

        creature_ptrs.pop_back();
        creature_x.pop_back();
        creature_y.pop_back();
        creature_z.pop_back();
    }

    void Chunk::UpdateCreaturePosition(const CreaturePtr& creature, const Position& newPosition)
    {
        const uint32_t updateIndex = creature->getChunkSlot();
        assert(updateIndex < creature_ptrs.size() and creature_ptrs[updateIndex] == creature);

        if (const uint8_t oldZ = creature_z[updateIndex]; oldZ != newPosition.z)
        {
            if (--floor_creature_counts[oldZ] == 0)
                --distinct_floor_count;

            if (floor_creature_counts[newPosition.z]++ == 0)
                ++distinct_floor_count;
        }

        creature_x[updateIndex] = newPosition.x;
        creature_y[updateIndex] = newPosition.y;
        creature_z[updateIndex] = newPosition.z;
    }

    void Chunk::SetTileBlockState(const uint32_t offsetX, const uint32_t offsetY, const uint8_t z, const bool blocksPath, const bool blocksSolid, const bool blocksProjectile)
    {
        const uint64_t bit = uint64_t{ 1 } << (offsetY * Floor::Size + offsetX);

        const auto apply = [bit](uint64_t& mask, const bool set)
        {
            if (set)
                mask |= bit;
            else
                mask &= ~bit;
        };

        apply(block_path_mask[z], blocksPath);
        apply(block_solid_mask[z], blocksSolid);
        apply(block_projectile_mask[z], blocksProjectile);
    }
}
