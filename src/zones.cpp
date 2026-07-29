// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "configmanager.h"
#include "game.h"
#include "map.h"
#include "iomap.h"
#include "monster.h"
#include "npc.h"
#include "scheduler.h"
#include "zones.h"
#include "events.h"
#include "pugicast.h"
#include "console.h"
#include <pugixml.hpp>
#include <fmt/format.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <numeric>
#include <cstdlib>

extern ConfigManager g_config;
extern Monsters      g_monsters;
extern Game          g_game;
extern Events*       g_events;

namespace Zones
{

    struct Offset
    {
        int forward_steps;
        int side_steps;   // left is up, right is down numerically
        int dz;
    };

    static Offset RotateOffset(const Offset& o, Direction dir)
    {
        switch (dir)
        {
            case DIRECTION_NORTH: return {  o.side_steps,   o.forward_steps, o.dz };
            case DIRECTION_EAST:  return {  o.forward_steps, -o.side_steps,  o.dz };
            case DIRECTION_SOUTH: return { -o.side_steps,   o.forward_steps, o.dz };
            case DIRECTION_WEST:  return { -o.forward_steps, -o.side_steps,  o.dz };
            default:              return o;
        }
    }

    struct WeightedEntry
    {
        std::string name;
        uint16_t    weight;
    };

    class WeightedLottery
    {
        std::vector<WeightedEntry> entries;
        std::mt19937 rng { std::random_device{}() };

    public:
        void Add(std::string name, uint16_t weight)
        {
            entries.push_back({ std::move(name), weight });
        }

        bool Empty() const noexcept { return entries.empty(); }

        WeightedEntry Draw()
        {
            std::vector<uint16_t> weights;
            weights.reserve(entries.size());
            
            for (const auto& e : entries)
                weights.push_back(e.weight);

            std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
            size_t index = dist(rng);

            WeightedEntry result = entries[index];
            std::swap(entries[index], entries.back());
            entries.pop_back();
            return result;
        }
    };

    // Parse "name:weight;name:weight;..." and draw one result.
    static std::optional<WeightedEntry> SelectMonster(const std::string& creatureDataString)
    {
        WeightedLottery lottery;
        std::stringstream ss(creatureDataString);
        std::string segment;

        while (std::getline(ss, segment, ';'))
        {
            if (segment.empty())
                continue;

            size_t colonPos = segment.find(':');
            if (colonPos != std::string::npos)
            {
                std::string name      = segment.substr(0, colonPos);
                std::string chanceStr = segment.substr(colonPos + 1);
                uint16_t chance = 0;
                try   { chance = static_cast<uint16_t>(std::stoi(chanceStr)); }
                catch (...) {}
                lottery.Add(std::move(name), chance);
            }
        }

        if (lottery.Empty())
            return std::nullopt;

        return lottery.Draw();
    }

    // Parse "x:y:z" string into a Position.
    static std::optional<Position> ParseXyz(std::string_view input)
    {
        std::string s(input);
        std::replace(s.begin(), s.end(), ':', ' ');
        std::stringstream ss(s);
        uint16_t x, y, bigZ;

        if (ss >> x and ss >> y and ss >> bigZ)
        {
            char trailing;
            if (ss >> trailing) return std::nullopt;
            if (bigZ > 255)     return std::nullopt;
            return Position(x, y, static_cast<uint8_t>(bigZ));
        }
        return std::nullopt;
    }

    SpawnTrigger ZoneManager::ParseTriggerString(const std::string& s)
    {
        static const std::unordered_map<std::string, SpawnTrigger> table =
        {
            { "Spawn",       SpawnTrigger::Spawn       },
            { "Enter",       SpawnTrigger::Enter       },
            { "Leave",       SpawnTrigger::Leave       },
            { "Death",       SpawnTrigger::Death       },
            { "Despawn",     SpawnTrigger::Despawn     },
            { "Login",       SpawnTrigger::Login       },
            { "Logout",      SpawnTrigger::Logout      },
            { "Use",         SpawnTrigger::Use         },
            { "Remove",      SpawnTrigger::Remove      },
            { "Transform",   SpawnTrigger::Transform   },
            { "Speak",       SpawnTrigger::Speak       },
            { "Look",        SpawnTrigger::Look        },
            { "Summon",      SpawnTrigger::Summon      },
            { "Equip",       SpawnTrigger::Equip       },
            { "DeEquip",     SpawnTrigger::DeEquip     },
            { "Attack",      SpawnTrigger::Attack      },
            { "Defend",      SpawnTrigger::Defend      },
            { "Idle",        SpawnTrigger::Idle        },
            { "GainSkull",   SpawnTrigger::GainSkull   },
            { "LoseSkull",   SpawnTrigger::LoseSkull   },
            { "Kill",        SpawnTrigger::Kill        },
            { "EnterBattle", SpawnTrigger::EnterBattle },
            { "LeaveBattle", SpawnTrigger::LeaveBattle },
        };
        auto it = table.find(s);
        return (it != table.end()) ? it->second : SpawnTrigger::None;
    }

    MinionBehavior ZoneManager::ParseMinionBehavior(const std::string& s)
    {
        if (s == "despawn_on_master_death") return MinionBehavior::DespawnOnMasterDeath;
        if (s == "finish_life_only")        return MinionBehavior::FinishLifeOnly;
        if (s == "independent_respawn")     return MinionBehavior::IndependentRespawn;
        return MinionBehavior::None;
    }

    ZoneFlag ZoneManager::ParseZoneFlagString(const std::string& s)
    {
        if (s == "protection")       return ZoneFlag::Protection;
        if (s == "nopvp")            return ZoneFlag::NoPvp;
        if (s == "pvp")              return ZoneFlag::Pvp;
        if (s == "nologout")         return ZoneFlag::NoLogout;
        if (s == "no_experience")    return ZoneFlag::NoExperience;
        if (s == "no_trading")       return ZoneFlag::NoTrading;
        if (s == "no_transaction")   return ZoneFlag::NoTransaction;
        if (s == "no_party")         return ZoneFlag::NoParty;
        if (s == "no_drop")          return ZoneFlag::NoDrop;
        if (s == "no_death")         return ZoneFlag::NoDeath;
        if (s == "no_death_penalty") return ZoneFlag::NoDeathPenalty;
        if (s == "no_equipment_loss") return ZoneFlag::NoEquipmentLoss;
        if (s == "no_summons")       return ZoneFlag::NoSummons;
        if (s == "ghost")            return ZoneFlag::Ghost;
        if (s == "no_pathfinding")   return ZoneFlag::NoPathfinding;
        if (s == "no_walk")          return ZoneFlag::NoWalk;
        if (s == "no_monsters")      return ZoneFlag::NoMonsters;
        if (s == "no_npcs")          return ZoneFlag::NoNpcs;
        return ZoneFlag::None;
    }

    SpawnType ZoneManager::ParseSpawnTypeString(const std::string& s)
    {
        if (s == "monster") return SpawnType::Monster;
        if (s == "npc")     return SpawnType::Npc;
        if (s == "boss")    return SpawnType::Boss;
        if (s == "item")    return SpawnType::Item;
        return SpawnType::None;
    }

    Policy ZoneManager::ParsePolicyString(const std::string& s)
    {
        if (s == "fixed")     return Policy::Fixed;
        if (s == "relative")  return Policy::Relative;
        if (s == "staged")    return Policy::Staged;
        if (s == "triggered") return Policy::Triggered;
        return Policy::None;
    }

    WeekDay ZoneManager::ParseWeekDayString(const std::string& s)
    {
        if (s == "monday")    return WeekDay::Monday;
        if (s == "tuesday")   return WeekDay::Tuesday;
        if (s == "wednesday") return WeekDay::Wednesday;
        if (s == "thursday")  return WeekDay::Thursday;
        if (s == "friday")    return WeekDay::Friday;
        if (s == "saturday")  return WeekDay::Saturday;
        if (s == "sunday")    return WeekDay::Sunday;
        return WeekDay::None;
    }

    Zone* ZonePool::TryGetMutable(ZoneHandle handle)
    {
        if (not handle.IsValid() or handle.slot_index >= slots.size())
            return nullptr;

        Slot& slot = slots[handle.slot_index];

        if (not slot.occupied or slot.generation != handle.generation)
            return nullptr;

        return &*slot.zone;
    }

    const Zone* ZonePool::TryGet(ZoneHandle handle) const
    {
        if (not handle.IsValid() or handle.slot_index >= slots.size())
            return nullptr;

        const Slot& slot = slots[handle.slot_index];

        if (not slot.occupied or slot.generation != handle.generation)
            return nullptr;

        return &*slot.zone;
    }

    bool ZonePool::Alive(ZoneHandle handle) const noexcept
    {
        return TryGet(handle) != nullptr;
    }

    bool ZonePool::Destroy(ZoneHandle handle)
    {
        if (not handle.IsValid() or handle.slot_index >= slots.size())
            return false;

        Slot& slot = slots[handle.slot_index];

        if (not slot.occupied or slot.generation != handle.generation)
            return false;

        Zone& zone = *slot.zone;

        bool ownFrameIsRunning =
            zone.dispatching
            or std::any_of(resumeStack.begin(), resumeStack.end(), [&zone](std::coroutine_handle<> frameHandle)
            {
                return std::find(zone.liveFrames.begin(), zone.liveFrames.end(), frameHandle) != zone.liveFrames.end();
            })
            or (g_timer_queue.currentlyResuming and std::find(zone.liveFrames.begin(), zone.liveFrames.end(), *g_timer_queue.currentlyResuming) != zone.liveFrames.end());

        if (ownFrameIsRunning)
        {
            graveyard.push_back(handle);
            return true;
        }

        auto frames = zone.liveFrames;

        for (auto frame : frames)
            frame.destroy();

        slot.zone.reset();
        ++slot.generation;
        slot.occupied = false;
        freelist.push_back(handle.slot_index);
        return true;
    }

    void ZonePool::Clear()
    {
        slots.clear();
        freelist.clear();
        resumeStack.clear();
        graveyard.clear();
    }

    void ZonePool::DrainGraveyard()
    {
        if (graveyard.empty())
            return;

        auto pending = std::move(graveyard);
        graveyard.clear();

        for (ZoneHandle handle : pending)
            Destroy(handle);
    }

    void ZoneOverlay::Trigger(CreaturePtr creature, SpawnTrigger trigger)
    {
        auto handles = zones;

        for (ZoneHandle handle : handles)
        {
            Zone* zone = ZoneManager::TryGetMutable(handle);

            if (not zone or not zone->Active() or not zone->onTrigger or zone->dispatching)
                continue;

            auto callback = zone->onTrigger;
            zone->dispatching = true;
            callback(creature, trigger);
            zone->dispatching = false;
        }
    }

