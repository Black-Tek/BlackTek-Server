// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "combat.h"
#include "game.h"
#include "weapons.h"
#include "configmanager.h"
#include "events.h"
#include "monster.h"

#include <optional>

extern Game g_game;
extern Weapons* g_weapons;
extern ConfigManager g_config;
extern Events* g_events;

std::vector<Tile*> getList(const MatrixArea& area, const Position& targetPos, const Direction dir)
{
	auto casterPos = getNextPosition(dir, targetPos);

	std::vector<Tile*> vec;

	auto center = area.getCenter();

	Position tmpPos(targetPos.x - center.first, targetPos.y - center.second, targetPos.z);
	for (uint32_t row = 0; row < area.getRows(); ++row, ++tmpPos.y) {
		for (uint32_t col = 0; col < area.getCols(); ++col, ++tmpPos.x) {
			if (area(row, col)) {
				if (g_game.isSightClear(casterPos, tmpPos, true)) {
					Tile* tile = g_game.map.getTile(tmpPos);
					if (!tile) {
						tile = new StaticTile(tmpPos.x, tmpPos.y, tmpPos.z);
						g_game.map.setTile(tmpPos, tile);
					}
					vec.push_back(tile);
				}
			}
		}
		tmpPos.x -= area.getCols();
	}
	return vec;
}

std::vector<Tile*> getCombatArea(const Position& centerPos, const Position& targetPos, const AreaCombat* area)
{
	if (targetPos.z >= MAP_MAX_LAYERS) {
		return {};
	}

	if (area) {
		return getList(area->getArea(centerPos, targetPos), targetPos, getDirectionTo(targetPos, centerPos));
	}

	Tile* tile = g_game.map.getTile(targetPos);
	if (!tile) {
		tile = new StaticTile(targetPos.x, targetPos.y, targetPos.z);
		g_game.map.setTile(targetPos, tile);
	}
	return {tile};
}

CombatDamage Combat::getCombatDamage(Creature* creature, Creature* target) const
{
	CombatDamage damage;
	damage.origin = params.origin;
	damage.primary.type = params.combatType;
	if (formulaType == COMBAT_FORMULA_DAMAGE) {
		damage.primary.value = normal_random(
			static_cast<int32_t>(mina),
			static_cast<int32_t>(maxa)
		);
	} else if (creature) {
		int32_t min, max;
		if (creature->getCombatValues(min, max)) {
			damage.primary.value = normal_random(min, max);
		} else if (Player* player = creature->getPlayer()) {
			if (params.valueCallback) {
				params.valueCallback->getMinMaxValues(player, damage);
			} else if (formulaType == COMBAT_FORMULA_LEVELMAGIC) {
				int32_t levelFormula = player->getLevel() * 2 + player->getMagicLevel() * 3;
				damage.primary.value = normal_random(std::fma(levelFormula, mina, minb), std::fma(levelFormula, maxa, maxb));
			} else if (formulaType == COMBAT_FORMULA_SKILL) {
				Item* tool = player->getWeapon();
				const Weapon* weapon = g_weapons->getWeapon(tool);
				if (weapon) {
					damage.primary.value = normal_random(minb, std::fma(weapon->getWeaponDamage(player, target, tool, true), maxa, maxb));
					damage.secondary.type = weapon->getElementType();
					damage.secondary.value = weapon->getElementDamage(player, target, tool);
				} else {
					damage.primary.value = normal_random(minb, maxb);
				}
			}
		}
	}
	return damage;
}

CombatType_t Combat::ConditionToDamageType(ConditionType_t type)
{
	switch (type) {
		case CONDITION_FIRE:
			return COMBAT_FIREDAMAGE;

		case CONDITION_ENERGY:
			return COMBAT_ENERGYDAMAGE;

		case CONDITION_BLEEDING:
			return COMBAT_PHYSICALDAMAGE;

		case CONDITION_DROWN:
			return COMBAT_DROWNDAMAGE;

		case CONDITION_POISON:
			return COMBAT_EARTHDAMAGE;

		case CONDITION_FREEZING:
			return COMBAT_ICEDAMAGE;

		case CONDITION_DAZZLED:
			return COMBAT_HOLYDAMAGE;

		case CONDITION_CURSED:
			return COMBAT_DEATHDAMAGE;

		default:
			break;
	}

	return COMBAT_NONE;
}

ConditionType_t Combat::DamageToConditionType(CombatType_t type)
{
	switch (type) {
		case COMBAT_FIREDAMAGE:
			return CONDITION_FIRE;

		case COMBAT_ENERGYDAMAGE:
			return CONDITION_ENERGY;

		case COMBAT_DROWNDAMAGE:
			return CONDITION_DROWN;

		case COMBAT_EARTHDAMAGE:
			return CONDITION_POISON;

		case COMBAT_ICEDAMAGE:
			return CONDITION_FREEZING;

		case COMBAT_HOLYDAMAGE:
			return CONDITION_DAZZLED;

		case COMBAT_DEATHDAMAGE:
			return CONDITION_CURSED;

		case COMBAT_PHYSICALDAMAGE:
			return CONDITION_BLEEDING;

		default:
			return CONDITION_NONE;
	}
}

bool Combat::isPlayerCombat(const Creature* target)
{
	if (target->getPlayer()) {
		return true;
	}

	if (target->isSummon() && target->getMaster()->getPlayer()) {
		return true;
	}

	return false;
}

ReturnValue Combat::canTargetCreature(Player* attacker, Creature* target)
{
	if (attacker == target) {
		return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
	}

	if (!attacker->hasFlag(PlayerFlag_IgnoreProtectionZone)) {
		//pz-zone
		if (attacker->getZone() == ZONE_PROTECTION) {
			return RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE;
		}

		if (target->getZone() == ZONE_PROTECTION) {
			return RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE;
		}

		//nopvp-zone
		if (isPlayerCombat(target)) {
			if (attacker->getZone() == ZONE_NOPVP) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}

			if (target->getZone() == ZONE_NOPVP) {
				return RETURNVALUE_YOUMAYNOTATTACKAPERSONINPROTECTIONZONE;
			}
		}
	}

	if (attacker->hasFlag(PlayerFlag_CannotUseCombat) || !target->isAttackable()) {
		if (target->getPlayer()) {
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		} else {
			return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
		}
	}

	if (target->getPlayer()) {
		if (isProtected(attacker, target->getPlayer())) {
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		}

		if (attacker->hasSecureMode() && !Combat::isInPvpZone(attacker, target) && attacker->getSkullClient(target->getPlayer()) == SKULL_NONE) {
			return RETURNVALUE_TURNSECUREMODETOATTACKUNMARKEDPLAYERS;
		}
	}

	return Combat::canDoCombat(attacker, target);
}

ReturnValue Combat::canDoCombat(Creature* caster, Tile* tile, bool aggressive)
{
	if (tile->hasProperty(CONST_PROP_BLOCKPROJECTILE)) {
		return RETURNVALUE_NOTENOUGHROOM;
	}

	if (tile->hasFlag(TILESTATE_FLOORCHANGE)) {
		return RETURNVALUE_NOTENOUGHROOM;
	}

	if (tile->getTeleportItem()) {
		return RETURNVALUE_NOTENOUGHROOM;
	}

	if (caster) {
		const Position& casterPosition = caster->getPosition();
		const Position& tilePosition = tile->getPosition();
		if (casterPosition.z < tilePosition.z) {
			return RETURNVALUE_FIRSTGODOWNSTAIRS;
		} else if (casterPosition.z > tilePosition.z) {
			return RETURNVALUE_FIRSTGOUPSTAIRS;
		}

		if (const Player* player = caster->getPlayer()) {
			if (player->hasFlag(PlayerFlag_IgnoreProtectionZone)) {
				return RETURNVALUE_NOERROR;
			}
		}
	}

	//pz-zone
	if (aggressive && tile->hasFlag(TILESTATE_PROTECTIONZONE)) {
		return RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE;
	}

	return g_events->eventCreatureOnAreaCombat(caster, tile, aggressive);
}

