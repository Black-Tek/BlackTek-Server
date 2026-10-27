/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * This file is an original BlackTek contribution, created since this project's
 * inception, and contains no code inherited from The Forgotten Server.
 * It is licensed solely under the MIT License (see the licensing/License file
 * in the project root for the full text).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "declarations.h"
#include "floorpool.h"
#include "handle.h"
#include "position.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace BlackTek::World
{
    struct ChunkTag {};
    using ChunkHandle = BlackTek::Handle<ChunkTag>;

    struct ChunkCoord
    {
        int32_t chunk_x = 0;
        int32_t chunk_y = 0;

        friend constexpr bool operator==(const ChunkCoord&, const ChunkCoord&) = default;
    };

    struct ChunkCoordHash
    {
        [[nodiscard]] size_t operator()(const ChunkCoord& coord) const noexcept
        {
            uint64_t packed = (static_cast<uint64_t>(static_cast<uint32_t>(coord.chunk_x)) << 32) | static_cast<uint32_t>(coord.chunk_y);
            packed *= 0x9E3779B97F4A7C15ull;
            return static_cast<size_t>(packed ^ (packed >> 32));
        }
    };

    [[nodiscard]] constexpr ChunkCoord ToChunkCoord(uint16_t x, uint16_t y) noexcept
    {
        return ChunkCoord{ x >> Floor::Bits, y >> Floor::Bits };
    }

    struct Chunk
    {
        std::array<FloorHandle, MaxLayers> floor_handles = {};
        std::vector<CreaturePtr> creature_ptrs;
        std::vector<uint16_t> creature_x;
        std::vector<uint16_t> creature_y;
        std::vector<uint8_t> creature_z;

        ChunkHandle south_neighbor;
        ChunkHandle east_neighbor;

        uint16_t player_count = 0;
        uint16_t monster_count = 0;
        uint16_t floor_creature_counts[MaxLayers] = {};
        uint8_t distinct_floor_count = 0;

        std::array<uint64_t, MaxLayers> block_path_mask = {};
        std::array<uint64_t, MaxLayers> block_solid_mask = {};
        std::array<uint64_t, MaxLayers> block_projectile_mask = {};

        [[nodiscard]] bool AllCreaturesSameFloor() const { return distinct_floor_count <= 1; }

        void AddCreature(const CreaturePtr& creature);
        void RemoveCreature(const CreaturePtr& creature);
        void UpdateCreaturePosition(const CreaturePtr& creature, const Position& newPosition);

        void SetTileBlockState(uint32_t offsetX, uint32_t offsetY, uint8_t z, bool blocksPath, bool blocksSolid, bool blocksProjectile);
    };
}
