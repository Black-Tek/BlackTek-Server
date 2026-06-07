// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "monsters.h"
#include "monster.h"
#include "spells.h"
#include "combat.h"
#include "configmanager.h"
#include "game.h"
#include "items.h"
#include "condition.h"
#include "script.h"

extern Game g_game;
extern Spells* g_spells;
extern Monsters g_monsters;
extern ConfigManager g_config;
extern Scripts* g_scripts;

gtl::flat_hash_map<std::string, SkillRegistry> monster_skills;

spellBlock_t::~spellBlock_t()
{
	if (combatSpell)
	{
		delete spell;
	}
}

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
	if (id == -1)
	{
		std::cout << "[Warning - MonsterType::loadCallback] Event not found. " << std::endl;
		return false;
	}

	info.scriptInterface = scriptInterface;
	if (info.eventType == MONSTERS_EVENT_THINK)
	{
		info.thinkEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_APPEAR)
	{
		info.creatureAppearEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_DISAPPEAR)
	{
		info.creatureDisappearEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_MOVE)
	{
		info.creatureMoveEvent = id;
	}
	else if (info.eventType == MONSTERS_EVENT_SAY)
	{
		info.creatureSayEvent = id;
	}
	return true;
}

void MonsterType::loadLoot(LootBlock lootBlock)
{
	if (lootBlock.childLoot.empty())
	{
		auto it = std::find_if(info.lootItems.begin(), info.lootItems.end(),
			[&lootBlock](const LootBlock& block) { return block.id == lootBlock.id; });

		if (it == info.lootItems.end())
		{
			info.lootItems.push_back(std::move(lootBlock));
		}
	}
	else
	{
		info.lootItems.push_back(std::move(lootBlock));
	}
}

MonsterType* Monsters::getMonsterType(const std::string& name, bool loadFromFile /*= true */)
{
	std::string lowerCaseName = asLowerCaseString(name);

	auto it = monsters.find(lowerCaseName);
	if (it == monsters.end())
	{
		return nullptr;
	}
	return &it->second;
}

bool Monsters::reload()
{
	loaded = false;
	monsters.clear();
	return g_scripts->loadScripts("monster", false, true);
}

static RaceType_t parseRaceType(std::string_view str)
{
	if (str == "venom")  return RACE_VENOM;
	if (str == "blood")  return RACE_BLOOD;
	if (str == "undead") return RACE_UNDEAD;
	if (str == "fire")   return RACE_FIRE;
	if (str == "energy") return RACE_ENERGY;
	return RACE_BLOOD;
}

static CombatType_t parseCombatType(std::string_view str)
{
	if (str == "physical")                    return COMBAT_PHYSICALDAMAGE;
	if (str == "energy")                      return COMBAT_ENERGYDAMAGE;
	if (str == "fire")                        return COMBAT_FIREDAMAGE;
	if (str == "earth" or str == "poison")    return COMBAT_EARTHDAMAGE;
	if (str == "drown")                       return COMBAT_DROWNDAMAGE;
	if (str == "ice")                         return COMBAT_ICEDAMAGE;
	if (str == "holy")                        return COMBAT_HOLYDAMAGE;
	if (str == "death")                       return COMBAT_DEATHDAMAGE;
	if (str == "lifedrain")                   return COMBAT_LIFEDRAIN;
	if (str == "manadrain")                   return COMBAT_MANADRAIN;
	if (str == "healing")                     return COMBAT_HEALING;
	return COMBAT_UNDEFINEDDAMAGE;
}

static ConditionType_t parseConditionType(std::string_view str)
{
	if (str == "physical" or str == "bleed") return CONDITION_BLEEDING;
	if (str == "energy")                     return CONDITION_ENERGY;
	if (str == "fire")                       return CONDITION_FIRE;
	if (str == "earth" or str == "poison")   return CONDITION_POISON;
	if (str == "drown")                      return CONDITION_DROWN;
	if (str == "ice")                        return CONDITION_FREEZING;
	if (str == "holy")                       return CONDITION_DAZZLED;
	if (str == "death")                      return CONDITION_CURSED;
	if (str == "paralyze")                   return CONDITION_PARALYZE;
	if (str == "outfit")                     return CONDITION_OUTFIT;
	if (str == "drunk")                      return CONDITION_DRUNK;
	if (str == "invisible")                  return CONDITION_INVISIBLE;
	if (str == "bleeding")                   return CONDITION_BLEEDING;
	return CONDITION_NONE;
}

