// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include <toml++/toml.hpp>

#include "monsters.h"
#include "monster.h"
#include "spells.h"
#include "combat.h"
#include "configmanager.h"
#include "game.h"
#include "items.h"
#include "condition.h"

extern Game g_game;
extern Spells* g_spells;
extern Monsters g_monsters;
extern ConfigManager g_config;

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
	return loadFromToml(true);
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

ConditionDamage* Monsters::getDamageCondition(ConditionType_t conditionType,
    int32_t maxDamage, int32_t minDamage, int32_t startDamage, uint32_t tickInterval)
{
	auto condition = new ConditionDamage(CONDITIONID_COMBAT, conditionType);
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
		auto combat = std::make_shared<BlackTek::Combat>();
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
	auto combat = std::make_shared<BlackTek::Combat>();
	auto combatSpell = new CombatSpell(combat, spell->needTarget, spell->needDirection);

	if (spell->name == "melee")
	{
		sb.isMelee = true;
		if (spell->conditionType != CONDITION_NONE)
		{
			uint32_t tickInterval = 2000;
			if (spell->tickInterval != 0)
			{
				tickInterval = static_cast<uint32_t>(spell->tickInterval);
			}
			ConditionDamage* condition = getDamageCondition(spell->conditionType,
				spell->conditionMaxDamage, spell->conditionMinDamage,
				spell->conditionStartDamage, tickInterval);
			/// combatSpell->getCombat()->addCondition(condition);
			delete condition;
		}
	}
	else if (spell->name == "speed" or spell->name == "paralyze" or spell->name == "haste")
	{
		// Speed-change spell → condition_paralyze or condition_haste
		/// Speed condition setup deferred — combat API not yet finalised
	}
	else if (spell->name == "outfit" or spell->name == "invisible" or spell->name == "drunk"
	         or spell->name == "firefield" or spell->name == "poisonfield"
	         or spell->name == "energyfield" or spell->name == "condition")
	{
		// Utility spells — condition or field setup deferred
	}
	else
	{
		// Damage spell — type determined by spell name or spell->combatType
		CombatType_t combatType = spell->combatType;
		if (combatType == COMBAT_UNDEFINEDDAMAGE)
		{
			combatType = parseCombatType(spell->name);
		}
		/// combat->setType(combatType);  // deferred until combat API stabilises
	}

	sb.spell = combatSpell;
	return true;
}

LootBlock Monsters::parseLootItem(const toml::table& lootTable)
{
	LootBlock lootBlock;

	std::string_view itemName = lootTable["name"].value_or<std::string_view>("");
	if (not itemName.empty())
	{
		lootBlock.id = Item::items.getItemIdByName(std::string(itemName));
	}
	else
	{
		lootBlock.id = lootTable["id"].value_or<uint16_t>(0);
	}

	lootBlock.chance   = lootTable["chance"].value_or<uint32_t>(0);
	lootBlock.countmin = lootTable["countmin"].value_or<uint16_t>(1);
	lootBlock.countmax = lootTable["countmax"].value_or<uint16_t>(
	    lootTable["count"].value_or<uint16_t>(1)
	);
	lootBlock.subType  = lootTable["subType"].value_or<int32_t>(-1);
	lootBlock.actionId = lootTable["actionId"].value_or<int32_t>(-1);
	lootBlock.text     = lootTable["text"].value_or<std::string>("");
	lootBlock.unique   = lootTable["unique"].value_or<bool>(false);

	if (auto children = lootTable["child"].as_array())
	{
		children->for_each([&lootBlock, this](auto&& child)
		{
			if (child.is_table())
			{
				lootBlock.childLoot.push_back(parseLootItem(*child.as_table()));
			}
		});
	}

	return lootBlock;
}

