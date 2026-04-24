// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <array>
#include <string>
#include <tuple>
#include <vector>
#include <cstdint>

using ExperienceStages = std::vector<std::tuple<uint32_t, uint32_t, float>>;

class ConfigManager
{
public:
    ConfigManager() = default;

    // Enum value names are intentionally UPPER_SNAKE_CASE to preserve the
    // existing Lua scripting interface (configKeys table exposed via registerEnumIn).
    enum BooleanConfig
    {
        ALLOW_CHANGEOUTFIT,
        ONE_PLAYER_ON_ACCOUNT,
        AIMBOT_HOTKEY_ENABLED,
        REMOVE_RUNE_CHARGES,
        REMOVE_WEAPON_AMMO,
        REMOVE_WEAPON_CHARGES,
        REMOVE_POTION_CHARGES,
        EXPERIENCE_FROM_PLAYERS,
        FREE_PREMIUM,
        REPLACE_KICK_ON_LOGIN,
        ALLOW_CLONES,
        ALLOW_WALKTHROUGH,
        BIND_ONLY_GLOBAL_ADDRESS,
        OPTIMIZE_DATABASE,
        MARKET_PREMIUM,
        EMOTE_SPELLS,
        STAMINA_SYSTEM,
        WARN_UNSAFE_SCRIPTS,
        CONVERT_UNSAFE_SCRIPTS,
        CLASSIC_EQUIPMENT_SLOTS,
        CLASSIC_ATTACK_SPEED,
        SCRIPTS_CONSOLE_LOGS,
        SERVER_SAVE_NOTIFY_MESSAGE,
        SERVER_SAVE_CLEAN_MAP,
        SERVER_SAVE_CLOSE,
        SERVER_SAVE_SHUTDOWN,
        ONLINE_OFFLINE_CHARLIST,
        YELL_ALLOW_PREMIUM,
        PREMIUM_TO_SEND_PRIVATE,
        FORCE_MONSTERTYPE_LOAD,
        DEFAULT_WORLD_LIGHT,
        HOUSE_OWNED_BY_ACCOUNT,
        LUA_ITEM_DESC,
        CLEAN_PROTECTION_ZONES,
        HOUSE_DOOR_SHOW_PRICE,
        ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS,
        REMOVE_ON_DESPAWN,
        PLAYER_CONSOLE_LOGS,
        CHECK_DUPLICATE_STORAGE_KEYS,
        BED_OFFLINE_TRAINING,
        AUGMENT_SLOT_PROTECTION,
        AUGMENT_STAMINA_RULE,
        AUGMENT_CRITICAL_ANIMATION,
        NPC_PZ_WALKTHROUGH,
        ENABLE_ACCOUNT_MANAGER,
        ENABLE_NO_PASS_LOGIN,
        HEALTH_REGEN_NOTIFICATION,
        MANA_REGEN_NOTIFICATION,
        AUTO_OPEN_CONTAINERS,
        ENABLE_IPV6,
        IPV6_FALLBACK_TO_IPV4,

        LAST_BOOLEAN_CONFIG
    };

    enum StringConfig
    {
        MAP_NAME,
        HOUSE_RENT_PERIOD,
        SERVER_NAME,
        OWNER_NAME,
        OWNER_EMAIL,
        URL,
        LOCATION,
        IP,
        MOTD,
        WORLD_TYPE,
        MYSQL_HOST,
        MYSQL_USER,
        MYSQL_PASS,
        MYSQL_DB,
        MYSQL_SOCK,
        DEFAULT_PRIORITY,
        MAP_AUTHOR,
        ACCOUNT_MANAGER_AUTH,
        ASSETS_DAT_PATH,
        IPV6,

        LAST_STRING_CONFIG
    };

