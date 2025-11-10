// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "configmanager.h"
#include "game.h"
#include "monster.h"
#include "scheduler.h"
#include "spawn.h"
#include "events.h"
#include "pugicast.h"
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

extern ConfigManager g_config;
extern Monsters g_monsters;
extern Game g_game;
extern Events* g_events;

namespace Spawns
{

    struct Offset
    {
        int forward_steps;
        int side_steps; // left is up, right is down.. numerically
        int dz;
    };

    Offset rotateOffset(const Offset& o, Direction dir)
    {
        switch (dir)
        {
            case DIRECTION_NORTH: return { o.side_steps, o.forward_steps, o.dz };
            case DIRECTION_EAST: return { o.forward_steps, -o.side_steps, o.dz };
            case DIRECTION_SOUTH: return { -o.side_steps, o.forward_steps, o.dz };
            case DIRECTION_WEST: return { -o.forward_steps, -o.side_steps, o.dz };
            default: return o;
        }
    }

    struct WeightedEntry
    {
        std::string name;
        uint16_t weight;
    };

    class WeightedLottery
    {
        std::vector<WeightedEntry> entries;
        std::mt19937 rng { std::random_device {}() };

    public:
        void add(std::string name, uint16_t weight)
        {
            entries.push_back({ std::move(name), weight });
        }

        bool empty() const noexcept { return entries.empty(); }

        WeightedEntry draw()
        {
            std::vector<uint16_t> weights;
            weights.reserve(entries.size());
            for (auto& entry : entries)
                weights.push_back(entry.weight);

            std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
            size_t index = dist(rng);

            WeightedEntry result = entries[index];
            std::swap(entries[index], entries.back());
            entries.pop_back();
            return result;
        }
    };

    static WeightedEntry SelectMonster(const std::string& creature_data_string)
    {
        WeightedLottery entries;
        std::stringstream ss(creature_data_string);
        std::string segment;

        while (std::getline(ss, segment, ';'))
        {
            if (segment.empty())
            {
                continue;
            }

            size_t colon_pos = segment.find(':');

            if (colon_pos != std::string::npos)
            {
                std::string name = segment.substr(0, colon_pos);

                std::string chance_str = segment.substr(colon_pos + 1);
                uint16_t chance = 0;
                try
                {
                    chance = std::stoi(chance_str);
                }
                catch (const std::exception& e)
                {
                    // std::cerr << "Error converting chance value '" << chance_str << "': " << e.what() << std::endl;
                }

                entries.add(std::move(name), chance);
            }
        }

        return entries.draw();
    }

    static std::optional<Position> parse_xyz(std::string_view input)
    {
        std::string s(input);
        uint16_t x, y, big_z;
        uint8_t z;
        std::replace(s.begin(), s.end(), ':', ' '); // Replace ':' with ' ' for stream extraction
        std::stringstream ss(s);
        if (ss >> x and ss >> y and ss >> big_z)
        {
            char trailing_char;
            if (ss >> trailing_char)
            {
                return std::nullopt;
            }
            if (big_z > 255)
            {
                return std::nullopt;
            }
            z = static_cast<uint8_t>(big_z);
            return Position(x, y, z);
        }
        return std::nullopt;
    }

    template <SpawnType EntityType, Policy RuleType, SpawnTrigger TriggerType>
    bool ZoneManager<EntityType, RuleType, TriggerType>::tryMonsterSpawn(MonsterPtr monster, Position position, Direction direction, MagicEffectClasses magic_effect /*CONST_ME_TELEPORT*/, bool startup /*false*/, bool artificial /*false*/)
    {
        if (g_events->eventMonsterOnSpawn(monster, position, true, false))
        {
            if (g_game.placeCreature(monster, position, false, true, magic_effect))
            {
                monster->setDirection(direction);
                monster->setSpawn(this->id);
                monster->setMasterPos(position);
                std::cout << "ZonedSpawn::spawn() placing creature : " << monster->getName() << " at position : " << position.x << " " << position.y << " " << static_cast<int>(position.z) << " \n";
                return true;
            }
        }
        else
        {
            // todo
            // what should we do here if the event blocks the spawn.. try again after interval has passed?
        }
        return false;
    }

