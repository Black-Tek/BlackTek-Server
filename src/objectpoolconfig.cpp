// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "objectpoolconfig.h"

#include <toml++/toml.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace ObjectPools
{

static constexpr std::string_view CONFIG_PATH = "config/object_pools.toml";

void Config::GenerateDefault()
{
    std::ofstream file{ std::string{CONFIG_PATH} };
    if (not file.is_open())
    {
        std::cerr << "[Warning - ObjectPools::Config] Could not write default config to "
                  << CONFIG_PATH << "\n";
        return;
    }

    file <<
R"(# Object pool memory configuration
# All creature and item pools draw from a single pre-allocated memory arena.
# Tuning these values avoids heap allocation during normal server operation.
#
# max_chunks  - maximum objects the pool keeps in a single slab before
#               requesting a new chunk from the arena.
# block_size  - maximum allocation the pool will handle, in bytes.
#               Must be >= sizeof(T) + 64 to cover the shared_ptr control block
#               and worst-case alignment padding.
#               If unsure, leave at the default or increase conservatively.

[arena]
# Total size of the pre-allocated memory arena in megabytes.
# All creature and item pools draw from this arena.
game_buffer_mb = 512

[pool.player]
max_chunks = 1000
block_size = 1024

[pool.monster]
max_chunks = 50000
block_size = 768

[pool.npc]
max_chunks = 200
block_size = 768

[pool.creature_nodes]
# Backs the internal nodes of the player/monster/npc lookup maps.
max_chunks = 50000
block_size = 128

[pool.item_nodes]
# Backs the internal nodes of item lookup containers.
max_chunks = 200000
block_size = 128

[pool.item]
# block_size must cover the largest derived Item subclass (Container) + 64.
max_chunks = 200000
block_size = 512
)";
}

Config::Config()
{
    if (not std::filesystem::exists(CONFIG_PATH))
    {
        GenerateDefault();
        return; // member-initializer defaults are already correct
    }

    toml::table tbl;
    try
    {
        tbl = toml::parse_file(CONFIG_PATH);
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "[Warning - ObjectPools::Config] Failed to parse " << CONFIG_PATH
                  << ": " << err.description() << " — using defaults.\n";
        return;
    }

    const auto mb = tbl["arena"]["game_buffer_mb"].value_or(int64_t{512});
    game_buffer_size = static_cast<size_t>(mb) * 1024 * 1024;

    auto loadPool = [&tbl](const char* key, PoolParams& p)
    {
        p.max_chunks = static_cast<size_t>(
            tbl["pool"][key]["max_chunks"].value_or(static_cast<int64_t>(p.max_chunks)));
        p.block_size = static_cast<size_t>(
            tbl["pool"][key]["block_size"].value_or(static_cast<int64_t>(p.block_size)));
    };

    loadPool("player",         player);
    loadPool("monster",        monster);
    loadPool("npc",            npc);
    loadPool("creature_nodes", creature_nodes);
    loadPool("item_nodes",     item_nodes);
    loadPool("item",           item);
}

Config& Config::Get()
{
    static Config instance;
    return instance;
}

} // namespace ObjectPools
