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