    template<>
    CoroTask ZoneManager<SpawnType::Monster, Policy::Relative>::spawn(std::shared_ptr<SpawnCreature> entry, bool startup)
    {
        auto spawn_count = 1;
        auto sleep_count = 0;

        while (not paused())
        {
            std::cout << "Not paused... \n";

            auto monster_data = SelectMonster(entry->creatures);
            auto monster_type = g_monsters.getMonsterType(monster_data.name);
            auto monster = std::make_shared<Monster>(monster_type);

            // todo: I'm using random twice here, I think I can combine the calls into a single result and then split the result appropriately using bitshift operations
            if (normal_random(1, 100) < entry->chance)
            {
                auto monster_data = SelectMonster(entry->creatures);
                auto monster_type = g_monsters.getMonsterType(monster_data.name);
                auto monster = std::make_shared<Monster>(monster_type);
                const auto& target = players[uniform_random(0, players.size() - 1)]; // <-- the other random

                Offset rel { // todo these static casts look ugly in our logic, lets write appropriate overloads for Offset instead
                    static_cast<int>(entry->position.x),
                    static_cast<int>(entry->position.y),
                    static_cast<int>(entry->position.z)
                };

                auto rotated = rotateOffset(rel, target->getDirection());

                Position spawn_pos = { // todo these static casts look ugly in our logic, lets write appropriate overloads for Position instead
                    static_cast<uint16_t>(target->getPosition().x + rotated.forward_steps),
                    static_cast<uint16_t>(target->getPosition().y + rotated.side_steps),
                    static_cast<uint8_t>(target->getPosition().z + rotated.dz)
                };


                if (spawn_multiplier > 1)
                {
                    spawn_count *= (static_cast<int>(spawn_multiplier));
                    // todo : spawn count needs to multiply amount of spawns per success
                    // the problem is the events don't support knowing when the last monster of a specific entry is the one that dies
                    // this could be integrated with the work planned for the memory pools and monsters, or just another unique id for the monsters of a specific entry..
                }

                if (tryMonsterSpawn(monster, spawn_pos, DIRECTION_NORTH, CONST_ME_TELEPORT, false, false))
                {
                    
                    active_creatures.insert({ monster->getID(), entry });
                    co_return;
                }
                else
                {
                    co_await SleepFor { entry->interval * 1000 };
                }
            }
        }
        creature_list.push_back(entry);
    }

    template<>
    CoroTask ZoneManager<SpawnType::Monster, Policy::Fixed>::spawn(std::shared_ptr<SpawnCreature> entry, bool startup)
    {
        auto spawn_count = 1;
        auto sleep_count = 0;

        while (not paused())
        {
            std::cout << "Not paused... \n";

            auto monster_data = SelectMonster(entry->creatures);
            auto monster_type = g_monsters.getMonsterType(monster_data.name);
            auto monster = std::make_shared<Monster>(monster_type);

            if (startup and passive_positional())
            {

                Position spawn_pos = { // todo these static casts look ugly in our logic, lets write appropriate overloads for Position instead
                    static_cast<uint16_t>(startpos.x + entry->position.x),
                    static_cast<uint16_t>(startpos.y + entry->position.y),
                    static_cast<uint8_t>(startpos.z + entry->position.z)
                };

                if (tryMonsterSpawn(monster, spawn_pos, DIRECTION_NORTH, CONST_ME_TELEPORT, false, false))
                {
                    active_creatures.insert({ monster->getID(), entry });
                    co_return;
                }
                else
                {
                    // You will want to log this, spawn is likely failing from bad location...
                    // we may even want to limit the retries or immediately cancel this coroutine all together
                    // there is no point in wasting resources right?
                    co_await SleepFor { entry->interval * 1000 };
                }
            }

             // todo: I'm using random twice here, I think I can combine the calls into a single result and then split the result appropriately using bitshift operations
            if (normal_random(1, 100) < entry->chance)
            {
                auto monster_data = SelectMonster(entry->creatures);
                auto monster_type = g_monsters.getMonsterType(monster_data.name);
                auto monster = std::make_shared<Monster>(monster_type);
                
                Position spawn_pos = {
                    static_cast<uint16_t>(startpos.x + entry->position.x),
                    static_cast<uint16_t>(startpos.y + entry->position.y),
                    static_cast<uint8_t>(startpos.z + entry->position.z)
                };

                if (passive() and forced() and not players.empty())
                {
                    while (sleep_count < 5) // make global config
                    {
                        // send magic effect to position
                        std::cout << "[Spawn] Passive spawn trying to spawn but player is there, sleeping.. \n";
                        co_await SleepFor { delay }; // make global retry time
                        ++sleep_count;
                    }
                }
                else if (passive() and not forced() and not players.empty())
                {
                    // todo : I don't like this recursive call here, I'm certain there is a better solution available for coroutines
                    // I need to expand my understanding of coroutines, and look into alternatives for this sloppy recursive workaround...
                    spawn(entry, false);
                    co_return;
                }

                if (spawn_multiplier > 1)
                {
                    spawn_count *= (static_cast<int>(spawn_multiplier));
                    // todo : spawn count needs to multiply amount of spawns per success
                    // the problem is the events don't support knowing when the last monster of a specific entry is the one that dies
                    // this could be integrated with the work planned for the memory pools and monsters, or just another unique id for the monsters of a specific entry..
                }

                if (tryMonsterSpawn(monster, spawn_pos, DIRECTION_NORTH, CONST_ME_TELEPORT, false, false))
                {
                    monster->setSpawn(this->id);
                    monster->setMasterPos(spawn_pos);
                    active_creatures.insert({ monster->getID(), entry });
                    co_return;
                }
                else
                {
                    // You will want to log this, spawn is likely failing from bad location...
                    // we may even want to limit the retries or immediately cancel this coroutine all together
                    // there is no point in wasting resources right?
                    co_await SleepFor { entry->interval * 1000 };
                }
            }
        }
        creature_list.push_back(entry);
    }

