// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "chunkgrid.h"

namespace
{
    constexpr size_t MaxDenseCells = 4'000'000;
}

namespace BlackTek::World
{
    void ChunkGrid::Reserve(uint16_t mapWidth, uint16_t mapHeight)
    {
        const int32_t chunksWide = (static_cast<int32_t>(mapWidth) + Floor::Mask) >> Floor::Bits;
        const int32_t chunksHigh = (static_cast<int32_t>(mapHeight) + Floor::Mask) >> Floor::Bits;
        const size_t cellCount = static_cast<size_t>(chunksWide) * static_cast<size_t>(chunksHigh);

        if (cellCount == 0 or cellCount > MaxDenseCells)
            return;

        dense_chunks_wide = chunksWide;
        dense_chunks_high = chunksHigh;
        dense_grid.assign(cellCount, ChunkHandle{});
    }

    bool ChunkGrid::InDenseRange(ChunkCoord coord) const noexcept
    {
        return coord.chunk_x >= 0 and coord.chunk_x < dense_chunks_wide
           and coord.chunk_y >= 0 and coord.chunk_y < dense_chunks_high;
    }

    size_t ChunkGrid::DenseIndex(ChunkCoord coord) const noexcept
    {
        return static_cast<size_t>(coord.chunk_y) * static_cast<size_t>(dense_chunks_wide) + static_cast<size_t>(coord.chunk_x);
    }

    ChunkHandle ChunkGrid::FindChunk(ChunkCoord coord) const
    {
        if (InDenseRange(coord))
            return dense_grid[DenseIndex(coord)];

        const auto found = overflow_directory.find(coord);

        if (found == overflow_directory.end())
            return ChunkHandle{};

        return found->second;
    }

    ChunkHandle ChunkGrid::GetOrCreateChunk(ChunkCoord coord)
    {
        if (const ChunkHandle existing = FindChunk(coord); existing.IsValid())
            return existing;

        const uint32_t newIndex = static_cast<uint32_t>(chunk_slots.size());
        chunk_slots.emplace_back();
        const ChunkHandle handle{ newIndex, 0 };

        if (InDenseRange(coord))
            dense_grid[DenseIndex(coord)] = handle;
        else
            overflow_directory.emplace(coord, handle);

        LinkNeighbors(handle, coord);
        return handle;
    }

    Chunk* ChunkGrid::GetChunk(ChunkHandle handle)
    {
        if (not handle.IsValid() or handle.slot_index >= chunk_slots.size())
            return nullptr;

        return &chunk_slots[handle.slot_index];
    }

    const Chunk* ChunkGrid::GetChunk(ChunkHandle handle) const
    {
        if (not handle.IsValid() or handle.slot_index >= chunk_slots.size())
            return nullptr;

        return &chunk_slots[handle.slot_index];
    }

    void ChunkGrid::LinkNeighbors(ChunkHandle handle, ChunkCoord coord)
    {
        Chunk* chunk = GetChunk(handle);

        if (const ChunkHandle north = FindChunk({ coord.chunk_x, coord.chunk_y - 1 }); north.IsValid())
            GetChunk(north)->south_neighbor = handle;

        if (const ChunkHandle south = FindChunk({ coord.chunk_x, coord.chunk_y + 1 }); south.IsValid())
            chunk->south_neighbor = south;

        if (const ChunkHandle west = FindChunk({ coord.chunk_x - 1, coord.chunk_y }); west.IsValid())
            GetChunk(west)->east_neighbor = handle;

        if (const ChunkHandle east = FindChunk({ coord.chunk_x + 1, coord.chunk_y }); east.IsValid())
            chunk->east_neighbor = east;
    }
}
