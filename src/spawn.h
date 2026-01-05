// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "declarations.h"
#include "position.h"
#include "tile.h"
#include <utility>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <toml++/toml.hpp>
#include <bitset>

class MonsterType;

namespace Spawns
{
    enum class ParseCode
    {
        Success = 0,
        FileNotFound,
        ParseError,
        InvalidFormat,
        DirectoryNotFound
    };

    enum class SpawnType
    {
        None,
        Boss,
        Monster,
        Npc,
        Item
    };

    enum class Policy
    {
        None,
        Staged, // to allow multiple forms for a boss
        Fixed,
        Relative,
        Triggered
    };

    enum class WeekDay : uint32_t
    {
        None,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday
    };

    enum class StageType : uint32_t
    {
        None,
        Wave,
        Timed,
        Linked,
        PerEntity
    };

    enum class LinkType : uint32_t
    {
        None,
        Peer,
        Slave,
        Master
    };

    struct ParseResult
    {
        ParseCode code;
        std::string msg;
    };

    struct EventAwaiter
    {
        using ResumeFn = std::function<void()>;
        using RegisterFn = std::function<void(ResumeFn)>;

        RegisterFn registerCallback;
        bool ready = false;

        bool await_ready() const noexcept { return ready; }

        void await_suspend(std::coroutine_handle<> h) const
        {
            auto resumeFn = [h]() mutable
            {
                h.resume();
            };
            registerCallback(resumeFn);
        }

        void await_resume() const noexcept { }
    };

    class SpawnCreature
    {
    public:
        std::string creatures;
        Position position;
        uint32_t uid = 0;
        uint32_t chance = 100;
        uint32_t interval = 0;
    };


    inline static uint32_t zoned_spawns_count = 0;
    inline static uint32_t positional_spawns_count = 0;
    inline static uint32_t item_spawns_count = 0;

    enum class SpawnTrigger // Change to just Trigger
    {
        None,
        Spawn, // could be used to chain things
        Enter, // same as step on
        Leave, // same as step off
        Death,
        Login,
        Logout,
        Use,
        Remove,
        Transform,
        Speak,
        Look,
        Summon,
        Equip,
        DeEquip,
        Attack,
        Defend,
        Idle,
        GainCondition,
        LoseCondition,
        Kill,
        HealthChange,
        ManaChange,
        SoulChange,
        StaminaChange,
        GainSkull,
        LoseSkull,
        EnterBattle,
        LeaveBattle,
        GainPzLock,
        LosePzLock,
    };

    enum class TriggeredAction
    {
        None,
        Spawn,
        Kill,
        Use,
        Remove,
        Transform,
        AddCondition,
        RemoveCondition,
        GiveHealth,
        RemoveHealth,
        GiveMana,
        RemoveMana,
        GiveSoul,
        RemoveSoul,
        GiveStamina,
        RemoveStamina,
        GiveSkull,
        RemoveSkull,
    };

    enum class ConfigFlag
    {
        None,
        Active,
        Paused,
        Positional,
        Passive,
        Forced,
        Instant,
        Rebootable,     // staged: wave progess always resets
        Resumable,      // staged: wave progress continue and pause
        Degradable,     // staged: progress decays
        Timed,          // staged: waves progress after time, rather than by death
    };

    template <SpawnType EntityType, Policy RuleType>
    class ZoneManager : public std::enable_shared_from_this<ZoneManager<EntityType, RuleType>>
    {
    public:

        // todo : make these data members private and use public methods for encapsulation
        std::unordered_map<uint32_t, std::shared_ptr<SpawnCreature>> active_creatures;
        std::function<void(CreaturePtr, SpawnTrigger)> onTrigger;
        std::vector<std::shared_ptr<SpawnCreature>> creature_list;
        std::vector<PlayerPtr> players;
        Position startpos;
        uint32_t id = 0;
        std::bitset<32> config;
        std::bitset<8> weekdays;
        uint16_t cooldown = 5 * 1000; // ms -- move the * multiplier to where it's used
        uint8_t spawn_multiplier = 1;
        uint8_t exp_multiplier = 1;
        uint8_t loot_multiplier = 1;
        uint8_t skill_multiplier = 1;

        // todo :: make different counts for the different types of spawns
        // and use if constexpr to increase appropriate counts, active counts, ect...
        // as well as do any specialized coded things I need done..
        ZoneManager() : id(++zoned_spawns_count) { }

