// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include <toml++/toml.hpp>
#include "otpch.h"

#include "raids.h"

#include "game.h"
#include "configmanager.h"
#include "scheduler.h"
#include "monster.h"
#include "tools.h"

#include <fmt/format.h>
#include <filesystem>

extern Game g_game;
extern ConfigManager g_config;

Raids::Raids()
{
	scriptInterface.initState();
}

Raids::~Raids()
{
	for (Raid* raid : raidList) {
		delete raid;
	}
}

bool Raids::loadFromToml()
{
	if (isLoaded())
	{
		return true;
	}

	for (const auto& file : std::filesystem::recursive_directory_iterator(folder))
	{
		if (not file.is_regular_file() or file.path().extension() != ".toml")
		{
			continue;
		}

		try {
			auto raid_file = toml::parse_file(file.path().string());

			for (const auto& [index, entry] : raid_file)
			{
				if (not entry.is_table())
				{
					std::cerr << "[Error - Raids::loadFromToml] Invalid entry in file: " << file.path() << std::endl;
					continue;
				}

				toml::table raid_data = *entry.as_table();
				
				std::string name = raid_data["name"].value_or("");
				if (name.empty())
				{
					std::cerr << "[Error - Raids::loadFromToml] Name tag missing for raid in file: " << file.path() << std::endl;
					continue;
				}

				uint32_t interval = raid_data["interval"].value_or(0) * 60;
				if (interval == 0)
				{
					std::cerr << "[Error - Raids::loadFromToml] interval tag missing or zero (would divide by 0) for raid: " << name << std::endl;
					continue;
				}

				uint32_t margin = raid_data["margin"].value_or(0) * 60 * 1000;
				bool repeat = raid_data["repeat"].value_or(false);

				auto events = raid_data["events"].as_array();
				if (not events)
				{
					std::cerr << "[Error - Raids::loadFromToml] events array missing for raid: " << name << std::endl;
					continue;
				}

				auto* newRaid = new Raid(name, interval, margin, repeat);
				if (newRaid->loadEvents(*events))
				{
					raidList.push_back(newRaid);
				}
				else
				{
					std::cerr << "[Error - Raids::loadFromToml] Failed to load events for raid: " << name << std::endl;
					delete newRaid;
				}
			}
		}
		catch (const toml::parse_error& err)
		{
			std::cerr << "[Error - Raids::loadFromToml] Error parsing file " << file.path() << ": " << err << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "[Error - Raids::loadFromToml] Failed to load TOML file: " << file.path() << " - " << e.what() << std::endl;
		}
	}

	loaded = true;
	return true;
}

static constexpr int32_t MAX_RAND_RANGE = 10000000;

bool Raids::startup()
{
	if (not isLoaded() or isStarted())
	{
		return false;
	}

	setLastRaidEnd(OTSYS_TIME());

	checkRaidsEvent = g_scheduler.addEvent(createSchedulerTask(CHECK_RAIDS_INTERVAL * 1000, [this]() { checkRaids(); }));

	started = true;
	return started;
}

void Raids::checkRaids()
{
	if (not getRunning())
	{
		uint64_t now = OTSYS_TIME();

		for (auto it = raidList.begin(), end = raidList.end(); it != end; ++it)
		{
			Raid* raid = *it;
			if (now >= (getLastRaidEnd() + raid->getMargin()))
			{
				if (((MAX_RAND_RANGE * CHECK_RAIDS_INTERVAL) / raid->getInterval()) >= static_cast<uint32_t>(uniform_random(0, MAX_RAND_RANGE)))
				{
					setRunning(raid);
					raid->startRaid();

					if (not raid->canBeRepeated())
					{
						raidList.erase(it);
					}
					break;
				}
			}
		}
	}

	checkRaidsEvent = g_scheduler.addEvent(createSchedulerTask(CHECK_RAIDS_INTERVAL * 1000, [this]() { checkRaids(); }));
}

