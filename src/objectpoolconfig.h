// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <cstddef>

namespace ObjectPools
{

struct PoolParams
{
    size_t max_chunks;
    size_t block_size;
};

// Lazily loads config/object_pools.toml on first access via Get().
// Because Game is a global (constructed before main), this config
// must be self-loading and independent of ConfigManager.
// If the file does not exist it is generated with defaults.
class Config
{
public:
    static Config& Get();

    size_t            GameBufferSize()     const noexcept { return game_buffer_size; }
    const PoolParams& PlayerPool()         const noexcept { return player; }
    const PoolParams& MonsterPool()        const noexcept { return monster; }
    const PoolParams& NpcPool()            const noexcept { return npc; }
    const PoolParams& CreatureNodePool()   const noexcept { return creature_nodes; }
    const PoolParams& ItemNodePool()       const noexcept { return item_nodes; }
    const PoolParams& ItemPool()           const noexcept { return item; }

private:
    Config();

    static void GenerateDefault();

    size_t     game_buffer_size = 512ULL * 1024 * 1024;
    PoolParams player           = { 1000,   1024 };
    PoolParams monster          = { 50000,   768 };
    PoolParams npc              = {   200,   768 };
    PoolParams creature_nodes   = { 50000,   128 };
    PoolParams item_nodes       = { 200000,  128 };
    PoolParams item             = { 200000,  512 };
};

} // namespace ObjectPools