MonsterType* Monsters::loadMonsterFromToml(const toml::table& data, const std::string& monsterKey, bool reloading)
{
	std::string name = data["name"].value_or<std::string>("");
	if (name.empty())
	{
		name = monsterKey;

		if (not name.empty())
		{
			name[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(name[0])));
		}
	}

	std::string lowerName = asLowerCaseString(name);

	MonsterType* mType = nullptr;
	if (reloading)
	{
		auto it = monsters.find(lowerName);
		if (it != monsters.end())
		{
			mType = &it->second;
			mType->info.lootItems.clear();
			mType->info.attackSpells.clear();
			mType->info.defenseSpells.clear();
			mType->info.scripts.clear();
			mType->info.summons.clear();
			mType->info.voiceVector.clear();
			mType->info.elementMap.clear();
			mType->info.thinkEvent = -1;
			mType->info.creatureAppearEvent = -1;
			mType->info.creatureDisappearEvent = -1;
			mType->info.creatureMoveEvent = -1;
			mType->info.creatureSayEvent = -1;
		}
	}

	if (not mType)
	{
		mType = &monsters[lowerName];
	}

	mType->name									= name;
	mType->registeredName						= lowerName;
	mType->nameDescription						= data["nameDescription"].value_or<std::string>("a " + name);
	mType->info.experience						= data["experience"].value_or<uint64_t>(0);
	mType->info.baseSpeed						= data["speed"].value_or<uint32_t>(200);
	mType->info.health							= static_cast<int32_t>(data["health"].value_or<int64_t>(mType->info.health));
	mType->info.healthMax						= static_cast<int32_t>(data["maxHealth"].value_or<int64_t>(mType->info.health));
	mType->info.manaCost						= data["manaCost"].value_or<uint32_t>(0);
	mType->info.race							= parseRaceType(data["race"].value_or<std::string_view>("blood"));
	mType->info.lookcorpse						= data["lookCorpse"].value_or<uint16_t>(0);
	mType->info.outfit.lookType					= data["lookType"].value_or<uint16_t>(0);
	mType->info.outfit.lookTypeEx				= data["lookTypeEx"].value_or<uint16_t>(0);
	mType->info.outfit.lookHead					= data["lookHead"].value_or<uint8_t>(0);
	mType->info.outfit.lookBody					= data["lookBody"].value_or<uint8_t>(0);
	mType->info.outfit.lookLegs					= data["lookLegs"].value_or<uint8_t>(0);
	mType->info.outfit.lookFeet					= data["lookFeet"].value_or<uint8_t>(0);
	mType->info.outfit.lookAddons				= data["lookAddons"].value_or<uint8_t>(0);
	mType->info.outfit.lookMount				= data["lookMount"].value_or<uint16_t>(0);
	mType->info.light.level						= data["lightLevel"].value_or<uint8_t>(0);
	mType->info.light.color						= data["lightColor"].value_or<uint8_t>(0);
	mType->info.defense							= data["defense"].value_or<int32_t>(0);
	mType->info.armor							= data["armor"].value_or<int32_t>(0);
	mType->info.targetDistance					= data["targetDistance"].value_or<int32_t>(1);
	mType->info.staticAttackChance				= data["staticAttack"].value_or<uint32_t>(95);
	mType->info.runAwayHealth					= data["runOnHealth"].value_or<int32_t>(0);
	mType->info.changeTargetSpeed				= data["targetInterval"].value_or<uint32_t>(0);
	mType->info.changeTargetChance				= data["targetChance"].value_or<int32_t>(0);
	mType->info.maxSummons						= data["maxSummons"].value_or<uint32_t>(0);
	mType->info.defense_charge_interval         = data["defenseChargeInterval"].value_or<uint32_t>(0);
	mType->info.defense_charges_cap             = data["defenseChargesCap"].value_or<uint32_t>(0);
	mType->info.armor_charges_cap               = data["armorChargesCap"].value_or<uint32_t>(0);
	mType->info.defense_charge_cost_multiplier  = data["defenseChargeCostMultiplier"].value_or<float>(1.0f);
	mType->info.armor_charge_cost_multiplier    = data["armorChargeCostMultiplier"].value_or<float>(1.0f);
	mType->info.yellSpeedTicks					= data["yellInterval"].value_or<uint32_t>(0);
	mType->info.yellChance						= data["yellChance"].value_or<uint32_t>(0);
	std::string_view skullStr					= data["skull"].value_or<std::string_view>("");
	mType->info.isSummonable					= data["summonable"].value_or<bool>(false);
	mType->info.isAttackable					= data["attackable"].value_or<bool>(true);
	mType->info.isHostile						= data["hostile"].value_or<bool>(true);
	mType->info.isConvinceable					= data["convinceable"].value_or<bool>(false);
	mType->info.isIllusionable					= data["illusionable"].value_or<bool>(false);
	mType->info.pushable						= data["pushable"].value_or<bool>(true);
	mType->info.canPushItems					= data["canPushItems"].value_or<bool>(false);
	mType->info.canPushCreatures				= data["canPushCreatures"].value_or<bool>(false);
	mType->info.isBoss							= data["isBoss"].value_or<bool>(false);
	mType->info.isRewardBoss					= data["isRewardBoss"].value_or<bool>(false);
	mType->info.isChallengeable					= data["isChallengeable"].value_or<bool>(true);
	mType->info.isIgnoringSpawnBlock			= data["ignoreSpawnBlock"].value_or<bool>(false);
	mType->info.hiddenHealth					= data["hiddenHealth"].value_or<bool>(false);
	mType->info.canWalkOnEnergy					= data["canWalkOnEnergy"].value_or<bool>(true);
	mType->info.canWalkOnFire					= data["canWalkOnFire"].value_or<bool>(true);
	mType->info.canWalkOnPoison					= data["canWalkOnPoison"].value_or<bool>(true);

	if (not skullStr.empty())
		mType->info.skull = getSkullType(std::string(skullStr));

	if (mType->info.canPushCreatures)
		mType->info.pushable = false;

	if (auto combatImm = data["combatImmunities"].as_array())
	{
		combatImm->for_each([mType](auto&& elem)
		{
			if (elem.is_string())
			{
				std::string_view immunity = elem.value_or<std::string_view>("");
				if (immunity == "physical")               mType->info.damageImmunities |= COMBAT_PHYSICALDAMAGE;
				else if (immunity == "energy")            mType->info.damageImmunities |= COMBAT_ENERGYDAMAGE;
				else if (immunity == "fire")              mType->info.damageImmunities |= COMBAT_FIREDAMAGE;
				else if (immunity == "earth" or immunity == "poison") mType->info.damageImmunities |= COMBAT_EARTHDAMAGE;
				else if (immunity == "drown")             mType->info.damageImmunities |= COMBAT_DROWNDAMAGE;
				else if (immunity == "ice")               mType->info.damageImmunities |= COMBAT_ICEDAMAGE;
				else if (immunity == "holy")              mType->info.damageImmunities |= COMBAT_HOLYDAMAGE;
				else if (immunity == "death")             mType->info.damageImmunities |= COMBAT_DEATHDAMAGE;
				else if (immunity == "lifedrain")         mType->info.damageImmunities |= COMBAT_LIFEDRAIN;
				else if (immunity == "manadrain")         mType->info.damageImmunities |= COMBAT_MANADRAIN;
				else
				{
					std::cerr << "[Warning - Monsters] Unknown combat immunity '" << immunity << "' for monster '" << mType->name << "'\n";
				}
			}
		});
	}

	if (auto condImm = data["conditionImmunities"].as_array())
	{
		condImm->for_each([mType](auto&& elem)
		{
			if (elem.is_string())
			{
				std::string_view immunity = elem.value_or<std::string_view>("");
				if (immunity == "physical" or immunity == "bleed") mType->info.conditionImmunities |= CONDITION_BLEEDING;
				else if (immunity == "energy")                     mType->info.conditionImmunities |= CONDITION_ENERGY;
				else if (immunity == "fire")                       mType->info.conditionImmunities |= CONDITION_FIRE;
				else if (immunity == "earth" or immunity == "poison") mType->info.conditionImmunities |= CONDITION_POISON;
				else if (immunity == "drown")                      mType->info.conditionImmunities |= CONDITION_DROWN;
				else if (immunity == "ice")                        mType->info.conditionImmunities |= CONDITION_FREEZING;
				else if (immunity == "holy")                       mType->info.conditionImmunities |= CONDITION_DAZZLED;
				else if (immunity == "death")                      mType->info.conditionImmunities |= CONDITION_CURSED;
				else if (immunity == "paralyze")                   mType->info.conditionImmunities |= CONDITION_PARALYZE;
				else if (immunity == "outfit")                     mType->info.conditionImmunities |= CONDITION_OUTFIT;
				else if (immunity == "drunk")                      mType->info.conditionImmunities |= CONDITION_DRUNK;
				else if (immunity == "invisible" or immunity == "invisibility") mType->info.conditionImmunities |= CONDITION_INVISIBLE;
				else
				{
					std::cerr << "[Warning - Monsters] Unknown condition immunity '" << immunity << "' for monster '" << mType->name << "'\n";
				}
			}
		});
	}

	if (auto elements = data["elements"].as_array())
	{
		elements->for_each([mType](auto&& elem)
		{
			if (not elem.is_table()) return;
			auto& tbl = *elem.as_table();
			std::string_view typeName = tbl["type"].value_or<std::string_view>("");
			int32_t percent = tbl["percent"].value_or<int32_t>(0);
			CombatType_t combatType = parseCombatType(typeName);
			if (combatType != COMBAT_UNDEFINEDDAMAGE)
			{
				mType->info.elementMap[combatType] = percent;
			}
		});
	}

	if (auto voices = data["voices"].as_array())
	{
		voices->for_each([mType](auto&& elem)
		{
			if (elem.is_string())
			{
				voiceBlock_t voice;
				voice.text     = elem.value_or<std::string>("");
				voice.yellText = false;
				mType->info.voiceVector.push_back(std::move(voice));
			}
			else if (elem.is_table())
			{
				auto& tbl = *elem.as_table();
				voiceBlock_t voice;
				voice.text     = tbl["text"].value_or<std::string>("");
				voice.yellText = tbl["yell"].value_or<bool>(false);
				mType->info.voiceVector.push_back(std::move(voice));
			}
		});
	}

	if (auto summons = data["summons"].as_array())
	{
		summons->for_each([mType](auto&& elem)
		{
			if (not elem.is_table()) return;
			auto& tbl = *elem.as_table();
			summonBlock_t summon;
			summon.name							= tbl["name"].value_or<std::string>("");
			summon.chance						= tbl["chance"].value_or<uint32_t>(100);
			summon.speed						= tbl["interval"].value_or<uint32_t>(2000);
			summon.max							= tbl["max"].value_or<uint32_t>(0);
			summon.force						= tbl["force"].value_or<bool>(false);
			std::string_view effectStr			= tbl["effect"].value_or<std::string_view>("");
			std::string_view masterEffectStr	= tbl["masterEffect"].value_or<std::string_view>("");
			summon.effect       = effectStr.empty()       ? CONST_ME_TELEPORT : getMagicEffect(std::string(effectStr));
			summon.masterEffect = masterEffectStr.empty() ? CONST_ME_NONE     : getMagicEffect(std::string(masterEffectStr));
			mType->info.summons.push_back(std::move(summon));
		});
	}

	if (auto loot = data["loot"].as_array())
	{
		loot->for_each([mType, this](auto&& elem)
		{
			if (not elem.is_table()) return;
			LootBlock lootBlock = parseLootItem(*elem.as_table());
			if (lootBlock.id != 0)
			{
				mType->loadLoot(std::move(lootBlock));
			}
		});
	}

	auto parseSpellTable = [mType, this](const toml::table& tbl, bool isDefense)
	{
		MonsterSpell spell;
		spell.name								= tbl["name"].value_or<std::string>("");
		spell.interval							= tbl["interval"].value_or<uint16_t>(2000);
		spell.chance							= tbl["chance"].value_or<uint8_t>(100);
		spell.range								= tbl["range"].value_or<uint8_t>(0);
		spell.minCombatValue					= tbl["min"].value_or<int32_t>(0);
		spell.maxCombatValue					= tbl["max"].value_or<int32_t>(0);
		spell.needTarget						= tbl["target"].value_or<bool>(false);
		spell.needDirection						= tbl["direction"].value_or<bool>(false);
		spell.radius							= tbl["radius"].value_or<int32_t>(0);
		spell.length							= tbl["length"].value_or<int32_t>(0);
		spell.spread							= tbl["spread"].value_or<int32_t>(0);
		spell.duration							= tbl["duration"].value_or<int32_t>(0);
		spell.drunkenness						= tbl["drunkenness"].value_or<uint8_t>(0);
		spell.attack							= tbl["attack"].value_or<int32_t>(0);
		spell.skill								= tbl["skill"].value_or<int32_t>(0);
		spell.minSpeedChange					= tbl["speedChange"].value_or<int32_t>(tbl["minSpeedChange"].value_or<int32_t>(0));
		spell.maxSpeedChange					= static_cast<int32_t>(tbl["maxSpeedChange"].value_or<int64_t>(spell.minSpeedChange));
		std::string_view shootEffectStr			= tbl["shootEffect"].value_or<std::string_view>("");
		std::string_view areaEffectStr			= tbl["areaEffect"].value_or<std::string_view>("");
		std::string_view combatTypeStr			= tbl["combatType"].value_or<std::string_view>("");
		spell.shoot								= shootEffectStr.empty() ? CONST_ANI_NONE : getShootType(std::string(shootEffectStr));
		spell.effect							= areaEffectStr.empty()  ? CONST_ME_NONE  : getMagicEffect(std::string(areaEffectStr));
		spell.combatType						= combatTypeStr.empty() ? COMBAT_UNDEFINEDDAMAGE : parseCombatType(combatTypeStr);

		if (auto condTbl = tbl["condition"].as_table())
		{
			std::string_view condTypeStr		= condTbl->at("type").value_or<std::string_view>("");
			spell.conditionType					= parseConditionType(condTypeStr);
			spell.conditionMinDamage			= condTbl->at("minDamage").value_or<int32_t>(0);
			spell.conditionMaxDamage			= condTbl->at("maxDamage").value_or<int32_t>(0);
			spell.conditionStartDamage			= condTbl->at("startDamage").value_or<int32_t>(0);
			spell.duration						= static_cast<int32_t>(condTbl->at("duration").value_or<int64_t>(spell.duration));
			spell.tickInterval					= condTbl->at("interval").value_or<int32_t>(2000);
		}

		if (auto outfitTbl = tbl["outfit"].as_table())
		{
			spell.outfit.lookType				= outfitTbl->at("lookType").value_or<uint16_t>(0);
			spell.outfit.lookTypeEx				= outfitTbl->at("lookTypeEx").value_or<uint16_t>(0);
		}

		spell.scriptName = tbl["script"].value_or<std::string>("");

		if (not spell.scriptName.empty())
			spell.isScripted = true;

		spellBlock_t sb;

		if (deserializeSpell(&spell, sb, mType->name))
		{
			if (isDefense)
			{
				mType->info.defenseSpells.push_back(std::move(sb));
			}
			else
			{
				mType->info.attackSpells.push_back(std::move(sb));
			}
		}
		else
		{
			std::cerr << "[Warning - Monsters] Could not load "			          << (isDefense ? "defense" : "attack") << " spell '"
			          << spell.name << "' for monster '" << mType->name << "'\n";
		}
	};

	if (auto attacks = data["attacks"].as_array())
	{
		attacks->for_each([&parseSpellTable](auto&& elem)
		{
			if (elem.is_table())
			{
				parseSpellTable(*elem.as_table(), false);
			}
		});
	}

	if (auto defenseSpells = data["defenseSpells"].as_array())
	{
		defenseSpells->for_each([&parseSpellTable](auto&& elem)
		{
			if (elem.is_table())
			{
				parseSpellTable(*elem.as_table(), true);
			}
		});
	}

	if (auto scripts = data["scripts"].as_array())
	{
		scripts->for_each([mType](auto&& elem)
		{
			if (elem.is_string())
			{
				mType->info.scripts.push_back(elem.value_or<std::string>(""));
			}
		});
	}

	return mType;
}