        bool boosted() const { return spawn_multiplier != 1 or exp_multiplier != 1 or loot_multiplier != 1 or skill_multiplier != 1; }
        bool active() const { return config.test(static_cast<size_t>(ConfigFlag::Active)); }
        bool paused() const { return config.test(static_cast<size_t>(ConfigFlag::Paused)); }
        bool positional() const {return config.test(static_cast<size_t>(ConfigFlag::Positional)); }
        bool passive() const { return config.test(static_cast<size_t>(ConfigFlag::Passive)); }
        bool forced() const { return config.test(static_cast<size_t>(ConfigFlag::Forced)); } // for deciding to spawn in front of players -- todo : global forced retries, effects (times too), and if forced when boosted
        bool instant() const { return config.test(static_cast<size_t>(ConfigFlag::Instant)); }
        bool rebootable() const { return config.test(static_cast<size_t>(ConfigFlag::Rebootable)); }
        bool resumable() const { return config.test(static_cast<size_t>(ConfigFlag::Resumable)); }
        bool degradable() const { return config.test(static_cast<size_t>(ConfigFlag::Degradable)); }
        bool timed() const { return config.test(static_cast<size_t>(ConfigFlag::Timed)); }
        bool passive_positional() const { return passive() and positional(); }
        bool active_today(std::chrono::weekday today) const { return weekdays.test(0) or weekdays.test(today.iso_encoding()); }
        bool tryMonsterSpawn(MonsterPtr creature, Position position, Direction direction, MagicEffectClasses magic_effect, bool startup, bool artificial);

        void pause() {config.set(static_cast<size_t>(ConfigFlag::Paused)); }
        void unpause() { config.reset(static_cast<size_t>(ConfigFlag::Paused)); }
        void positionalize() { config.set(static_cast<size_t>(ConfigFlag::Positional)); }
        void passivize() { config.set(static_cast<size_t>(ConfigFlag::Passive)); }
        
        void activate();
        void deactivate();

        // stuff to probably make private
        ParseResult set_range(const toml::v3::table& range_table);
        void process_creatures();

        CoroTask delay_spawn(uint32_t milliseconds, std::shared_ptr<SpawnCreature> entry, bool startup = true);
        CoroTask spawn(std::shared_ptr<SpawnCreature> entry, bool startup = false);
        CoroTask run();
    };

    using PositionalMonsterSpawn = std::shared_ptr<ZoneManager<SpawnType::Monster, Policy::Fixed>>;
    using RelativeMonsterSpawn = std::shared_ptr<ZoneManager<SpawnType::Monster, Policy::Relative>>;

    using PositionalNpcSpawn = std::shared_ptr<ZoneManager<SpawnType::Npc, Policy::Fixed>>;
    using RelativeNpcSpawn = std::shared_ptr<ZoneManager<SpawnType::Npc, Policy::Relative>>;
    using AnySpawn = std::variant<PositionalMonsterSpawn, PositionalNpcSpawn, RelativeMonsterSpawn, RelativeNpcSpawn, std::nullptr_t>;

    class SpawnZones
    {
        std::unique_ptr<std::vector<PositionalMonsterSpawn>> pos_mon_spawns;
        std::unique_ptr<std::vector<RelativeMonsterSpawn>> rel_mon_spawns;
        std::unique_ptr<std::vector<PositionalNpcSpawn>> pos_npc_spawns;
        std::unique_ptr<std::vector<RelativeNpcSpawn>> rel_npc_spawns;

    public:

        SpawnZones() { }

        SpawnZones(PositionalMonsterSpawn spawn)
        {
            pos_mon_spawns = std::make_unique<std::vector<PositionalMonsterSpawn>>();
            pos_mon_spawns->push_back(spawn);
        }

        SpawnZones(RelativeMonsterSpawn spawn)
        {
            rel_mon_spawns = std::make_unique<std::vector<RelativeMonsterSpawn>>();
            rel_mon_spawns->push_back(spawn);
        }

        SpawnZones(PositionalNpcSpawn spawn)
        {
            pos_npc_spawns = std::make_unique<std::vector<PositionalNpcSpawn>>();
            pos_npc_spawns->push_back(spawn);
        }

        SpawnZones(RelativeNpcSpawn spawn)
        {
            rel_npc_spawns = std::make_unique<std::vector<RelativeNpcSpawn>>();
            rel_npc_spawns->push_back(spawn);
        }

        bool hasPositionalMonsterSpawn() const { return pos_mon_spawns.get() and not pos_mon_spawns->empty(); }
        bool hasRelativeMonsterSpawn() const { return rel_mon_spawns.get() and not rel_mon_spawns->empty(); }
        bool hasPositionalNpcSpawn() const { return pos_npc_spawns.get() and not pos_npc_spawns->empty(); }
        bool hasRelativeNpcSpawn() const { return rel_npc_spawns.get() and not rel_npc_spawns->empty(); }
        bool hasAnySpawn() const { return hasPositionalMonsterSpawn() or hasRelativeMonsterSpawn() or hasPositionalNpcSpawn() or hasRelativeNpcSpawn(); }