void Raids::clear()
{
	g_scheduler.stopEvent(checkRaidsEvent);
	checkRaidsEvent = 0;

	for (auto* raid : raidList)
	{
		raid->stopEvents();
		delete raid;
	}
	raidList.clear();

	loaded = false;
	started = false;
	running = nullptr;
	lastRaidEnd = 0;

	scriptInterface.reInitState();
}

bool Raids::reload()
{
	clear();
	return loadFromToml();
}

Raid* Raids::getRaidByName(const std::string& name) const
{
	for (auto* raid : raidList)
	{
		if (caseInsensitiveEqual(raid->getName(), name))
		{
			return raid;
		}
	}
	return nullptr;
}

Raid::~Raid()
{
	for (auto* raidEvent : raidEvents)
	{
		delete raidEvent;
	}
}

bool Raid::loadEvents(const toml::array& events)
{
	if (isLoaded())
	{
		return true;
	}

	events.for_each([this](auto&& element)
	{
		if (not element.is_table())
		{
			return;
		}
		
		const toml::table& eventTable = *element.as_table();
		std::string_view eventType = eventTable["type"].value_or("unknown");
		
		RaidEvent* event = nullptr;
		
		if (caseInsensitiveEqual(eventType, "announce"))
		{
			event = new AnnounceEvent();
		}
		else if (caseInsensitiveEqual(eventType, "singlespawn"))
		{
			event = new SingleSpawnEvent();
		}
		else if (caseInsensitiveEqual(eventType, "areaspawn"))
		{
			event = new AreaSpawnEvent();
		}
		else if (caseInsensitiveEqual(eventType, "script"))
		{
			event = new ScriptEvent(&g_game.raids.getScriptInterface());
		}
		else
		{
			std::cerr << "[Error - Raid::loadEvents] Unknown event type: " << eventType << " for raid: " << name << std::endl;
			return;
		}

		if (event->configureRaidEvent(eventTable))
		{
			raidEvents.push_back(event);
		}
		else
		{
			std::cerr << "[Error - Raid::loadEvents] Failed to configure event type: " << eventType << " for raid: " << name << std::endl;
			delete event;
		}
	});

	if (raidEvents.empty())
	{
		std::cerr << "[Error - Raid::loadEvents] No valid events found for raid: " << name << std::endl;
		return false;
	}

	// sort by delay time
	std::sort(raidEvents.begin(), raidEvents.end(), [](const RaidEvent* lhs, const RaidEvent* rhs)
		{
			return lhs->getDelay() < rhs->getDelay();
		}
	);

	loaded = true;
	return true;
}

void Raid::startRaid()
{
	auto* raidEvent = getNextRaidEvent();
	if (raidEvent)
	{
		state = RAIDSTATE_EXECUTING;
		nextEventEvent = g_scheduler.addEvent(createSchedulerTask(raidEvent->getDelay(), [=, this]() { executeRaidEvent(raidEvent); }));
	}
}

void Raid::executeRaidEvent(RaidEvent* raidEvent)
{
	if (raidEvent->executeEvent())
	{
		nextEvent++;
		RaidEvent* newRaidEvent = getNextRaidEvent();

		if (newRaidEvent)
		{
			uint32_t ticks = static_cast<uint32_t>(std::max<int32_t>(RAID_MINTICKS, newRaidEvent->getDelay() - raidEvent->getDelay()));
			nextEventEvent = g_scheduler.addEvent(createSchedulerTask(ticks, [=, this]() { executeRaidEvent(newRaidEvent); }));
		}
		else
		{
			resetRaid();
		}
	}
	else
	{
		resetRaid();
	}
}

void Raid::resetRaid()
{
	nextEvent = 0;
	state = RAIDSTATE_IDLE;
	g_game.raids.setRunning(nullptr);
	g_game.raids.setLastRaidEnd(OTSYS_TIME());
}

void Raid::stopEvents()
{
	if (nextEventEvent != 0)
	{
		g_scheduler.stopEvent(nextEventEvent);
		nextEventEvent = 0;
	}
}