    void System::RegisterSpawnPosition(Position& position, const AnySpawn& spawn)
    {
        std::visit([&](auto&& zone)
            {
                using SpawnZoneType = std::decay_t<decltype(zone)>;
                if constexpr (not std::is_same_v<SpawnZoneType, std::nullptr_t>)
                {
                    auto entry = _spawn_position_registry.find(position);
                    if (entry != _spawn_position_registry.end())
                        entry->second->addSpawn(zone);
                    else
                        _spawn_position_registry.insert({ position, std::make_shared<SpawnZones>(zone) });
                }
            },
            spawn);
    }

    template <SpawnType EntityType, Policy RuleType, SpawnTrigger TriggerType>
    void ZoneManager<EntityType, RuleType, TriggerType>::activate()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        run();
    }

    template <SpawnType EntityType, Policy RuleType, SpawnTrigger TriggerType>
    void ZoneManager<EntityType, RuleType, TriggerType>::deactivate()
    {
        config.reset(static_cast<size_t>(ConfigFlag::Active));
    }

    template <SpawnType EntityType, Policy RuleType, SpawnTrigger TriggerType>
    ParseResult ZoneManager<EntityType, RuleType, TriggerType>::set_range(const toml::v3::table& range_table)
    {
        auto start_str = range_table.get("startpos")->value<std::string>();
        auto end_str = range_table.get("endpos")->value<std::string>();

        uint16_t sx, sy, ex, ey;
        uint8_t sz, ez;

        if (not start_str or not end_str) // todo : we need to validate start pos < end pos
            return { ParseCode::ParseError, "Missing or invalid start or end position" };

        if (auto start_pos = parse_xyz(*start_str))
        {
            sx = start_pos->x;
            sy = start_pos->y;
            sz = start_pos->z;
            startpos = std::move(*start_pos);
        }
        else
        {
            return { ParseCode::InvalidFormat, "Failed to deserialize start position" };
        }

        if (auto end_pos = parse_xyz(*end_str))
        {
            ex = end_pos->x;
            ey = end_pos->y;
            ez = end_pos->z;
            startpos = std::move(*end_pos);
        }
        else
        {
            return { ParseCode::InvalidFormat, "Failed to deserialize end position" };
        }

        for (uint16_t x = std::min(sx, ex); x <= std::max(sx, ex); ++x)
        {
            for (uint16_t y = std::min(sy, ey); y <= std::max(sy, ey); ++y)
            {
                for (uint8_t z = std::min(sz, ez); z <= std::max(sz, ez); ++z)
                {
                    Position pos = Position(x, y, z);
                    System::RegisterSpawnPosition(pos, this->shared_from_this());
                }
            }
        }
        return { ParseCode::Success, "" };
    }

    template <SpawnType EntityType, Policy RuleType, SpawnTrigger TriggerType>
    void ZoneManager<EntityType, RuleType, TriggerType>::process_creatures()
    {
        if (not creature_list.empty())
        {
            for (const auto& entry : creature_list)
            {
                spawn(entry);
            }
            creature_list.clear();
        }
    }

    template <SpawnType EntityType, Policy RuleType, SpawnTrigger TriggerType>
    CoroTask ZoneManager<EntityType, RuleType, TriggerType>::delay_spawn(uint32_t milliseconds, std::shared_ptr<SpawnCreature> entry, bool startup)
    {
        co_await SleepFor(milliseconds);
        spawn(entry, startup);
    }

    template<>
    CoroTask ZoneManager<SpawnType::Monster, Policy::Relative>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
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
                                {
                                    std::cout << "[Spawn] A Player has entered zone \n";
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                    {
                                        if (players.empty())
                                        {
                                            players.push_back(player);
                                            process_creatures();
                                        }
                                        else
                                        {
                                            players.push_back(player);
                                        }
                                    }
                                    break;
                                }

                                case SpawnTrigger::Leave:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    std::cout << "[Spawn] Player left, total = " << players.size() << "\n";

                                    if (players.empty())
                                    {
                                        std::cout << "[Spawn] Last player left spawn zone, deactivating spawning\n";
                                        pause();
                                    }
                                    break;
                                }

                                case SpawnTrigger::Death:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    if (players.empty())
                                    {
                                        std::cout << "[Spawn] Last player left died, deactivating spawning\n";
                                        pause();
                                    }
                                    break;
                                }

                                case SpawnTrigger::Login:
                                {
                                    std::cout << "[Spawn] A Player has logged into a zone \n";
                                    if (std::find(players.begin(), players.end(), player) == players.end())
                                    {
                                        if (players.empty())
                                        {
                                            players.push_back(player);
                                            process_creatures();
                                        }
                                        else
                                        {
                                            players.push_back(player);
                                        }
                                    }
                                    break;
                                }

                                case SpawnTrigger::Logout:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    std::cout << "[Spawn] Player left, total = " << players.size() << "\n";

                                    if (players.empty())
                                    {
                                        std::cout << "[Spawn] Last player left spawn zone, deactivating spawning\n";
                                        pause();
                                    }
                                    break;
                                }

                                default:
                                {
                                    // log this, shouldnt happen
                                }
                            }
                        }
                        else if (const auto& monster = creature->getMonster())
                        {
                            // todo : it might be cool to add some of the other triggers for monster to be forwarded to the dev
                            // they could write their own "walkback" logic, or make the monsters behave however they wish after such triggers
                            if (trigger == SpawnTrigger::Death)
                            {
                                std::cout << "[Spawn] a monster died! yay! \n";
                                auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv)
                                    {
                                        return kv.first == monster->getID();
                                    });
                                if (it != active_creatures.end())
                                {
                                    std::cout << "[Spawn] found monsters entry, restarting it's spawning \n";
                                    auto entry = it->second;
                                    active_creatures.erase(it);

                                    if (not players.empty())
                                        spawn(entry);
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

    template<>
    CoroTask ZoneManager<SpawnType::Monster, Policy::Fixed>::run()
    {
        auto startup = true;
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            if (startup and passive() and players.empty() and not creature_list.empty())
            {
                std::cout << "Starting up a positional spawn \n";
                startup = false;
                for (const auto& entry : creature_list)
                {
                    spawn(entry, true); // todo: move this to proccess_creatures() by forwarding the startup bool
                }
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
                                {
                                    if (not passive())
                                    {
                                        std::cout << "[Spawn] A Player has entered a non-passive positional zone \n";
                                        if (std::find(players.begin(), players.end(), player) == players.end())
                                        {
                                            if (players.empty())
                                            {
                                                players.push_back(player);
                                                process_creatures();
                                            }
                                            else
                                            {
                                                players.push_back(player);
                                            }
                                        }
                                        break;
                                    }
                                    else if (not forced())
                                    {
                                        std::cout << "[Spawn] A Player has entered a passive positional zone which is not forced \n";
                                        pause();
                                        players.push_back(player);
                                        break;
                                    }
                                    else
                                    { // passive but forced
                                        std::cout << "[Spawn] A Player has entered a forced, passive positional zone \n";
                                        players.push_back(player);
                                        break;
                                    }
                                    [[unlikely]]
                                    // log this, shouldn't happen
                                    break;
                                }

                                case SpawnTrigger::Leave:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    if (not passive())
                                    {
                                        std::cout << "[Spawn] Player left a non-passive positional spawn, total = " << players.size() << "\n";

                                        if (players.empty())
                                        {
                                            std::cout << "[Spawn] Last player left non-passive positional zone, deactivating spawning \n";
                                            pause();
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        unpause();
                                        // we need to figure out what to do about these
                                        // should we just delay the spawns to keep people from killing and leaving to trigger instant respawn?
                                        // should we have a continous timer running always to go by?
                                        // if we delay, should the delay be by monster's individual timer, or should we make it per spawn, or both options?
                                        // if we delay, should the delay be by monster's individual timer, or should we make it per spawn, or both options?
                                        if (not creature_list.empty() and players.empty())
                                        {
                                            for (const auto& entry : creature_list)
                                            {
                                                spawn(entry);
                                            }
                                            creature_list.clear();
                                        }
                                        break;
                                    }
                                    break;
                                }

                                case SpawnTrigger::Death:
                                {
                                    // todo : this death doesn't work properly if this positional aka fixed spawn happens to be an "active" or "non-passive" spawn, see leave/logout triggers.
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    if (passive() and players.empty() and not creature_list.empty())
                                    {
                                        unpause();
                                        process_creatures();
                                    }
                                    break;
                                }

                                case SpawnTrigger::Login:
                                {
                                    if (not passive())
                                    {
                                        std::cout << "[Spawn] A Player has entered a non-passive positional zone \n";
                                        if (std::find(players.begin(), players.end(), player) == players.end())
                                        {
                                            if (players.empty())
                                            {
                                                players.push_back(player);
                                                process_creatures();
                                            }
                                            else
                                            {
                                                players.push_back(player);
                                            }
                                        }
                                        break;
                                    }
                                    else if (not forced())
                                    {
                                        std::cout << "[Spawn] A Player has logged into a passive positional zone which is not forced \n";
                                        pause();
                                        players.push_back(player);
                                        break;
                                    }
                                    else
                                    { // passive but forced
                                        std::cout << "[Spawn] A Player has logged into a forced, passive positional zone \n";
                                        players.push_back(player);
                                        break;
                                    }
                                    [[unlikely]]
                                    // log this, shouldn't happen
                                    break;
                                }

                                case SpawnTrigger::Logout:
                                {
                                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                                    if (not passive())
                                    {
                                        std::cout << "[Spawn] Player logged out in a non-passive positional spawn, total = " << players.size() << "\n";

                                        if (players.empty())
                                        {
                                            std::cout << "[Spawn] No more players in non-passive positional zone, deactivating spawning \n";
                                            pause();
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        unpause();
                                        // we need to figure out what to do about these
                                        // should we just delay the spawns to keep people from killing and leaving to trigger instant respawn?
                                        // should we have a continous timer running always to go by?
                                        // if we delay, should the delay be by monster's individual timer, or should we make it per spawn, or both options?
                                        // if we delay, should the delay be by monster's individual timer, or should we make it per spawn, or both options?
                                        if (not creature_list.empty() and players.empty())
                                        {
                                            process_creatures();
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                        else if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                                auto it = std::find_if(active_creatures.begin(), active_creatures.end(), [&](const auto& kv)
                                    {
                                        return kv.first == monster->getID();
                                    });

                                if (it != active_creatures.end())
                                {
                                    auto entry = it->second;
                                    active_creatures.erase(it);

                                    if ((not passive() and not players.empty()) or (passive() and players.empty()))
                                    {
                                        std::cout << "[Spawn] Doing a spawn from monster death in positional spawn... \n";
                                        spawn(entry, true);
                                    }
                                    else if (not passive())
                                    {
                                        std::cout << "[Spawn] Doing a delay_spawn from monster death in positional spawn... \n";
                                        delay_spawn(entry->interval, entry);
                                    }
                                    else
                                    {
                                        std::cout << "[Spawn] Doing a push back to the creature list in positional spawn... \n";
                                        creature_list.push_back(entry);
                                    }
                                    std::cout << "[Spawn] Active Creature Count : " << active_creatures.size() << "\n";
                                    std::cout << "[Spawn] Inactive Creature Count : " << creature_list.size() << "\n";
                                }
                            }
                        }

                        cb();
                    };
                }
            };
        }
    }


    // All the different configurations for spawns, needing their logic defined still

    // staged spawns need rollback option and capabilities
    template<>
    CoroTask ZoneManager<SpawnType::Monster, Policy::Staged>::run()
    {
        while (active())
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
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {

                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {

                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template<>
    CoroTask ZoneManager<SpawnType::Monster, Policy::Triggered>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    // daily, set time, interval
    // make option for boss to continue being active (healing and such) when players leave
    // make the interval the cooldown time
    // add announcer system for notifying those entering (and their party members get special messages if not also entering)
    // ex "The earth started shaking... something stirs in the distance"

    template <>
    CoroTask ZoneManager<SpawnType::Boss, Policy::Fixed>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }
    
    template <>
    CoroTask ZoneManager<SpawnType::Boss, Policy::Relative>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Boss, Policy::Staged>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Boss, Policy::Triggered>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Npc, Policy::Fixed>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Npc, Policy::Relative>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Npc, Policy::Staged>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Npc, Policy::Triggered>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }


    // bigstone:40, boulder:30, stone:30 <-- name:percent == fixed, relative
    // bigstone:40, boulder:30, stone:30 <-- name:health == staged
    // if it's staged we need to provide an event so user can define how to remove the "health"
    // it would likely be beneficial to still store a list of id's for tools that can be used on the item
    template <>
    CoroTask ZoneManager<SpawnType::Item, Policy::Fixed>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Item, Policy::Relative>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Item, Policy::Staged>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    template <>
    CoroTask ZoneManager<SpawnType::Item, Policy::Triggered>::run()
    {
        config.set(static_cast<size_t>(ConfigFlag::Active));
        while (config.test(static_cast<size_t>(ConfigFlag::Active)))
        {
            co_await EventAwaiter {
                [&](auto cb)
                {
                    onTrigger = [&, cb](CreaturePtr creature, SpawnTrigger trigger)
                    {
                        if (const auto& player = creature->getPlayer())
                        {
                            switch (trigger)
                            {
                                case SpawnTrigger::Enter:
                                {
                                }

                                case SpawnTrigger::Leave:
                                {
                                }

                                case SpawnTrigger::Death:
                                {
                                }

                                case SpawnTrigger::Login:
                                {
                                }

                                case SpawnTrigger::Logout:
                                {
                                }

                                default:
                                {
                                }
                            }
                        }
                        if (const auto& monster = creature->getMonster())
                        {
                            if (trigger == SpawnTrigger::Death)
                            {
                            }
                        }
                        cb();
                    };
                }
            };
        }
    }

    void System::RegisterSpawn(uint32_t id, AnySpawn zone)
    {
        _spawn_registry.insert({ id, zone });
    }

    AnySpawn System::GetSpawn(uint32_t id)
    {
        auto it = _spawn_registry.find(id);
        return (it != _spawn_registry.end()) ? it->second : AnySpawn { nullptr };
    }

    std::shared_ptr<SpawnZones> System::GetSpawns(Position position)
    {
        auto it = _spawn_position_registry.find(position);
        if (it != _spawn_position_registry.end())
        {
            return it->second;
        }
        return nullptr;
    }

    std::pair<std::shared_ptr<SpawnCreature>, ParseResult> System::SerializeMonsterEntry(const toml::v3::node& toml_data)
    {
        auto entry = std::make_shared<SpawnCreature>();
        if (auto monster_table = toml_data.as_table())
        {
            entry->chance = monster_table->get("chance")->value_or(100);
            entry->interval = monster_table->get("interval")->value_or(60) * 1000; // todo make global config

            if (auto list_node = monster_table->get("list"))
            {
                if (auto list = list_node->as_array())
                {
                    for (const auto& list_table : *list)
                    {
                        if (const auto& creature_entry = list_table.as_table())
                        {
                            if (auto creature_name = creature_entry->get("name"))
                            {
                                const auto& name = creature_name->value<std::string>();
                                const auto& chance = creature_entry->get("chance")->value_or(100);
                                auto creature_data = name.value() + ":" + std::to_string(chance) + ";";
                                entry->creatures += creature_data;
                            }
                        }
                    }
                }
            }
            else if (auto name_node = monster_table->get("name"))
            {
                entry->creatures += name_node->value<std::string>().value() + ":" + std::to_string(entry->chance) + ";";
            }

            auto position = monster_table->get("position")->value<std::string>();

            if (position.has_value())
            {

                if (auto spawn_pos = parse_xyz(*position))
                {
                    entry->position = std::move(*spawn_pos);
                }
                else
                {
                    return std::make_pair<std::shared_ptr<SpawnCreature>, ParseResult>(nullptr, { ParseCode::InvalidFormat, "Invalid Position data" }); // return {error_code, message}
                }
            }
            else
            {
                // require a position
            }

        }
        return std::make_pair<std::shared_ptr<SpawnCreature>, ParseResult>(std::move(entry), { ParseCode::Success, "" });
    }

    ParseResult System::ParseFixedMonsterSpawn(const toml::table& zone_table)
    {
        try
        {
            auto fixed = zone_table["positional"].value_or(false);
            auto zone = std::make_shared<ZoneManager<SpawnType::Monster, Policy::Fixed>>();
            if (zone_table["passive"].value_or(false)) { zone->passivize(); }

            // monsters
            if (auto monsters_array = zone_table["monsters"].as_array())
            {
                for (const auto& monster_node : *monsters_array)
                {
                    if (auto result = SerializeMonsterEntry(monster_node); result.second.code == ParseCode::Success)
                        zone->creature_list.push_back(std::move(result.first));
                }
            }

            // zone positions (range)
            if (auto range_table = zone_table["range"].as_table())
            {
                if (auto rangeParsed = zone->set_range(*range_table); rangeParsed.code != ParseCode::Success)
                    return rangeParsed;
            }

            System::RegisterSpawn(zone->id, zone);

            zone->run();
            return { ParseCode::Success, "" };
        }
        catch (const std::exception&)
        {
            // todo :: handle exceptions case by case here
            return { ParseCode::InvalidFormat, "Invalid Format" };
        }
    }

    ParseResult System::ParseRelativeMonsterSpawn(const toml::table& zone_table)
    {
        try
        {
            auto interval = 30; // todo make global config
            auto zone = std::make_shared<ZoneManager<SpawnType::Monster, Policy::Relative>>();

            if (auto monsters_array = zone_table["monsters"].as_array())
            {
                for (const auto& monster_node : *monsters_array)
                {
                    if (auto result = SerializeMonsterEntry(monster_node); result.second.code == ParseCode::Success)
                        zone->creature_list.push_back(std::move(result.first));
                }
            }

            if (auto range_table = zone_table["range"].as_table())
            {
                if (auto rangeParsed = zone->set_range(*range_table); rangeParsed.code != ParseCode::Success)
                    return rangeParsed;
            }

            System::RegisterSpawn(zone->id, zone);

            zone->run();
            return { ParseCode::Success, "" };
        }
        catch (const std::exception&)
        {
            // todo :: handle exceptions case by case here
            return { ParseCode::InvalidFormat, "Invalid Format" };
        }
    }

    // always at startup or day change
    ParseResult System::ParseFixedNpcSpawn(const toml::table& zone_table)
    {
        try
        {
            auto interval = 30; // todo make global config
            auto zone = std::make_shared<ZoneManager<SpawnType::Npc, Policy::Fixed>>();

            // npcs
            if (auto npcs_array = zone_table["npcs"].as_array())
            {
                for (const auto& npc_node : *npcs_array)
                {
                   // if (auto result = SerializeMonsterEntry(monster_node); result.second == ParseResult::Success)
                   //     zone->creature_list.push_back(std::move(result.first));
                }
            }

            if (auto range_table = zone_table["range"].as_table())
            {
                if (auto rangeParsed = zone->set_range(*range_table); rangeParsed.code != ParseCode::Success)
                    return rangeParsed;
            }

            System::RegisterSpawn(zone->id, zone);

            zone->run();
            return { ParseCode::Success, "" };
        }
        catch (const std::exception&)
        {
            // todo :: handle exceptions case by case here
            return { ParseCode::InvalidFormat, "Invalid Format" };
        }
    }

    ParseResult System::ParseRelativeNpcSpawn(const toml::table& zone_table)
    {
        try
        {
            auto interval = 30; // todo make global config
            auto zone = std::make_shared<ZoneManager<SpawnType::Monster, Policy::Fixed>>();

            // npcs
            if (auto npcs_array = zone_table["npcs"].as_array())
            {
                for (const auto& npc_node : *npcs_array)
                {
                    //if (auto result = SerializeMonsterEntry(npc_node); result.second == ParseResult::Success)
                    //    zone->creature_list.push_back(std::move(result.first));
                }
            }

            // zone positions (range)
            if (auto range_table = zone_table["range"].as_table())
            {
                if (auto rangeParsed = zone->set_range(*range_table); rangeParsed.code != ParseCode::Success)
                    return rangeParsed;
            }

            System::RegisterSpawn(zone->id, zone);

            zone->run();
            return { ParseCode::Success, "" };
        }
        catch (const std::exception&)
        {
            // todo :: handle exceptions case by case here
            return { ParseCode::InvalidFormat, "Invalid Format" };
        }
    }

    ParseResult System::ParseFile(const std::filesystem::path& file_path)
    {
        std::cout << " PARSING FILE \n";
        try
        {
            toml::table spawn_file = toml::parse_file(file_path.string());

            if (auto fixed_monster_spawn = spawn_file["FixedMonsterSpawn"].as_array())
            {
                for (const auto& entry : *fixed_monster_spawn)
                {
                    if (not entry.is_table())
                    {
                        // log it
                        continue;
                    }
                    std::cout << "FixedMonsterSpawn found \n";
                    const toml::table& spawn = *entry.as_table();
                    auto result = ParseFixedMonsterSpawn(spawn);
                    if (result.code != ParseCode::Success)
                        return result;
                }
                
            }

            if (auto relative_monster_spawn = spawn_file["RelativeMonsterSpawn"].as_array())
            {
                for (const auto& entry : *relative_monster_spawn)
                {
                    if (not entry.is_table())
                    {
                        // log it
                        continue;
                    }
                    std::cout << "RelativeMonsterSpawn found \n";
                    const toml::table& spawn = *entry.as_table();
                    auto result = ParseRelativeMonsterSpawn(spawn);
                    if (result.code != ParseCode::Success)
                        return result;
                }
            }

            if (auto fixed_npc_spawn = spawn_file["FixedNpcSpawn"].as_array())
            {
                for (const auto& entry : *fixed_npc_spawn)
                {
                    if (not entry.is_table())
                    {
                        // log it
                        continue;
                    }
                    std::cout << "FixedNpcSpawn found \n";
                    const toml::table& spawn = *entry.as_table();
                    auto result = ParseFixedNpcSpawn(spawn);
                    if (result.code != ParseCode::Success)
                        return result;
                }
            }

            if (auto relative_npc_spawn = spawn_file["RelativeNpcSpawn"].as_array())
            {
                for (const auto& entry : *relative_npc_spawn)
                {
                    if (not entry.is_table())
                    {
                        // log it
                        continue;
                    }
                    std::cout << "RelativeNpcSpawn found \n";
                    const toml::table& spawn = *entry.as_table();
                    auto result = ParseRelativeNpcSpawn(spawn);
                    if (result.code != ParseCode::Success)
                        return result;
                }
            }

            return { ParseCode::Success, "" };
        }

        // todo :: handle exceptions case by case here
        catch (const toml::parse_error&)
        {
            return { ParseCode::ParseError, "Error reading file" }; 
        }
        catch (const std::filesystem::filesystem_error&)
        {
            return { ParseCode::FileNotFound, "File not found" };
        }
        catch (const std::exception&)
        {
            return { ParseCode::InvalidFormat, "Invalid format" };
        }
    }

    ParseResult System::LoadSpawns(const std::string& directory_path)
    {
        try
        {
            if (not std::filesystem::exists(directory_path))
                return { ParseCode::DirectoryNotFound, "Directory Not found" };

            if (not std::filesystem::is_directory(directory_path))
                return  {ParseCode::DirectoryNotFound, "Directory Not found" };

            // Recursively iterate through all TOML files
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path))
            {
                if (entry.is_regular_file() and entry.path().extension() == ".toml")
                {
                    auto result = ParseFile(entry.path());
                    if (result.code != ParseCode::Success)
                    {
                        // lets log and continue on this one so that the rest the files load still
                        std::cerr << "Failed to parse file: " << entry.path() << " (Error: " << static_cast<int>(result.code) << ")\n";
                        return result; // return {error_code, message}
                    }
                }
            }

            return { ParseCode::Success, "" }; // return {error_code, message}
        }

        // todo :: handle exceptions case by case here
        catch (const toml::parse_error&)
        {
            return { ParseCode::ParseError, "Error reading file" };
        }
        catch (const std::filesystem::filesystem_error&)
        {
            return { ParseCode::FileNotFound, "File not found" };
        }
        catch (const std::exception&)
        {
            return { ParseCode::InvalidFormat, "Invalid format" };
        }
    }

    void System::LoadSpawns()
    {
        std::cout << " LOADING SPAWNS \n";
        auto result = LoadSpawns("./data/world/spawns/");
        if (result.code != ParseCode::Success)
        {
            std::cerr << "Failed to load spawns with error code: " << static_cast<int>(result.code) << std::endl;
        }
    }

    void System::Initialize()
    {

    }

    CoroTask System::Supervise()
    {
        auto now = std::chrono::system_clock::now();
        auto today = std::chrono::floor<std::chrono::days>(now);
        std::chrono::weekday wd { today };

        std::cout << "Today is: " << wd << std::endl;
        // 1 == Monday, Sunday == 7 , 0 == all
        std::cout << "iso_encoding: " << wd.iso_encoding() << std::endl;

        for (uint32_t id = 0; id < zoned_spawns_count; id++)
        {
            auto spawn = Spawns::System::GetSpawn(id);
            std::visit(
                [today](auto&& s)
                {
                    if constexpr (not std::is_null_pointer_v<std::decay_t<decltype(s)>>)
                    {
                        if (s->active_today(today))
                        {
                            if (not s->active())
                                s->run();
                        }
                        else
                        {
                            if (s->active())
                                s->deactivate();
                        }
                    }
                },
                spawn);
        }

        co_return;
    }

} // namespace Spawns