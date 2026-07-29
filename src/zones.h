// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "declarations.h"
#include "position.h"
#include "tile.h"
#include "handle.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <memory_resource>
#include <toml++/toml.hpp>
#include <bitset>
#include <functional>
#include <array>
#include <filesystem>
#include <optional>
#include <coroutine>
#include <gtl/phmap.hpp>

class MonsterType;
class Map;
struct CoroTask;

namespace Zones
{
    struct ZoneTag {};
    using ZoneHandle = BlackTek::Handle<ZoneTag>;

    class Zone;

    struct LegacyZoneFlagTile
    {
        Position position;
        uint32_t legacy_bits = 0;
        bool     patchable = false;
        std::array<size_t, 4> raw_byte_offsets{};
    };

    enum class ZoneFlag : uint32_t
    {
        None                = 0,
        Protection          = 1 << 0,
        NoPvp               = 1 << 1,
        Pvp                 = 1 << 2,
        NoLogout            = 1 << 3,
		NoExperience        = 1 << 4,
		NoTrading           = 1 << 5,
		NoTransaction       = 1 << 6,
		NoParty             = 1 << 7,
		NoDrop              = 1 << 8,
		NoDeath             = 1 << 9,
        NoDeathPenalty      = 1 << 10,
		NoEquipmentLoss     = 1 << 11,
		NoSummons           = 1 << 12,
		Ghost               = 1 << 13,
		NoPathfinding       = 1 << 14,
		NoWalk              = 1 << 15,
		NoMonsters          = 1 << 16,
		NoNpcs              = 1 << 17
    };

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
        Staged,     // wave-based progression
        Fixed,      // absolute (positional) placement
        Relative,   // placement relative to a player
        Triggered   // activated by a configurable game event
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
        Minion,
        Master
    };

    enum class MinionBehavior : uint8_t
    {
        None,
        DespawnOnMasterDeath,
        FinishLifeOnly,
        IndependentRespawn
    };

    enum class SpawnTrigger
    {
        None,
        Spawn,
        Enter,
        Leave,
        Death,
        Despawn,
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
        Rebootable,     // staged: wave progress always resets on player leave
        Resumable,      // staged: wave progress pauses and resumes
        Degradable,     // staged: progress decays over time
        Timed,          // staged: waves advance by time rather than by death count
    };

    struct ParseResult
    {
        ParseCode   code;
        std::string msg;
    };

    class SpawnCreature
    {
    public:
        // Semicolon delimited "name:weight;name:weight;" string for the weighted lottery.
        // For NPCs this holds the NPC name directly (no weights needed).
        std::string creatures;

        Position position;
        uint32_t uid = 0;
        uint32_t chance = 100;
        uint32_t interval = 0;
        uint32_t delay = 0;
        LinkType link_type = LinkType::None;
        uint8_t  wave = 0;
    };

    using SpawnCreaturePtr = std::shared_ptr<SpawnCreature>;

    struct SpawnCoroTask
    {
        struct promise_type
        {
            Zone* owner = nullptr;

            promise_type() : owner(nullptr) {}
            template <typename... Args>
            promise_type(Zone& zone, Args&&...);
            ~promise_type();

            SpawnCoroTask get_return_object() noexcept { return {}; }
            std::suspend_never initial_suspend() noexcept { return {}; }
            std::suspend_never final_suspend()   noexcept { return {}; }
            void return_void()        noexcept {}
            void unhandled_exception() noexcept {}

            static void* operator new(std::size_t size)
            {
                if (s_pool)
                    return s_pool->allocate(size, alignof(std::max_align_t));
                return ::operator new(size);
            }

            static void operator delete(void* ptr, std::size_t size) noexcept
            {
                if (s_pool)
                    s_pool->deallocate(ptr, size, alignof(std::max_align_t));
                else
                    ::operator delete(ptr);
            }
        };

        inline static std::pmr::memory_resource* s_pool = nullptr;
    };

    struct EventAwaiter
    {
        using ResumeFn = std::function<void()>;
        using RegisterFn = std::function<void(ResumeFn)>;

        RegisterFn registerCallback;
        bool ready = false;

        bool await_ready() const noexcept { return ready; }
        void await_suspend(std::coroutine_handle<> h) const;
        void await_resume() const noexcept {}
    };

    class Zone
    {
    public:

        Zone() = default;
        explicit Zone(int zone_id) : id(zone_id) {}

        ZoneHandle  self;

        std::vector<std::coroutine_handle<>> liveFrames;

        int                     id = 0;
        std::string             name;
        Position                startpos;
        std::optional<Position> range_end;
        std::vector<Position>   positions;
        uint32_t                world_flags = 0;
        SpawnType               entity_type = SpawnType::None;
        Policy                  policy = Policy::None;

        std::unordered_map<uint32_t, SpawnCreaturePtr> active_creatures;
        std::vector<SpawnCreaturePtr>                  creature_list;

        std::function<void(CreaturePtr, SpawnTrigger)> onTrigger;
        bool dispatching = false;
        std::vector<PlayerPtr> players;
        std::bitset<32> config;

        std::bitset<8>  weekdays{0b00000001};

        uint16_t cooldown = 5 * 1000;
        uint8_t  spawn_multiplier = 1;
        uint8_t  exp_multiplier = 1;
        uint8_t  loot_multiplier = 1;
        uint8_t  skill_multiplier = 1;

        MinionBehavior minion_behavior = MinionBehavior::None;
        uint32_t       master_creature_id = 0;

        uint8_t  current_wave = 1;
        std::unordered_map<uint8_t, std::vector<SpawnCreaturePtr>> waves;

        SpawnTrigger configured_trigger = SpawnTrigger::None;

        bool HasFlag(ZoneFlag flag) const
        {
            return (world_flags & static_cast<uint32_t>(flag)) != 0;
        }

        void SetFlag(ZoneFlag flag)
        {
            world_flags |= static_cast<uint32_t>(flag);
        }

        void ClearFlag(ZoneFlag flag)
        {
            world_flags &= ~static_cast<uint32_t>(flag);
        }

        bool Boosted() const
        {
            return spawn_multiplier != 1 or exp_multiplier != 1
                or loot_multiplier != 1 or skill_multiplier != 1;
        }

        bool Active()          const { return config.test(static_cast<size_t>(ConfigFlag::Active)); }
        bool Paused()          const { return config.test(static_cast<size_t>(ConfigFlag::Paused)); }
        bool Positional()      const { return config.test(static_cast<size_t>(ConfigFlag::Positional)); }
        bool Passive()         const { return config.test(static_cast<size_t>(ConfigFlag::Passive)); }
        bool Forced()          const { return config.test(static_cast<size_t>(ConfigFlag::Forced)); }
        bool Instant()         const { return config.test(static_cast<size_t>(ConfigFlag::Instant)); }
        bool Rebootable()      const { return config.test(static_cast<size_t>(ConfigFlag::Rebootable)); }
        bool Resumable()       const { return config.test(static_cast<size_t>(ConfigFlag::Resumable)); }
        bool Degradable()      const { return config.test(static_cast<size_t>(ConfigFlag::Degradable)); }
        bool Timed()           const { return config.test(static_cast<size_t>(ConfigFlag::Timed)); }
        bool PassivePositional() const { return Passive() and Positional(); }

        bool ActiveToday(std::chrono::weekday today) const
        {
            return weekdays.test(0) or weekdays.test(today.iso_encoding());
        }

        void Pause() { config.set(static_cast<size_t>(ConfigFlag::Paused)); }
        void Unpause() { config.reset(static_cast<size_t>(ConfigFlag::Paused)); }
        void Positionalize() { config.set(static_cast<size_t>(ConfigFlag::Positional)); }
        void Passivize() { config.set(static_cast<size_t>(ConfigFlag::Passive)); }

        void Activate()
        {
            if (liveFrames.empty())
                Run();
            else
                config.set(static_cast<size_t>(ConfigFlag::Active));
        }

        void Deactivate() { config.reset(static_cast<size_t>(ConfigFlag::Active)); }

        bool TryMonsterSpawn(MonsterPtr creature, Position position, Direction direction, MagicEffectClasses magic_effect, bool startup, bool artificial);

        void SetRange(Position start, Position end);
        void ProcessCreatures();

        SpawnCoroTask Run();
        SpawnCoroTask Spawn(SpawnCreaturePtr entry, bool startup = false);
        SpawnCoroTask DelaySpawn(uint32_t milliseconds, SpawnCreaturePtr entry, bool startup = false);

        void HandleMasterDeath(const SpawnCreaturePtr& masterEntry, bool& bossCycleEnded);

    private:

        SpawnCoroTask RunMonsterFixed();
        SpawnCoroTask RunMonsterRelative();
        SpawnCoroTask RunMonsterStaged();
        SpawnCoroTask RunMonsterTriggered();
        SpawnCoroTask RunBossFixed();
        SpawnCoroTask RunBossRelative();
        SpawnCoroTask RunBossStaged();
        SpawnCoroTask RunBossTriggered();
        SpawnCoroTask RunNpcFixed();
        SpawnCoroTask RunNpcRelative();

        SpawnCoroTask SpawnMonsterFixed(SpawnCreaturePtr entry, bool startup);
        SpawnCoroTask SpawnMonsterRelative(SpawnCreaturePtr entry);
        SpawnCoroTask SpawnMonsterSimple(SpawnCreaturePtr entry, bool startup);   // staged / triggered / boss-fixed
        SpawnCoroTask SpawnBossRelative(SpawnCreaturePtr entry, bool startup);
        SpawnCoroTask SpawnNpcFixed(SpawnCreaturePtr entry, bool startup);
        SpawnCoroTask SpawnNpcRelative(SpawnCreaturePtr entry, bool startup);


    };

    template <typename... Args>
    SpawnCoroTask::promise_type::promise_type(Zone& zone, Args&&...) : owner(&zone)
    {
        owner->liveFrames.push_back(std::coroutine_handle<promise_type>::from_promise(*this));
    }

    inline SpawnCoroTask::promise_type::~promise_type()
    {
        if (owner)
            std::erase(owner->liveFrames, std::coroutine_handle<promise_type>::from_promise(*this));
    }

    class ZonePool
    {
    public:

        template <typename... Args>
        ZoneHandle Emplace(Args&&... args)
        {
            if (not freelist.empty())
            {
                uint32_t index = freelist.back();
                freelist.pop_back();

                Slot& slot = slots[index];
                slot.zone.emplace(std::forward<Args>(args)...);
                slot.occupied = true;

                ZoneHandle handle { index, slot.generation };
                slot.zone->self = handle;
                return handle;
            }

            slots.push_back(Slot{});
            uint32_t index = static_cast<uint32_t>(slots.size() - 1);
            Slot& slot = slots[index];
            slot.zone.emplace(std::forward<Args>(args)...);
            slot.occupied = true;

            ZoneHandle handle { index, slot.generation };
            slot.zone->self = handle;
            return handle;
        }

        [[nodiscard]] Zone*       TryGetMutable(ZoneHandle handle);
        [[nodiscard]] const Zone* TryGet(ZoneHandle handle) const;
        [[nodiscard]] bool        Alive(ZoneHandle handle) const noexcept;

        bool Destroy(ZoneHandle handle);

        void Clear();
        void BeginResume(std::coroutine_handle<> handle) { resumeStack.push_back(handle); }
        void EndResume() noexcept { resumeStack.pop_back(); }

        void DrainGraveyard();

    private:

        struct Slot
        {
            std::optional<Zone> zone;
            uint32_t generation = 0;
            bool     occupied   = false;
        };

        std::deque<Slot>       slots;
        std::vector<uint32_t>  freelist;

        std::vector<std::coroutine_handle<>>   resumeStack;
        std::vector<ZoneHandle>                graveyard;
    };

    class ZoneOverlay
    {
        std::vector<ZoneHandle> zones;

    public:

        ZoneOverlay() = default;

        explicit ZoneOverlay(ZoneHandle zone)
        {
            zones.push_back(zone);
        }

        bool HasAnyZone() const { return not zones.empty(); }

        void AddZone(ZoneHandle zone)
        {
            zones.push_back(zone);
        }

        bool RemoveZone(ZoneHandle handle)
        {
            std::erase_if(zones, [handle](const auto& zone) { return zone == handle; });
            return zones.empty();
        }

        bool operator==(const ZoneOverlay& other) const
        {
            if (zones.size() != other.zones.size())
                return false;

            for (const auto& zone : zones)
            {
                if (std::find(other.zones.begin(), other.zones.end(), zone) == other.zones.end())
                    return false;
            }

            return true;
        }

        bool operator!=(const ZoneOverlay& other) const
        {
            return not (*this == other);
        }

        void Trigger(CreaturePtr creature, SpawnTrigger trigger);
    };

    class ZoneManager
    {
    public:

        struct PositionHash
        {
            std::size_t operator()(const Position& pos) const noexcept
            {
                return (static_cast<std::size_t>(pos.x) << 32)
                    ^ (static_cast<std::size_t>(pos.y) << 16)
                    ^ static_cast<std::size_t>(pos.z);
            }
        };

        static std::optional<ZoneHandle> RegisterZone(Zone&& zone);
        static bool     RemoveZone(int id);
        static bool     RemoveZoneByName(const std::string& name);
        static Zone*       TryGetMutable(ZoneHandle handle);
        static const Zone* TryGet(ZoneHandle handle);
        static bool        Alive(ZoneHandle handle) noexcept;

        static void BeginResume(std::coroutine_handle<> handle);
        static void EndResume() noexcept;
        static void DrainGraveyard();

        static Zone&    CreateZone(int id, std::vector<Position> positions);
        static void     LoadZones();
        static void     Clear();
        static void     Reload();
        static size_t   Count();

        static bool IsInZone(const Position& centerPos, int32_t radius, const Position& pos);

        static void StampWorldZoneFlags();

        static bool     HasWorldFlag(const Position& pos, ZoneFlag flag);
        static uint32_t GetWorldFlags(const Position& pos);

        static void SetWorldFlag(const Position& pos, ZoneFlag flag);

        static ZoneType_t GetZoneType(const Position& pos);

        static void SmartConvertLegacyZoneFlags(Map& map);
        static void ConvertLegacySpawns(const std::filesystem::path& legacyXmlPath, const std::filesystem::path& otbmFilePath);

        static std::vector<ZoneHandle>& Get();

        static Zone* GetZone(int id);

        static std::optional<ZoneHandle> GetZoneByName(const std::string& name);
        static std::vector<int> GetZonesByPosition(const Position& position);

        static void RegisterSpawnPosition(Position& position, ZoneHandle zone);
        static void RegisterTriggered(uint32_t triggered_id, SpawnTrigger trigger);
        static void RegisterStaged(uint32_t staged_id, StageType stage_type);
        static void RegisterLinked(uint32_t linked_id, LinkType link_type);

        static ZoneOverlay* GetSpawns(Position position);

        static size_t SpawnCount();

        static ParseResult ParseZoneEntry(const std::string& name, const toml::table& zone_table);
        static ZoneFlag      ParseZoneFlagString(const std::string& s);
        static SpawnType     ParseSpawnTypeString(const std::string& s);
        static Policy        ParsePolicyString(const std::string& s);
        static MinionBehavior ParseMinionBehavior(const std::string& s);
        static SpawnTrigger  ParseTriggerString(const std::string& s);
        static WeekDay       ParseWeekDayString(const std::string& s);
        static std::optional<std::pair<Position, Position>> ParseRangeTable(const toml::table& zone_table);
        static std::vector<Position> FillRectangle(Position start, Position end);
        static std::pair<Position, Position> BoundingBox(const std::vector<Position>& positions);

        static void Initialize();

        static void ActivateAll();

        static void SetSpawnPool(std::pmr::memory_resource* pool)
        {
            SpawnCoroTask::s_pool = pool;
        }

        CoroTask Supervise();

    private:

        inline static std::vector<ZoneHandle> zone_registry = { ZoneHandle{} };
        inline static std::multimap<Position, int>       position_zone_index;
        inline static std::unordered_map<std::string, ZoneHandle> name_registry;
        inline static gtl::flat_hash_map<Position, uint32_t, PositionHash> world_flag_cache;
        inline static gtl::flat_hash_map<Position, uint32_t, PositionHash> external_world_flags;
        inline static std::unordered_map<Position, ZoneOverlay, PositionHash>  spawn_position_registry;
        inline static std::unordered_map<uint32_t, SpawnTrigger>                                triggered_spawns;
        inline static std::unordered_map<uint32_t, StageType>                                   staged_spawns;
        inline static std::unordered_map<uint32_t, LinkType>                                    linked_spawns;

        inline static uint32_t zone_id_counter = 0;

        inline static ZonePool zone_pool;

        static std::pair<SpawnCreaturePtr, ParseResult> SerializeMonsterEntry(const toml::v3::node& toml_data);
        static std::pair<SpawnCreaturePtr, ParseResult> SerializeNpcEntry(const toml::v3::node& toml_data);
    };
}