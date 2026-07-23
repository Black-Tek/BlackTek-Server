// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "chunk.h"

#include <deque>
#include <vector>
#include <gtl/phmap.hpp>

namespace BlackTek::World
{
    class ChunkGrid
    {
        public:
            void Reserve(uint16_t mapWidth, uint16_t mapHeight);

            [[nodiscard]] ChunkHandle FindChunk(ChunkCoord coord) const;
            ChunkHandle GetOrCreateChunk(ChunkCoord coord);

            [[nodiscard]] Chunk* GetChunk(ChunkHandle handle);
            [[nodiscard]] const Chunk* GetChunk(ChunkHandle handle) const;

        private:
            void LinkNeighbors(ChunkHandle handle, ChunkCoord coord);

            [[nodiscard]] bool InDenseRange(ChunkCoord coord) const noexcept;
            [[nodiscard]] size_t DenseIndex(ChunkCoord coord) const noexcept;

            std::vector<ChunkHandle> dense_grid;
            int32_t dense_chunks_wide = 0;
            int32_t dense_chunks_high = 0;

            gtl::flat_hash_map<ChunkCoord, ChunkHandle, ChunkCoordHash> overflow_directory;
            std::deque<Chunk> chunk_slots;
    };
}