        void addSpawn(PositionalMonsterSpawn spawn)
        {
            if (pos_mon_spawns.get() == nullptr)
                pos_mon_spawns = std::make_unique<std::vector<PositionalMonsterSpawn>>();

            pos_mon_spawns->push_back(spawn);
        }

        void addSpawn(RelativeMonsterSpawn spawn)
        {
            if (rel_mon_spawns.get() == nullptr)
                rel_mon_spawns = std::make_unique<std::vector<RelativeMonsterSpawn>>();

            auto spawns = rel_mon_spawns.get();
            spawns->push_back(spawn);
        }

        void addSpawn(PositionalNpcSpawn spawn)
        {
            if (pos_npc_spawns.get() == nullptr)
                pos_npc_spawns = std::make_unique<std::vector<PositionalNpcSpawn>>();

            auto spawns = pos_npc_spawns.get();
            spawns->push_back(spawn);
        }

        void addSpawn(RelativeNpcSpawn spawn)
        {
            if (rel_npc_spawns.get() == nullptr)
                rel_npc_spawns = std::make_unique<std::vector<RelativeNpcSpawn>>();

            auto spawns = rel_npc_spawns.get();
            spawns->push_back(spawn);
        }

        void trigger(CreaturePtr c_ptr, SpawnTrigger s_trigger)
        {
            if (hasPositionalMonsterSpawn())
            {
                for (const auto& spawn : *pos_mon_spawns.get())
                {
                    spawn->onTrigger(c_ptr, s_trigger);
                }
            }

            if (hasRelativeMonsterSpawn())
            {
                for (const auto& spawn : *rel_mon_spawns.get())
                {
                    spawn->onTrigger(c_ptr, s_trigger);
                }
            }

            if (hasPositionalNpcSpawn())
            {
                for (const auto& spawn : *pos_npc_spawns.get())
                {
                    spawn->onTrigger(c_ptr, s_trigger);
                }
            }

            if (hasRelativeMonsterSpawn())
            {
                for (const auto& spawn : *rel_npc_spawns.get())
                {
                    spawn->onTrigger(c_ptr, s_trigger);
                }
            }
        }
    };

    class System
    {
    public:

        struct PositionHash
        {
            std::size_t operator()(const Position& pos) const noexcept
            {
                return (static_cast<std::size_t>(pos.x) << 32) ^ (static_cast<std::size_t>(pos.y) << 16) ^ static_cast<std::size_t>(pos.z);
            }
        };

        inline static std::unordered_map<Position, std::shared_ptr<SpawnZones>, PositionHash> _spawn_position_registry;
        inline static std::unordered_map<uint32_t, AnySpawn> _spawn_registry;
        inline static std::unordered_map<uint32_t, SpawnTrigger> _triggered_spawns;
        inline static std::unordered_map<uint32_t, StageType> _staged_spawns;
        inline static std::unordered_map<uint32_t, LinkType> _linked_spawns;

        //inline static std::chrono::time_point<uint32_t> _next_day;
        //inline static std::chrono::weekday _today;

        static void RegisterSpawnPosition(Position& position, const AnySpawn& spawn);

        static void RegisterSpawn(uint32_t spawn_id, AnySpawn zone);
        static void RegisterTriggered(uint32_t triggered_spawn_id, SpawnTrigger trigger);
        static void RegisterStaged(uint32_t staged_spawn_id, StageType s_type);
        static void RegisterLinked(uint32_t linked_spawn_id, LinkType l_type);
        static AnySpawn GetSpawn(uint32_t id);
        static std::shared_ptr<SpawnZones> GetSpawns(Position position);

        static std::pair<std::shared_ptr<SpawnCreature>, ParseResult> SerializeMonsterEntry(const toml::v3::node& toml_data);

        static ParseResult ParseFixedMonsterSpawn(const toml::table& zone_table);
        static ParseResult ParseRelativeMonsterSpawn(const toml::table& zone_table);
        static ParseResult ParseFixedNpcSpawn(const toml::table& zone_table);
        static ParseResult ParseRelativeNpcSpawn(const toml::table& zone_table);
        static ParseResult ParseFile(const std::filesystem::path& file_path);
        static ParseResult LoadSpawns(const std::string& directory_path);
        static void LoadSpawns();
        static void Initialize();

        CoroTask Supervise();

    };
}
