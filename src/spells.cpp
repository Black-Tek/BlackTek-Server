// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "events.h"
#include "combat.h"
#include "configmanager.h"
#include "game.h"
#include "luavariant.h"
#include "monster.h"
#include "pugicast.h"
#include "spells.h"

extern Game g_game;
extern Spells* g_spells;
extern Monsters g_monsters;
extern Vocations g_vocations;
extern ConfigManager g_config;
extern LuaEnvironment g_luaEnvironment;
extern Events* g_events;

Spells::Spells()
{
	scriptInterface.initState();
}

Spells::~Spells()
{
	clear(false);
}

TalkActionResult_t Spells::playerSaySpell(const PlayerPtr& player, std::string& words)
{
	std::string str_words = words;

	//strip trailing spaces
	trimString(str_words);

	InstantSpell* instantSpell = getInstantSpell(str_words);
	if (!instantSpell) {
		return TALKACTION_CONTINUE;
	}

	std::string param;

	if (instantSpell->getHasParam()) {
		size_t spellLen = instantSpell->getWords().length();
		size_t paramLen = str_words.length() - spellLen;
		std::string paramText = str_words.substr(spellLen, paramLen);
		if (!paramText.empty() && paramText.front() == ' ') {
			size_t loc1 = paramText.find('"', 1);
			if (loc1 != std::string::npos) {
				size_t loc2 = paramText.find('"', loc1 + 1);
				if (loc2 == std::string::npos) {
					loc2 = paramText.length();
				} else if (paramText.find_last_not_of(' ') != loc2) {
					return TALKACTION_CONTINUE;
				}

				param = paramText.substr(loc1 + 1, loc2 - loc1 - 1);
			} else {
				trimString(paramText);
				loc1 = paramText.find(' ', 0);
				if (loc1 == std::string::npos) {
					param = paramText;
				} else {
					return TALKACTION_CONTINUE;
				}
			}
		}
	}

	if (instantSpell->playerCastInstant(player, param)) {
		words = instantSpell->getWords();

		if (instantSpell->getHasParam() && !param.empty()) {
			words += " \"" + param + "\"";
		}

		return TALKACTION_BREAK;
	}

	return TALKACTION_FAILED;
}

void Spells::clearMaps(bool fromLua)
{
	for (auto instant = instants.begin(); instant != instants.end(); ) {
		if (fromLua == instant->second.fromLua) {
			instant = instants.erase(instant);
		} else {
			++instant;
		}
	}

	for (auto rune = runes.begin(); rune != runes.end(); ) {
		if (fromLua == rune->second.fromLua) {
			rune = runes.erase(rune);
		} else {
			++rune;
		}
	}
}

void Spells::clear(bool fromLua)
{
	clearMaps(fromLua);

	reInitState(fromLua);
}

LuaScriptInterface& Spells::getScriptInterface()
{
	return scriptInterface;
}

Event_ptr Spells::getEvent(const std::string& nodeName)
{
	if (caseInsensitiveEqual(nodeName, "rune")) {
		return Event_ptr(new RuneSpell(&scriptInterface));
	} else if (caseInsensitiveEqual(nodeName, "instant")) {
		return Event_ptr(new InstantSpell(&scriptInterface));
	}
	return nullptr;
}

bool Spells::registerEvent(Event_ptr event, const pugi::xml_node&)
{
	std::cout << "[Warning - Spells::registerEvent] registerEvent is a deprecated function! \n";
	return false;
}

bool Spells::registerInstantLuaEvent(InstantSpell* event)
{
	InstantSpell_ptr instant { event };
	if (instant) {
		std::string words = instant->getWords();
		auto result = instants.emplace(instant->getWords(), std::move(*instant));
		if (!result.second) {
			std::cout << "[Warning - Spells::registerInstantLuaEvent] Duplicate registered instant spell with words: " << words << std::endl;
		}
		return result.second;
	}

	return false;
}

bool Spells::registerRuneLuaEvent(RuneSpell* event)
{
	RuneSpell_ptr rune { event };
	if (rune) {
		uint16_t id = rune->getRuneItemId();
		auto result = runes.emplace(rune->getRuneItemId(), std::move(*rune));
		if (!result.second) {
			std::cout << "[Warning - Spells::registerRuneLuaEvent] Duplicate registered rune with id: " << id << std::endl;
		}
		return result.second;
	}

	return false;
}