bool Combat::isInPvpZone(const Creature* attacker, const Creature* target)
{
	return attacker->getZone() == ZONE_PVP && target->getZone() == ZONE_PVP;
}

bool Combat::isProtected(const Player* attacker, const Player* target)
{
	uint32_t protectionLevel = g_config.getNumber(ConfigManager::PROTECTION_LEVEL);
	if (target->getLevel() < protectionLevel || attacker->getLevel() < protectionLevel) {
		return true;
	}

	if (!attacker->getVocation()->allowsPvp() || !target->getVocation()->allowsPvp()) {
		return true;
	}

	if (attacker->getSkull() == SKULL_BLACK && attacker->getSkullClient(target) == SKULL_NONE) {
		return true;
	}

	return false;
}

ReturnValue Combat::canDoCombat(Creature* attacker, Creature* target)
{
	if (!attacker) {
		return g_events->eventCreatureOnTargetCombat(attacker, target);
	}

	if (const Player* targetPlayer = target->getPlayer()) {
		if (targetPlayer->hasFlag(PlayerFlag_CannotBeAttacked)) {
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		}

		if (const Player* attackerPlayer = attacker->getPlayer()) {
			if (attackerPlayer->hasFlag(PlayerFlag_CannotAttackPlayer)) {
				return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
			}

			if (isProtected(attackerPlayer, targetPlayer)) {
				return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
			}

			//nopvp-zone
			const Tile* targetPlayerTile = targetPlayer->getTile();
			if (targetPlayerTile->hasFlag(TILESTATE_NOPVPZONE)) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			} else if (attackerPlayer->getTile()->hasFlag(TILESTATE_NOPVPZONE) && !targetPlayerTile->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE)) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}
		}

		if (attacker->isSummon()) {
			if (const Player* masterAttackerPlayer = attacker->getMaster()->getPlayer()) {
				if (masterAttackerPlayer->hasFlag(PlayerFlag_CannotAttackPlayer)) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
				}

				if (targetPlayer->getTile()->hasFlag(TILESTATE_NOPVPZONE)) {
					return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
				}

				if (isProtected(masterAttackerPlayer, targetPlayer)) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
				}
			}
		}
	} else if (target->getMonster()) {
		if (const Player* attackerPlayer = attacker->getPlayer()) {
			if (attackerPlayer->hasFlag(PlayerFlag_CannotAttackMonster)) {
				return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
			}

			if (target->isSummon() && target->getMaster()->getPlayer() && target->getZone() == ZONE_NOPVP) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}
		} else if (attacker->getMonster()) {
			const Creature* targetMaster = target->getMaster();

			if (!targetMaster || !targetMaster->getPlayer()) {
				const Creature* attackerMaster = attacker->getMaster();

				if (!attackerMaster || !attackerMaster->getPlayer()) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
				}
			}
		}
	}

	if (g_game.getWorldType() == WORLD_TYPE_NO_PVP) {
		if (attacker->getPlayer() || (attacker->isSummon() && attacker->getMaster()->getPlayer())) {
			if (target->getPlayer()) {
				if (!isInPvpZone(attacker, target)) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
				}
			}

			if (target->isSummon() && target->getMaster()->getPlayer()) {
				if (!isInPvpZone(attacker, target)) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
				}
			}
		}
	}
	return g_events->eventCreatureOnTargetCombat(attacker, target);
}

void Combat::setPlayerCombatValues(formulaType_t formulaType, double mina, double minb, double maxa, double maxb)
{
	this->formulaType = formulaType;
	this->mina = mina;
	this->minb = minb;
	this->maxa = maxa;
	this->maxb = maxb;
}

bool Combat::setParam(CombatParam_t param, uint32_t value)
{
	switch (param) {
		case COMBAT_PARAM_TYPE: {
			params.combatType = static_cast<CombatType_t>(value);
			return true;
		}

		case COMBAT_PARAM_EFFECT: {
			params.impactEffect = static_cast<uint8_t>(value);
			return true;
		}

		case COMBAT_PARAM_DISTANCEEFFECT: {
			params.distanceEffect = static_cast<uint8_t>(value);
			return true;
		}

		case COMBAT_PARAM_BLOCKARMOR: {
			params.blockedByArmor = (value != 0);
			return true;
		}

		case COMBAT_PARAM_BLOCKSHIELD: {
			params.blockedByShield = (value != 0);
			return true;
		}

		case COMBAT_PARAM_TARGETCASTERORTOPMOST: {
			params.targetCasterOrTopMost = (value != 0);
			return true;
		}

		case COMBAT_PARAM_CREATEITEM: {
			params.itemId = value;
			return true;
		}

		case COMBAT_PARAM_AGGRESSIVE: {
			params.aggressive = (value != 0);
			return true;
		}

		case COMBAT_PARAM_DISPEL: {
			params.dispelType = static_cast<ConditionType_t>(value);
			return true;
		}

		case COMBAT_PARAM_USECHARGES: {
			params.useCharges = (value != 0);
			return true;
		}
	}
	return false;
}

int32_t Combat::getParam(CombatParam_t param)
{
	switch (param) {
		case COMBAT_PARAM_TYPE:
			return static_cast<int32_t>(params.combatType);

		case COMBAT_PARAM_EFFECT:
			return static_cast<int32_t>(params.impactEffect);

		case COMBAT_PARAM_DISTANCEEFFECT:
			return static_cast<int32_t>(params.distanceEffect);

		case COMBAT_PARAM_BLOCKARMOR:
			return params.blockedByArmor ? 1 : 0;

		case COMBAT_PARAM_BLOCKSHIELD:
			return params.blockedByShield ? 1 : 0;

		case COMBAT_PARAM_TARGETCASTERORTOPMOST:
			return params.targetCasterOrTopMost ? 1 : 0;

		case COMBAT_PARAM_CREATEITEM:
			return params.itemId;

		case COMBAT_PARAM_AGGRESSIVE:
			return params.aggressive ? 1 : 0;

		case COMBAT_PARAM_DISPEL:
			return static_cast<int32_t>(params.dispelType);

		case COMBAT_PARAM_USECHARGES:
			return params.useCharges ? 1 : 0;

		default:
			return std::numeric_limits<int32_t>().max();
	}
}

void Combat::setArea(AreaCombat* area) 
{ 
	this->area.reset(area); 
}

bool Combat::setCallback(CallBackParam_t key)
{
	switch (key) {
		case CALLBACK_PARAM_LEVELMAGICVALUE: {
			params.valueCallback.reset(new ValueCallback(COMBAT_FORMULA_LEVELMAGIC));
			return true;
		}

		case CALLBACK_PARAM_SKILLVALUE: {
			params.valueCallback.reset(new ValueCallback(COMBAT_FORMULA_SKILL));
			return true;
		}

		case CALLBACK_PARAM_TARGETTILE: {
			params.tileCallback.reset(new TileCallback());
			return true;
		}

		case CALLBACK_PARAM_TARGETCREATURE: {
			params.targetCallback.reset(new TargetCallback());
			return true;
		}
	}
	return false;
}

