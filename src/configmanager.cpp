// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "configmanager.h"
#include "game.h"
#include "monster.h"
#include "tools.h"
#include <toml++/toml.h>

extern Game g_game;

namespace
{

toml::table SafeParseFile(std::string_view path)
{
    try
    {
        return toml::parse_file(path);
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "[Warning - ConfigManager] Failed to parse " << path
                  << ": " << err.description() << "\n";
        return toml::table{};
    }
}

ExperienceStages LoadExperienceStages()
{
    const std::string_view filePath = "config/stages.toml";

    ExperienceStages stages;

    try
    {
        auto tbl = toml::parse_file(filePath);

        if (auto config = tbl["config"].as_table())
        {
            auto enabledNode = config->get("enabled");
            if (enabledNode and enabledNode->is_boolean() and not enabledNode->as_boolean()->get())
            {
                return {};
            }
        }

        if (auto stageArray = tbl["stage"].as_array())
        {
            for (const auto& elem : *stageArray)
            {
                if (not elem.is_table())
                {
                    continue;
                }

                const auto& stage = *elem.as_table();
                auto minLevel   = stage["minlevel"].value_or(uint32_t{1});
                auto maxLevel   = stage["maxlevel"].value_or(std::numeric_limits<uint32_t>::max());
                auto multiplier = static_cast<float>(stage["multiplier"].value_or(1.0));

                stages.emplace_back(minLevel, maxLevel, multiplier);
            }
        }
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "[Warning - ConfigManager] Failed to parse " << filePath
                  << ": " << err.description() << "\n";
        return {};
    }

    std::ranges::sort(stages);
    return stages;
}

} // namespace