RaidEvent* Raid::getNextRaidEvent() const
{
	if (nextEvent < raidEvents.size())
	{
		return raidEvents[nextEvent];
	}
	else
	{
		return nullptr;
	}
}

bool RaidEvent::configureRaidEvent(const toml::table& eventTable)
{
	auto delayValue = eventTable["delay"].value<uint32_t>();
	if (not delayValue)
	{
		std::cerr << "[Error - RaidEvent::configureRaidEvent] delay tag missing." << std::endl;
		return false;
	}

	delay = std::max<uint32_t>(RAID_MINTICKS, *delayValue);
	return true;
}

bool AnnounceEvent::configureRaidEvent(const toml::table& eventTable)
{
	if (not RaidEvent::configureRaidEvent(eventTable))
	{
		return false;
	}

	auto messageValue = eventTable["message"].value<std::string>();
	if (not messageValue or messageValue->empty())
	{
		std::cerr << "[Error - AnnounceEvent::configureRaidEvent] message tag missing for announce event." << std::endl;
		return false;
	}
	message = *messageValue;

	std::string_view typeValue = eventTable["messagetype"].value_or("event");
	std::string tmpStrValue = asLowerCaseString(std::string(typeValue));
	
	if (tmpStrValue == "warning")
	{
		messageType = MESSAGE_STATUS_WARNING;
	}
	else if (tmpStrValue == "event")
	{
		messageType = MESSAGE_EVENT_ADVANCE;
	}
	else if (tmpStrValue == "default")
	{
		messageType = MESSAGE_EVENT_DEFAULT;
	}
	else if (tmpStrValue == "description")
	{
		messageType = MESSAGE_INFO_DESCR;
	}
	else if (tmpStrValue == "smallstatus")
	{
		messageType = MESSAGE_STATUS_SMALL;
	}
	else if (tmpStrValue == "blueconsole")
	{
		messageType = MESSAGE_STATUS_CONSOLE_BLUE;
	}
	else if (tmpStrValue == "redconsole")
	{
		messageType = MESSAGE_STATUS_CONSOLE_RED;
	}
	else
	{
		std::cout << "[Notice - AnnounceEvent::configureRaidEvent] Unknown message type: " << typeValue << ". Using default: event" << std::endl;
		messageType = MESSAGE_EVENT_ADVANCE;
	}
	
	return true;
}

bool AnnounceEvent::executeEvent()
{
	g_game.broadcastMessage(message, messageType);
	return true;
}

bool SingleSpawnEvent::configureRaidEvent(const toml::table& eventTable)
{
	if (not RaidEvent::configureRaidEvent(eventTable))
	{
		return false;
	}

	auto name_value = eventTable["name"].value<std::string>();
	if (not name_value or name_value->empty())
	{
		std::cerr << "[Error - SingleSpawnEvent::configureRaidEvent] name tag missing for singlespawn event." << std::endl;
		return false;
	}
	monsterName = *name_value;

	if (auto pos_array = eventTable["position"].as_array())
	{
		if (pos_array->size() >= 3)
		{
			position.x = pos_array->get(0)->value_or(0);
			position.y = pos_array->get(1)->value_or(0);
			position.z = pos_array->get(2)->value_or(0);
		}
		else
		{
			std::cerr << "[Error - SingleSpawnEvent::configureRaidEvent] position array must have at least 3 elements." << std::endl;
			return false;
		}
	}
	else
	{
		auto x_value = eventTable["x"].value<uint16_t>();
		auto y_value = eventTable["y"].value<uint16_t>();
		auto z_value = eventTable["z"].value<uint16_t>();

		if (not x_value)
		{
			std::cerr << "[Error - SingleSpawnEvent::configureRaidEvent] x tag missing for singlespawn event." << std::endl;
			return false;
		}
		if (not y_value)
		{
			std::cerr << "[Error - SingleSpawnEvent::configureRaidEvent] y tag missing for singlespawn event." << std::endl;
			return false;
		}
		if (not z_value)
		{
			std::cerr << "[Error - SingleSpawnEvent::configureRaidEvent] z tag missing for singlespawn event." << std::endl;
			return false;
		}

		position.x = *x_value;
		position.y = *y_value;
		position.z = *z_value;
	}

	return true;
}