CallBack* Combat::getCallback(CallBackParam_t key)
{
	switch (key) {
		case CALLBACK_PARAM_LEVELMAGICVALUE:
		case CALLBACK_PARAM_SKILLVALUE: {
			return params.valueCallback.get();
		}

		case CALLBACK_PARAM_TARGETTILE: {
			return params.tileCallback.get();
		}

		case CALLBACK_PARAM_TARGETCREATURE: {
			return params.targetCallback.get();
		}
	}
	return nullptr;
}

void Combat::combatTileEffects(const SpectatorVec& spectators, Creature* caster, Tile* tile, const CombatParams& params)
{
	if (params.itemId != 0) {
		uint16_t itemId = params.itemId;
		switch (itemId) {
			case ITEM_FIREFIELD_PERSISTENT_FULL:
				itemId = ITEM_FIREFIELD_PVP_FULL;
				break;

			case ITEM_FIREFIELD_PERSISTENT_MEDIUM:
				itemId = ITEM_FIREFIELD_PVP_MEDIUM;
				break;

			case ITEM_FIREFIELD_PERSISTENT_SMALL:
				itemId = ITEM_FIREFIELD_PVP_SMALL;
				break;

			case ITEM_ENERGYFIELD_PERSISTENT:
				itemId = ITEM_ENERGYFIELD_PVP;
				break;

			case ITEM_POISONFIELD_PERSISTENT:
				itemId = ITEM_POISONFIELD_PVP;
				break;

			case ITEM_MAGICWALL_PERSISTENT:
				itemId = ITEM_MAGICWALL;
				break;

			case ITEM_WILDGROWTH_PERSISTENT:
				itemId = ITEM_WILDGROWTH;
				break;

			default:
				break;
		}

		if (caster) {
			Player* casterPlayer;
			if (caster->isSummon()) {
				casterPlayer = caster->getMaster()->getPlayer();
			} else {
				casterPlayer = caster->getPlayer();
			}

			if (casterPlayer) {
				if (g_game.getWorldType() == WORLD_TYPE_NO_PVP || tile->hasFlag(TILESTATE_NOPVPZONE)) {
					if (itemId == ITEM_FIREFIELD_PVP_FULL) {
						itemId = ITEM_FIREFIELD_NOPVP;
					} else if (itemId == ITEM_POISONFIELD_PVP) {
						itemId = ITEM_POISONFIELD_NOPVP;
					} else if (itemId == ITEM_ENERGYFIELD_PVP) {
						itemId = ITEM_ENERGYFIELD_NOPVP;
					} else if (itemId == ITEM_MAGICWALL) {
						itemId = ITEM_MAGICWALL_NOPVP;
					} else if (itemId == ITEM_WILDGROWTH) {
						itemId = ITEM_WILDGROWTH_NOPVP;
					}
				} else if (itemId == ITEM_FIREFIELD_PVP_FULL || itemId == ITEM_POISONFIELD_PVP || itemId == ITEM_ENERGYFIELD_PVP) {
					casterPlayer->addInFightTicks();
				}
			}
		}

		Item* item = Item::CreateItem(itemId);
		if (caster) {
			item->setOwner(caster->getID());
		}

		ReturnValue ret = g_game.internalAddItem(tile, item);
		if (ret == RETURNVALUE_NOERROR) {
			g_game.startDecay(item);
		} else {
			delete item;
		}
	}

	if (params.tileCallback) {
		params.tileCallback->onTileCombat(caster, tile);
	}

	if (params.impactEffect != CONST_ME_NONE) {
		Game::addMagicEffect(spectators, tile->getPosition(), params.impactEffect);
	}
}

void Combat::postCombatEffects(Creature* caster, const Position& pos, const CombatParams& params)
{
	if (caster && params.distanceEffect != CONST_ANI_NONE) {
		addDistanceEffect(caster, caster->getPosition(), pos, params.distanceEffect);
	}
}

void Combat::addDistanceEffect(Creature* caster, const Position& fromPos, const Position& toPos, uint8_t effect)
{
	if (effect == CONST_ANI_WEAPONTYPE) {
		if (!caster) {
			return;
		}

		Player* player = caster->getPlayer();
		if (!player) {
			return;
		}

		switch (player->getWeaponType()) {
			case WEAPON_AXE:
				effect = CONST_ANI_WHIRLWINDAXE;
				break;
			case WEAPON_SWORD:
				effect = CONST_ANI_WHIRLWINDSWORD;
				break;
			case WEAPON_CLUB:
				effect = CONST_ANI_WHIRLWINDCLUB;
				break;
			default:
				effect = CONST_ANI_NONE;
				break;
		}
	}

	if (effect != CONST_ANI_NONE) {
		g_game.addDistanceEffect(fromPos, toPos, effect);
	}
}

void Combat::doCombat(Creature* caster, Creature* target) const
{
	//target combat callback function
	if (params.combatType != COMBAT_NONE) {
		CombatDamage damage = getCombatDamage(caster, target);

		bool canCombat = !params.aggressive || (caster != target && Combat::canDoCombat(caster, target) == RETURNVALUE_NOERROR);
		if ((caster == target || canCombat) && params.impactEffect != CONST_ME_NONE) {
			g_game.addMagicEffect(target->getPosition(), params.impactEffect);
		}

		if (canCombat) {
			doTargetCombat(caster, target, damage, params);
		}
	} else {
		if (!params.aggressive || (caster != target && Combat::canDoCombat(caster, target) == RETURNVALUE_NOERROR)) {
			SpectatorVec spectators;
			g_game.map.getSpectators(spectators, target->getPosition(), true, true);

			if (params.origin != ORIGIN_MELEE) {
				for (const auto& condition : params.conditionList) {
					if (caster == target || !target->isImmune(condition->getType())) {
						Condition* conditionCopy = condition->clone();
						conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());
						target->addCombatCondition(conditionCopy);
					}
				}
			}

			if (params.dispelType & CONDITION_PARALYZE) {
				target->removeCondition(CONDITION_PARALYZE);
			} else {
				target->removeCombatCondition(params.dispelType);
			}

			combatTileEffects(spectators, caster, target->getTile(), params);

			if (params.targetCallback) {
				params.targetCallback->onTargetCombat(caster, target);
			}

			/*
			if (params.impactEffect != CONST_ME_NONE) {
				g_game.addMagicEffect(target->getPosition(), params.impactEffect);
			}
			*/

			if (caster && params.distanceEffect != CONST_ANI_NONE) {
				addDistanceEffect(caster, caster->getPosition(), target->getPosition(), params.distanceEffect);
			}
		}
	}
}

