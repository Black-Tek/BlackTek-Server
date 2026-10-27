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