Spell* Spells::getSpellByName(const std::string& name)
{
	Spell* spell = getRuneSpellByName(name);
	if (!spell) {
		spell = getInstantSpellByName(name);
	}
	return spell;
}

RuneSpell* Spells::getRuneSpell(uint32_t id)
{
	auto it = runes.find(id);
	if (it == runes.end()) {
		for (auto& rune : runes) {
			if (rune.second.getId() == id) {
				return &rune.second;
			}
		}
		return nullptr;
	}
	return &it->second;
}

RuneSpell* Spells::getRuneSpellByName(const std::string& name)
{
	for (auto& it : runes) {
		if (caseInsensitiveEqual(it.second.getName(), name)) {
			return &it.second;
		}
	}
	return nullptr;
}

InstantSpell* Spells::getInstantSpell(const std::string& words)
{
	InstantSpell* result = nullptr;

	for (auto& it : instants) {
		const std::string& instantSpellWords = it.second.getWords();
		size_t spellLen = instantSpellWords.length();
		if (caseInsensitiveStartsWith(words, instantSpellWords)) {
			if (!result || spellLen > result->getWords().size()) {
				result = &it.second;
				if (words.length() == spellLen) {
					break;
				}
			}
		}
	}

	if (result) {
		const std::string& resultWords = result->getWords();
		if (words.length() > resultWords.length()) {
			if (!result->getHasParam()) {
				return nullptr;
			}

			size_t spellLen = resultWords.length();
			size_t paramLen = words.length() - spellLen;
			if (paramLen < 2 || words[spellLen] != ' ') {
				return nullptr;
			}
		}
		return result;
	}
	return nullptr;
}

InstantSpell* Spells::getInstantSpellByName(const std::string& name)
{
	for (auto& it : instants) {
		if (caseInsensitiveEqual(it.second.getName(), name)) {
			return &it.second;
		}
	}
	return nullptr;
}

Position Spells::getCasterPosition(const CreaturePtr& creature, Direction dir)
{
	return getNextPosition(dir, creature->getPosition());
}

CombatSpell::CombatSpell(const Combat_ptr& combat, bool needTarget, bool needDirection) :
	Event(&g_spells->getScriptInterface()),
	combat(combat),
	needDirection(needDirection),
	needTarget(needTarget)
{}

bool CombatSpell::loadScriptCombat()
{
	combat = g_luaEnvironment.getCombatObject(g_luaEnvironment.lastCombatId);
	return combat != nullptr;
}

bool CombatSpell::castSpell(const CreaturePtr& creature)
{
	if (scripted) {
		LuaVariant var;

		if (needDirection) {
			var.setPosition(Spells::getCasterPosition(creature, creature->getDirection()));
		} else {
			var.setPosition(creature->getPosition());
		}

		return executeCastSpell(creature, var);
	}

	Position pos;
	if (needDirection) {
		pos = Spells::getCasterPosition(creature, creature->getDirection());
	} else {
		pos = creature->getPosition();
	}

	combat->doCombat(creature, pos);
	return true;
}

bool CombatSpell::castSpell(const CreaturePtr& creature, const CreaturePtr& target)
{
	if (scripted) {
		LuaVariant var;

		if (combat->hasArea()) {

			if (needTarget) {
				var.setPosition(target->getPosition());
			} else if (needDirection) {
				var.setPosition(Spells::getCasterPosition(creature, creature->getDirection()));
			} else {
				var.setPosition(creature->getPosition());
			}
		} else {
			var.setNumber(target->getID());
		}
		return executeCastSpell(creature, var);
	}

	if (combat->hasArea()) {
		if (needTarget) {
			combat->doCombat(creature, target->getPosition());
		} else {
			return castSpell(creature);
		}
	} else {
		combat->doCombat(creature, target);
	}
	return true;
}