void Combat::doCombat(Creature* caster, const Position& position) const
{
	//area combat callback function
	if (params.combatType != COMBAT_NONE) {
		CombatDamage damage = getCombatDamage(caster, nullptr);
		doAreaCombat(caster, position, area.get(), damage, params);
	} else {
		auto tiles = caster ? getCombatArea(caster->getPosition(), position, area.get()) : getCombatArea(position, position, area.get());

		SpectatorVec spectators;
		uint32_t maxX = 0;
		uint32_t maxY = 0;

		//calculate the max viewable range
		for (Tile* tile : tiles) {
			const Position& tilePos = tile->getPosition();

			uint32_t diff = Position::getDistanceX(tilePos, position);
			if (diff > maxX) {
				maxX = diff;
			}

			diff = Position::getDistanceY(tilePos, position);
			if (diff > maxY) {
				maxY = diff;
			}
		}

		const int32_t rangeX = maxX + Map::maxViewportX;
		const int32_t rangeY = maxY + Map::maxViewportY;
		g_game.map.getSpectators(spectators, position, true, true, rangeX, rangeX, rangeY, rangeY);

		postCombatEffects(caster, position, params);

		for (Tile* tile : tiles) {
			if (canDoCombat(caster, tile, params.aggressive) != RETURNVALUE_NOERROR) {
				continue;
			}

			combatTileEffects(spectators, caster, tile, params);

			if (CreatureVector* creatures = tile->getCreatures()) {
				const Creature* topCreature = tile->getTopCreature();
				for (Creature* creature : *creatures) {
					if (params.targetCasterOrTopMost) {
						if (caster && caster->getTile() == tile) {
							if (creature != caster) {
								continue;
							}
						} else if (creature != topCreature) {
							continue;
						}
					}

					if (!params.aggressive || (caster != creature && Combat::canDoCombat(caster, creature) == RETURNVALUE_NOERROR)) {
						for (const auto& condition : params.conditionList) {
							if (caster == creature || !creature->isImmune(condition->getType())) {
								Condition* conditionCopy = condition->clone();
								if (caster) {
									conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());
								}

								//TODO: infight condition until all aggressive conditions has ended
								creature->addCombatCondition(conditionCopy);
							}
						}
					}

					if (params.dispelType & CONDITION_PARALYZE) {
						creature->removeCondition(CONDITION_PARALYZE);
					} else {
						creature->removeCombatCondition(params.dispelType);
					}

					if (params.targetCallback) {
						params.targetCallback->onTargetCombat(caster, creature);
					}

					if (params.targetCasterOrTopMost) {
						break;
					}
				}
			}
		}
	}
}