bool SingleSpawnEvent::executeEvent()
{
	auto monster = Monster::createMonster(monsterName);
	if (not monster)
	{
		std::cerr << "[Error - SingleSpawnEvent::executeEvent] Can't create monster " << monsterName << std::endl;
		return false;
	}

	if (not g_game.placeCreature(std::move(monster), position, false, true))
	{
		std::cerr << "[Error - SingleSpawnEvent::executeEvent] Can't place monster " << monsterName << std::endl;
		return false;
	}
	return true;
}

bool AreaSpawnEvent::configureRaidEvent(const toml::table& eventTable)
{
	if (not RaidEvent::configureRaidEvent(eventTable))
	{
		return false;
	}

	auto radiusValue = eventTable["radius"].value<int32_t>();
	if (radiusValue)
	{
		int32_t radius = *radiusValue;
		Position center_pos;

		if (auto center_array = eventTable["center"].as_array())
		{
			if (center_array->size() >= 3)
			{
				center_pos.x = center_array->get(0)->value_or(0);
				center_pos.y = center_array->get(1)->value_or(0);
				center_pos.z = center_array->get(2)->value_or(0);
			}
			else
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] center array must have at least 3 elements." << std::endl;
				return false;
			}
		}
		else
		{
			auto center_x = eventTable["centerx"].value<uint16_t>();
			auto center_y = eventTable["centery"].value<uint16_t>();
			auto center_z = eventTable["centerz"].value<uint16_t>();

			if (not center_x)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] centerx tag missing for areaspawn event." << std::endl;
				return false;
			}
			if (not center_y)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] centery tag missing for areaspawn event." << std::endl;
				return false;
			}
			if (not center_z)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] centerz tag missing for areaspawn event." << std::endl;
				return false;
			}

			center_pos.x = *center_x;
			center_pos.y = *center_y;
			center_pos.z = *center_z;
		}

		fromPos.x = std::max<int32_t>(0, center_pos.getX() - radius);
		fromPos.y = std::max<int32_t>(0, center_pos.getY() - radius);
		fromPos.z = center_pos.z;

		toPos.x = std::min<int32_t>(0xFFFF, center_pos.getX() + radius);
		toPos.y = std::min<int32_t>(0xFFFF, center_pos.getY() + radius);
		toPos.z = center_pos.z;
	}
	else
	{
		if (auto from_array = eventTable["from"].as_array())
		{
			if (from_array->size() >= 3)
			{
				fromPos.x = from_array->get(0)->value_or(0);
				fromPos.y = from_array->get(1)->value_or(0);
				fromPos.z = from_array->get(2)->value_or(0);
			}
			else
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] from array must have at least 3 elements." << std::endl;
				return false;
			}
		}
		else
		{
			auto from_x = eventTable["fromx"].value<uint16_t>();
			auto from_y = eventTable["fromy"].value<uint16_t>();
			auto from_z = eventTable["fromz"].value<uint16_t>();

			if (not from_x)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] fromx tag missing for areaspawn event." << std::endl;
				return false;
			}
			if (not from_y)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] fromy tag missing for areaspawn event." << std::endl;
				return false;
			}
			if (not from_z)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] fromz tag missing for areaspawn event." << std::endl;
				return false;
			}

			fromPos.x = *from_x;
			fromPos.y = *from_y;
			fromPos.z = *from_z;
		}

		if (auto to_array = eventTable["to"].as_array())
		{
			if (to_array->size() >= 3)
			{
				toPos.x = to_array->get(0)->value_or(0);
				toPos.y = to_array->get(1)->value_or(0);
				toPos.z = to_array->get(2)->value_or(0);
			}
			else
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] to array must have at least 3 elements." << std::endl;
				return false;
			}
		}
		else
		{
			auto to_x = eventTable["tox"].value<uint16_t>();
			auto to_y = eventTable["toy"].value<uint16_t>();
			auto to_z = eventTable["toz"].value<uint16_t>();

			if (not to_x)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] tox tag missing for areaspawn event." << std::endl;
				return false;
			}
			if (not to_y)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] toy tag missing for areaspawn event." << std::endl;
				return false;
			}
			if (not to_z)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] toz tag missing for areaspawn event." << std::endl;
				return false;
			}

			toPos.x = *to_x;
			toPos.y = *to_y;
			toPos.z = *to_z;
		}
	}

	auto monsters = eventTable["monsters"].as_array();
	if (not monsters)
	{
		std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] monsters array missing for areaspawn event." << std::endl;
		return false;
	}

	monsters->for_each([this](auto&& element)
	{
		if (not element.is_table())
		{
			return;
		}
		
		const toml::table& monster_table = *element.as_table();
		
		auto name_value = monster_table["name"].value<std::string>();
		if (not name_value or name_value->empty())
		{
			std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] name tag missing for monster node." << std::endl;
			return;
		}

		uint32_t min_amount = monster_table["minamount"].value_or(0);
		uint32_t max_amount = monster_table["maxamount"].value_or(0);

		if (max_amount == 0 and min_amount == 0)
		{
			auto amount = monster_table["amount"].value<uint32_t>();
			if (not amount)
			{
				std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] amount tag missing for monster node." << std::endl;
				return;
			}
			min_amount = *amount;
			max_amount = min_amount;
		}

		spawnList.emplace_back(*name_value, min_amount, max_amount);
	});

	if (spawnList.empty())
	{
		std::cerr << "[Error - AreaSpawnEvent::configureRaidEvent] No valid monsters found for areaspawn event." << std::endl;
		return false;
	}

	return true;
}