bool ConfigManager::Load()
{
    auto serverTbl   = SafeParseFile("config/server.toml");
    auto databaseTbl = SafeParseFile("config/database.toml");
    auto gameplayTbl = SafeParseFile("config/gameplay.toml");
    auto combatTbl   = SafeParseFile("config/combat.toml");
    auto ratesTbl    = SafeParseFile("config/rates.toml");
    auto monstersTbl = SafeParseFile("config/monsters.toml");
    auto scriptsTbl  = SafeParseFile("config/scripts.toml");

    // -------------------------------------------------------------------------
    // One-time settings: only loaded on the first call to Load().
    // Command-line overrides (SetString/SetNumber) applied before Load() are
    // preserved by skipping empty/zero values below.
    // -------------------------------------------------------------------------
    if (not loaded)
    {
        booleans[BIND_ONLY_GLOBAL_ADDRESS] = serverTbl["network"]["bind_only_global_address"].value_or(false);
        booleans[OPTIMIZE_DATABASE]        = databaseTbl["mysql"]["optimize_on_startup"].value_or(false);
        booleans[ENABLE_IPV6]              = serverTbl["network"]["enable_ipv6"].value_or(false);
        booleans[IPV6_FALLBACK_TO_IPV4]    = serverTbl["network"]["ipv6_fallback_to_ipv4"].value_or(true);

        if (strings[IP].empty())
        {
            strings[IP] = serverTbl["network"]["ip"].value_or<std::string>("127.0.0.1");
        }

        if (strings[IPV6].empty())
        {
            strings[IPV6] = serverTbl["network"]["ipv6"].value_or<std::string>("::");
        }

        strings[MAP_NAME]          = serverTbl["world"]["map_name"].value_or<std::string>("forgotten");
        strings[MAP_AUTHOR]        = serverTbl["world"]["map_author"].value_or<std::string>("Unknown");
        strings[HOUSE_RENT_PERIOD] = gameplayTbl["houses"]["rent_period"].value_or<std::string>("never");
        strings[MYSQL_HOST]        = databaseTbl["mysql"]["host"].value_or<std::string>("127.0.0.1");
        strings[MYSQL_USER]        = databaseTbl["mysql"]["user"].value_or<std::string>("forgottenserver");
        strings[MYSQL_PASS]        = databaseTbl["mysql"]["pass"].value_or<std::string>("");
        strings[MYSQL_DB]          = databaseTbl["mysql"]["database"].value_or<std::string>("forgottenserver");
        strings[MYSQL_SOCK]        = databaseTbl["mysql"]["socket"].value_or<std::string>("");
        strings[ASSETS_DAT_PATH]   = serverTbl["world"]["assets_dat_path"].value_or<std::string>("data/items/assets.dat");

        integers[SQL_PORT]              = static_cast<int32_t>(databaseTbl["mysql"]["port"].value_or(int64_t{3306}));
        integers[MARKET_OFFER_DURATION] = static_cast<int32_t>(gameplayTbl["market"]["offer_duration"].value_or(int64_t{2592000}));

        if (integers[GAME_PORT] == 0)
        {
            integers[GAME_PORT] = static_cast<int32_t>(serverTbl["network"]["game_port"].value_or(int64_t{7172}));
        }

        if (integers[LOGIN_PORT] == 0)
        {
            integers[LOGIN_PORT] = static_cast<int32_t>(serverTbl["network"]["login_port"].value_or(int64_t{7171}));
        }

        integers[STATUS_PORT] = static_cast<int32_t>(serverTbl["network"]["status_port"].value_or(int64_t{7171}));
    }

    // -------------------------------------------------------------------------
    // Reloadable settings: refreshed on every Load() / Reload() call.
    // -------------------------------------------------------------------------

    // Server identity
    strings[SERVER_NAME]      = serverTbl["identity"]["name"].value_or<std::string>("");
    strings[OWNER_NAME]       = serverTbl["identity"]["owner_name"].value_or<std::string>("");
    strings[OWNER_EMAIL]      = serverTbl["identity"]["owner_email"].value_or<std::string>("");
    strings[URL]              = serverTbl["identity"]["url"].value_or<std::string>("");
    strings[LOCATION]         = serverTbl["identity"]["location"].value_or<std::string>("");
    strings[MOTD]             = serverTbl["identity"]["motd"].value_or<std::string>("");
    strings[WORLD_TYPE]       = serverTbl["world"]["type"].value_or<std::string>("pvp");
    strings[DEFAULT_PRIORITY] = serverTbl["startup"]["default_priority"].value_or<std::string>("high");

    // Network / accounts
    booleans[ONE_PLAYER_ON_ACCOUNT]   = serverTbl["accounts"]["one_player_per_account"].value_or(true);
    booleans[FREE_PREMIUM]            = serverTbl["accounts"]["free_premium"].value_or(false);
    booleans[ONLINE_OFFLINE_CHARLIST] = serverTbl["accounts"]["online_offline_charlist"].value_or(false);
    booleans[REPLACE_KICK_ON_LOGIN]   = serverTbl["network"]["replace_kick_on_login"].value_or(true);
    booleans[ALLOW_CLONES]            = serverTbl["network"]["allow_clones"].value_or(false);
    booleans[ALLOW_WALKTHROUGH]       = serverTbl["network"]["allow_walkthrough"].value_or(true);

    integers[MAX_PLAYERS]           = static_cast<int32_t>(serverTbl["network"]["max_players"].value_or(int64_t{0}));
    integers[MAX_PACKETS_PER_SECOND] = static_cast<int32_t>(serverTbl["network"]["max_packets_per_second"].value_or(int64_t{25}));
    integers[STATUSQUERY_TIMEOUT]   = static_cast<int32_t>(serverTbl["network"]["status_timeout"].value_or(int64_t{5000}));

    // Account manager
    booleans[ENABLE_ACCOUNT_MANAGER] = serverTbl["account_manager"]["enabled"].value_or(true);
    booleans[ENABLE_NO_PASS_LOGIN]   = serverTbl["account_manager"]["allow_no_pass_login"].value_or(true);
    strings[ACCOUNT_MANAGER_AUTH]    = serverTbl["account_manager"]["password"].value_or<std::string>("1");
    integers[ACCOUNT_MANAGER_POS_X]  = static_cast<int32_t>(serverTbl["account_manager"]["position_x"].value_or(int64_t{0}));
    integers[ACCOUNT_MANAGER_POS_Y]  = static_cast<int32_t>(serverTbl["account_manager"]["position_y"].value_or(int64_t{0}));
    integers[ACCOUNT_MANAGER_POS_Z]  = static_cast<int32_t>(serverTbl["account_manager"]["position_z"].value_or(int64_t{0}));

    // Combat
    booleans[AIMBOT_HOTKEY_ENABLED]   = combatTbl["hotkey"]["aimbot_enabled"].value_or(true);
    booleans[REMOVE_RUNE_CHARGES]     = combatTbl["items"]["remove_rune_charges"].value_or(true);
    booleans[REMOVE_WEAPON_AMMO]      = combatTbl["items"]["remove_weapon_ammo"].value_or(true);
    booleans[REMOVE_WEAPON_CHARGES]   = combatTbl["items"]["remove_weapon_charges"].value_or(true);
    booleans[REMOVE_POTION_CHARGES]   = combatTbl["items"]["remove_potion_charges"].value_or(true);
    booleans[EXPERIENCE_FROM_PLAYERS] = combatTbl["pvp"]["experience_from_players"].value_or(false);
    booleans[CLASSIC_ATTACK_SPEED]    = combatTbl["classic"]["attack_speed"].value_or(false);
    booleans[CLASSIC_EQUIPMENT_SLOTS] = combatTbl["classic"]["equipment_slots"].value_or(false);

    integers[PZ_LOCKED]                  = static_cast<int32_t>(combatTbl["skull"]["pz_locked"].value_or(int64_t{60000}));
    integers[KILLS_TO_RED]               = static_cast<int32_t>(combatTbl["skull"]["kills_to_red"].value_or(int64_t{3}));
    integers[KILLS_TO_BLACK]             = static_cast<int32_t>(combatTbl["skull"]["kills_to_black"].value_or(int64_t{6}));
    integers[FRAG_TIME]                  = static_cast<int32_t>(combatTbl["skull"]["frag_time"].value_or(int64_t{86400}));
    integers[WHITE_SKULL_TIME]           = static_cast<int32_t>(combatTbl["skull"]["white_skull_time"].value_or(int64_t{900}));
    integers[PROTECTION_LEVEL]           = static_cast<int32_t>(combatTbl["pvp"]["protection_level"].value_or(int64_t{1}));
    integers[EXP_FROM_PLAYERS_LEVEL_RANGE] = static_cast<int32_t>(combatTbl["pvp"]["exp_from_players_level_range"].value_or(int64_t{75}));
    integers[STAIRHOP_DELAY]             = static_cast<int32_t>(combatTbl["movement"]["stairhop_delay"].value_or(int64_t{2000}));
    integers[PLAYER_DEFENSE_CHARGE_INTERVAL]  = static_cast<int32_t>(combatTbl["defense"]["player_charge_interval"].value_or(int64_t{1000}));
    integers[MONSTER_DEFENSE_CHARGE_INTERVAL] = static_cast<int32_t>(combatTbl["defense"]["monster_charge_interval"].value_or(int64_t{1000}));
    integers[PLAYER_DEFENSE_CHARGES_CAP]      = static_cast<int32_t>(combatTbl["defense"]["player_defense_charges_cap"].value_or(int64_t{2}));
    integers[PLAYER_ARMOR_CHARGES_CAP]        = static_cast<int32_t>(combatTbl["defense"]["player_armor_charges_cap"].value_or(int64_t{2}));
    integers[PLAYER_AUGMENT_CHARGES_CAP]      = static_cast<int32_t>(combatTbl["defense"]["player_augment_charges_cap"].value_or(int64_t{2}));
    integers[MONSTER_DEFENSE_CHARGES_CAP]     = static_cast<int32_t>(combatTbl["defense"]["monster_defense_charges_cap"].value_or(int64_t{2}));
    integers[MONSTER_ARMOR_CHARGES_CAP]       = static_cast<int32_t>(combatTbl["defense"]["monster_armor_charges_cap"].value_or(int64_t{2}));
    integers[DEFAULT_DEFENSE_CHARGE_COST]     = static_cast<int32_t>(combatTbl["defense"]["default_defense_charge_cost"].value_or(int64_t{1}));
    integers[DEFAULT_ARMOR_CHARGE_COST]       = static_cast<int32_t>(combatTbl["defense"]["default_armor_charge_cost"].value_or(int64_t{1}));
    integers[DEFAULT_AUGMENT_CHARGE_COST]     = static_cast<int32_t>(combatTbl["defense"]["default_augment_charge_cost"].value_or(int64_t{1}));

    // Gameplay
    booleans[ALLOW_CHANGEOUTFIT]              = gameplayTbl["player"]["change_outfit"].value_or(true);
    booleans[EMOTE_SPELLS]                    = gameplayTbl["player"]["emote_spells"].value_or(false);
    booleans[MARKET_PREMIUM]                  = gameplayTbl["market"]["premium_required"].value_or(true);
    booleans[STAMINA_SYSTEM]                  = gameplayTbl["stamina"]["enabled"].value_or(true);
    booleans[SERVER_SAVE_NOTIFY_MESSAGE]      = gameplayTbl["server_save"]["notify_message"].value_or(true);
    booleans[SERVER_SAVE_CLEAN_MAP]           = gameplayTbl["server_save"]["clean_map"].value_or(false);
    booleans[SERVER_SAVE_CLOSE]               = gameplayTbl["server_save"]["close"].value_or(false);
    booleans[SERVER_SAVE_SHUTDOWN]            = gameplayTbl["server_save"]["shutdown"].value_or(true);
    booleans[YELL_ALLOW_PREMIUM]              = gameplayTbl["chat"]["yell_allow_premium"].value_or(false);
    booleans[PREMIUM_TO_SEND_PRIVATE]         = gameplayTbl["chat"]["premium_to_send_private"].value_or(false);
    booleans[DEFAULT_WORLD_LIGHT]             = gameplayTbl["world"]["default_world_light"].value_or(true);
    booleans[HOUSE_OWNED_BY_ACCOUNT]          = gameplayTbl["houses"]["owned_by_account"].value_or(false);
    booleans[CLEAN_PROTECTION_ZONES]          = gameplayTbl["world"]["clean_protection_zones"].value_or(false);
    booleans[HOUSE_DOOR_SHOW_PRICE]           = gameplayTbl["houses"]["door_show_price"].value_or(true);
    booleans[ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS] = gameplayTbl["houses"]["only_invited_can_move_items"].value_or(true);
    booleans[BED_OFFLINE_TRAINING]            = gameplayTbl["stamina"]["bed_offline_training"].value_or(true);
    booleans[AUGMENT_SLOT_PROTECTION]         = gameplayTbl["augments"]["slot_protection"].value_or(true);
    booleans[AUGMENT_STAMINA_RULE]            = gameplayTbl["augments"]["stamina_rule"].value_or(false);
    booleans[AUGMENT_CRITICAL_ANIMATION]      = gameplayTbl["augments"]["critical_animation"].value_or(true);
    booleans[NPC_PZ_WALKTHROUGH]              = gameplayTbl["npc"]["pz_walkthrough"].value_or(false);
    booleans[HEALTH_REGEN_NOTIFICATION]       = gameplayTbl["notifications"]["health_regen"].value_or(false);
    booleans[MANA_REGEN_NOTIFICATION]         = gameplayTbl["notifications"]["mana_regen"].value_or(false);
    booleans[AUTO_OPEN_CONTAINERS]            = gameplayTbl["notifications"]["auto_open_containers"].value_or(true);

    integers[HOUSE_PRICE]                              = static_cast<int32_t>(gameplayTbl["houses"]["price_per_sqm"].value_or(int64_t{1000}));
    integers[MAX_MESSAGEBUFFER]                        = static_cast<int32_t>(gameplayTbl["chat"]["max_message_buffer"].value_or(int64_t{4}));
    integers[ACTIONS_DELAY_INTERVAL]                   = static_cast<int32_t>(gameplayTbl["items"]["time_between_actions"].value_or(int64_t{200}));
    integers[EX_ACTIONS_DELAY_INTERVAL]                = static_cast<int32_t>(gameplayTbl["items"]["time_between_ex_actions"].value_or(int64_t{1000}));
    integers[KICK_AFTER_MINUTES]                       = static_cast<int32_t>(gameplayTbl["player"]["kick_after_minutes"].value_or(int64_t{15}));
    integers[DEATH_LOSE_PERCENT]                       = static_cast<int32_t>(gameplayTbl["deaths"]["lose_percent"].value_or(int64_t{-1}));
    integers[CHECK_EXPIRED_MARKET_OFFERS_EACH_MINUTES] = static_cast<int32_t>(gameplayTbl["market"]["check_expired_each_minutes"].value_or(int64_t{60}));
    integers[MAX_MARKET_OFFERS_AT_A_TIME_PER_PLAYER]   = static_cast<int32_t>(gameplayTbl["market"]["max_offers_per_player"].value_or(int64_t{100}));
    integers[SERVER_SAVE_NOTIFY_DURATION]              = static_cast<int32_t>(gameplayTbl["server_save"]["notify_duration"].value_or(int64_t{5}));
    integers[YELL_MINIMUM_LEVEL]                       = static_cast<int32_t>(gameplayTbl["chat"]["yell_minimum_level"].value_or(int64_t{2}));
    integers[MINIMUM_LEVEL_TO_SEND_PRIVATE]            = static_cast<int32_t>(gameplayTbl["chat"]["minimum_level_to_send_private"].value_or(int64_t{1}));
    integers[VIP_FREE_LIMIT]                           = static_cast<int32_t>(gameplayTbl["limits"]["vip_free"].value_or(int64_t{20}));
    integers[VIP_PREMIUM_LIMIT]                        = static_cast<int32_t>(gameplayTbl["limits"]["vip_premium"].value_or(int64_t{100}));
    integers[DEPOT_FREE_LIMIT]                         = static_cast<int32_t>(gameplayTbl["limits"]["depot_free"].value_or(int64_t{2000}));
    integers[DEPOT_PREMIUM_LIMIT]                      = static_cast<int32_t>(gameplayTbl["limits"]["depot_premium"].value_or(int64_t{10000}));
    integers[PLAYER_SPEED_PER_LEVEL]                   = static_cast<int32_t>(gameplayTbl["player"]["speed_per_level"].value_or(int64_t{2}));
    integers[PLAYER_MAX_SPEED]                         = static_cast<int32_t>(gameplayTbl["player"]["max_speed"].value_or(int64_t{1500}));
    integers[PLAYER_MIN_SPEED]                         = static_cast<int32_t>(gameplayTbl["player"]["min_speed"].value_or(int64_t{120}));
    integers[PARTY_EXP_SHARE_RANGE]                    = static_cast<int32_t>(gameplayTbl["party"]["exp_share_range"].value_or(int64_t{30}));
    integers[PARTY_EXP_SHARE_FLOORS]                   = static_cast<int32_t>(gameplayTbl["party"]["exp_share_floors"].value_or(int64_t{1}));
    integers[MAXIMUM_PARTY_SIZE]                       = static_cast<int32_t>(gameplayTbl["party"]["maximum_size"].value_or(int64_t{10}));
    integers[MAXIMUM_INVITE_COUNT]                     = static_cast<int32_t>(gameplayTbl["party"]["maximum_invite_count"].value_or(int64_t{20}));

    // Rates
    integers[RATE_EXPERIENCE] = static_cast<int32_t>(ratesTbl["rates"]["experience"].value_or(int64_t{5}));
    integers[RATE_SKILL]      = static_cast<int32_t>(ratesTbl["rates"]["skill"].value_or(int64_t{3}));
    integers[RATE_LOOT]       = static_cast<int32_t>(ratesTbl["rates"]["loot"].value_or(int64_t{2}));
    integers[RATE_MAGIC]      = static_cast<int32_t>(ratesTbl["rates"]["magic"].value_or(int64_t{3}));
    integers[RATE_SPAWN]      = static_cast<int32_t>(ratesTbl["rates"]["spawn"].value_or(int64_t{1}));

    floats[REWARD_BASE_RATE]         = static_cast<float>(ratesTbl["reward"]["base_rate"].value_or(1.0));
    floats[REWARD_RATE_DAMAGE_DONE]  = static_cast<float>(ratesTbl["reward"]["damage_done"].value_or(1.0));
    floats[REWARD_RATE_DAMAGE_TAKEN] = static_cast<float>(ratesTbl["reward"]["damage_taken"].value_or(1.0));
    floats[REWARD_RATE_HEALING_DONE] = static_cast<float>(ratesTbl["reward"]["healing_done"].value_or(1.0));

    // Monsters
    booleans[FORCE_MONSTERTYPE_LOAD] = monstersTbl["spawn"]["force_monstertype_load"].value_or(true);
    booleans[REMOVE_ON_DESPAWN]      = monstersTbl["despawn"]["remove_on_despawn"].value_or(true);

    integers[DEFAULT_DESPAWNRANGE]      = Monster::despawnRange  = static_cast<int32_t>(monstersTbl["despawn"]["range"].value_or(int64_t{2}));
    integers[DEFAULT_DESPAWNRADIUS]     = Monster::despawnRadius = static_cast<int32_t>(monstersTbl["despawn"]["radius"].value_or(int64_t{50}));
    integers[DEFAULT_WALKTOSPAWNRADIUS] = static_cast<int32_t>(monstersTbl["despawn"]["walk_to_spawn_radius"].value_or(int64_t{15}));
    integers[SUMMON_PROXIMITY]          = static_cast<int32_t>(monstersTbl["spawn"]["summon_proximity"].value_or(int64_t{2}));

    // Scripts
    booleans[WARN_UNSAFE_SCRIPTS]          = scriptsTbl["safety"]["warn_unsafe"].value_or(true);
    booleans[CONVERT_UNSAFE_SCRIPTS]       = scriptsTbl["safety"]["convert_unsafe"].value_or(true);
    booleans[SCRIPTS_CONSOLE_LOGS]         = scriptsTbl["logging"]["scripts_in_console"].value_or(true);
    booleans[PLAYER_CONSOLE_LOGS]          = scriptsTbl["logging"]["player_in_console"].value_or(true);
    booleans[CHECK_DUPLICATE_STORAGE_KEYS] = scriptsTbl["validation"]["check_duplicate_storage_keys"].value_or(false);
    booleans[LUA_ITEM_DESC]                = scriptsTbl["validation"]["lua_item_desc"].value_or(false);

    // Experience stages
    expStages = LoadExperienceStages();
    expStages.shrink_to_fit();

    loaded = true;
    return true;
}