void Combat::doTargetCombat(Creature* caster, Creature* target, CombatDamage& damage, const CombatParams& params, bool sendDistanceEffect)
{	
	// To-do : I need to properly handle augment based damage which requires entire reworking of this method.
	// The thing that needs to happen is for augment based damage should not interact again with other aumgent
	// based damage. Instead of using origin for this, would possibly be better as fields on the combat or combat params.
	
	if (params.distanceEffect != CONST_ANI_NONE && sendDistanceEffect) {
		addDistanceEffect(caster, caster->getPosition(), target->getPosition(), params.distanceEffect);
	}
	// To-do : Get rid of these optionals (unsure, but I think they are a leak)
	// by adding isPlayer and isMonster member methods to all the Creature classes.
	std::optional<Player*> casterPlayer = caster && caster->getPlayer() ? std::optional<Player*>(caster->getPlayer()) : std::nullopt;
	std::optional<Player*> targetPlayer = target && target->getPlayer() ? std::optional<Player*>(target->getPlayer()) : std::nullopt;
	std::optional<Monster*> casterMonster = caster && caster->getMonster() ? std::optional<Monster*>(caster->getMonster()) : std::nullopt;

	std::unordered_map<uint8_t, ModifierTotals> attackModData;
	
	if (casterPlayer.has_value() && target) {
		attackModData.reserve(ATTACK_MODIFIER_LAST);
		auto targetType = CREATURETYPE_ATTACKABLE;
		// to-do: this is ugly, lets make it a function and assign its return to the variable above instead.
		if (target->getMonster()) {
			targetType = casterPlayer.value()->getCreatureType(*target->getMonster());
		} else if (target->getPlayer()) {
			targetType = CREATURETYPE_PLAYER;
		}
		attackModData = casterPlayer.value()->getAttackModifierTotals(damage.primary.type, damage.origin, targetType, target->getRace(), target->getName());
		/// we do conversion here incase someone wants to convert say healing to mana or mana to death.

		const auto& conversionTotals = casterPlayer.value()->getConvertedTotals(ATTACK_MODIFIER_CONVERSION, damage.primary.type, damage.origin, targetType, target->getRace(), target->getName()) ;
		if (!conversionTotals.empty() && params.origin != ORIGIN_AUGMENT) {
			casterPlayer.value()->convertDamage(target->getCreature(), damage, conversionTotals);
			if (damage.primary.value == 0) {
				return;
			}
		}

		if (damage.primary.type != COMBAT_MANADRAIN && damage.primary.type != COMBAT_HEALING) {
			// to-do: checking against origin for augment is too limiting.. Lets make piercing like crit and leech, ect.
			if (!attackModData.empty() && params.origin != ORIGIN_PIERCING) {
				const auto& [piercingFlatTotal, piercingPercentTotal] = attackModData[ATTACK_MODIFIER_PIERCING];

				int32_t piercingDamage = 0;
				const auto& originalDamage = std::abs(damage.primary.value);
				if (piercingPercentTotal) {
					const auto& piercePercent = static_cast<int32_t>(piercingPercentTotal);

					piercingDamage = (piercingPercentTotal <= 100)
						? (originalDamage * piercePercent / 100)
						: damage.primary.value;
					
				}

				if (piercingFlatTotal) {
					piercingDamage += static_cast<int32_t>(piercingFlatTotal);
				}

				if (piercingDamage) {
					piercingDamage = std::min<int32_t>(piercingDamage, originalDamage);
					damage.primary.value += piercingDamage;
					
					CombatDamage piercing;
					piercing.origin = ORIGIN_AUGMENT;
					piercing.primary.value = (0 - piercingDamage);
					piercing.primary.type = COMBAT_UNDEFINEDDAMAGE;

					CombatParams piercingParams;
					piercingParams.origin = ORIGIN_AUGMENT;
					piercingParams.combatType = COMBAT_UNDEFINEDDAMAGE;
					piercingParams.impactEffect = CONST_ME_SKULLHORIZONTAL;

					const auto message = "You pierced " + target->getName() + " for " + std::to_string(piercingDamage) + " damage!" ;
					casterPlayer.value()->sendTextMessage(MESSAGE_EVENT_DEFAULT, message);
					g_game.combatChangeHealth(caster, target, piercing);

					if (damage.primary.value == 0) {
						return;
					}
				}
			}

			if (params.origin != ORIGIN_PIERCING) {
			auto blocked =g_game.combatBlockHit(damage, caster, target, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);
				if (blocked) {
					return;
				}
			}

			if (!damage.critical) {
				int32_t percentTotal = 0;
				int32_t flatTotal = 0;
				if (!attackModData.empty()) {
					percentTotal = attackModData[ATTACK_MODIFIER_CRITICAL].percentTotal;
					flatTotal = attackModData[ATTACK_MODIFIER_CRITICAL].flatTotal;
				}

				// normal crits are the old ones and are percent based
				const auto& normalCritChance = static_cast<int32_t>(casterPlayer.value()->getSpecialSkill(SPECIALSKILL_CRITICALHITCHANCE));
				const auto& normalCritDamage = static_cast<int32_t>(casterPlayer.value()->getSpecialSkill(SPECIALSKILL_CRITICALHITAMOUNT));

				// note : the way this works, its own damage increase is independent allowing for more than 100
				// and also at the same time, its chance is independent, so it doesn't add to augmented crit's chance.
				if ((normalCritChance && normalCritDamage) && (normal_random(1, 100) <= normalCritChance)) {
					percentTotal += normalCritDamage;
				}

				// we do percent based crits first, so that the flat damage doesn't add to the percent increase.
				if (percentTotal) {
					auto damageIncrease = std::abs(damage.primary.value * percentTotal / 100);
					damage.primary.value -= damageIncrease;
					damage.critical = true;
				}

				if (flatTotal) {
					damage.primary.value -= flatTotal;
					damage.critical = true;
				}
			}

			if (targetPlayer.has_value() && (casterPlayer.value() != targetPlayer.value()) && params.origin != ORIGIN_AUGMENT) {
				const auto& reformTotals = targetPlayer.value()->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
				if (!reformTotals.empty()) {
					targetPlayer.value()->reformDamage(*casterPlayer.value()->getCreature(), damage, reformTotals);
					if (damage.primary.value == 0) {
						return;
					}
				}

				const auto& defenseModData = targetPlayer.value()->getDefenseModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
				if (!defenseModData.empty()) {
					for (const auto& [modkind, modTotals] : defenseModData) {
						if (modTotals.percentTotal  || modTotals.flatTotal) {
							applyDamageReductionModifier(modkind, damage, *targetPlayer.value()->getPlayer(), *caster->getCreature(), static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);
							if (damage.primary.value == 0) {
								return;
							}
						}
					}
				}
			}
			
		}
	} else if (casterMonster.has_value()) {
		if (g_game.combatBlockHit(damage, caster, target, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances)) {
			return;
		}

		if (targetPlayer.has_value()) {
			const auto& attackerType = targetPlayer.value()->getCreatureType(*casterMonster.value()->getMonster());
			const auto& defenseModData = targetPlayer.value()->getDefenseModifierTotals(damage.primary.type, damage.origin, attackerType, casterMonster.value()->getRace(), casterMonster.value()->getName());
			auto reformTotals = targetPlayer.value()->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, attackerType, casterMonster.value()->getRace(), casterMonster.value()->getName());
			if (!reformTotals.empty() && params.origin != ORIGIN_AUGMENT) {
				targetPlayer.value()->reformDamage(*casterMonster.value()->getCreature(), damage, reformTotals);
				if (damage.primary.value == 0) {
					return;
				}
			}

			if (!defenseModData.empty() && params.origin != ORIGIN_AUGMENT) {
				for (const auto& [modkind, modTotals] : defenseModData) {
					if (modTotals.percentTotal || modTotals.flatTotal) {
						applyDamageReductionModifier(modkind, damage, *targetPlayer.value()->getPlayer(), *caster->getCreature(), static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);
						if (damage.primary.value == 0) {
							return;
						}
					}
				}
			}
		}
	} else [[unlikely]] {
		std::cout << "Getting calls to doTargetCombat without a target. \n";
	}

	auto success = (damage.primary.type == COMBAT_MANADRAIN) ?
		g_game.combatChangeMana(caster, target, damage) :
		g_game.combatChangeHealth(caster, target, damage);
	
	if (success) {

		if (target && caster && target != caster) {
			if (damage.critical) {
				g_game.addMagicEffect(target->getPosition(), CONST_ME_CRITICAL_DAMAGE);
			}

			for (const auto& condition : params.conditionList)
			{
				if (!target->isImmune(condition->getType())) {
					Condition* conditionCopy = condition->clone();
					if (caster) {
						conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());
					}
					//TODO: infight condition until all aggressive conditions has ended
					target->addCombatCondition(conditionCopy);
				}
			}
			// hopefully runes are counted as spells and not ranged
			if (casterPlayer.has_value()
				&& (damage.origin == ORIGIN_MELEE || damage.origin == ORIGIN_RANGED)
				&& (damage.primary.type != COMBAT_HEALING && damage.primary.type != COMBAT_MANADRAIN )) {
				for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {

					Item* item = casterPlayer.value()->getInventoryItem(static_cast<slots_t>(slot));
					if (!item) {
						continue;
					}
					if (item->hasImbuements()) {
						
						for (auto& imbuement : item->getImbuements()) {
							if (!imbuement->value) {
								continue;
							}
							const auto& originalDamage = abs(damage.primary.value);
							const auto& conversionAmount = (originalDamage * imbuement->value) / 100;
							const int32_t& difference = (originalDamage - conversionAmount);
							
							CombatDamage imbueDamage;
							imbueDamage.blockType = BLOCK_NONE;
							imbueDamage.origin = ORIGIN_IMBUEMENT;

							switch (imbuement->imbuetype) {
								case IMBUEMENT_TYPE_FIRE_DAMAGE:
									imbueDamage.primary.type = COMBAT_FIREDAMAGE;
									break;
								case IMBUEMENT_TYPE_ENERGY_DAMAGE:
									imbueDamage.primary.type = COMBAT_ENERGYDAMAGE;
									break;
								case IMBUEMENT_TYPE_EARTH_DAMAGE:
									imbueDamage.primary.type = COMBAT_EARTHDAMAGE;
									break;
								case IMBUEMENT_TYPE_ICE_DAMAGE:
									imbueDamage.primary.type = COMBAT_ICEDAMAGE;
									break;
								case IMBUEMENT_TYPE_HOLY_DAMAGE:
									imbueDamage.primary.type = COMBAT_HOLYDAMAGE;
									break;
								case IMBUEMENT_TYPE_DEATH_DAMAGE:
									imbueDamage.primary.type = COMBAT_DEATHDAMAGE;
									break;
								default: [[unlikely]]
									break;
							}

							if (difference < 0) {
								imbueDamage.primary.value -= originalDamage;
								g_game.combatChangeHealth(caster, target, imbueDamage);
								break;
							} // else
							imbueDamage.primary.value -= conversionAmount;
							g_game.combatChangeHealth(caster, target, imbueDamage);
						}
					}
				}
			}

				if (targetPlayer.has_value() && damage.primary.type != COMBAT_HEALING && damage.primary.type != COMBAT_MANADRAIN ) {
					for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot)
					{
						Item* item = targetPlayer.value()->getInventoryItem(static_cast<slots_t>(slot));
						if (!item) {
							continue;
						}

						if (item->hasImbuements()) {
							for (const auto& imbuement : item->getImbuements()) {
								const auto combatType = damage.primary.type;
								const auto& originalDamage = abs(damage.primary.value);
								const auto& resistance = (originalDamage * imbuement->value) / 100;
								const int32_t& difference = (originalDamage - resistance);
								switch (imbuement->imbuetype) {
								case ImbuementType::IMBUEMENT_TYPE_FIRE_RESIST:
									if (combatType == COMBAT_FIREDAMAGE) {
										if (difference < 0) {
											damage.primary.value = 0;
											return;
										}
										damage.primary.value += difference;
									}
									break;
								case ImbuementType::IMBUEMENT_TYPE_EARTH_RESIST:
									if (combatType == COMBAT_EARTHDAMAGE) {
										if (difference < 0) {
											damage.primary.value = 0;
											return;
										}
										damage.primary.value += difference;
									}
									break;
								case ImbuementType::IMBUEMENT_TYPE_ICE_RESIST:
									if (combatType == COMBAT_ICEDAMAGE) {
										if (difference < 0) {
											damage.primary.value = 0;
											return;
										}
										damage.primary.value += difference;
									}
									break;
								case ImbuementType::IMBUEMENT_TYPE_ENERGY_RESIST:
									if (combatType == COMBAT_ENERGYDAMAGE) {
										if (difference < 0) {
											damage.primary.value = 0;
											return;
										}
										damage.primary.value += difference;
									}
									break;
								case ImbuementType::IMBUEMENT_TYPE_DEATH_RESIST:
									if (combatType == COMBAT_DEATHDAMAGE) {
										if (difference < 0) {
											damage.primary.value = 0;
											return;
										}
										damage.primary.value += difference;
									}
									break;
								case ImbuementType::IMBUEMENT_TYPE_HOLY_RESIST:
									if (combatType == COMBAT_HOLYDAMAGE) {
										if (difference < 0) {
											damage.primary.value = 0;
											return;
										}
										damage.primary.value += difference;
									}
									break;
								default: [[unlikely]]
									break;
								}
							}
						}
					}
				}

			if (!damage.leeched && damage.primary.type != COMBAT_HEALING
				&& casterPlayer
				&& damage.origin != ORIGIN_CONDITION) {
				const auto totalDamage = std::abs(damage.primary.value + damage.secondary.value);
				int32_t lifeStealPercentTotal = 0, manaStealPercentTotal = 0, staminaStealPercentTotal = 0, soulStealPercentTotal = 0;
				int32_t lifeStealFlatTotal = 0, manaStealFlatTotal = 0, staminaStealFlatTotal = 0, soulStealFlatTotal = 0;
				int32_t lifeStealGain = 0, manaStealGain = 0, soulGain = 0, staminaGain = 0;
				
				// Static cast everything to int32_t to ensure consistency
				if (!attackModData.empty() && params.origin != ORIGIN_AUGMENT) {
					// Percents
					lifeStealPercentTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_LIFESTEAL].percentTotal);
					manaStealPercentTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_MANASTEAL].percentTotal);
					staminaStealPercentTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_STAMINASTEAL].percentTotal);
					soulStealPercentTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_SOULSTEAL].percentTotal);

					// Flats
					lifeStealFlatTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_LIFESTEAL].flatTotal);
					manaStealFlatTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_MANASTEAL].flatTotal);
					staminaStealFlatTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_STAMINASTEAL].flatTotal);
					soulStealFlatTotal = static_cast<int32_t>(attackModData[ATTACK_MODIFIER_SOULSTEAL].flatTotal);
				}

				const auto lifeLeechChance = static_cast<int32_t>(casterPlayer.value()->getSpecialSkill(SPECIALSKILL_LIFELEECHCHANCE));
				const auto lifeLeechAmount = static_cast<int32_t>(casterPlayer.value()->getSpecialSkill(SPECIALSKILL_LIFELEECHAMOUNT));
				const auto manaLeechChance = static_cast<int32_t>(casterPlayer.value()->getSpecialSkill(SPECIALSKILL_MANALEECHCHANCE));
				const auto manaLeechAmount = static_cast<int32_t>(casterPlayer.value()->getSpecialSkill(SPECIALSKILL_MANALEECHAMOUNT));

				// Lifesteal
				if ((lifeLeechChance && lifeLeechAmount) && (normal_random(1, 100) <= lifeLeechChance)) {
					lifeStealGain += totalDamage * lifeLeechAmount / 100;
				}

				if (lifeStealPercentTotal) {
					lifeStealGain += totalDamage * lifeStealPercentTotal / 100;
				}

				if (lifeStealFlatTotal) {
					lifeStealGain += (lifeStealFlatTotal);
				}

				if (lifeStealGain) {
					CombatDamage lifeStealCombat;
					lifeStealCombat.origin = ORIGIN_AUGMENT;
					lifeStealCombat.leeched = true;
					lifeStealCombat.primary.type = COMBAT_LIFEDRAIN;
					lifeStealCombat.primary.value = lifeStealGain;
					g_game.combatChangeHealth(target, caster, lifeStealCombat);
				}

				/// Manasteal
				if ((manaLeechChance && manaLeechAmount) && (normal_random(1, 100) <= manaLeechChance)) {
					manaStealGain += totalDamage * manaLeechAmount / 100;
				}

				if (manaStealPercentTotal) {
					manaStealGain += totalDamage * manaStealPercentTotal / 100;
				}

				if (manaStealFlatTotal) {
					manaStealGain += manaStealFlatTotal;
				}

				if (manaStealGain) {
					CombatDamage manaStealCombat;
					manaStealCombat.origin = ORIGIN_AUGMENT;
					manaStealCombat.leeched = true;
					manaStealCombat.primary.type = COMBAT_MANADRAIN;
					manaStealCombat.primary.value = manaStealGain;
					g_game.combatChangeMana(target, caster, manaStealCombat);
				}

				/// Staminasteal
				if (staminaStealPercentTotal) {
					staminaGain += totalDamage * staminaStealPercentTotal / 100;
				}

				if (staminaStealFlatTotal) {
					staminaGain += staminaStealFlatTotal;
				}

				if (staminaGain) {
					if (staminaGain <= std::numeric_limits<uint16_t>::max()) {
						const uint16_t trueStaminaGain = g_config.getBoolean(ConfigManager::AUGMENT_STAMINA_RULE) ?
							static_cast<uint16_t>(staminaGain) :
							static_cast<uint16_t>(staminaGain / 60);
						
						const uint16_t currentStamina = casterPlayer.value()->getStaminaMinutes();
						const uint16_t missingStamina = (MaximumStamina - currentStamina);
						if ((trueStaminaGain + currentStamina) >= missingStamina) {
							casterPlayer.value()->addStamina(missingStamina);
						} else {
							casterPlayer.value()->addStamina(trueStaminaGain);
						}
					} else {
						casterPlayer.value()->addStamina(MaximumStamina - casterPlayer.value()->getStaminaMinutes());
					}
					g_game.addMagicEffect(casterPlayer.value()->getPosition(), CONST_ME_YELLOWENERGY);
				}

				// Soulsteal
				if (soulStealPercentTotal) {
					soulGain += totalDamage * soulStealPercentTotal /  100 ;
				}

				if (soulStealFlatTotal) {
					soulGain += soulStealFlatTotal;
				}

				if (soulGain) {
					if (soulGain <= std::numeric_limits<uint8_t>::max()) {
						const uint8_t trueSoulGain = static_cast<uint8_t>(soulGain);
						const uint8_t currentSoul = casterPlayer.value()->getSoul();
						const uint8_t maxSoul = casterPlayer.value()->getVocation()->getSoulMax();
						const uint8_t missingSoul = (maxSoul - currentSoul);
						if ((trueSoulGain + currentSoul) >= maxSoul) {
							casterPlayer.value()->addSoul(missingSoul);
						} else {
							casterPlayer.value()->addSoul(trueSoulGain);
						}
					} else {
						casterPlayer.value()->addSoul(casterPlayer.value()->getVocation()->getSoulMax() - casterPlayer.value()->getSoul());
					}

					g_game.addMagicEffect(casterPlayer.value()->getPosition(), CONST_ME_MAGIC_GREEN);
				}
			}
		}
		if (params.dispelType & CONDITION_PARALYZE) {
			target->removeCondition(CONDITION_PARALYZE);
		} else {
			target->removeCombatCondition(params.dispelType);
		}
	}

	if (params.targetCallback) {
		params.targetCallback->onTargetCombat(caster, target);
	}
}

