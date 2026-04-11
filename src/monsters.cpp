// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "monsters.h"
#include "monster.h"
#include "spells.h"
#include "combat.h"
#include "weapons.h"
#include "configmanager.h"
#include "game.h"
#include "matrixarea.h"

extern Game g_game;
extern Spells* g_spells;
extern Monsters g_monsters;
extern ConfigManager g_config;

gtl::flat_hash_map<std::string, SkillRegistry> monster_skills;

bool Monsters::addMonsterSkill(std::string monster_name, std::string_view skill_name, const std::shared_ptr<CustomSkill>& skill)
{
	auto& skillMap = monster_skills[monster_name];
	return skillMap.try_emplace(skill_name, skill).second;
}

std::optional<std::shared_ptr<CustomSkill>> Monsters::getMonsterSkill(std::string_view skill_name, std::string monster_name)
{
	if (auto it = monster_skills.find(monster_name); it != monster_skills.end()) 
	{
		const auto& skills = it->second;
		if (auto skillIt = skills.find(skill_name); skillIt != skills.end()) 
		{
			return skillIt->second;
		}
	}
	return std::nullopt;
}

// std::expected candidate <container, bool>
// it would save on the overhead of creating the empty container
// and assigning it later down the line of execution perhaps?
SkillRegistry Monsters::getRegisteredSkills(std::string monster_name)
{
	if (auto it = monster_skills.find(monster_name); it != monster_skills.end())
	{
		return it->second;
	}
	return SkillRegistry();
}

bool MonsterType::loadCallback(LuaScriptInterface* scriptInterface)
{
	int32_t id = scriptInterface->getEvent();
	if (id == -1) {
		std::cout << "[Warning - MonsterType::loadCallback] Event not found. " << std::endl;
		return false;
	}

	info.scriptInterface = scriptInterface;
	if (info.eventType == MONSTERS_EVENT_THINK) {
		info.thinkEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_APPEAR) {
		info.creatureAppearEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_DISAPPEAR) {
		info.creatureDisappearEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_MOVE) {
		info.creatureMoveEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_SAY) {
		info.creatureSayEvent = id;
	}
	return true;
}

MonsterType* Monsters::getMonsterType(const std::string& name, bool loadFromFile /*= true */)
{
	std::string lowerCaseName = asLowerCaseString(name);

	auto it = monsters.find(lowerCaseName);
	if (it == monsters.end()) {
		if (!loadFromFile) {
			return nullptr;
		}

		auto it2 = unloadedMonsters.find(lowerCaseName);
		if (it2 == unloadedMonsters.end()) {
			return nullptr;
		}

		return loadMonster(it2->second, name);
	}
	return &it->second;
}