    void EventAwaiter::await_suspend(std::coroutine_handle<> h) const
    {
        registerCallback([h]() mutable
        {
            ZoneManager::BeginResume(h);
            h.resume();
            ZoneManager::EndResume();
        });
    }

    bool Zone::TryMonsterSpawn(MonsterPtr monster, Position position, Direction direction, MagicEffectClasses magic_effect, bool startup, bool artificial)
    {
        if (g_events->eventMonsterOnSpawn(monster, position, startup, artificial))
        {
            if (g_game.placeCreature(monster, position, false, true, magic_effect))
            {
                monster->setDirection(direction);
                monster->setSpawn(this->id);
                monster->setMasterPos(position);
                return true;
            }
        }
        return false;
    }

    void Zone::SetRange(Position start, Position end)
    {
        startpos = start;
        range_end = end;

        uint16_t sx = start.x, sy = start.y, ex = end.x, ey = end.y;
        uint8_t  sz = start.z, ez = end.z;

        const uint32_t minX = std::min(sx, ex), maxX = std::max(sx, ex);
        const uint32_t minY = std::min(sy, ey), maxY = std::max(sy, ey);
        const uint16_t minZ = std::min(sz, ez), maxZ = std::max(sz, ez);

        for (uint32_t x = minX; x <= maxX; ++x)
        {
            for (uint32_t y = minY; y <= maxY; ++y)
            {
                for (uint16_t z = minZ; z <= maxZ; ++z)
                {
                    Position pos(static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint8_t>(z));
                    ZoneManager::RegisterSpawnPosition(pos, self);
                }
            }
        }
    }

    void Zone::ProcessCreatures()
    {
        if (not creature_list.empty())
        {
            for (const auto& entry : creature_list)
                Spawn(entry);
            creature_list.clear();
        }
    }

    SpawnCoroTask Zone::DelaySpawn(uint32_t milliseconds, SpawnCreaturePtr entry, bool startup)
    {
        co_await SleepFor { milliseconds, [handle = self] { return ZoneManager::Alive(handle); } };
        Spawn(entry, startup);
    }

    SpawnCoroTask Zone::Run()
    {
        switch (entity_type)
        {
            case SpawnType::Monster:
                switch (policy)
                {
                    case Policy::Fixed:     RunMonsterFixed();     break;
                    case Policy::Relative:  RunMonsterRelative();  break;
                    case Policy::Staged:    RunMonsterStaged();    break;
                    case Policy::Triggered: RunMonsterTriggered(); break;
                    default: break;
                }
                break;

            case SpawnType::Boss:
                switch (policy)
                {
                    case Policy::Fixed:     RunBossFixed();     break;
                    case Policy::Relative:  RunBossRelative();  break;
                    case Policy::Staged:    RunBossStaged();    break;
                    case Policy::Triggered: RunBossTriggered(); break;
                    default: break;
                }
                break;

            case SpawnType::Npc:
                switch (policy)
                {
                    case Policy::Fixed:    RunNpcFixed();    break;
                    case Policy::Relative: RunNpcRelative(); break;
                    default: break;
                }
                break;

            default: break;
        }

        co_return;
    }

    SpawnCoroTask Zone::Spawn(SpawnCreaturePtr entry, bool startup)
    {
        switch (entity_type)
        {
            case SpawnType::Monster:
                switch (policy)
                {
                    case Policy::Fixed:     SpawnMonsterFixed(entry, startup);  break;
                    case Policy::Relative:  SpawnMonsterRelative(entry);        break;
                    case Policy::Staged:
                    case Policy::Triggered: SpawnMonsterSimple(entry, startup); break;
                    default: break;
                }
                break;

            case SpawnType::Boss:
                switch (policy)
                {
                    case Policy::Fixed:
                    case Policy::Staged:
                    case Policy::Triggered: SpawnMonsterSimple(entry, startup);   break;
                    case Policy::Relative:  SpawnBossRelative(entry, startup);    break;
                    default: break;
                }
                break;

            case SpawnType::Npc:
                switch (policy)
                {
                    case Policy::Fixed:    SpawnNpcFixed(entry, startup);    break;
                    case Policy::Relative: SpawnNpcRelative(entry, startup); break;
                    default: break;
                }
                break;

            default: break;
        }

        co_return;
    }

    SpawnCoroTask Zone::SpawnMonsterFixed(SpawnCreaturePtr entry, bool startup)
    {
        auto sleepCount = 0;

        while (not Paused())
        {
            auto monsterData = SelectMonster(entry->creatures);
            if (not monsterData)
                co_return;

            auto monsterType = g_monsters.getMonsterType(monsterData->name);

            if (not monsterType)
                co_return;

            auto monster = std::make_shared<Monster>(monsterType);

            if (startup and PassivePositional())
            {
                Position spawnPos = 
                {
                    static_cast<uint16_t>(startpos.x + entry->position.x),
                    static_cast<uint16_t>(startpos.y + entry->position.y),
                    static_cast<uint8_t> (startpos.z + entry->position.z)
                };

                if (TryMonsterSpawn(monster, spawnPos, DIRECTION_NORTH, CONST_ME_TELEPORT, true, false))
                {
                    active_creatures.insert({ monster->getID(), entry });
                    co_return;
                }
                else
                {
                    co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
                }
            }

            if (normal_random(1, 100) <= static_cast<int32_t>(entry->chance))
            {
                monsterData = SelectMonster(entry->creatures);
                if (not monsterData)
                    co_return;

                monsterType = g_monsters.getMonsterType(monsterData->name);

                if (not monsterType)
                    co_return;

                monster = std::make_shared<Monster>(monsterType);

                Position spawnPos = 
                {
                    static_cast<uint16_t>(startpos.x + entry->position.x),
                    static_cast<uint16_t>(startpos.y + entry->position.y),
                    static_cast<uint8_t> (startpos.z + entry->position.z)
                };

                if (Passive() and Forced() and not players.empty())
                {
                    while (sleepCount < 5)
                    {
                        co_await SleepFor { cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
                        ++sleepCount;
                    }
                }
                else if (Passive() and not Forced() and not players.empty())
                {
                    creature_list.push_back(entry);
                    co_return;
                }

                if (TryMonsterSpawn(monster, spawnPos, DIRECTION_NORTH, CONST_ME_TELEPORT, false, false))
                {
                    monster->setSpawn(this->id);
                    monster->setMasterPos(spawnPos);
                    active_creatures.insert({ monster->getID(), entry });
                    co_return;
                }
                else
                {
                    co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
                }
            }
            else
            {
                co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
            }
        }

        creature_list.push_back(entry);
    }

    SpawnCoroTask Zone::SpawnMonsterRelative(SpawnCreaturePtr entry)
    {

        while (not Paused())
        {
            if (players.empty())
            {
                creature_list.push_back(entry);
                co_return;
            }

            if (normal_random(1, 100) <= static_cast<int32_t>(entry->chance))
            {
                auto monsterData = SelectMonster(entry->creatures);
                if (not monsterData)
                    co_return;

                auto monsterType = g_monsters.getMonsterType(monsterData->name);

                if (not monsterType)
                    co_return;

                auto monster = std::make_shared<Monster>(monsterType);
                const auto& target = players[uniform_random(0, static_cast<int32_t>(players.size()) - 1)];

                Offset rel {
                    static_cast<int>(entry->position.x),
                    static_cast<int>(entry->position.y),
                    static_cast<int>(entry->position.z)
                };

                auto rotated = RotateOffset(rel, target->getDirection());

                Position spawnPos = {
                    static_cast<uint16_t>(target->getPosition().x + rotated.forward_steps),
                    static_cast<uint16_t>(target->getPosition().y + rotated.side_steps),
                    static_cast<uint8_t> (target->getPosition().z + rotated.dz)
                };

                if (TryMonsterSpawn(monster, spawnPos, DIRECTION_NORTH, CONST_ME_TELEPORT, false, false))
                {
                    active_creatures.insert({ monster->getID(), entry });
                    co_return;
                }
                else
                {
                    co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
                }
            }
            else
            {
                co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
            }
        }

        creature_list.push_back(entry);
    }

    SpawnCoroTask Zone::SpawnMonsterSimple(SpawnCreaturePtr entry, bool startup)
    {

        auto monsterData = SelectMonster(entry->creatures);
        if (not monsterData)
            co_return;

        auto monsterType = g_monsters.getMonsterType(monsterData->name);
        if (not monsterType)
            co_return;

        auto monster = std::make_shared<Monster>(monsterType);

        Position spawnPos =
        {
            static_cast<uint16_t>(startpos.x + entry->position.x),
            static_cast<uint16_t>(startpos.y + entry->position.y),
            static_cast<uint8_t> (startpos.z + entry->position.z)
        };

        if (TryMonsterSpawn(monster, spawnPos, DIRECTION_NORTH, CONST_ME_TELEPORT, startup, false))
        {
            active_creatures.insert({ monster->getID(), entry });
            co_return;
        }
        else
        {
            co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
            SpawnMonsterSimple(entry, false);
        }
    }

    SpawnCoroTask Zone::SpawnBossRelative(SpawnCreaturePtr entry, bool startup)
    {

        if (players.empty())
        {
            creature_list.push_back(entry);
            co_return;
        }

        auto monsterData = SelectMonster(entry->creatures);
        if (not monsterData)
            co_return;

        auto monsterType = g_monsters.getMonsterType(monsterData->name);

        if (not monsterType)
            co_return;

        auto monster = std::make_shared<Monster>(monsterType);
        const auto& target = players[uniform_random(0, static_cast<int32_t>(players.size()) - 1)];

        Offset rel {
            static_cast<int>(entry->position.x),
            static_cast<int>(entry->position.y),
            static_cast<int>(entry->position.z)
        };
        auto rotated = RotateOffset(rel, target->getDirection());

        Position spawnPos = 
        {
            static_cast<uint16_t>(target->getPosition().x + rotated.forward_steps),
            static_cast<uint16_t>(target->getPosition().y + rotated.side_steps),
            static_cast<uint8_t> (target->getPosition().z + rotated.dz)
        };

        if (TryMonsterSpawn(monster, spawnPos, DIRECTION_NORTH, CONST_ME_TELEPORT, startup, false))
        {
            active_creatures.insert({ monster->getID(), entry });
            co_return;
        }
        else
        {
            co_await SleepFor { entry->interval > 0 ? entry->interval : cooldown, [handle = self] { return ZoneManager::Alive(handle); } };
            SpawnBossRelative(entry, false);
        }
    }

    SpawnCoroTask Zone::SpawnNpcFixed(SpawnCreaturePtr entry, bool startup)
    {

        const auto& npc = Npc::createNpc(entry->creatures);

        if (not npc)
            co_return;

        Position spawnPos = 
        {
            static_cast<uint16_t>(startpos.x + entry->position.x),
            static_cast<uint16_t>(startpos.y + entry->position.y),
            static_cast<uint8_t> (startpos.z + entry->position.z)
        };

        npc->setMasterPos(spawnPos);

        bool placed = startup
            ? g_game.internalPlaceCreature(npc, spawnPos, true)
            : g_game.placeCreature(npc, spawnPos, false, true);

        if (placed)
        {
            active_creatures.insert({ npc->getID(), entry });
        }
        else if (entry->interval > 0)
        {
            co_await SleepFor { entry->interval, [handle = self] { return ZoneManager::Alive(handle); } };
            SpawnNpcFixed(entry, false);
        }
    }

    SpawnCoroTask Zone::SpawnNpcRelative(SpawnCreaturePtr entry, bool startup)
    {
        Position spawnPos;

        if (not players.empty())
        {
            const auto& target = players[0];

            Offset rel {
                static_cast<int>(entry->position.x),
                static_cast<int>(entry->position.y),
                static_cast<int>(entry->position.z)
            };

            auto rotated = RotateOffset(rel, target->getDirection());

            spawnPos = 
            {
                static_cast<uint16_t>(target->getPosition().x + rotated.forward_steps),
                static_cast<uint16_t>(target->getPosition().y + rotated.side_steps),
                static_cast<uint8_t> (target->getPosition().z + rotated.dz)
            };
        }
        else
        {
            spawnPos = 
            {
                static_cast<uint16_t>(startpos.x + entry->position.x),
                static_cast<uint16_t>(startpos.y + entry->position.y),
                static_cast<uint8_t> (startpos.z + entry->position.z)
            };
        }

        const auto& npc = Npc::createNpc(entry->creatures);

        if (not npc)
            co_return;

        npc->setMasterPos(spawnPos);

        bool placed = startup
            ? g_game.internalPlaceCreature(npc, spawnPos, true)
            : g_game.placeCreature(npc, spawnPos, false, true);

        if (placed)
        {
            active_creatures.insert({ npc->getID(), entry });
        }
        else if (entry->interval > 0)
        {
            co_await SleepFor { entry->interval, [handle = self] { return ZoneManager::Alive(handle); } };
            SpawnNpcRelative(entry, false);
        }
    }

    void Zone::HandleMasterDeath(const SpawnCreaturePtr& masterEntry, bool& bossCycleEnded)
    {
        master_creature_id = 0;

        switch (minion_behavior)
        {
            case MinionBehavior::DespawnOnMasterDeath:
            {
                for (const auto& [minionId, minionEntry] : active_creatures)
                {
                    if (auto minionCreature = g_game.getCreatureByID(minionId))
                        g_game.removeCreature(minionCreature, false);

                    creature_list.push_back(minionEntry);
                }
                active_creatures.clear();
                break;
            }

            case MinionBehavior::FinishLifeOnly:
            {
                for (const auto& [minionId, minionEntry] : active_creatures)
                    creature_list.push_back(minionEntry);

                active_creatures.clear();
                break;
            }

            case MinionBehavior::IndependentRespawn:
            {
                active_creatures.clear();
                break;
            }

            default:
                break;
        }

        creature_list.push_back(masterEntry);
        bossCycleEnded = true;
    }


    SpawnCoroTask Zone::RunMonsterFixed()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        auto startup = true;

        while (Active())
        {
            if (startup and Passive() and players.empty() and not creature_list.empty())
            {
                startup = false;

                for (const auto& entry : creature_list)
                    Spawn(entry, true);

                creature_list.clear();
            }

            co_await EventAwaiter
            {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                case SpawnTrigger::Login:
                                {
                                    if (not Passive())
                                    {
                                        if (std::find(players.begin(), players.end(), player) == players.end())
                                        {
                                            if (players.empty())
                                            {
                                                players.push_back(player);
                                                ProcessCreatures();
                                            }
                                            else
                                            {
                                                players.push_back(player);
                                            }
                                        }
                                    }
                                    else if (not Forced())
                                    {
                                        Pause();
                                        players.push_back(player);
                                    }
                                    else
                                    {
                                        players.push_back(player);
                                    }
                                    break;
                                }

                                case SpawnTrigger::Leave:
                                case SpawnTrigger::Logout:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());

                                    if (not Passive())
                                    {
                                        if (players.empty())
                                            Pause();
                                    }
                                    else if (players.empty())
                                    {
                                        Unpause();
                                        if (not creature_list.empty())
                                            ProcessCreatures();
                                    }
                                    break;
                                }

                                case SpawnTrigger::Death:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());