void Combat::doAreaCombat(Creature* caster, const Position& position, const AreaCombat* area, CombatDamage& damage, const CombatParams& params)
{
	auto tiles = caster ? getCombatArea(caster->getPosition(), position, area) : getCombatArea(position, position, area);

	Player* casterPlayer = caster ? caster->getPlayer() : nullptr;

	uint32_t maxX = 0;
	uint32_t maxY = 0;

	//calculate the max viewable range
	for (Tile* tile : tiles) {
		const Position& tilePos = tile->getPosition();

		uint32_t diff = Position::getDistanceX(tilePos, position);
		if (diff > maxX) {
			maxX = diff;
		}

		diff = Position::getDistanceY(tilePos, position);
		if (diff > maxY) {
			maxY = diff;
		}
	}

	const int32_t rangeX = maxX + Map::maxViewportX;
	const int32_t rangeY = maxY + Map::maxViewportY;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, position, true, true, rangeX, rangeX, rangeY, rangeY);

	postCombatEffects(caster, position, params);

	std::vector<Creature*> toDamageCreatures;
	toDamageCreatures.reserve(100);

	for (Tile* tile : tiles) {
		if (canDoCombat(caster, tile, params.aggressive) != RETURNVALUE_NOERROR) {
			continue;
		}

		combatTileEffects(spectators, caster, tile, params);

		if (CreatureVector* creatures = tile->getCreatures()) {
			const Creature* topCreature = tile->getTopCreature();
			for (Creature* creature : *creatures) {
				if (params.targetCasterOrTopMost) {
					if (caster && caster->getTile() == tile) {
						if (creature != caster) {
							continue;
						}
					} else if (creature != topCreature) {
						continue;
					}
				}

				if (!params.aggressive || (caster != creature && Combat::canDoCombat(caster, creature) == RETURNVALUE_NOERROR)) {
					toDamageCreatures.push_back(creature);

					if (params.targetCasterOrTopMost) {
						break;
					}
				}
			}
		}
	}

	for (Creature* target : toDamageCreatures) {
		CombatDamage damageCopy = damage; // we cannot avoid copying here, because we don't know if it's player combat or not, so we can't modify the initial damage.
		Combat::doTargetCombat(caster, target, damageCopy, params, false);
	}
}