bool CombatSpell::executeCastSpell(const CreaturePtr& creature, const LuaVariant& var)
{
	//onCastSpell(creature, var)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CombatSpell::executeCastSpell] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);

	LuaScriptInterface::pushVariant(L, var);

	return scriptInterface->callFunction(2);
}

bool Spell::playerSpellCheck(const PlayerPtr& player) const
{
	if (player->hasFlag(PlayerFlag_CannotUseSpells)) {
		return false;
	}

	if (player->hasFlag(PlayerFlag_IgnoreSpellCheck)) {
		return true;
	}

	if (!enabled) {
		return false;
	}

	if ((aggressive || pzLock) && (range < 1 || (range > 0 && !player->getAttackedCreature())) && player->getSkull() == SKULL_BLACK) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		return false;
	}

	if ((aggressive || pzLock) && player->hasCondition(CONDITION_PACIFIED)) {
		player->sendCancelMessage(RETURNVALUE_YOUAREEXHAUSTED);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if ((aggressive || pzLock) && !player->hasFlag(PlayerFlag_IgnoreProtectionZone) && player->getZone() == ZONE_PROTECTION) {
		player->sendCancelMessage(RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE);
		return false;
	}

	if (player->hasCondition(CONDITION_SPELLGROUPCOOLDOWN, group) || player->hasCondition(CONDITION_SPELLCOOLDOWN, spellId) || (secondaryGroup != SPELLGROUP_NONE && player->hasCondition(CONDITION_SPELLGROUPCOOLDOWN, secondaryGroup))) {
		player->sendCancelMessage(RETURNVALUE_YOUAREEXHAUSTED);

		if (isInstant()) {
			g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		}

		return false;
	}

	if (player->getLevel() < level) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHLEVEL);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (player->getMagicLevel() < magLevel) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHMAGICLEVEL);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (player->getMana() < getManaCost(player) && !player->hasFlag(PlayerFlag_HasInfiniteMana)) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHMANA);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (player->getSoul() < soul && !player->hasFlag(PlayerFlag_HasInfiniteSoul)) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHSOUL);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (isInstant() && isLearnable()) {
		if (!player->hasLearnedInstantSpell(getName())) {
			player->sendCancelMessage(RETURNVALUE_YOUNEEDTOLEARNTHISSPELL);
			g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
			return false;
		}
	} else if (!vocSpellMap.empty() && vocSpellMap.find(player->getVocationId()) == vocSpellMap.end()) {
		player->sendCancelMessage(RETURNVALUE_YOURVOCATIONCANNOTUSETHISSPELL);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (needWeapon) {
		switch (player->getWeaponType()) {
			case WEAPON_SWORD:
			case WEAPON_CLUB:
			case WEAPON_AXE:
            case WEAPON_DISTANCE:
			case WEAPON_WAND:
				break;

			default: {
				player->sendCancelMessage(RETURNVALUE_YOUNEEDAWEAPONTOUSETHISSPELL);
				g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
				return false;
			}
		}
	}

	if (isPremium() && !player->isPremium()) {
		player->sendCancelMessage(RETURNVALUE_YOUNEEDPREMIUMACCOUNT);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	return true;
}

bool Spell::playerInstantSpellCheck(const PlayerPtr& player, const Position& toPos)
{
	if (toPos.x == 0xFFFF) {
		return true;
	}

	const Position& playerPos = player->getPosition();
	if (playerPos.z > toPos.z) {
		player->sendCancelMessage(RETURNVALUE_FIRSTGOUPSTAIRS);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	} else if (playerPos.z < toPos.z) {
		player->sendCancelMessage(RETURNVALUE_FIRSTGODOWNSTAIRS);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	auto tile = g_game.map.getTile(toPos);
	if (!tile) {
		tile = std::make_shared<Tile>(toPos.x, toPos.y, toPos.z);
		g_game.map.setTile(toPos, tile);
	}

	if (blockingCreature && tile->getBottomVisibleCreature(player) != nullptr) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (blockingSolid && tile->hasFlag(TILESTATE_BLOCKSOLID)) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (!g_events->eventPlayerOnSpellTry(player, this, SPELL_INSTANT)) {
		return false;
	}

	return true;
}

bool Spell::playerRuneSpellCheck(const PlayerPtr& player, const Position& toPos)
{
	if (!playerSpellCheck(player)) {
		return false;
	}

	if (toPos.x == 0xFFFF) {
		return true;
	}

	const Position& playerPos = player->getPosition();
	if (playerPos.z > toPos.z) {
		player->sendCancelMessage(RETURNVALUE_FIRSTGOUPSTAIRS);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	} else if (playerPos.z < toPos.z) {
		player->sendCancelMessage(RETURNVALUE_FIRSTGODOWNSTAIRS);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	auto tile = g_game.map.getTile(toPos);
	if (!tile) {
		player->sendCancelMessage(RETURNVALUE_NOTPOSSIBLE);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (range != -1 && !g_game.canThrowObjectTo(playerPos, toPos, true, true, range, range)) {
		player->sendCancelMessage(RETURNVALUE_DESTINATIONOUTOFREACH);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	ReturnValue ret = Combat::canDoCombat(player, tile, aggressive);
	if (ret != RETURNVALUE_NOERROR) {
		player->sendCancelMessage(ret);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	const auto& topVisibleCreature = tile->getBottomVisibleCreature(player);
	if (blockingCreature && topVisibleCreature) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	} else if (blockingSolid && tile->hasFlag(TILESTATE_BLOCKSOLID)) {
		player->sendCancelMessage(RETURNVALUE_NOTENOUGHROOM);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (needTarget && !topVisibleCreature) {
		player->sendCancelMessage(RETURNVALUE_CANONLYUSETHISRUNEONCREATURES);
		g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		return false;
	}

	if (aggressive && needTarget && topVisibleCreature && player->hasSecureMode()) {
		const auto& targetPlayer = topVisibleCreature->getPlayer();
		if (targetPlayer && targetPlayer != player && player->getSkullClient(targetPlayer) == SKULL_NONE && !Combat::isInPvpZone(player, targetPlayer)) {
			player->sendCancelMessage(RETURNVALUE_TURNSECUREMODETOATTACKUNMARKEDPLAYERS);
			g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
			return false;
		}
	}

	if (!g_events->eventPlayerOnSpellTry(player, this, SPELL_RUNE)) {
		return false;
	}

	return true;
}

void Spell::addCooldowns(const PlayerPtr& player) const
{
	if (cooldown > 0) {
		Condition* condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_SPELLCOOLDOWN, cooldown, 0, false, spellId);
		player->addCondition(condition);
	}

	if (group != SPELLGROUP_NONE && groupCooldown > 0) {
		Condition* condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_SPELLGROUPCOOLDOWN, groupCooldown, 0, false, group);
		player->addCondition(condition);
	}

	if (secondaryGroup != SPELLGROUP_NONE && secondaryGroupCooldown > 0) {
		Condition* condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_SPELLGROUPCOOLDOWN, secondaryGroupCooldown, 0, false, secondaryGroup);
		player->addCondition(condition);
	}
}

void Spell::postCastSpell(const PlayerPtr& player, bool finishedCast /*= true*/, bool payCost /*= true*/) const
{
	if (finishedCast) {
		if (!player->hasFlag(PlayerFlag_HasNoExhaustion)) {
			addCooldowns(player);
		}

		if (aggressive) {
			player->addInFightTicks();
		}
	}

	if (payCost) {
		Spell::postCastSpell(player, getManaCost(player), getSoulCost());
	}
}

void Spell::postCastSpell(const PlayerPtr& player, uint32_t manaCost, uint32_t soulCost)
{
    if (manaCost > 0 && !player->hasFlag(PlayerFlag_HasInfiniteMana)) {
        player->addManaSpent(manaCost);
        player->changeMana(-static_cast<int32_t>(manaCost));
    }

	if (!player->hasFlag(PlayerFlag_HasInfiniteSoul)) {
		if (soulCost > 0) {
			player->changeSoul(-static_cast<int32_t>(soulCost));
		}
	}
}

uint32_t Spell::getManaCost(const PlayerConstPtr& player) const
{
	if (mana != 0) {
		return mana;
	}

	if (manaPercent != 0) {
		uint32_t maxMana = player->getMaxMana();
		uint32_t manaCost = (maxMana * manaPercent) / 100;
		return manaCost;
	}

	return 0;
}

bool InstantSpell::playerCastInstant(const PlayerPtr& player, std::string& param)
{
	if (!playerSpellCheck(player)) {
		return false;
	}

	LuaVariant var;

	if (selfTarget) {
		var.setNumber(player->getID());
	} else if (needTarget || casterTargetOrDirection) {
		CreaturePtr target = nullptr;
		bool useDirection = false;

		if (hasParam) {
			PlayerPtr playerTarget = nullptr;
			ReturnValue ret = g_game.getPlayerByNameWildcard(param);

			if (playerTarget && playerTarget->isAccessPlayer() && !player->isAccessPlayer()) {
				playerTarget = nullptr;
			}

			target = playerTarget;
			if (!target || target->isRemoved() || target->isDead()) {
				if (!casterTargetOrDirection) {
					addCooldowns(player);

					player->sendCancelMessage(ret);
					g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
					return false;
				}

				useDirection = true;
			}

			if (playerTarget) {
				param = playerTarget->getName();
			}
		} else {
			target = player->getAttackedCreature();
			if (!target || target->isRemoved() || target->isDead()) {
				if (!casterTargetOrDirection) {
					player->sendCancelMessage(RETURNVALUE_YOUCANONLYUSEITONCREATURES);
					g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
					return false;
				}

				useDirection = true;
			}
		}

		if (!useDirection) {
			if (!canThrowSpell(player, target)) {
				player->sendCancelMessage(RETURNVALUE_CREATUREISNOTREACHABLE);
				g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
				return false;
			}

			var.setNumber(target->getID());
		} else {
			var.setPosition(Spells::getCasterPosition(player, player->getDirection()));

			if (!playerInstantSpellCheck(player, var.getPosition())) {
				return false;
			}
		}
	} else if (hasParam) {
		if (getHasPlayerNameParam()) {
			PlayerPtr playerTarget = nullptr;
			ReturnValue ret = g_game.getPlayerByNameWildcard(param);

			if (ret != RETURNVALUE_NOERROR) {
				addCooldowns(player);

				player->sendCancelMessage(ret);
				g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
				return false;
			}

			if (playerTarget && (!playerTarget->isAccessPlayer() || player->isAccessPlayer())) {
				param = playerTarget->getName();
			}
		}

		var.setString(param);
	} else {
		if (needDirection) {
			var.setPosition(Spells::getCasterPosition(player, player->getDirection()));
		} else {
			var.setPosition(player->getPosition());
		}

		if (!playerInstantSpellCheck(player, var.getPosition())) {
			return false;
		}
	}

	bool result = internalCastSpell(player, var);
	if (result) {
		postCastSpell(player);
	}

	return result;
}

bool InstantSpell::canThrowSpell(const CreatureConstPtr& creature, const CreatureConstPtr& target) const
{
	const Position& fromPos = creature->getPosition();
	const Position& toPos = target->getPosition();
	if (fromPos.z != toPos.z ||
			(range == -1 && !g_game.canThrowObjectTo(fromPos, toPos, checkLineOfSight, true, 7, 5)) ||
			(range != -1 && !g_game.canThrowObjectTo(fromPos, toPos, checkLineOfSight, true, range, range))) {
		return false;
	}
	return true;
}

bool InstantSpell::castSpell(const CreaturePtr& creature)
{
	LuaVariant var;

	if (casterTargetOrDirection) {
		const auto& target = creature->getAttackedCreature();
		if (target && target->getHealth() > 0) {
			if (!canThrowSpell(creature, target)) {
				return false;
			}

			var.setNumber(target->getID());
			return internalCastSpell(creature, var);
		}

		return false;
	} else if (needDirection) {
		var.setPosition(Spells::getCasterPosition(creature, creature->getDirection()));
	} else {
		var.setPosition(creature->getPosition());
	}

	return internalCastSpell(creature, var);
}

bool InstantSpell::castSpell(const CreaturePtr& creature, const CreaturePtr& target)
{
	if (needTarget) {
		LuaVariant var;
		var.setNumber(target->getID());
		return internalCastSpell(creature, var);
	} else {
		return castSpell(creature);
	}
}

bool InstantSpell::internalCastSpell(const CreaturePtr& creature, const LuaVariant& var)
{
	return executeCastSpell(creature, var);
}

bool InstantSpell::executeCastSpell(const CreaturePtr& creature, const LuaVariant& var)
{
	//onCastSpell(creature, var)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - InstantSpell::executeCastSpell] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);

	LuaScriptInterface::pushVariant(L, var);

	return scriptInterface->callFunction(2);
}

bool InstantSpell::canCast(const PlayerConstPtr& player) const
{
	if (player->hasFlag(PlayerFlag_CannotUseSpells)) {
		return false;
	}

	if (player->hasFlag(PlayerFlag_IgnoreSpellCheck)) {
		return true;
	}

	if (isLearnable()) {
		if (player->hasLearnedInstantSpell(getName())) {
			return true;
		}
	} else {
		if (vocSpellMap.empty() || vocSpellMap.contains(player->getVocationId())) {
			return true;
		}
	}

	return false;
}

ReturnValue RuneSpell::canExecuteAction(const PlayerConstPtr& player, const Position& toPos)
{
	if (player->hasFlag(PlayerFlag_CannotUseSpells)) {
		return RETURNVALUE_CANNOTUSETHISOBJECT;
	}

	ReturnValue ret = Action::canExecuteAction(player, toPos);
	if (ret != RETURNVALUE_NOERROR) {
		return ret;
	}

	if (toPos.x == 0xFFFF) {
		if (needTarget) {
			return RETURNVALUE_CANONLYUSETHISRUNEONCREATURES;
		} else if (!selfTarget) {
			return RETURNVALUE_NOTENOUGHROOM;
		}
	}

	return RETURNVALUE_NOERROR;
}

bool RuneSpell::executeUse(const PlayerPtr& player, const ItemPtr& item, const Position&, const ThingPtr& target, const Position& toPosition, bool isHotkey)
{
	if (!playerRuneSpellCheck(player, toPosition)) {
		return false;
	}

	if (!scripted) {
		return false;
	}

	LuaVariant var;

	if (needTarget) {

		if (target == nullptr) {
			if (const auto& toTile = g_game.map.getTile(toPosition)) {
				if (const auto& visibleCreature = toTile->getBottomVisibleCreature(player)) {
					var.setNumber(visibleCreature->getID());
				}
			}
		} else {
			var.setNumber(target->getCreature()->getID());
		}
	} else {
		var.setPosition(toPosition);
	}

	if (!internalCastSpell(player, var, isHotkey)) {
		return false;
	}

	postCastSpell(player);

	if (var.isNumber()) {
		auto n_target = g_game.getCreatureByID(var.getNumber());
		if (getPzLock() && n_target) {
			player->onAttackedCreature(n_target->getCreature());
		}
	}

	if (hasCharges && item && g_config.getBoolean(ConfigManager::REMOVE_RUNE_CHARGES)) {
		int32_t newCount = std::max<int32_t>(0, item->getItemCount() - 1);
		g_game.transformItem(item, item->getID(), newCount);
	}
	return true;
}

bool RuneSpell::castSpell(const CreaturePtr& creature)
{
	LuaVariant var;
	var.setNumber(creature->getID());
	return internalCastSpell(creature, var, false);
}

bool RuneSpell::castSpell(const CreaturePtr& creature, const CreaturePtr& target)
{
	LuaVariant var;
	var.setNumber(target->getID());
	return internalCastSpell(creature, var, false);
}

bool RuneSpell::internalCastSpell(const CreaturePtr& creature, const LuaVariant& var, bool isHotkey)
{
	bool result;
	if (scripted) {
		result = executeCastSpell(creature, var, isHotkey);
	} else {
		result = false;
	}
	return result;
}

bool RuneSpell::executeCastSpell(const CreaturePtr& creature, const LuaVariant& var, bool isHotkey)
{
	//onCastSpell(creature, var, isHotkey)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - RuneSpell::executeCastSpell] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);

	LuaScriptInterface::pushVariant(L, var);

	LuaScriptInterface::pushBoolean(L, isHotkey);

	return scriptInterface->callFunction(3);
}