    enum IntegerConfig
    {
        SQL_PORT,
        MAX_PLAYERS,
        PZ_LOCKED,
        DEFAULT_DESPAWNRANGE,
        DEFAULT_DESPAWNRADIUS,
        DEFAULT_WALKTOSPAWNRADIUS,
        RATE_EXPERIENCE,
        RATE_SKILL,
        RATE_LOOT,
        RATE_MAGIC,
        RATE_SPAWN,
        HOUSE_PRICE,
        KILLS_TO_RED,
        KILLS_TO_BLACK,
        MAX_MESSAGEBUFFER,
        ACTIONS_DELAY_INTERVAL,
        EX_ACTIONS_DELAY_INTERVAL,
        KICK_AFTER_MINUTES,
        PROTECTION_LEVEL,
        DEATH_LOSE_PERCENT,
        STATUSQUERY_TIMEOUT,
        FRAG_TIME,
        WHITE_SKULL_TIME,
        GAME_PORT,
        LOGIN_PORT,
        STATUS_PORT,
        STAIRHOP_DELAY,
        MARKET_OFFER_DURATION,
        CHECK_EXPIRED_MARKET_OFFERS_EACH_MINUTES,
        MAX_MARKET_OFFERS_AT_A_TIME_PER_PLAYER,
        EXP_FROM_PLAYERS_LEVEL_RANGE,
        MAX_PACKETS_PER_SECOND,
        SERVER_SAVE_NOTIFY_DURATION,
        YELL_MINIMUM_LEVEL,
        MINIMUM_LEVEL_TO_SEND_PRIVATE,
        VIP_FREE_LIMIT,
        VIP_PREMIUM_LIMIT,
        DEPOT_FREE_LIMIT,
        DEPOT_PREMIUM_LIMIT,
        ACCOUNT_MANAGER_POS_X,
        ACCOUNT_MANAGER_POS_Y,
        ACCOUNT_MANAGER_POS_Z,
        SUMMON_PROXIMITY,
        PARTY_EXP_SHARE_RANGE,
        PARTY_EXP_SHARE_FLOORS,
        MAXIMUM_PARTY_SIZE,
        MAXIMUM_INVITE_COUNT,
        PLAYER_SPEED_PER_LEVEL,
        PLAYER_MAX_SPEED,
        PLAYER_MIN_SPEED,
        PLAYER_DEFENSE_CHARGE_INTERVAL,
        MONSTER_DEFENSE_CHARGE_INTERVAL,
        PLAYER_DEFENSE_CHARGES_CAP,
        PLAYER_ARMOR_CHARGES_CAP,
        PLAYER_AUGMENT_CHARGES_CAP,
        MONSTER_DEFENSE_CHARGES_CAP,
        MONSTER_ARMOR_CHARGES_CAP,
        DEFAULT_DEFENSE_CHARGE_COST,
        DEFAULT_ARMOR_CHARGE_COST,
        DEFAULT_AUGMENT_CHARGE_COST,

        LAST_INTEGER_CONFIG
    };

    enum FloatConfig
    {
        REWARD_BASE_RATE,
        REWARD_RATE_DAMAGE_DONE,
        REWARD_RATE_DAMAGE_TAKEN,
        REWARD_RATE_HEALING_DONE,

        LAST_FLOAT_CONFIG
    };

    bool Load();
    bool Reload();

    const std::string& GetString(StringConfig what) const;
    int32_t            GetNumber(IntegerConfig what) const;
    bool               GetBoolean(BooleanConfig what) const;
    float              GetExperienceStage(uint32_t level) const;
    float              GetFloat(FloatConfig what) const;

    bool SetString(StringConfig what, std::string_view value);
    bool SetNumber(IntegerConfig what, int32_t value);
    bool SetBoolean(BooleanConfig what, bool value);
    bool SetFloat(FloatConfig what, float value);

private:
    std::array<std::string, LAST_STRING_CONFIG>  strings  = {};
    std::array<int32_t,     LAST_INTEGER_CONFIG> integers = {};
    std::array<bool,        LAST_BOOLEAN_CONFIG> booleans = {};
    std::array<float,       LAST_FLOAT_CONFIG>   floats   = {};

    ExperienceStages expStages = {};
    bool loaded = false;
};