                                    if (Passive() and players.empty() and not creature_list.empty())
                                    {
                                        Unpause();
                                        ProcessCreatures();
                                    }
                                    break;
                                }

                                default: break;
                            }
                        }
                        else if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death or trigger == SpawnTrigger::Despawn)
                            {
                                auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == monster->getID(); });

                                if (it != active_creatures.end())
                                {
                                    auto entry = it->second;
                                    active_creatures.erase(it);

                                    if ((not Passive() and not players.empty()) or (Passive() and players.empty()))
                                        DelaySpawn(entry->interval > 0 ? entry->interval : cooldown, entry);
                                    else
                                        creature_list.push_back(entry);
                                }
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    SpawnCoroTask Zone::RunMonsterRelative()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            co_await EventAwaiter
            {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                case SpawnTrigger::Login:
                                {
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                    {
                                        if (players.empty())
                                        {
                                            players.push_back(player);
                                            ProcessCreatures();
                                        }
                                        else
                                        {
                                            players.push_back(player);
                                        }
                                    }
                                    break;
                                }

                                case SpawnTrigger::Leave:
                                case SpawnTrigger::Logout:
                                case SpawnTrigger::Death:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    if (players.empty())
                                        Pause();
                                    break;
                                }

                                default: break;
                            }
                        }
                        else if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death or trigger == SpawnTrigger::Despawn)
                            {
                                auto it = std::find_if(active_creatures.begin(), active_creatures.end(),
                                    [&](const auto& kv) { return kv.first == monster->getID(); });

                                if (it != active_creatures.end())
                                {
                                    auto entry = it->second;
                                    active_creatures.erase(it);

                                    if (not players.empty())
                                        Spawn(entry);
                                    else
                                        creature_list.push_back(entry);
                                }
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    SpawnCoroTask Zone::RunMonsterStaged()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            co_await EventAwaiter
            {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            bool playerAlreadyTracked = std::find(players.begin(), players.end(), player) != players.end();
                            bool wasEmpty = players.empty();

                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                case SpawnTrigger::Login:
                                {
                                    if (not playerAlreadyTracked)
                                    {
                                        players.push_back(player);
                                        if (wasEmpty)
                                        {
                                            if (Rebootable())
                                            {
                                                current_wave = 1;
                                                active_creatures.clear();
                                            }

                                            auto waveIt = waves.find(current_wave);
                                            if (waveIt != waves.end() and creature_list.empty())
                                            {
                                                creature_list = waveIt->second;
                                                ProcessCreatures();
                                            }
                                            else if (Resumable() and not creature_list.empty())
                                            {
                                                ProcessCreatures();
                                            }
                                        }
                                    }
                                    break;
                                }

                                case SpawnTrigger::Leave:
                                case SpawnTrigger::Logout:
                                case SpawnTrigger::Death:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());

                                    if (players.empty())
                                    {
                                        if (Rebootable())
                                        {
                                            active_creatures.clear();
                                            creature_list.clear();
                                            current_wave = 1;
                                        }
                                        else if (Resumable())
                                        {
                                            Pause();
                                        }
                                        else
                                        {
                                            active_creatures.clear();
                                            creature_list.clear();
                                            current_wave = 1;
                                        }
                                    }
                                    break;
                                }

                                default: break;
                            }
                        }
                        else if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death or trigger == SpawnTrigger::Despawn)
                            {
                                auto it = std::find_if(active_creatures.begin(), active_creatures.end(),
                                    [&](const auto& kv) { return kv.first == monster->getID(); });

                                if (it != active_creatures.end())
                                {
                                    active_creatures.erase(it);

                                    if (active_creatures.empty() and creature_list.empty() and not players.empty())
                                    {
                                        ++current_wave;
                                        auto nextWaveIt = waves.find(current_wave);
                                        if (nextWaveIt != waves.end())
                                        {
                                            creature_list = nextWaveIt->second;
                                            ProcessCreatures();
                                        }
                                    }
                                }
                            }
                        }
                        cb();
                    };
                }
            };

            if (Paused())
                Unpause();
        }
    }

    SpawnCoroTask Zone::RunMonsterTriggered()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            co_await EventAwaiter
            {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (trigger == configured_trigger)
                            ProcessCreatures();

                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death or trigger == SpawnTrigger::Despawn)
                            {
                                auto it = std::find_if(active_creatures.begin(), active_creatures.end(),
                                    [&](const auto& kv) { return kv.first == monster->getID(); });

                                if (it != active_creatures.end())
                                {
                                    auto entry = it->second;
                                    active_creatures.erase(it);
                                    DelaySpawn(entry->interval > 0 ? entry->interval : cooldown, entry, false);
                                }
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    SpawnCoroTask Zone::RunBossFixed()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            bool bossStarted = false;

            while (Active() and not bossStarted)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if ((trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login) and std::find(players.begin(), players.end(), player) == players.end())
                                {
                                    bool wasEmpty = players.empty();
                                    players.push_back(player);

                                    if (wasEmpty)
                                        bossStarted = true;
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            if (not Active())
                co_return;

            SpawnCreaturePtr masterEntry;
            std::vector<SpawnCreaturePtr> minionEntries;

            for (const auto& entry : creature_list)
            {
                if (entry->link_type == LinkType::Master)
                    masterEntry = entry;
                else if (entry->link_type == LinkType::Minion)
                    minionEntries.push_back(entry);
            }
            creature_list.clear();

            if (masterEntry)
            {
                Spawn(masterEntry, false);

                for (const auto& [id, e] : active_creatures)
                {
                    if (e == masterEntry)
                    {
                        master_creature_id = id;
                        break;
                    }
                }
            }

            for (const auto& minion : minionEntries)
                DelaySpawn(minion->delay, minion, false);

            bool bossCycleEnded = false;

            while (Active() and not bossCycleEnded)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if (trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login)
                                {
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                        players.push_back(player);
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout
                                         or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            else if (const auto& monster = creature->getMonster())
                            {
                                if (trigger == SpawnTrigger::Death)
                                {
                                    auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == monster->getID(); });

                                    if (it != active_creatures.end())
                                    {
                                        bool isMaster = (monster->getID() == master_creature_id);
                                        auto entry    = it->second;
                                        active_creatures.erase(it);

                                        if (isMaster)
                                            HandleMasterDeath(entry, bossCycleEnded);
                                        else if (not bossCycleEnded)
                                            creature_list.push_back(entry);

                                        if (active_creatures.empty() and not bossCycleEnded)
                                            bossCycleEnded = true;
                                    }
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            players.clear();
            uint32_t respawnCooldown = masterEntry ? (masterEntry->interval > 0 ? masterEntry->interval : cooldown) : cooldown;
            co_await SleepFor { respawnCooldown, [handle = self] { return ZoneManager::Alive(handle); } };
        }
    }

    SpawnCoroTask Zone::RunBossRelative()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            bool bossStarted = false;

            while (Active() and not bossStarted)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if ((trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login) and std::find(players.begin(), players.end(), player) == players.end())
                                {
                                    bool wasEmpty = players.empty();
                                    players.push_back(player);

                                    if (wasEmpty)
                                        bossStarted = true;
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            if (not Active())
                co_return;

            SpawnCreaturePtr masterEntry;
            std::vector<SpawnCreaturePtr> minionEntries;

            for (const auto& entry : creature_list)
            {
                if (entry->link_type == LinkType::Master)
                    masterEntry = entry;
                else if (entry->link_type == LinkType::Minion)
                    minionEntries.push_back(entry);
            }
            creature_list.clear();

            if (masterEntry)
            {
                Spawn(masterEntry, false);

                for (const auto& [id, e] : active_creatures)
                {
                    if (e == masterEntry)
                    {
                        master_creature_id = id;
                        break;
                    }
                }
            }

            for (const auto& minion : minionEntries)
                DelaySpawn(minion->delay, minion, false);

            bool bossCycleEnded = false;

            while (Active() and not bossCycleEnded)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if (trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login)
                                {
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                        players.push_back(player);
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout
                                         or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            else if (const auto& monster = creature->getMonster())
                            {
                                if (trigger == SpawnTrigger::Death)
                                {
                                    auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == monster->getID(); });

                                    if (it != active_creatures.end())
                                    {
                                        bool isMaster = (monster->getID() == master_creature_id);
                                        auto entry    = it->second;
                                        active_creatures.erase(it);

                                        if (isMaster)
                                            HandleMasterDeath(entry, bossCycleEnded);
                                        else if (not bossCycleEnded)
                                            creature_list.push_back(entry);

                                        if (active_creatures.empty() and not bossCycleEnded)
                                            bossCycleEnded = true;
                                    }
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            players.clear();
            uint32_t respawnCooldown = masterEntry ? (masterEntry->interval > 0 ? masterEntry->interval : cooldown) : cooldown;
            co_await SleepFor { respawnCooldown, [handle = self] { return ZoneManager::Alive(handle); } };
        }
    }

    SpawnCoroTask Zone::RunBossStaged()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            bool bossStarted = false;

            while (Active() and not bossStarted)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if ((trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login) and std::find(players.begin(), players.end(), player) == players.end())
                                {
                                    bool wasEmpty = players.empty();
                                    players.push_back(player);

                                    if (wasEmpty)
                                        bossStarted = true;
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            if (not Active())
                co_return;

            // Identify master from creature_list, load first minion wave separately.
            SpawnCreaturePtr masterEntry;

            for (const auto& entry : creature_list)
            {
                if (entry->link_type == LinkType::Master)
                    masterEntry = entry;
            }

            creature_list.clear();

            if (masterEntry)
            {
                Spawn(masterEntry, false);
                for (const auto& [id, e] : active_creatures)
                {
                    if (e == masterEntry) { master_creature_id = id; break; }
                }
            }

            auto waveIt = waves.find(current_wave);

            if (waveIt != waves.end())
            {
                for (const auto& entry : waveIt->second)
                    DelaySpawn(entry->delay, entry, false);
            }

            bool bossCycleEnded = false;

            while (Active() and not bossCycleEnded)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if (trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login)
                                {
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                        players.push_back(player);
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            else if (const auto& monster = creature->getMonster())
                            {
                                if (trigger == SpawnTrigger::Death)
                                {
                                    auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == monster->getID(); });

                                    if (it != active_creatures.end())
                                    {
                                        bool isMaster = (monster->getID() == master_creature_id);
                                        auto entry    = it->second;
                                        active_creatures.erase(it);

                                        if (isMaster)
                                        {
                                            master_creature_id = 0;
                                            creature_list.push_back(entry);
                                            bossCycleEnded = true;
                                        }
                                        else if (active_creatures.empty() and not bossCycleEnded)
                                        {
                                            ++current_wave;
                                            auto nextWave = waves.find(current_wave);

                                            if (nextWave != waves.end())
                                            {
                                                for (const auto& nextEntry : nextWave->second)
                                                    DelaySpawn(nextEntry->delay, nextEntry, false);
                                            }
                                        }
                                    }
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            players.clear();
            current_wave = 1;
            uint32_t respawnCooldown = masterEntry ? (masterEntry->interval > 0 ? masterEntry->interval : cooldown) : cooldown;
            co_await SleepFor { respawnCooldown, [handle = self] { return ZoneManager::Alive(handle); } };
        }
    }

    SpawnCoroTask Zone::RunBossTriggered()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            bool bossStarted = false;

            while (Active() and not bossStarted)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if (trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login)
                                {
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                        players.push_back(player);
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }

                                if (trigger == configured_trigger)
                                    bossStarted = true;
                            }
                            cb();
                        };
                    }
                };
            }

            if (not Active())
                co_return;

            SpawnCreaturePtr masterEntry;
            std::vector<SpawnCreaturePtr> minionEntries;

            for (const auto& entry : creature_list)
            {
                if (entry->link_type == LinkType::Master)
                    masterEntry = entry;
                else if (entry->link_type == LinkType::Minion)
                    minionEntries.push_back(entry);
            }
            creature_list.clear();

            if (masterEntry)
            {
                Spawn(masterEntry, false);

                for (const auto& [id, e] : active_creatures)
                {
                    if (e == masterEntry)
                    {
                        master_creature_id = id;
                        break;
                    }
                }
            }

            for (const auto& minion : minionEntries)
                DelaySpawn(minion->delay, minion, false);

            bool bossCycleEnded = false;

            while (Active() and not bossCycleEnded)
            {
                co_await EventAwaiter
                {
                    [&](auto cb)
                    {
                        onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                        {
                            if (const auto& player = creature->getPlayer())
                            {
                                if (trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login)
                                {
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                        players.push_back(player);
                                }
                                else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout
                                         or trigger == SpawnTrigger::Death)
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                }
                            }
                            else if (const auto& monster = creature->getMonster())
                            {
                                if (trigger == SpawnTrigger::Death)
                                {
                                    auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == monster->getID(); });

                                    if (it != active_creatures.end())
                                    {
                                        bool isMaster = (monster->getID() == master_creature_id);
                                        auto entry    = it->second;
                                        active_creatures.erase(it);

                                        if (isMaster)
                                            HandleMasterDeath(entry, bossCycleEnded);
                                        else if (not bossCycleEnded)
                                            creature_list.push_back(entry);

                                        if (active_creatures.empty() and not bossCycleEnded)
                                            bossCycleEnded = true;
                                    }
                                }
                            }
                            cb();
                        };
                    }
                };
            }

            players.clear();
            uint32_t respawnCooldown = masterEntry ? (masterEntry->interval > 0 ? masterEntry->interval : cooldown) : cooldown;
            co_await SleepFor { respawnCooldown, [handle = self] { return ZoneManager::Alive(handle); } };
        }
    }

    SpawnCoroTask Zone::RunNpcFixed()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        for (const auto& entry : creature_list)
            Spawn(entry, true);

        creature_list.clear();

        while (Active())
        {
            co_await EventAwaiter
            {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (not creature->getPlayer() and trigger == SpawnTrigger::Death)
                        {
                            auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == creature->getID(); });

                            if (it != active_creatures.end())
                            {
                                auto entry = it->second;
                                active_creatures.erase(it);
                                if (entry->interval > 0)
                                    DelaySpawn(entry->interval, entry, false);
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    SpawnCoroTask Zone::RunNpcRelative()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));

        while (Active())
        {
            co_await EventAwaiter
            {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            if (trigger == SpawnTrigger::Enter or trigger == SpawnTrigger::Login)
                            {
                                if (std::find(players.begin(), players.end(), player) == players.end())
                                {
                                    bool wasEmpty = players.empty();
                                    players.push_back(player);

                                    if (wasEmpty)
                                        ProcessCreatures();
                                }
                            }
                            else if (trigger == SpawnTrigger::Leave or trigger == SpawnTrigger::Logout or trigger == SpawnTrigger::Death)
                            {
                                players.erase(std::remove(players.begin(), players.end(), player), players.end());
                            }
                        }
                        else if (not creature->getPlayer() and trigger == SpawnTrigger::Death)
                        {
                            auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv) { return kv.first == creature->getID(); });

                            if (it != active_creatures.end())
                            {
                                auto entry = it->second;
                                active_creatures.erase(it);

                                if (entry->interval > 0)
                                    DelaySpawn(entry->interval, entry, false);
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    size_t ZoneManager::Count()
    {
        return zone_registry.size();
    }

    size_t ZoneManager::SpawnCount()
    {
        size_t count = 0;

        for (ZoneHandle handle : zone_registry)
        {
            Zone* zone = zone_pool.TryGetMutable(handle);

            if (zone and zone->entity_type != SpawnType::None)
                ++count;
        }

        return count;
    }

    std::vector<ZoneHandle>& ZoneManager::Get()
    {
        return zone_registry;
    }

    Zone* ZoneManager::GetZone(int id)
    {
        if (id < 0 or id >= static_cast<int>(zone_registry.size()))
            return nullptr;

        return zone_pool.TryGetMutable(zone_registry[id]);
    }

    std::optional<ZoneHandle> ZoneManager::GetZoneByName(const std::string& name)
    {
        auto it = name_registry.find(name);
        return (it != name_registry.end()) ? std::optional<ZoneHandle>(it->second) : std::nullopt;
    }

    std::vector<int> ZoneManager::GetZonesByPosition(const Position& position)
    {
        std::vector<int> result;
        auto range = position_zone_index.equal_range(position);

        for (auto it = range.first; it != range.second; ++it)
            result.push_back(it->second);

        return result;
    }

    std::optional<ZoneHandle> ZoneManager::RegisterZone(Zone&& zone)
    {
        if (not zone.name.empty() and name_registry.contains(zone.name))
        {
            BlackTek::Console::Warn("[ZoneManager::RegisterZone] Duplicate zone name '{}'", zone.name);
            return std::nullopt;
        }

        if (zone.id == 0)
            zone.id = static_cast<int>(++zone_id_counter);

        if (zone.id < static_cast<int>(zone_registry.size()) and zone_registry[zone.id].IsValid())
        {
            BlackTek::Console::Warn("[ZoneManager::RegisterZone] Duplicate zone id {}", zone.id);
            return std::nullopt;
        }

        if (zone.id >= static_cast<int>(zone_registry.size()))
            zone_registry.resize(static_cast<size_t>(zone.id) + 1);

        int id = zone.id;
        ZoneHandle handle = zone_pool.Emplace(std::move(zone));
        Zone& registered = *zone_pool.TryGetMutable(handle);

        for (const auto& position : registered.positions)
            position_zone_index.insert({ position, id });

        if (registered.world_flags != 0)
        {
            for (const auto& position : registered.positions)
                world_flag_cache[position] |= registered.world_flags;
        }

        if (not registered.name.empty())
            name_registry[registered.name] = handle;

        zone_registry[id] = handle;
        return handle;
    }

    bool ZoneManager::RemoveZone(int id)
    {
        if (id <= 0 or id >= static_cast<int>(zone_registry.size()) or not zone_registry[id].IsValid())
            return false;

        ZoneHandle handle = zone_registry[id];
        Zone* zone = zone_pool.TryGetMutable(handle);

        if (not zone)
            return false;

        zone->Deactivate();

        for (const auto& position : zone->positions)
        {
            auto range = position_zone_index.equal_range(position);

            for (auto it = range.first; it != range.second; )
            {
                if (it->second == id)
                    it = position_zone_index.erase(it);
                else
                    ++it;
            }
        }

        for (const auto& position : zone->positions)
        {
            uint32_t otherZoneFlags = 0;
            auto range = position_zone_index.equal_range(position);

            for (auto it = range.first; it != range.second; ++it)
            {
                if (it->second > 0 and it->second < static_cast<int>(zone_registry.size()) and zone_registry[it->second].IsValid())
                {
                    if (Zone* other = zone_pool.TryGetMutable(zone_registry[it->second]))
                        otherZoneFlags |= other->world_flags;
                }
            }

            auto externalIt = external_world_flags.find(position);
            uint32_t externalFlags = (externalIt != external_world_flags.end() ? externalIt->second : 0);

            auto cacheIt = world_flag_cache.find(position);
            uint32_t remainingFlags = (cacheIt != world_flag_cache.end() ? cacheIt->second & ~zone->world_flags : 0) | otherZoneFlags | externalFlags;

            if (remainingFlags != 0)
                world_flag_cache[position] = remainingFlags;
            else
                world_flag_cache.erase(position);
        }

        if (zone->range_end)
        {
            for (const auto& position : FillRectangle(zone->startpos, *zone->range_end))
            {
                auto it = spawn_position_registry.find(position);

                if (it != spawn_position_registry.end() and it->second.RemoveZone(handle))
                    spawn_position_registry.erase(it);
            }
        }

        if (not zone->name.empty())
            name_registry.erase(zone->name);

        zone_registry[id] = ZoneHandle{};
        zone_pool.Destroy(handle);
        return true;
    }

    bool ZoneManager::RemoveZoneByName(const std::string& name)
    {
        auto handle = GetZoneByName(name);

        if (not handle)
            return false;

        Zone* zone = zone_pool.TryGetMutable(*handle);
        return zone and RemoveZone(zone->id);
    }

    Zone& ZoneManager::CreateZone(int id, std::vector<Position> positions)
    {
        if (id < static_cast<int>(zone_registry.size()) and zone_registry[id].IsValid())
        {
            if (Zone* existing = zone_pool.TryGetMutable(zone_registry[id]))
                return *existing;
        }

        if (id >= static_cast<int>(zone_registry.size()))
            zone_registry.resize(static_cast<size_t>(id) + 1);

        for (const auto& pos : positions)
            position_zone_index.insert({ pos, id });

        Zone newZone(id);
        newZone.positions = std::move(positions);

        ZoneHandle handle = zone_pool.Emplace(std::move(newZone));
        zone_registry[id] = handle;
        return *zone_pool.TryGetMutable(handle);
    }

    std::optional<std::pair<Position, Position>> ZoneManager::ParseRangeTable(const toml::table& zone_table)
    {
        auto rangeTable = zone_table["range"].as_table();

        if (not rangeTable)
            return std::nullopt;

        auto startNode = rangeTable->get("start");
        auto endNode   = rangeTable->get("end");

        if (not startNode or not endNode)
            return std::nullopt;

        auto startStr = startNode->value<std::string>();
        auto endStr   = endNode->value<std::string>();

        if (not startStr or not endStr)
            return std::nullopt;

        auto start = ParseXyz(*startStr);
        auto end   = ParseXyz(*endStr);

        if (not start or not end)
            return std::nullopt;

        return std::make_pair(*start, *end);
    }

    std::vector<Position> ZoneManager::FillRectangle(Position start, Position end)
    {
        std::vector<Position> positions;

        uint32_t minX = std::min(start.x, end.x), maxX = std::max(start.x, end.x);
        uint32_t minY = std::min(start.y, end.y), maxY = std::max(start.y, end.y);
        uint16_t minZ = std::min(start.z, end.z), maxZ = std::max(start.z, end.z);

        for (uint32_t x = minX; x <= maxX; ++x)
            for (uint32_t y = minY; y <= maxY; ++y)
                for (uint16_t z = minZ; z <= maxZ; ++z)
                    positions.emplace_back(static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint8_t>(z));

        return positions;
    }

    std::pair<Position, Position> ZoneManager::BoundingBox(const std::vector<Position>& positions)
    {
        Position minPos = positions.front();
        Position maxPos = positions.front();

        for (const auto& pos : positions)
        {
            minPos.x = std::min(minPos.x, pos.x);
            minPos.y = std::min(minPos.y, pos.y);
            minPos.z = std::min(minPos.z, pos.z);
            maxPos.x = std::max(maxPos.x, pos.x);
            maxPos.y = std::max(maxPos.y, pos.y);
            maxPos.z = std::max(maxPos.z, pos.z);
        }

        return { minPos, maxPos };
    }

    ParseResult ZoneManager::ParseZoneEntry(const std::string& name, const toml::table& zone_table)
    {
        try
        {
            Zone zone;
            zone.name = name;

            std::vector<Position> explicitPositions;

            if (auto posArray = zone_table["positions"].as_array())
            {
                for (const auto& posEntry : *posArray)
                {
                    if (auto posStr = posEntry.value<std::string>())
                    {
                        if (auto pos = ParseXyz(*posStr))
                            explicitPositions.push_back(*pos);
                    }
                }
            }

            auto explicitRange = ParseRangeTable(zone_table);

            if (auto flagsArray = zone_table["flags"].as_array())
            {
                for (const auto& flagEntry : *flagsArray)
                {
                    if (auto flagStr = flagEntry.value<std::string>())
                    {
                        auto zf = ParseZoneFlagString(*flagStr);

                        if (zf != ZoneFlag::None)
                            zone.SetFlag(zf);
                    }
                }
            }

            if (auto spawnTypeStr = zone_table["spawn_type"].value<std::string>())
                zone.entity_type = ParseSpawnTypeString(*spawnTypeStr);

            if (auto policyStr = zone_table["policy"].value<std::string>())
                zone.policy = ParsePolicyString(*policyStr);

            if (zone.entity_type == SpawnType::None and explicitPositions.empty() and not explicitRange)
                return { ParseCode::InvalidFormat, "Zone '" + name + "' has neither 'positions' nor 'range'" };

            if (explicitPositions.empty() and explicitRange)
                explicitPositions = FillRectangle(explicitRange->first, explicitRange->second);

            if (not explicitRange and zone.entity_type != SpawnType::None)
            {
                if (explicitPositions.empty())
                    return { ParseCode::InvalidFormat, "Spawn zone '" + name + "' has neither 'range' nor 'positions' to derive one from" };

                explicitRange = BoundingBox(explicitPositions);
            }

            zone.positions = std::move(explicitPositions);

            if (zone_table["passive"].value_or(false))    zone.Passivize();
            if (zone_table["forced"].value_or(false))     zone.config.set(static_cast<size_t>(ConfigFlag::Forced));
            if (zone_table["rebootable"].value_or(false)) zone.config.set(static_cast<size_t>(ConfigFlag::Rebootable));
            if (zone_table["resumable"].value_or(false))  zone.config.set(static_cast<size_t>(ConfigFlag::Resumable));
            if (zone_table["degradable"].value_or(false)) zone.config.set(static_cast<size_t>(ConfigFlag::Degradable));
            if (zone_table["timed"].value_or(false))      zone.config.set(static_cast<size_t>(ConfigFlag::Timed));

            if (auto cooldownSeconds = zone_table["cooldown"].value<uint32_t>())
                zone.cooldown = static_cast<uint16_t>(*cooldownSeconds * 1000u);

            if (auto triggerStr = zone_table["trigger"].value<std::string>())
                zone.configured_trigger = ParseTriggerString(*triggerStr);

            if (auto behaviorStr = zone_table["minion_behavior"].value<std::string>())
                zone.minion_behavior = ParseMinionBehavior(*behaviorStr);

            if (auto weekdaysArray = zone_table["weekdays"].as_array())
            {
                if (not weekdaysArray->empty())
                {
                    zone.weekdays.reset();

                    for (const auto& weekdayEntry : *weekdaysArray)
                    {
                        if (auto weekdayStr = weekdayEntry.value<std::string>())
                        {
                            auto parsed = ParseWeekDayString(*weekdayStr);

                            if (parsed != WeekDay::None)
                                zone.weekdays.set(static_cast<size_t>(parsed));
                        }
                    }
                }
            }

            if (auto monstersArray = zone_table["monsters"].as_array())
            {
                for (const auto& monsterNode : *monstersArray)
                {
                    auto result = SerializeMonsterEntry(monsterNode);

                    if (result.second.code != ParseCode::Success)
                        return result.second;

                    if (zone.policy == Policy::Staged)
                    {
                        auto tbl = monsterNode.as_table();
                        result.first->wave = static_cast<uint8_t>(tbl and tbl->get("wave") ? tbl->get("wave")->value_or(1u) : 1u);
                        zone.waves[result.first->wave].push_back(result.first);
                    }
                    else
                    {
                        zone.creature_list.push_back(result.first);
                    }
                }

                if (zone.policy == Policy::Staged)
                {
                    if (auto it = zone.waves.find(1); it != zone.waves.end())
                        zone.creature_list = it->second;
                }
            }

            if (auto npcsArray = zone_table["npcs"].as_array())
            {
                for (const auto& npcNode : *npcsArray)
                {
                    auto result = SerializeNpcEntry(npcNode);

                    if (result.second.code != ParseCode::Success)
                        return result.second;

                    zone.creature_list.push_back(result.first);
                }
            }

            if (auto masterNode = zone_table["master"].as_table())
            {
                auto result = SerializeMonsterEntry(*masterNode);

                if (result.second.code != ParseCode::Success)
                    return result.second;

                result.first->link_type = LinkType::Master;
                zone.creature_list.push_back(result.first);
            }

            if (auto minionsArray = zone_table["minions"].as_array())
            {
                for (const auto& minionNode : *minionsArray)
                {
                    auto result = SerializeMonsterEntry(minionNode);

                    if (result.second.code != ParseCode::Success)
                        return result.second;

                    result.first->link_type = LinkType::Minion;

                    if (auto tbl = minionNode.as_table())
                        result.first->delay = tbl->get("delay") ? tbl->get("delay")->value_or(0u) : 0u;

                    zone.creature_list.push_back(result.first);
                }
            }

            auto handle = ZoneManager::RegisterZone(std::move(zone));

            if (not handle)
                return { ParseCode::InvalidFormat, "Zone '" + name + "' failed to register (duplicate id or name)" };

            if (explicitRange)
            {
                Zone& registered = *ZoneManager::TryGetMutable(*handle);
                registered.SetRange(explicitRange->first, explicitRange->second);
            }

            return { ParseCode::Success, "" };
        }
        catch (const std::exception&)
        {
            return { ParseCode::InvalidFormat, "Invalid zone format for '" + name + "'" };
        }
    }

    void ZoneManager::LoadZones()
    {
        auto folder = "data/world/" + g_config.GetString(ConfigManager::MAP_NAME) + "-zones";

        if (not std::filesystem::exists(folder) or not std::filesystem::is_directory(folder))
        {
            std::error_code ec;
            if (not std::filesystem::create_directories(folder, ec))
            {
                BlackTek::Console::Error("Failed to detect and failed to create zones folder: '{}': {}", folder, ec.message());
            }
            return;
        }

        for (const auto& file : std::filesystem::recursive_directory_iterator(folder))
        {
            if (not file.is_regular_file() or file.path().extension() != ".toml")
                continue;

            try
            {
                toml::table fileTable = toml::parse_file(file.path().string());

                for (const auto& [key, entry] : fileTable)
                {
                    if (not entry.is_table())
                        continue;

                    auto result = ParseZoneEntry(std::string(key.str()), *entry.as_table());

                    if (result.code != ParseCode::Success)
                    {
                        BlackTek::Console::Warn("[ZoneManager::LoadZones] {}: {}", file.path().string(), result.msg);
                    }
                }
            }
            catch (const toml::parse_error& err)
            {
                BlackTek::Console::Warn("[ZoneManager::LoadZones] TOML parse error in {}: {}", file.path().string(), err.description());
            }
        }
    }

    void ZoneManager::Clear()
    {
        zone_registry.clear();
        zone_registry.push_back(ZoneHandle{});
        position_zone_index.clear();
        world_flag_cache.clear();
        external_world_flags.clear();
        name_registry.clear();
        spawn_position_registry.clear();
        triggered_spawns.clear();
        staged_spawns.clear();
        linked_spawns.clear();
        zone_id_counter = 0;
        zone_pool.Clear();
    }

    void ZoneManager::Reload()
    {
        Clear();
        LoadZones();
    }

    void ZoneManager::RegisterSpawnPosition(Position& position, ZoneHandle zone)
    {
        auto entry = spawn_position_registry.find(position);
        if (entry != spawn_position_registry.end())
            entry->second.AddZone(zone);
        else
            spawn_position_registry.emplace(position, ZoneOverlay(zone));
    }

    void ZoneManager::RegisterTriggered(uint32_t triggered_id, SpawnTrigger trigger)
    {
        triggered_spawns.insert({ triggered_id, trigger });
    }

    void ZoneManager::RegisterStaged(uint32_t staged_id, StageType stage_type)
    {
        staged_spawns.insert({ staged_id, stage_type });
    }

    void ZoneManager::RegisterLinked(uint32_t linked_id, LinkType link_type)
    {
        linked_spawns.insert({ linked_id, link_type });
    }

    ZoneOverlay* ZoneManager::GetSpawns(Position position)
    {
        auto it = spawn_position_registry.find(position);
        return (it != spawn_position_registry.end()) ? &it->second : nullptr;
    }

    Zone* ZoneManager::TryGetMutable(ZoneHandle handle)
    {
        return zone_pool.TryGetMutable(handle);
    }

    const Zone* ZoneManager::TryGet(ZoneHandle handle)
    {
        return zone_pool.TryGet(handle);
    }

    bool ZoneManager::Alive(ZoneHandle handle) noexcept
    {
        return zone_pool.Alive(handle);
    }

    void ZoneManager::BeginResume(std::coroutine_handle<> handle)
    {
        zone_pool.BeginResume(handle);
    }

    void ZoneManager::EndResume() noexcept
    {
        zone_pool.EndResume();
    }

    void ZoneManager::DrainGraveyard()
    {
        zone_pool.DrainGraveyard();
    }

    std::pair<SpawnCreaturePtr, ParseResult>
    ZoneManager::SerializeMonsterEntry(const toml::v3::node& toml_data)
    {
        auto entry = std::make_shared<SpawnCreature>();

        if (auto monsterTable = toml_data.as_table())
        {
            entry->chance   = monsterTable->get("chance") ? monsterTable->get("chance")->value_or(100u) : 100u;
            entry->interval = (monsterTable->get("interval") ? monsterTable->get("interval")->value_or(60u) : 60u) * 1000u;

            if (auto listNode = monsterTable->get("list"))
            {
                if (auto list = listNode->as_array())
                {
                    for (const auto& listEntry : *list)
                    {
                        if (const auto& creatureEntry = listEntry.as_table())
                        {
                            if (auto nameNode = creatureEntry->get("name"))
                            {
                                const auto& name   = nameNode->value<std::string>();
                                const auto  chance = creatureEntry->get("chance") ? creatureEntry->get("chance")->value_or(100u) : 100u;

                                if (name.has_value())
                                    entry->creatures += *name + ":" + std::to_string(chance) + ";";
                            }
                        }
                    }
                }
            }
            else if (auto nameNode = monsterTable->get("name"))
            {
                if (auto nameStr = nameNode->value<std::string>())
                    entry->creatures += *nameStr + ":" + std::to_string(entry->chance) + ";";
            }

            if (auto posNode = monsterTable->get("position"))
            {
                if (auto posStr = posNode->value<std::string>())
                {
                    if (auto pos = ParseXyz(*posStr))
                        entry->position = std::move(*pos);
                    else
                        return { nullptr, { ParseCode::InvalidFormat, "Invalid position data" } };
                }
            }
        }

        return { std::move(entry), { ParseCode::Success, "" } };
    }

    std::pair<SpawnCreaturePtr, ParseResult>
    ZoneManager::SerializeNpcEntry(const toml::v3::node& toml_data)
    {
        auto entry = std::make_shared<SpawnCreature>();

        if (auto npcTable = toml_data.as_table())
        {
            auto nameNode = npcTable->get("name");

            if (not nameNode)
                return { nullptr, { ParseCode::InvalidFormat, "NPC entry missing 'name'" } };

            auto nameStr = nameNode->value<std::string>();

            if (not nameStr.has_value())
                return { nullptr, { ParseCode::InvalidFormat, "Invalid NPC name" } };

            entry->creatures = std::move(*nameStr);
            entry->interval  = npcTable->get("interval") ? npcTable->get("interval")->value_or(0u) * 1000u : 0u;

            if (auto posNode = npcTable->get("position"))
            {
                if (auto posStr = posNode->value<std::string>())
                {
                    if (auto pos = ParseXyz(*posStr))
                        entry->position = std::move(*pos);
                    else
                        return { nullptr, { ParseCode::InvalidFormat, "Invalid NPC position" } };
                }
            }
        }

        return { std::move(entry), { ParseCode::Success, "" } };
    }

    void ZoneManager::Initialize()
    {
        // Reserved for future initialization work.
    }

    void ZoneManager::ActivateAll()
    {
        for (ZoneHandle handle : zone_registry)
        {
            Zone* zone = zone_pool.TryGetMutable(handle);

            if (zone and zone->entity_type != SpawnType::None)
                zone->Run();
        }
    }

    CoroTask ZoneManager::Supervise()
    {
        while (true)
        {
            auto now   = std::chrono::system_clock::now();
            auto today = std::chrono::floor<std::chrono::days>(now);
            std::chrono::weekday wd { today };

            for (ZoneHandle handle : zone_registry)
            {
                Zone* zone = zone_pool.TryGetMutable(handle);

                if (not zone or zone->entity_type == SpawnType::None)
                    continue;

                if (zone->ActiveToday(wd))
                {
                    if (not zone->Active())
                        zone->Activate();
                }
                else
                {
                    if (zone->Active())
                        zone->Deactivate();
                }
            }

            auto tomorrow       = today + std::chrono::days(1);
            auto msUntilMidnight = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(tomorrow - now).count());

            co_await SleepFor { msUntilMidnight };
        }
    }

    bool ZoneManager::IsInZone(const Position& centerPos, int32_t radius, const Position& pos)
    {
        if (radius == -1)
            return true;

        return (pos.getX() >= centerPos.getX() - radius) and (pos.getX() <= centerPos.getX() + radius)
           and (pos.getY() >= centerPos.getY() - radius) and (pos.getY() <= centerPos.getY() + radius);
    }

    bool ZoneManager::HasWorldFlag(const Position& pos, ZoneFlag flag)
    {
        if (world_flag_cache.empty())
            return false;

        auto it = world_flag_cache.find(pos);
        return it != world_flag_cache.end() and (it->second & static_cast<uint32_t>(flag)) != 0;
    }

    uint32_t ZoneManager::GetWorldFlags(const Position& pos)
    {
        if (world_flag_cache.empty())
            return 0;

        auto it = world_flag_cache.find(pos);
        return it != world_flag_cache.end() ? it->second : 0;
    }

    void ZoneManager::SetWorldFlag(const Position& pos, ZoneFlag flag)
    {
        world_flag_cache[pos]     |= static_cast<uint32_t>(flag);
        external_world_flags[pos] |= static_cast<uint32_t>(flag);
    }

    ZoneType_t ZoneManager::GetZoneType(const Position& pos)
    {
        uint32_t flags = GetWorldFlags(pos);

        if (flags & static_cast<uint32_t>(ZoneFlag::Protection)) return ZONE_PROTECTION;
        if (flags & static_cast<uint32_t>(ZoneFlag::NoPvp))      return ZONE_NOPVP;
        if (flags & static_cast<uint32_t>(ZoneFlag::Pvp))        return ZONE_PVP;
        if (flags & static_cast<uint32_t>(ZoneFlag::NoLogout))   return ZONE_NOLOGOUT;

        return ZONE_NORMAL;
    }

    void ZoneManager::StampWorldZoneFlags()
    {
        for (ZoneHandle handle : zone_registry)
        {
            Zone* zone = zone_pool.TryGetMutable(handle);

            if (not zone or zone->world_flags == 0)
                continue;

            for (const auto& position : zone->positions)
                world_flag_cache[position] |= zone->world_flags;
        }
    }

    static uint32_t TranslateTileFlagsToWorldFlags(uint32_t otbmFlags)
    {
        uint32_t worldFlags = 0;

        if (otbmFlags & OTBM_TILEFLAG_PROTECTIONZONE) worldFlags |= static_cast<uint32_t>(ZoneFlag::Protection);
        if (otbmFlags & OTBM_TILEFLAG_NOPVPZONE)      worldFlags |= static_cast<uint32_t>(ZoneFlag::NoPvp);
        if (otbmFlags & OTBM_TILEFLAG_PVPZONE)        worldFlags |= static_cast<uint32_t>(ZoneFlag::Pvp);
        if (otbmFlags & OTBM_TILEFLAG_NOLOGOUT)       worldFlags |= static_cast<uint32_t>(ZoneFlag::NoLogout);

        return worldFlags;
    }

    static std::filesystem::path ConvertedOutputRoot()
    {
        return std::filesystem::path("data/world/converted") / g_config.GetString(ConfigManager::MAP_NAME);
    }

    static std::filesystem::path ConvertedZonesFolder()
    {
        auto folder = ConvertedOutputRoot() / (g_config.GetString(ConfigManager::MAP_NAME) + "-zones");
        std::error_code dirError;
        std::filesystem::create_directories(folder, dirError);
        return folder;
    }

    static std::filesystem::path EnsureConvertedMapCopy(const std::filesystem::path& sourcePath, const std::filesystem::path& outputDir)
    {
        if (sourcePath.empty())
            return {};

        std::error_code dirError;
        std::filesystem::create_directories(outputDir, dirError);

        auto destPath = outputDir / sourcePath.filename();

        if (std::filesystem::exists(destPath))
            return destPath;

        std::error_code copyError;
        std::filesystem::copy_file(sourcePath, destPath, std::filesystem::copy_options::overwrite_existing, copyError);

        if (copyError)
        {
            BlackTek::Console::Error("[Zones] Failed to write converted map copy {}: {}", destPath.string(), copyError.message());
            return {};
        }

        return destPath;
    }

    static std::string FormatPositionString(const Position& pos)
    {
        return fmt::format("\"{}:{}:{}\"", pos.x, pos.y, pos.z);
    }

    static std::string FormatMonsterEntryToml(const SpawnCreaturePtr& entry)
    {
        std::vector<std::pair<std::string, uint16_t>> weighted;
        std::stringstream ss(entry->creatures);
        std::string segment;

        while (std::getline(ss, segment, ';'))
        {
            if (segment.empty())
                continue;

            auto colonPos = segment.find(':');

            if (colonPos == std::string::npos)
                continue;

            weighted.emplace_back(segment.substr(0, colonPos), static_cast<uint16_t>(std::stoi(segment.substr(colonPos + 1))));
        }

        std::string out = fmt::format("{{ position = {}", FormatPositionString(entry->position));

        if (entry->interval > 0)
            out += fmt::format(", interval = {}", entry->interval / 1000u);

        if (weighted.size() == 1)
        {
            out += fmt::format(", name = \"{}\"", weighted.front().first);
        }
        else if (weighted.size() > 1)
        {
            out += ", list = [";
            bool first = true;

            for (const auto& [monsterName, chance] : weighted)
            {
                if (not first)
                    out += ", ";

                out += fmt::format("{{ name = \"{}\", chance = {} }}", monsterName, chance);
                first = false;
            }

            out += "]";
        }

        out += " }";
        return out;
    }

    static std::string FormatNpcEntryToml(const SpawnCreaturePtr& entry)
    {
        return fmt::format("{{ position = {}, name = \"{}\" }}", FormatPositionString(entry->position), entry->creatures);
    }

    void ZoneManager::SmartConvertLegacyZoneFlags(Map& map)
    {
        if (map.legacyZoneFlagTiles.empty())
            return;

        std::unordered_map<uint32_t, std::vector<Position>> buckets;
        std::vector<const LegacyZoneFlagTile*> patchTargets;

        for (const auto& legacyTile : map.legacyZoneFlagTiles)
        {
            uint32_t residualBits = legacyTile.legacy_bits;

            for (const auto& zoneId : GetZonesByPosition(legacyTile.position))
            {
                if (zoneId <= 0 or zoneId >= static_cast<int>(zone_registry.size()) or not zone_registry[zoneId].IsValid())
                    continue;

                Zone* coveredZone = zone_pool.TryGetMutable(zone_registry[zoneId]);

                if (not coveredZone)
                    continue;

                uint32_t coveredWorldFlags = coveredZone->world_flags;

                if (coveredWorldFlags & static_cast<uint32_t>(ZoneFlag::Protection)) residualBits &= ~OTBM_TILEFLAG_PROTECTIONZONE;
                if (coveredWorldFlags & static_cast<uint32_t>(ZoneFlag::NoPvp))      residualBits &= ~OTBM_TILEFLAG_NOPVPZONE;
                if (coveredWorldFlags & static_cast<uint32_t>(ZoneFlag::Pvp))        residualBits &= ~OTBM_TILEFLAG_PVPZONE;
                if (coveredWorldFlags & static_cast<uint32_t>(ZoneFlag::NoLogout))   residualBits &= ~OTBM_TILEFLAG_NOLOGOUT;
            }

            if (residualBits == 0)
                continue;

            buckets[residualBits].push_back(legacyTile.position);

            if (legacyTile.patchable)
                patchTargets.push_back(&legacyTile);
        }

        if (buckets.empty())
            return;

        static constexpr std::array<std::pair<ZoneFlag, const char*>, 4> legacyFlagNames
        {{
            { ZoneFlag::Protection, "protection" },
            { ZoneFlag::NoPvp,      "nopvp"      },
            { ZoneFlag::Pvp,        "pvp"        },
            { ZoneFlag::NoLogout,   "nologout"   },
        }};

        std::string generatedToml =
            "# Legacy world-flag zones, smart-converted from this map's raw OTBM tile flags.\n"
            "# Only tiles not already covered by an authored zone elsewhere in this folder end up here.\n\n";

        size_t createdCount = 0;
        size_t zoneIndex    = 0;

        for (auto& [bits, positions] : buckets)
        {
            Zone zone;
            zone.positions   = positions;
            zone.world_flags = TranslateTileFlagsToWorldFlags(bits);
            zone.name        = fmt::format("LegacyFlags{}", ++zoneIndex);

            std::string zoneName   = zone.name;
            uint32_t    worldFlags = zone.world_flags;

            if (not ZoneManager::RegisterZone(std::move(zone)))
                continue;

            ++createdCount;

            generatedToml += fmt::format("[{}]\n", zoneName);
            generatedToml += "positions = [";

            bool first = true;

            for (const auto& pos : positions)
            {
                if (not first)
                    generatedToml += ", ";

                generatedToml += FormatPositionString(pos);
                first = false;
            }

            generatedToml += "]\nflags = [";
            first = true;

            for (const auto& [flag, flagName] : legacyFlagNames)
            {
                if ((worldFlags & static_cast<uint32_t>(flag)) == 0)
                    continue;

                if (not first)
                    generatedToml += ", ";

                generatedToml += fmt::format("\"{}\"", flagName);
                first = false;
            }

            generatedToml += "]\n\n";
        }

        if (createdCount > 0)
        {
            auto outputPath = ConvertedZonesFolder() / "legacy-flags.toml";
            std::ofstream outputFile(outputPath, std::ios::trunc);

            if (outputFile.is_open())
                outputFile << generatedToml;

            BlackTek::Console::Info("[Zones] Smart-converted {} legacy zone-flag region(s) from map binary to {}", createdCount, outputPath.string());
        }

        if (map.otbmFilePath.empty())
            return;

        auto mapCopyPath = EnsureConvertedMapCopy(map.otbmFilePath, ConvertedOutputRoot());

        if (mapCopyPath.empty())
            return;

        if (not g_config.GetBoolean(ConfigManager::STRIP_LEGACY_ZONE_FLAGS) or patchTargets.empty())
            return;

        std::fstream otbmFile(mapCopyPath, std::ios::in | std::ios::out | std::ios::binary);

        if (not otbmFile.is_open())
        {
            BlackTek::Console::Error("[Zones] Wrote {} but could not reopen it for patching.", mapCopyPath.string());
            return;
        }

        static constexpr uint32_t clearedValue = 0;
        const char* clearedBytes = reinterpret_cast<const char*>(&clearedValue);
        size_t patchedCount = 0;

        for (const auto* legacyTile : patchTargets)
        {
            for (size_t i = 0; i < legacyTile->raw_byte_offsets.size(); ++i)
            {
                otbmFile.seekp(static_cast<std::streamoff>(legacyTile->raw_byte_offsets[i]));
                otbmFile.write(clearedBytes + i, 1);
            }

            ++patchedCount;
        }

        otbmFile.flush();

        BlackTek::Console::Info("[Zones] Patched {} tile(s) in {} to remove converted legacy zone flag bits (source map untouched).", patchedCount, mapCopyPath.string());
    }

    struct LegacySpawnUnit
    {
        Position centerPos;
        std::vector<SpawnCreaturePtr> monsterEntries;
        std::vector<SpawnCreaturePtr> npcEntries;
    };

    void ZoneManager::ConvertLegacySpawns(const std::filesystem::path& legacyXmlPath, const std::filesystem::path& otbmFilePath)
    {
        if (legacyXmlPath.empty() or not std::filesystem::exists(legacyXmlPath))
            return;

        auto markerPath = legacyXmlPath;
        markerPath += ".converted";

        if (std::filesystem::exists(markerPath))
            return;

        pugi::xml_document doc;
        pugi::xml_parse_result parseResult = doc.load_file(legacyXmlPath.string().c_str());

        if (not parseResult)
        {
            BlackTek::Console::Error("[Zones] Failed to parse legacy spawn file {}: {}", legacyXmlPath.string(), parseResult.description());
            return;
        }

        std::vector<LegacySpawnUnit> units;

        for (auto spawnNode : doc.child("spawns").children())
        {
            LegacySpawnUnit unit;
            unit.centerPos = Position(
                pugi::cast<uint16_t>(spawnNode.attribute("centerx").value()),
                pugi::cast<uint16_t>(spawnNode.attribute("centery").value()),
                pugi::cast<uint16_t>(spawnNode.attribute("centerz").value())
            );

            for (auto childNode : spawnNode.children())
            {
                std::string_view nodeName = childNode.name();

                if (nodeName == "monsters")
                {
                    auto entry = std::make_shared<SpawnCreature>();
                    entry->position = Position(
                        static_cast<uint16_t>(unit.centerPos.x + pugi::cast<uint16_t>(childNode.attribute("x").value())),
                        static_cast<uint16_t>(unit.centerPos.y + pugi::cast<uint16_t>(childNode.attribute("y").value())),
                        unit.centerPos.z
                    );
                    entry->interval = pugi::cast<uint32_t>(childNode.attribute("spawntime").value()) * 1000u;

                    for (auto monsterNode : childNode.children())
                    {
                        auto nameAttribute = monsterNode.attribute("name");

                        if (not nameAttribute)
                            continue;

                        uint16_t chance = monsterNode.attribute("chance")
                            ? pugi::cast<uint16_t>(monsterNode.attribute("chance").value())
                            : 100u;

                        entry->creatures += std::string(nameAttribute.as_string()) + ":" + std::to_string(chance) + ";";
                    }

                    if (not entry->creatures.empty())
                        unit.monsterEntries.push_back(std::move(entry));
                }
                else if (nodeName == "monster")
                {
                    auto nameAttribute = childNode.attribute("name");

                    if (not nameAttribute)
                        continue;

                    auto entry = std::make_shared<SpawnCreature>();
                    entry->position = Position(
                        static_cast<uint16_t>(unit.centerPos.x + pugi::cast<uint16_t>(childNode.attribute("x").value())),
                        static_cast<uint16_t>(unit.centerPos.y + pugi::cast<uint16_t>(childNode.attribute("y").value())),
                        unit.centerPos.z
                    );
                    entry->interval = pugi::cast<uint32_t>(childNode.attribute("spawntime").value()) * 1000u;
                    entry->creatures = std::string(nameAttribute.as_string()) + ":100;";
                    unit.monsterEntries.push_back(std::move(entry));
                }
                else if (nodeName == "npc")
                {
                    auto nameAttribute = childNode.attribute("name");

                    if (not nameAttribute)
                        continue;

                    auto entry = std::make_shared<SpawnCreature>();
                    entry->position = Position(
                        static_cast<uint16_t>(unit.centerPos.x + pugi::cast<uint16_t>(childNode.attribute("x").value())),
                        static_cast<uint16_t>(unit.centerPos.y + pugi::cast<uint16_t>(childNode.attribute("y").value())),
                        unit.centerPos.z
                    );
                    entry->creatures = nameAttribute.as_string();
                    unit.npcEntries.push_back(std::move(entry));
                }
            }

            units.push_back(std::move(unit));
        }

        if (units.empty())
        {
            BlackTek::Console::Info("[Zones] Legacy spawn file {} contained nothing to convert.", legacyXmlPath.string());
            return;
        }

        std::vector<size_t> parent(units.size());
        std::iota(parent.begin(), parent.end(), size_t{0});

        std::function<size_t(size_t)> findRoot = [&](size_t index) -> size_t
        {
            while (parent[index] != index)
            {
                parent[index] = parent[parent[index]];
                index = parent[index];
            }
            return index;
        };

        auto unite = [&](size_t a, size_t b)
        {
            a = findRoot(a);
            b = findRoot(b);

            if (a != b)
                parent[a] = b;
        };

        const int32_t clusterRadius = g_config.GetNumber(ConfigManager::LEGACY_SPAWN_CLUSTER_RADIUS);

        for (size_t i = 0; i < units.size(); ++i)
        {
            for (size_t j = i + 1; j < units.size(); ++j)
            {
                if (units[i].centerPos.z != units[j].centerPos.z)
                    continue;

                int32_t dx = std::abs(static_cast<int32_t>(units[i].centerPos.x) - static_cast<int32_t>(units[j].centerPos.x));
                int32_t dy = std::abs(static_cast<int32_t>(units[i].centerPos.y) - static_cast<int32_t>(units[j].centerPos.y));

                if (std::max(dx, dy) <= clusterRadius)
                    unite(i, j);
            }
        }

        std::unordered_map<size_t, std::vector<size_t>> clusters;

        for (size_t i = 0; i < units.size(); ++i)
            clusters[findRoot(i)].push_back(i);

        std::unordered_map<uint8_t, std::string> tomlByFloor;
        size_t convertedSpawnCount = 0;
        size_t batchedClusterCount = 0;
        size_t zoneIndex           = 0;

        auto registerConverted = [&](SpawnType entityType, const std::vector<SpawnCreaturePtr>& absoluteEntries)
        {
            if (absoluteEntries.empty())
                return;

            uint16_t minX = absoluteEntries.front()->position.x;
            uint16_t maxX = minX;
            uint16_t minY = absoluteEntries.front()->position.y;
            uint16_t maxY = minY;
            uint8_t  z    = absoluteEntries.front()->position.z;

            for (const auto& entry : absoluteEntries)
            {
                minX = std::min(minX, entry->position.x);
                maxX = std::max(maxX, entry->position.x);
                minY = std::min(minY, entry->position.y);
                maxY = std::max(maxY, entry->position.y);
            }

            constexpr int32_t padding = 2;
            int32_t rangeMinX = std::max(0,     static_cast<int32_t>(minX) - padding);
            int32_t rangeMinY = std::max(0,     static_cast<int32_t>(minY) - padding);
            int32_t rangeMaxX = std::min(65535, static_cast<int32_t>(maxX) + padding);
            int32_t rangeMaxY = std::min(65535, static_cast<int32_t>(maxY) + padding);

            Position paddedStart(static_cast<uint16_t>(rangeMinX), static_cast<uint16_t>(rangeMinY), z);
            Position paddedEnd(static_cast<uint16_t>(rangeMaxX), static_cast<uint16_t>(rangeMaxY), z);

            Zone zone;
            zone.entity_type = entityType;
            zone.policy      = Policy::Fixed;
            zone.name        = fmt::format("ConvertedSpawn_z{}_{}", z, ++zoneIndex);

            if (entityType == SpawnType::Monster)
            {
                zone.Passivize();
                zone.config.set(static_cast<size_t>(ConfigFlag::Forced));
            }

            std::vector<SpawnCreaturePtr> relativeEntries;

            for (const auto& entry : absoluteEntries)
            {
                auto converted = std::make_shared<SpawnCreature>(*entry);
                converted->position = Position(
                    static_cast<uint16_t>(entry->position.x - paddedStart.x),
                    static_cast<uint16_t>(entry->position.y - paddedStart.y),
                    0
                );
                relativeEntries.push_back(converted);
                zone.creature_list.push_back(converted);
            }

            zone.positions = FillRectangle(paddedStart, paddedEnd);
            std::string zoneName = zone.name;
            auto handle = ZoneManager::RegisterZone(std::move(zone));

            if (not handle)
                return;

            Zone& registered = *ZoneManager::TryGetMutable(*handle);
            registered.SetRange(paddedStart, paddedEnd);

            std::string& floorToml = tomlByFloor[z];

            floorToml += fmt::format("[{}]\n", zoneName);
            floorToml += fmt::format("range = {{ start = {}, end = {} }}\n", FormatPositionString(paddedStart), FormatPositionString(paddedEnd));
            floorToml += fmt::format("spawn_type = \"{}\"\n", entityType == SpawnType::Npc ? "npc" : "monster");
            floorToml += "policy = \"fixed\"\n";

            if (entityType == SpawnType::Monster)
                floorToml += "passive = true\nforced = true\n";

            floorToml += (entityType == SpawnType::Npc ? "npcs = [\n" : "monsters = [\n");

            for (const auto& entry : relativeEntries)
            {
                floorToml += "    ";
                floorToml += (entityType == SpawnType::Npc) ? FormatNpcEntryToml(entry) : FormatMonsterEntryToml(entry);
                floorToml += ",\n";
            }

            floorToml += "]\n\n";

            ++convertedSpawnCount;
        };

        for (const auto& [root, memberIndices] : clusters)
        {
            std::vector<SpawnCreaturePtr> mergedMonsters;
            std::vector<SpawnCreaturePtr> mergedNpcs;

            for (size_t index : memberIndices)
            {
                const auto& unit = units[index];
                mergedMonsters.insert(mergedMonsters.end(), unit.monsterEntries.begin(), unit.monsterEntries.end());
                mergedNpcs.insert(mergedNpcs.end(), unit.npcEntries.begin(), unit.npcEntries.end());
            }

            if (memberIndices.size() > 1)
                ++batchedClusterCount;

            registerConverted(SpawnType::Monster, mergedMonsters);
            registerConverted(SpawnType::Npc, mergedNpcs);
        }

        if (convertedSpawnCount == 0)
        {
            BlackTek::Console::Info("[Zones] Legacy spawn file {} contained nothing to convert.", legacyXmlPath.string());
            return;
        }

        auto zonesDir = ConvertedZonesFolder();

        for (const auto& [floor, floorToml] : tomlByFloor)
        {
            auto outputPath = zonesDir / fmt::format("floor-{}.toml", floor);
            std::ofstream outputFile(outputPath, std::ios::trunc);

            if (outputFile.is_open())
                outputFile << floorToml;
        }

        auto mapCopyPath = EnsureConvertedMapCopy(otbmFilePath, ConvertedOutputRoot());

        std::ofstream marker(markerPath);
        marker << "Converted to " << zonesDir.string();

        if (not mapCopyPath.empty())
            marker << " (map copy: " << mapCopyPath.string() << ")";

        marker << "\n";
        marker.close();

        BlackTek::Console::Info("[Zones] Smart converted {} legacy <spawn> block(s) into {} Zones across {} floor files ({} cluster(s) batched) from {} to {}",
            units.size(), convertedSpawnCount, tomlByFloor.size(), batchedClusterCount, legacyXmlPath.string(), zonesDir.string());

        if (g_config.GetBoolean(ConfigManager::DISABLE_LEGACY_SPAWN_FILE_AFTER_CONVERSION))
        {
            auto disabledPath = legacyXmlPath;
            disabledPath += ".disabled";

            std::error_code renameError;
            std::filesystem::rename(legacyXmlPath, disabledPath, renameError);

            if (renameError)
            {
                BlackTek::Console::Error("[Zones] Converted {} but failed to rename it aside: {}", legacyXmlPath.string(), renameError.message());
            }
        }
    }
}