bool AreaSpawnEvent::executeEvent()
{
	for (const auto& spawn : spawnList)
	{
		uint32_t amount = uniform_random(spawn.minAmount, spawn.maxAmount);
		for (uint32_t i = 0; i < amount; ++i)
		{
			auto monster = Monster::createMonster(spawn.name);
			if (not monster)
			{
				std::cerr << "[Error - AreaSpawnEvent::executeEvent] Can't create monster " << spawn.name << std::endl;
				return false;
			}

			bool success = false;
			for (int32_t tries = 0; tries < MAXIMUM_TRIES_PER_MONSTER; tries++)
			{
				const auto& tile = g_game.map.getTile(uniform_random(fromPos.x, toPos.x), uniform_random(fromPos.y, toPos.y), uniform_random(fromPos.z, toPos.z));
				if (tile and not tile->isMoveableBlocking() and not tile->hasFlag(TILESTATE_PROTECTIONZONE) and tile->getTopCreature() == nullptr and g_game.placeCreature(monster, tile->getPosition(), false, true))
				{
					success = true;
					break;
				}
			}

			if (not success)
			{
				monster.reset();
			}
		}
	}
	return true;
}

bool ScriptEvent::configureRaidEvent(const toml::table& eventTable)
{
	if (not RaidEvent::configureRaidEvent(eventTable))
	{
		return false;
	}

	auto scriptValue = eventTable["script"].value<std::string>();
	if (not scriptValue or scriptValue->empty())
	{
		std::cerr << "[Error - ScriptEvent::configureRaidEvent] No script file found for raid" << std::endl;
		return false;
	}

	if (not loadScript("data/raids/scripts/" + *scriptValue))
	{
		std::cerr << "[Error - ScriptEvent::configureRaidEvent] Can not load raid script: " << *scriptValue << std::endl;
		return false;
	}
	return true;
}

bool ScriptEvent::executeEvent()
{
	//onRaid()
	if (not scriptInterface->reserveScriptEnv())
	{
		std::cerr << "[Error - ScriptEvent::executeEvent] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	scriptInterface->pushFunction(scriptId);

	return scriptInterface->callFunction(0);
}