ConditionHandle Monsters::getDamageCondition(ConditionType_t conditionType,
    int32_t maxDamage, int32_t minDamage, int32_t startDamage, uint32_t tickInterval)
{
	ConditionHandle condition{ new ConditionDamage(CONDITIONID_COMBAT, conditionType) };
	condition->setParam(CONDITION_PARAM_TICKS, -1);
	condition->setParam(CONDITION_PARAM_MINVALUE, minDamage);
	condition->setParam(CONDITION_PARAM_MAXVALUE, maxDamage);
	condition->setParam(CONDITION_PARAM_STARTVALUE, startDamage);
	condition->setParam(CONDITION_PARAM_TICKINTERVAL, static_cast<int32_t>(tickInterval));
	condition->setParam(CONDITION_PARAM_FORCEUPDATE, 1);
	return condition;
}

bool Monsters::deserializeSpell(MonsterSpell* spell, spellBlock_t& sb, const std::string& description)
{
	sb.speed           = spell->interval;
	sb.chance          = spell->chance;
	sb.range           = spell->range;
	sb.minCombatValue  = spell->minCombatValue;
	sb.maxCombatValue  = spell->maxCombatValue;
	sb.combatSpell     = false;
	sb.isMelee         = false;

	if (not spell->scriptName.empty())
	{
		InstantSpell* instantSpell = g_spells->getInstantSpellByName(spell->scriptName);
		if (instantSpell)
		{
			sb.spell = instantSpell;
			return true;
		}

		sb.combatSpell = true;
		auto combat = BlackTek::g_combat_registry.Create();
		auto combatSpell = new CombatSpell(combat, spell->needTarget, spell->needDirection);
		if (not combatSpell->loadScriptCombat())
		{
			std::cerr << "[Warning - Monsters::deserializeSpell] Could not load scripted spell '" << spell->scriptName << "' for '" << description << "'\n";
			delete combatSpell;
			return false;
		}
		sb.spell = combatSpell;
		return true;
	}

	if (spell->name.empty())
	{
		return false;
	}

	InstantSpell* instantSpell = g_spells->getInstantSpellByName(spell->name);
	if (instantSpell)
	{
		sb.spell = instantSpell;
		return true;
	}

	sb.combatSpell = true;
	auto combat = BlackTek::g_combat_registry.Create();
	auto combatSpell = new CombatSpell(combat, spell->needTarget, spell->needDirection);

	if (spell->name == "melee")
	{
		sb.isMelee = true;
		sb.range = 1;
		combat->SetConfig(BlackTek::Combat::Config::Aggressive);
		combat->SetConfig(BlackTek::Combat::Config::BlockedByArmor);
		combat->SetConfig(BlackTek::Combat::Config::BlockedByDefense);
		combat->SetDamageType(static_cast<uint16_t>(BlackTek::Combat::DamageType::Physical));
		if (spell->conditionType != CONDITION_NONE)
		{
			uint32_t tickInterval = 2000;
			if (spell->tickInterval != 0)
				tickInterval = static_cast<uint32_t>(spell->tickInterval);

			if (auto condition = getDamageCondition(spell->conditionType,
				spell->conditionMaxDamage, spell->conditionMinDamage,
				spell->conditionStartDamage, tickInterval))
			{
				combatSpell->getCombat()->AddCondition(std::move(condition));
			}
		}
	}
	else if (spell->name == "speed" or spell->name == "paralyze" or spell->name == "haste")
	{
		// Speed-change spell — condition setup deferred
	}
	else if (spell->name == "outfit" or spell->name == "invisible" or spell->name == "drunk"
	         or spell->name == "firefield" or spell->name == "poisonfield"
	         or spell->name == "energyfield" or spell->name == "condition")
	{
		// Utility spells — condition or field setup deferred
	}
	else
	{
		CombatType_t combatType = spell->combatType;
		if (combatType == COMBAT_UNDEFINEDDAMAGE)
			combatType = parseCombatType(spell->name);

		combat->SetDamageType(static_cast<uint16_t>(combatType));
		combat->SetConfig(BlackTek::Combat::Config::Aggressive);
	}

	sb.spell = combatSpell;
	return true;
}