bool Monsters::loadFromToml(bool reloading)
{
	const std::filesystem::path monsterPath{ "data/monster" };

	if (not std::filesystem::exists(monsterPath))
	{
		std::cerr << "[Error - Monsters::loadFromToml] Monster directory not found: " << monsterPath << "\n";
		return false;
	}

	bool anyLoaded = false;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(monsterPath))
	{
		if (not entry.is_regular_file() or entry.path().extension() != ".toml")
		{
			continue;
		}

		try
		{
			auto file = toml::parse_file(entry.path().string());

			for (const auto& [key, value] : file)
			{
				if (not value.is_table())
				{
					std::cerr << "[Warning - Monsters::loadFromToml] Skipping non-table entry '" << key << "' in " << entry.path() << "\n";
					continue;
				}

				const toml::table& monsterData = *value.as_table();
				MonsterType* mType = loadMonsterFromToml(monsterData, std::string(key.str()), reloading);
				if (mType)
				{
					anyLoaded = true;
				}
			}
		}
		catch (const toml::parse_error& err)
		{
			std::cerr << "[Warning - Monsters::loadFromToml] Parse error in " << entry.path() << ": " << err.description() << "\n";
		}
		catch (const std::exception& ex)
		{
			std::cerr << "[Warning - Monsters::loadFromToml] Error loading " << entry.path() << ": " << ex.what() << "\n";
		}
	}

	loaded = anyLoaded;
	return loaded;
}