void Combat::applyDamageIncreaseModifier(uint8_t modifierType, CombatDamage& damage, int32_t percentValue, int32_t flatValue) {

	if (percentValue) {
		if (percentValue <= 100) {
			damage.primary.value += damage.primary.value * (percentValue / 100.0);
		} else {
			damage.primary.value *= 2;
		}
	}
	if (percentValue) {
		damage.primary.value += percentValue;
	}

}

void Combat::applyDamageReductionModifier(uint8_t modifierType, CombatDamage& damage, Player& damageTarget, std::optional<std::reference_wrapper<Creature>> attacker, int32_t percent, int32_t flat, CombatOrigin paramOrigin, uint8_t areaEffect, uint8_t distanceEffect) {

	switch (modifierType) {
		case DEFENSE_MODIFIER_ABSORB:
			damageTarget.absorbDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_RESTORE:
			damageTarget.restoreManaFromDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_REPLENISH:
			damageTarget.replenishStaminaFromDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_RESIST:
			damageTarget.resistDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_REVIVE:
			damageTarget.reviveSoulFromDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_REFLECT:
			damageTarget.reflectDamage(attacker, damage, percent, flat, areaEffect, distanceEffect);
			return;

		case DEFENSE_MODIFIER_DEFLECT:
			damageTarget.deflectDamage(attacker, damage, percent, flat, paramOrigin, areaEffect, distanceEffect);
			return;

		case DEFENSE_MODIFIER_RICOCHET:
			damageTarget.ricochetDamage(damage, percent, flat, areaEffect, distanceEffect);
			return;

		default:
			return;
	}
}

//**********************************************************//