bool ConfigManager::Reload()
{
    bool result = Load();
    if (transformToSHA1(GetString(ConfigManager::MOTD)) != g_game.getMotdHash())
    {
        g_game.incrementMotdNum();
    }
    return result;
}

static std::string dummy_str;

const std::string& ConfigManager::GetString(StringConfig what) const
{
    if (what >= LAST_STRING_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::GetString] Accessing invalid index: " << what << "\n";
        return dummy_str;
    }
    return strings[what];
}

int32_t ConfigManager::GetNumber(IntegerConfig what) const
{
    if (what >= LAST_INTEGER_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::GetNumber] Accessing invalid index: " << what << "\n";
        return 0;
    }
    return integers[what];
}

bool ConfigManager::GetBoolean(BooleanConfig what) const
{
    if (what >= LAST_BOOLEAN_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::GetBoolean] Accessing invalid index: " << what << "\n";
        return false;
    }
    return booleans[what];
}

float ConfigManager::GetExperienceStage(uint32_t level) const
{
    auto it = std::ranges::find_if(expStages, [level](const auto& stage)
    {
        auto&& [minLevel, maxLevel, multiplier] = stage;
        return level >= minLevel and level <= maxLevel;
    });

    if (it == expStages.end())
    {
        return static_cast<float>(GetNumber(ConfigManager::RATE_EXPERIENCE));
    }

    return std::get<2>(*it);
}

float ConfigManager::GetFloat(FloatConfig what) const
{
    if (what >= LAST_FLOAT_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::GetFloat] Accessing invalid index: " << what << "\n";
        return 0.0f;
    }
    return floats[what];
}

bool ConfigManager::SetString(StringConfig what, std::string_view value)
{
    if (what >= LAST_STRING_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::SetString] Accessing invalid index: " << what << "\n";
        return false;
    }
    strings[what] = value;
    return true;
}

bool ConfigManager::SetNumber(IntegerConfig what, int32_t value)
{
    if (what >= LAST_INTEGER_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::SetNumber] Accessing invalid index: " << what << "\n";
        return false;
    }
    integers[what] = value;
    return true;
}

bool ConfigManager::SetBoolean(BooleanConfig what, bool value)
{
    if (what >= LAST_BOOLEAN_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::SetBoolean] Accessing invalid index: " << what << "\n";
        return false;
    }
    booleans[what] = value;
    return true;
}

bool ConfigManager::SetFloat(FloatConfig what, float value)
{
    if (what >= LAST_FLOAT_CONFIG)
    {
        std::cout << "[Warning - ConfigManager::SetFloat] Accessing invalid index: " << what << "\n";
        return false;
    }
    floats[what] = value;
    return true;
}