void ValueCallback::getMinMaxValues(Player* player, CombatDamage& damage) const
{
	//onGetPlayerMinMaxValues(...)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - ValueCallback::getMinMaxValues] Call stack overflow" << std::endl;
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	if (!env->setCallbackId(scriptId, scriptInterface)) {
		scriptInterface->resetScriptEnv();
		return;
	}

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushUserdata<Player>(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	int parameters = 1;
	switch (type) {
		case COMBAT_FORMULA_LEVELMAGIC: {
			//onGetPlayerMinMaxValues(player, level, maglevel)
			lua_pushinteger(L, player->getLevel());
			lua_pushinteger(L, player->getMagicLevel());
			parameters += 2;
			break;
		}

		case COMBAT_FORMULA_SKILL: {
			//onGetPlayerMinMaxValues(player, attackSkill, attackValue, attackFactor)
			Item* tool = player->getWeapon();
			const Weapon* weapon = g_weapons->getWeapon(tool);
			Item* item = nullptr;

			int32_t attackValue = 7;
			if (weapon) {
				attackValue = tool->getAttack();
				if (tool->getWeaponType() == WEAPON_AMMO) {
					item = player->getWeapon(true);
					if (item) {
						attackValue += item->getAttack();
					}
				}

				damage.secondary.type = weapon->getElementType();
				damage.secondary.value = weapon->getElementDamage(player, nullptr, tool);
			}

			lua_pushinteger(L, player->getWeaponSkill(item ? item : tool));
			lua_pushinteger(L, attackValue);
			lua_pushnumber(L, player->getAttackFactor());
			parameters += 3;
			break;
		}

		default: {
			std::cout << "ValueCallback::getMinMaxValues - unknown callback type" << std::endl;
			scriptInterface->resetScriptEnv();
			return;
		}
	}

	int size0 = lua_gettop(L);
	if (lua_pcall(L, parameters, 2, 0) != 0) {
		LuaScriptInterface::reportError(nullptr, LuaScriptInterface::popString(L));
	} else {
		damage.primary.value = normal_random(
			LuaScriptInterface::getNumber<int32_t>(L, -2),
			LuaScriptInterface::getNumber<int32_t>(L, -1)
		);
		lua_pop(L, 2);
	}

	if ((lua_gettop(L) + parameters + 1) != size0) {
		LuaScriptInterface::reportError(nullptr, "Stack size changed!");
	}

	scriptInterface->resetScriptEnv();
}

//**********************************************************//

void TileCallback::onTileCombat(Creature* creature, Tile* tile) const
{
	//onTileCombat(creature, pos)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - TileCallback::onTileCombat] Call stack overflow" << std::endl;
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	if (!env->setCallbackId(scriptId, scriptInterface)) {
		scriptInterface->resetScriptEnv();
		return;
	}

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	if (creature) {
		LuaScriptInterface::pushUserdata<Creature>(L, creature);
		LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	} else {
		lua_pushnil(L);
	}
	LuaScriptInterface::pushPosition(L, tile->getPosition());

	scriptInterface->callFunction(2);
}

//**********************************************************//

void TargetCallback::onTargetCombat(Creature* creature, Creature* target) const
{
	//onTargetCombat(creature, target)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - TargetCallback::onTargetCombat] Call stack overflow" << std::endl;
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	if (!env->setCallbackId(scriptId, scriptInterface)) {
		scriptInterface->resetScriptEnv();
		return;
	}

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	if (creature) {
		LuaScriptInterface::pushUserdata<Creature>(L, creature);
		LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	} else {
		lua_pushnil(L);
	}

	if (target) {
		LuaScriptInterface::pushUserdata<Creature>(L, target);
		LuaScriptInterface::setCreatureMetatable(L, -1, target);
	} else {
		lua_pushnil(L);
	}

	int size0 = lua_gettop(L);

	if (lua_pcall(L, 2, 0 /*nReturnValues*/, 0) != 0) {
		LuaScriptInterface::reportError(nullptr, LuaScriptInterface::popString(L));
	}

	if ((lua_gettop(L) + 2 /*nParams*/ + 1) != size0) {
		LuaScriptInterface::reportError(nullptr, "Stack size changed!");
	}

	scriptInterface->resetScriptEnv();
}

const MatrixArea& AreaCombat::getArea(const Position& centerPos, const Position& targetPos) const {
	int32_t dx = Position::getOffsetX(targetPos, centerPos);
	int32_t dy = Position::getOffsetY(targetPos, centerPos);

	Direction dir;
	if (dx < 0) {
		dir = DIRECTION_WEST;
	} else if (dx > 0) {
		dir = DIRECTION_EAST;
	} else if (dy < 0) {
		dir = DIRECTION_NORTH;
	} else {
		dir = DIRECTION_SOUTH;
	}

	if (hasExtArea) {
		if (dx < 0 && dy < 0) {
			dir = DIRECTION_NORTHWEST;
		} else if (dx > 0 && dy < 0) {
			dir = DIRECTION_NORTHEAST;
		} else if (dx < 0 && dy > 0) {
			dir = DIRECTION_SOUTHWEST;
		} else if (dx > 0 && dy > 0) {
			dir = DIRECTION_SOUTHEAST;
		}
	}

	if (dir >= areas.size()) {
		// this should not happen. it means we forgot to call setupArea.
		static MatrixArea empty;
		return empty;
	}
	return areas[dir];
}

void AreaCombat::setupArea(const std::vector<uint32_t>& vec, uint32_t rows)
{
	auto area = createArea(vec, rows);
	if (areas.size() == 0) {
		areas.resize(4);
	}

	areas[DIRECTION_EAST] = area.rotate90();
	areas[DIRECTION_SOUTH] = area.rotate180();
	areas[DIRECTION_WEST] = area.rotate270();
	areas[DIRECTION_NORTH] = std::move(area);
}

void AreaCombat::setupArea(int32_t length, int32_t spread)
{
	uint32_t rows = length;
	int32_t cols = 1;

	if (spread != 0) {
		cols = ((length - (length % spread)) / spread) * 2 + 1;
	}

	int32_t colSpread = cols;

	std::vector<uint32_t> vec;
	vec.reserve(rows * cols);
	for (uint32_t y = 1; y <= rows; ++y) {
		int32_t mincol = cols - colSpread + 1;
		int32_t maxcol = cols - (cols - colSpread);

		for (int32_t x = 1; x <= cols; ++x) {
			if (y == rows && x == ((cols - (cols % 2)) / 2) + 1) {
				vec.push_back(3);
			} else if (x >= mincol && x <= maxcol) {
				vec.push_back(1);
			} else {
				vec.push_back(0);
			}
		}

		if (spread > 0 && y % spread == 0) {
			--colSpread;
		}
	}

	setupArea(vec, rows);
}

void AreaCombat::setupArea(int32_t radius)
{
	int32_t area[13][13] = {
		{0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 8, 8, 7, 8, 8, 0, 0, 0, 0},
		{0, 0, 0, 8, 7, 6, 6, 6, 7, 8, 0, 0, 0},
		{0, 0, 8, 7, 6, 5, 5, 5, 6, 7, 8, 0, 0},
		{0, 8, 7, 6, 5, 4, 4, 4, 5, 6, 7, 8, 0},
		{0, 8, 6, 5, 4, 3, 2, 3, 4, 5, 6, 8, 0},
		{8, 7, 6, 5, 4, 2, 1, 2, 4, 5, 6, 7, 8},
		{0, 8, 6, 5, 4, 3, 2, 3, 4, 5, 6, 8, 0},
		{0, 8, 7, 6, 5, 4, 4, 4, 5, 6, 7, 8, 0},
		{0, 0, 8, 7, 6, 5, 5, 5, 6, 7, 8, 0, 0},
		{0, 0, 0, 8, 7, 6, 6, 6, 7, 8, 0, 0, 0},
		{0, 0, 0, 0, 8, 8, 7, 8, 8, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0}
	};

	std::vector<uint32_t> vec;
	vec.reserve(13 * 13);
	for (auto& row : area) {
		for (int cell : row) {
			if (cell == 1) {
				vec.push_back(3);
			} else if (cell > 0 && cell <= radius) {
				vec.push_back(1);
			} else {
				vec.push_back(0);
			}
		}
	}

	setupArea(vec, 13);
}

void AreaCombat::setupExtArea(const std::vector<uint32_t>& vec, uint32_t rows)
{
	if (vec.empty()) {
		return;
	}

	hasExtArea = true;
	auto area = createArea(vec, rows);
	areas.resize(8);
	areas[DIRECTION_NORTHEAST] = area.rotate90();
	areas[DIRECTION_SOUTHEAST] = area.rotate180();
	areas[DIRECTION_SOUTHWEST] = area.rotate270();
	areas[DIRECTION_NORTHWEST] = std::move(area);
}

//**********************************************************//

void MagicField::onStepInField(Creature* creature)
{
	//remove magic walls/wild growth
	if (id == ITEM_MAGICWALL || id == ITEM_WILDGROWTH || id == ITEM_MAGICWALL_SAFE || id == ITEM_WILDGROWTH_SAFE || isBlocking()) {
		if (!creature->isInGhostMode()) {
			g_game.internalRemoveItem(this, 1);
		}

		return;
	}

	//remove magic walls/wild growth (only nopvp tiles/world)
	if (id == ITEM_MAGICWALL_NOPVP || id == ITEM_WILDGROWTH_NOPVP) {
		if (g_game.getWorldType() == WORLD_TYPE_NO_PVP || getTile()->hasFlag(TILESTATE_NOPVPZONE)) {
			g_game.internalRemoveItem(this, 1);
		}
		return;
	}

	const ItemType& it = items[getID()];
	if (it.conditionDamage) {
		Condition* conditionCopy = it.conditionDamage->clone();
		uint32_t ownerId = getOwner();
		if (ownerId) {
			bool harmfulField = true;

			if (g_game.getWorldType() == WORLD_TYPE_NO_PVP || getTile()->hasFlag(TILESTATE_NOPVPZONE)) {
				Creature* owner = g_game.getCreatureByID(ownerId);
				if (owner) {
					if (owner->getPlayer() || (owner->isSummon() && owner->getMaster()->getPlayer())) {
						harmfulField = false;
					}
				}
			}

			Player* targetPlayer = creature->getPlayer();
			if (targetPlayer) {
				Player* attackerPlayer = g_game.getPlayerByID(ownerId);
				if (attackerPlayer) {
					if (Combat::isProtected(attackerPlayer, targetPlayer)) {
						harmfulField = false;
					}
				}
			}

			if (!harmfulField || (OTSYS_TIME() - createTime <= 5000) || creature->hasBeenAttacked(ownerId)) {
				conditionCopy->setParam(CONDITION_PARAM_OWNER, ownerId);
			}
		}

		creature->addCondition(conditionCopy);
	}
}
