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

static std::vector<TilePtr> getList(const MatrixArea& area, const Position& targetPos, const Direction dir)
{
	const auto& casterPos = getNextPosition(dir, targetPos);

	std::vector<TilePtr> vec;

	const auto& center = area.getCenter();

	Position tmpPos(targetPos.x - center.first, targetPos.y - center.second, targetPos.z);
	for (uint32_t row = 0; row < area.getRows(); ++row, ++tmpPos.y) {
		for (uint32_t col = 0; col < area.getCols(); ++col, ++tmpPos.x) {
			if (area(row, col)) {
				if (g_game.isSightClear(casterPos, tmpPos, true)) {
					auto tile = g_game.map.getTile(tmpPos);
					if (!tile) {
						tile = std::make_shared<Tile>(tmpPos.x, tmpPos.y, tmpPos.z);
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

std::vector<TilePtr> getCombatArea(const Position& centerPos, const Position& targetPos, const AreaCombat* area)
{
	if (targetPos.z >= MAP_MAX_LAYERS) {
		return {};
	}

	if (area) {
		return getList(area->getArea(centerPos, targetPos), targetPos, getDirectionTo(targetPos, centerPos));
	}

	auto tile = g_game.map.getTile(targetPos);
	if (!tile) {
		tile = std::make_shared<Tile>(targetPos.x, targetPos.y, targetPos.z);
		g_game.map.setTile(targetPos, tile);
	}
	return {tile};
}

CombatDamage Combat::getCombatDamage(const CreaturePtr& creature, const CreaturePtr& target) const
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
		// to-do rework : getCombatValues(min, max) to instead return {min, max}
		if (creature->getCombatValues(min, max)) {
			damage.primary.value = normal_random(min, max);
		} else if (const auto& player = creature->getPlayer()) {
			if (params.valueCallback) {
				params.valueCallback->getMinMaxValues(player, damage);
			} else if (formulaType == COMBAT_FORMULA_LEVELMAGIC) {
				int32_t levelFormula = player->getLevel() * 2 + player->getMagicLevel() * 3;
				damage.primary.value = normal_random(std::fma(levelFormula, mina, minb), std::fma(levelFormula, maxa, maxb));
			} else if (formulaType == COMBAT_FORMULA_SKILL) {
				const auto& tool = player->getWeapon();
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

CombatType_t Combat::ConditionToDamageType(const ConditionType_t type)
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

		[[unlikely]]
		default:
			return COMBAT_NONE;
	}	
}

ConditionType_t Combat::DamageToConditionType(const CombatType_t type)
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

		[[unlikely]]
		default:
			return CONDITION_NONE;
	}
}

bool Combat::isPlayerCombat(const CreatureConstPtr& target)
{
	if (target->getPlayer()) {
		return true;
	}

	if (target->isSummon() and target->getMaster()->getPlayer()) {
		return true;
	}

	return false;
}

ReturnValue Combat::canTargetCreature(const PlayerPtr& attacker, const CreaturePtr& target)
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

	if (attacker->hasFlag(PlayerFlag_CannotUseCombat) or !target->isAttackable()) {
		if (target->getPlayer()) {
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		} else {
			return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
		}
	}
	// we use implicit conversion here to get the const
	// auto won't work for that as it will not know we need the const version
	if (const PlayerConstPtr& p_target = target->getPlayer()) {
		if (isProtected(attacker, p_target)) {
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		}

		if (attacker->hasSecureMode() and not Combat::isInPvpZone(attacker, target) and attacker->getSkullClient(target->getPlayer()) == SKULL_NONE) {
			return RETURNVALUE_TURNSECUREMODETOATTACKUNMARKEDPLAYERS;
		}
	}

	return Combat::canDoCombat(attacker, target);
}

ReturnValue Combat::canDoCombat(const CreaturePtr& caster, const TilePtr& tile, bool aggressive)
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

		if (const auto& player = caster->getPlayer()) {
			if (player->hasFlag(PlayerFlag_IgnoreProtectionZone)) {
				return RETURNVALUE_NOERROR;
			}
		}
	}

	//pz-zone
	if (aggressive and tile->hasFlag(TILESTATE_PROTECTIONZONE)) {
		return RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE;
	}

	return g_events->eventCreatureOnAreaCombat(caster, tile, aggressive);
}

bool Combat::isInPvpZone(const CreatureConstPtr& attacker, const CreatureConstPtr& target)
{
	return attacker->getZone() == ZONE_PVP and target->getZone() == ZONE_PVP;
}

bool Combat::isProtected(const PlayerConstPtr& attacker, const PlayerConstPtr& target)
{
	uint32_t protectionLevel = g_config.getNumber(ConfigManager::PROTECTION_LEVEL);
	if (target->getLevel() < protectionLevel or attacker->getLevel() < protectionLevel) {
		return true;
	}

	if (not attacker->getVocation()->allowsPvp() or not target->getVocation()->allowsPvp()) {
		return true;
	}

	if (attacker->getSkull() == SKULL_BLACK and attacker->getSkullClient(target) == SKULL_NONE) {
		return true;
	}

	return false;
}

ReturnValue Combat::canDoCombat(const CreaturePtr& attacker, const CreaturePtr& target)
{
	if (not attacker) {
		return g_events->eventCreatureOnTargetCombat(attacker, target);
	}

	if (const auto& targetPlayer = target->getPlayer()) {
		if (targetPlayer->hasFlag(PlayerFlag_CannotBeAttacked)) {
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		}

		if (const auto& attackerPlayer = attacker->getPlayer()) {
			if (attackerPlayer->hasFlag(PlayerFlag_CannotAttackPlayer)) {
				return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
			}

			if (isProtected(attackerPlayer, targetPlayer)) {
				return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
			}

			//nopvp-zone
			const auto& targetPlayerTile = targetPlayer->getTile();
			if (targetPlayerTile->hasFlag(TILESTATE_NOPVPZONE)) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			} else if (attackerPlayer->getTile()->hasFlag(TILESTATE_NOPVPZONE) and not targetPlayerTile->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE)) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}
		}

		if (attacker->isSummon()) {
			if (const auto& masterAttackerPlayer = attacker->getMaster()->getPlayer()) {
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
		if (const auto& attackerPlayer = attacker->getPlayer()) {
			if (attackerPlayer->hasFlag(PlayerFlag_CannotAttackMonster)) {
				return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
			}

			if (target->isSummon() and target->getMaster()->getPlayer() and target->getZone() == ZONE_NOPVP) {
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}
		} else if (attacker->getMonster()) {
			const auto& targetMaster = target->getMaster();

			if (not targetMaster or not targetMaster->getPlayer()) {
				const auto& attackerMaster = attacker->getMaster();

				if (!attackerMaster or !attackerMaster->getPlayer()) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
				}
			}
		}
	}

	if (g_game.getWorldType() == WORLD_TYPE_NO_PVP) {
		if (attacker->getPlayer() or (attacker->isSummon() and attacker->getMaster()->getPlayer())) {
			if (target->getPlayer()) {
				if (!isInPvpZone(attacker, target)) {
					return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
				}
			}

			if (target->isSummon() and target->getMaster()->getPlayer()) {
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

int32_t Combat::getParam(CombatParam_t param) const
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

CallBack* Combat::getCallback(CallBackParam_t key) const
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

void Combat::combatTileEffects(const SpectatorVec& spectators,const CreaturePtr& caster, TilePtr tile, const CombatParams& params)
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
			PlayerPtr casterPlayer;
			if (caster->isSummon()) {
				casterPlayer = caster->getMaster()->getPlayer();
			} else {
				casterPlayer = caster->getPlayer();
			}

			if (casterPlayer) {
				if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or tile->hasFlag(TILESTATE_NOPVPZONE)) {
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
				} else if (itemId == ITEM_FIREFIELD_PVP_FULL or itemId == ITEM_POISONFIELD_PVP or itemId == ITEM_ENERGYFIELD_PVP) {
					casterPlayer->addInFightTicks();
				}
			}
		}

		auto item = Item::CreateItem(itemId);
		if (caster) {
			item->setOwner(caster->getID());
		}
		CylinderPtr holder = tile;
		ReturnValue ret = g_game.internalAddItem(holder, item);
		if (ret == RETURNVALUE_NOERROR) {
			g_game.startDecay(item);
		}
	}

	if (params.tileCallback) {
		params.tileCallback->onTileCombat(caster, tile);
	}

	if (params.impactEffect != CONST_ME_NONE) {
		Game::addMagicEffect(spectators, tile->getPosition(), params.impactEffect);
	}
}

void Combat::postCombatEffects(const CreaturePtr& caster, const Position& pos, const CombatParams& params)
{
	if (caster and (params.distanceEffect != CONST_ANI_NONE)) {
		addDistanceEffect(caster, caster->getPosition(), pos, params.distanceEffect);
	}
}

void Combat::addDistanceEffect(const CreaturePtr& caster, const Position& fromPos, const Position& toPos, uint8_t effect)
{
	if (effect == CONST_ANI_WEAPONTYPE) {
		if (!caster) {
			return;
		}

		auto player = caster->getPlayer();
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

void Combat::doCombat(const CreaturePtr& caster, const CreaturePtr& target) const
{
	// Early exit if no combat or effects are possible
	if (params.combatType == COMBAT_NONE
		and params.conditionList.empty()
		and params.dispelType == 0
		and params.impactEffect == CONST_ME_NONE
		and params.distanceEffect == CONST_ANI_NONE 
		and params.aggressive == 1) {
		return;
	}

	// Consolidate combat check once
	bool canCombat = (not params.aggressive) or (caster != target and Combat::canDoCombat(caster, target) == RETURNVALUE_NOERROR);

	if (params.combatType != COMBAT_NONE) {
		CombatDamage damage = getCombatDamage(caster, target);

		// Apply impact effect only if valid
		if ((caster == target or canCombat) and (params.impactEffect != CONST_ME_NONE)) {
			g_game.addMagicEffect(target->getPosition(), params.impactEffect);
		}

		if (canCombat) {
			doTargetCombat(caster, target, damage, params);
		}
	}
	else if (canCombat) {
		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, target->getPosition(), true, true);

		if (params.origin != ORIGIN_MELEE) {
			for (const auto& condition : params.conditionList) {
				if (caster == target or not target->isImmune(condition->getType())) {
					auto conditionCopy = condition->clone();
					conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());
					target->addCombatCondition(conditionCopy);
				}
			}
		}

		if (params.dispelType & CONDITION_PARALYZE) {
			target->removeCondition(CONDITION_PARALYZE);
		}
		else {
			target->removeCombatCondition(params.dispelType);
		}

		combatTileEffects(spectators, caster, target->getTile(), params);

		if (params.targetCallback) {
			params.targetCallback->onTargetCombat(caster, target);
		}

		if (caster and params.distanceEffect != CONST_ANI_NONE) {
			addDistanceEffect(caster, caster->getPosition(), target->getPosition(), params.distanceEffect);
		}
	}
}

void Combat::doCombat(const CreaturePtr& caster, const Position& position) const
{
	// Early exit if no combat or effects are possible
	if (params.combatType == COMBAT_NONE and params.conditionList.empty() and params.dispelType == 0) {
		return;
	}

	if (params.combatType != COMBAT_NONE) {
		CombatDamage damage = getCombatDamage(caster, nullptr);
		doAreaCombat(caster, position, area.get(), damage, params);
	}
	else {
		const auto& tiles = caster ? getCombatArea(caster->getPosition(), position, area.get()) : getCombatArea(position, position, area.get());
		if (tiles.empty()) {
			return;
		}

		uint32_t maxX = 0;
		uint32_t maxY = 0;
		for (const auto& tile : tiles) {
			const Position& tilePos = tile->getPosition();
			const uint32_t diffX = Position::getDistanceX(tilePos, position);
			if (diffX > maxX) {
				maxX = diffX;
			}
			const uint32_t diffY = Position::getDistanceY(tilePos, position);
			if (diffY > maxY) {
				maxY = diffY;
			}
		}

		const int32_t rangeX = maxX + Map::maxViewportX;
		const int32_t rangeY = maxY + Map::maxViewportY;
		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, position, true, true, rangeX, rangeX, rangeY, rangeY);

		postCombatEffects(caster, position, params);

		for (const auto& tile : tiles) {
			if (canDoCombat(caster, tile, params.aggressive) != RETURNVALUE_NOERROR) {
				continue;
			}

			combatTileEffects(spectators, caster, tile, params);

			if (const auto& creatures = tile->getCreatures()) {
				const auto& topCreature = tile->getTopCreature();
				for (const auto& creature : *creatures) {
					if (params.targetCasterOrTopMost) {
						if (caster and caster->getTile() == tile) {
							if (creature != caster) {
								continue;
							}
						}
						else if (creature != topCreature) {
							continue;
						}
					}

					if (not params.aggressive or (caster != creature && Combat::canDoCombat(caster, creature) == RETURNVALUE_NOERROR)) {
						for (const auto& condition : params.conditionList) {
							if (caster == creature and not creature->isImmune(condition->getType())) {
								const auto& conditionCopy = condition->clone();
								if (caster) {
									conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());
								}
								creature->addCombatCondition(conditionCopy);
							}
						}
					}

					if (params.dispelType & CONDITION_PARALYZE) {
						creature->removeCondition(CONDITION_PARALYZE);
					}
					else {
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

void Combat::doTargetCombat(const CreaturePtr& caster, const CreaturePtr& target, CombatDamage& damage, const CombatParams& params, bool sendDistanceEffect)
{	
	// To-do : I need to properly handle augment based damage which requires entire reworking of this method.
	// The thing that needs to happen is for augment based damage should not interact again with other aumgent
	// based damage. Instead of using origin for this, would possibly be better as fields on the combat or combat params.
	
	if (params.distanceEffect != CONST_ANI_NONE and sendDistanceEffect) {
		addDistanceEffect(caster, caster->getPosition(), target->getPosition(), params.distanceEffect);
	}

	gtl::node_hash_map<uint8_t, ModifierTotals> attackModData;
	bool isAugmented = damage.augmented;
	if (caster) {
		if (caster->isPlayer() and target) {
			const auto& casterPlayer = caster->getPlayer();
			attackModData.reserve(ATTACK_MODIFIER_LAST);
			auto targetType = CREATURETYPE_ATTACKABLE;
			// to-do: this is ugly, lets make it a function and assign its return to the variable above instead.
			if (target->getMonster()) {
				targetType = casterPlayer->getCreatureType(target->getMonster());
			}
			else if (target->getPlayer()) {
				targetType = CREATURETYPE_PLAYER;
			}
			attackModData = casterPlayer->getAttackModifierTotals(damage.primary.type, damage.origin, targetType, target->getRace(), target->getName());
			/// we do conversion here incase someone wants to convert say healing to mana or mana to death.

			const auto& conversionTotals = casterPlayer->getConvertedTotals(ATTACK_MODIFIER_CONVERSION, damage.primary.type, damage.origin, targetType, target->getRace(), target->getName());
			if (!conversionTotals.empty() and not isAugmented) {
				casterPlayer->convertDamage(target->getCreature(), damage, conversionTotals);
				if (damage.primary.value == 0) {
					return;
				}
			}

			if (damage.primary.type != COMBAT_MANADRAIN and damage.primary.type != COMBAT_HEALING) {
				// to-do: checking against origin for augment is too limiting.. Lets make piercing like crit and leech, ect.
				if (!attackModData.empty() and params.origin != ORIGIN_PIERCING) {
					const auto [piercingFlatTotal, piercingPercentTotal] = attackModData[ATTACK_MODIFIER_PIERCING];

					int32_t piercingDamage = 0;
					const auto originalDamage = std::abs(damage.primary.value);
					if (piercingPercentTotal) {
						const auto piercePercent = static_cast<int32_t>(piercingPercentTotal);

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
						auto piercing = CombatDamage(COMBAT_UNDEFINEDDAMAGE, ORIGIN_AUGMENT, BLOCK_NONE, (0 - piercingDamage), false, true, true);
						casterPlayer->sendTextMessage(MESSAGE_EVENT_DEFAULT, "You pierced " + target->getName() + " for " + std::to_string(piercingDamage) + " damage!");
						g_game.combatChangeHealth(caster, target, piercing);

						if (damage.primary.value == 0) {
							return;
						}
					}
				}

				if (params.origin != ORIGIN_PIERCING) {
					const auto blocked = g_game.combatBlockHit(damage, caster, target, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);
					if (blocked) {
						return;
					}
				}

				if (not damage.critical) {
					int32_t percentTotal = 0;
					int32_t flatTotal = 0;
					if (!attackModData.empty()) {
						percentTotal = attackModData[ATTACK_MODIFIER_CRITICAL].percentTotal;
						flatTotal = attackModData[ATTACK_MODIFIER_CRITICAL].flatTotal;
					}

					// normal crits are the old ones and are percent based
					const auto normalCritChance = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_CRITICALHITCHANCE));
					const auto normalCritDamage = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_CRITICALHITAMOUNT));

					// note : the way this works, its own damage increase is independent allowing for more than 100
					// and also at the same time, its chance is independent, so it doesn't add to augmented crit's chance.
					if ((normalCritChance and normalCritDamage) and (normal_random(1, 100) <= normalCritChance)) {
						percentTotal += normalCritDamage;
					}

					// we do percent based crits first, so that the flat damage doesn't add to the percent increase.
					if (percentTotal) {
						const auto damageIncrease = std::abs(damage.primary.value * percentTotal / 100);
						damage.primary.value -= damageIncrease;
						damage.critical = true;
						damage.augmented = true;
					}

					if (flatTotal) {
						damage.primary.value -= flatTotal;
						damage.critical = true;
						damage.augmented = true;
					}
				}

				if (target->isPlayer() and (caster != target) and not isAugmented) {
					const auto& targetPlayer = target->getPlayer();
					const auto& reformTotals = targetPlayer->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
					if (!reformTotals.empty()) {
						targetPlayer->reformDamage(casterPlayer->getCreature(), damage, reformTotals);
						if (damage.primary.value == 0) {
							return;
						}
					}

					const auto& defenseModData = targetPlayer->getDefenseModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
					if (!defenseModData.empty()) {
						for (const auto& [modkind, modTotals] : defenseModData) {
							if (modTotals.percentTotal or modTotals.flatTotal) {
								applyDamageReductionModifier(modkind, damage, targetPlayer, caster->getCreature(), static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);
								if (damage.primary.value == 0) {
									return;
								}
							}
						}
					}
				}
			}
		}
		else if (caster->isMonster()) {
			const auto& casterMonster = caster->getMonster();
			if (g_game.combatBlockHit(damage, caster, target, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances)) {
				return;
			}

			if (target and target->isPlayer()) {
				const auto& targetPlayer = target->getPlayer();
				const auto& attackerType = targetPlayer->getCreatureType(casterMonster->getMonster());
				const auto& defenseModData = targetPlayer->getDefenseModifierTotals(damage.primary.type, damage.origin, attackerType, casterMonster->getRace(), casterMonster->getName());
				const auto& reformTotals = targetPlayer->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, attackerType, casterMonster->getRace(), casterMonster->getName());
				if (!reformTotals.empty() and not isAugmented) {
					targetPlayer->reformDamage(casterMonster->getCreature(), damage, reformTotals);
					if (damage.primary.value == 0) {
						return;
					}
				}

				if (!defenseModData.empty() and not isAugmented) {
					for (const auto& [modkind, modTotals] : defenseModData) {
						if (modTotals.percentTotal or modTotals.flatTotal) {
							applyDamageReductionModifier(modkind, damage, targetPlayer, caster->getCreature(), static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);
							if (damage.primary.value == 0) {
								return;
							}
						}
					}
				}
			}
		}
	}

	const auto& success = (damage.primary.type == COMBAT_MANADRAIN) ?
		g_game.combatChangeMana(caster, target, damage) :
		g_game.combatChangeHealth(caster, target, damage);
	
	if (success) {
		if (target and caster and target != caster) {
			if (damage.critical) {
				if ((damage.augmented and g_config.getBoolean(ConfigManager::AUGMENT_CRITICAL_ANIMATION)) or not (damage.augmented)) {
					g_game.addMagicEffect(target->getPosition(), CONST_ME_CRITICAL_DAMAGE);
				}
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
			if (caster->isPlayer()
				and (damage.origin == ORIGIN_MELEE or damage.origin == ORIGIN_RANGED)
				and (damage.primary.type != COMBAT_HEALING and damage.primary.type != COMBAT_MANADRAIN )) {
				auto casterPlayer = caster->getPlayer();
				for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot) {

					auto item = casterPlayer->getInventoryItem(slot);
					if (!item) {
						continue;
					}
					if (item->hasImbuements()) {
						
						for (auto& imbuement : item->getImbuements()) {
							if (!imbuement->value) {
								continue;
							}
							const auto originalDamage = abs(damage.primary.value);
							const auto conversionAmount = (originalDamage * imbuement->value) / 100;
							const int32_t difference = (originalDamage - conversionAmount);
							
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

			if (target->isPlayer() and damage.primary.type != COMBAT_HEALING and damage.primary.type != COMBAT_MANADRAIN) {
					const auto& targetPlayer = target->getPlayer();
					for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot)
					{
						auto item = targetPlayer->getInventoryItem(slot);
						if (!item) {
							continue;
						}

						if (item->hasImbuements()) {
							for (const auto& imbuement : item->getImbuements()) {
								const auto combatType = damage.primary.type;
								const auto originalDamage = abs(damage.primary.value);
								const auto resistance = (originalDamage * imbuement->value) / 100;
								const int32_t difference = (originalDamage - resistance);
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

			if (!damage.leeched 
				and damage.primary.type != COMBAT_HEALING 
				and caster->isPlayer()
				and damage.origin != ORIGIN_CONDITION) {
				const auto& casterPlayer = caster->getPlayer();
				const auto totalDamage = std::abs(damage.primary.value + damage.secondary.value);
				int32_t lifeStealPercentTotal = 0, manaStealPercentTotal = 0, staminaStealPercentTotal = 0, soulStealPercentTotal = 0;
				int32_t lifeStealFlatTotal = 0, manaStealFlatTotal = 0, staminaStealFlatTotal = 0, soulStealFlatTotal = 0;
				int32_t lifeStealGain = 0, manaStealGain = 0, soulGain = 0, staminaGain = 0;
				
				// Static cast everything to int32_t to ensure consistency
				if (!attackModData.empty() and not isAugmented) {
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

				const auto lifeLeechChance = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_LIFELEECHCHANCE));
				const auto lifeLeechAmount = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_LIFELEECHAMOUNT));
				const auto manaLeechChance = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_MANALEECHCHANCE));
				const auto manaLeechAmount = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_MANALEECHAMOUNT));

				// Lifesteal
				if ((lifeLeechChance and lifeLeechAmount) and (normal_random(1, 100) <= lifeLeechChance)) {
					lifeStealGain += totalDamage * lifeLeechAmount / 100;
				}

				if (lifeStealPercentTotal) {
					lifeStealGain += totalDamage * lifeStealPercentTotal / 100;
				}

				if (lifeStealFlatTotal) {
					lifeStealGain += (lifeStealFlatTotal);
				}

				if (lifeStealGain) {
					auto lifeStealCombat = CombatDamage(COMBAT_LIFEDRAIN, ORIGIN_AUGMENT, BLOCK_NONE, lifeStealGain, damage.critical, true, true);
					g_game.combatChangeHealth(target, caster, lifeStealCombat);
				}

				/// Manasteal
				if ((manaLeechChance and manaLeechAmount) and (normal_random(1, 100) <= manaLeechChance)) {
					manaStealGain += totalDamage * manaLeechAmount / 100;
				}

				if (manaStealPercentTotal) {
					manaStealGain += totalDamage * manaStealPercentTotal / 100;
				}

				if (manaStealFlatTotal) {
					manaStealGain += manaStealFlatTotal;
				}

				if (manaStealGain) {
					auto manaStealCombat = CombatDamage(COMBAT_MANADRAIN, ORIGIN_AUGMENT, BLOCK_NONE, manaStealGain, damage.critical, true, true);
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
						
						const uint16_t currentStamina = casterPlayer->getStaminaMinutes();
						const uint16_t missingStamina = (MaximumStamina - currentStamina);
						if ((trueStaminaGain + currentStamina) >= missingStamina) {
							casterPlayer->addStamina(missingStamina);
						} else {
							casterPlayer->addStamina(trueStaminaGain);
						}
					} else {
						casterPlayer->addStamina(MaximumStamina - casterPlayer->getStaminaMinutes());
					}
					g_game.addMagicEffect(casterPlayer->getPosition(), CONST_ME_YELLOWENERGY);
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
						const uint8_t currentSoul = casterPlayer->getSoul();
						const uint8_t maxSoul = casterPlayer->getVocation()->getSoulMax();
						const uint8_t missingSoul = (maxSoul - currentSoul);
						if ((trueSoulGain + currentSoul) >= maxSoul) {
							casterPlayer->addSoul(missingSoul);
						} else {
							casterPlayer->addSoul(trueSoulGain);
						}
					} else {
						casterPlayer->addSoul(casterPlayer->getVocation()->getSoulMax() - casterPlayer->getSoul());
					}

					g_game.addMagicEffect(casterPlayer->getPosition(), CONST_ME_MAGIC_GREEN);
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

void Combat::doAreaCombat(const CreaturePtr& caster, const Position& position, const AreaCombat* area, const CombatDamage& damage, const CombatParams& params)
{
	const auto& tiles = caster ? getCombatArea(caster->getPosition(), position, area) : getCombatArea(position, position, area);

	const auto& casterPlayer = caster ? caster->getPlayer() : nullptr;

	uint32_t maxX = 0;
	uint32_t maxY = 0;

	//calculate the max viewable range
	for (const auto& tile : tiles) {
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

	std::vector<CreaturePtr> toDamageCreatures;
	toDamageCreatures.reserve(100);

	for (const auto& tile : tiles) {
		if (canDoCombat(caster, tile, params.aggressive) != RETURNVALUE_NOERROR) {
			continue;
		}

		combatTileEffects(spectators, caster, tile, params);

		if (const auto& creatures = tile->getCreatures()) {
			const auto& topCreature = tile->getTopCreature();
			for (const auto& creature : *creatures) {
				if (params.targetCasterOrTopMost) {
					if (caster and caster->getTile() == tile) {
						if (creature != caster) {
							continue;
						}
					} else if (creature != topCreature) {
						continue;
					}
				}

				if (!params.aggressive or (caster != creature and Combat::canDoCombat(caster, creature) == RETURNVALUE_NOERROR)) {
					toDamageCreatures.push_back(creature);

					if (params.targetCasterOrTopMost) {
						break;
					}
				}
			}
		}
	}

	for (const auto& target : toDamageCreatures) {
		CombatDamage damageCopy = damage; // we cannot avoid copying here, because we don't know if it's player combat or not, so we can't modify the initial damage.
		Combat::doTargetCombat(caster, target, damageCopy, params, false);
	}
}

void Combat::applyDamageIncreaseModifier
	(const uint8_t modifierType,
	CombatDamage& damage,
	const int32_t percentValue,
	const int32_t flatValue) {

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

void Combat::applyDamageReductionModifier
	(const uint8_t modifierType,
	CombatDamage& damage, 
	const PlayerPtr& damageTarget, 
	const std::optional<CreaturePtr>& attacker,
	const int32_t percent,
	const int32_t flat,
	const CombatOrigin paramOrigin,
	uint8_t areaEffect,
	uint8_t distanceEffect) {

	switch (modifierType) {
		case DEFENSE_MODIFIER_ABSORB:
			damageTarget->absorbDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_RESTORE:
			damageTarget->restoreManaFromDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_REPLENISH:
			damageTarget->replenishStaminaFromDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_RESIST:
			damageTarget->resistDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_REVIVE:
			damageTarget->reviveSoulFromDamage(attacker, damage, percent, flat);
			return;

		case DEFENSE_MODIFIER_REFLECT:
			damageTarget->reflectDamage(attacker, damage, percent, flat, areaEffect, distanceEffect);
			return;

		case DEFENSE_MODIFIER_DEFLECT:
			damageTarget->deflectDamage(attacker, damage, percent, flat, paramOrigin, areaEffect, distanceEffect);
			return;

		case DEFENSE_MODIFIER_RICOCHET:
			damageTarget->ricochetDamage(damage, percent, flat, areaEffect, distanceEffect);
			return;

		case DEFENSE_MODIFIER_WEAKNESS:
			damageTarget->increaseDamage(attacker, damage, percent, flat);
			return;

		default:
			return;
	}
}

//**********************************************************//

void ValueCallback::getMinMaxValues(const PlayerPtr& player, CombatDamage& damage) const
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

	LuaScriptInterface::pushSharedPtr(L, player);
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
			auto tool = player->getWeapon();
			const Weapon* weapon = g_weapons->getWeapon(tool);
			ItemPtr item;

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

void TileCallback::onTileCombat(const CreaturePtr& creature, const TilePtr& tile) const
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
		LuaScriptInterface::pushSharedPtr(L, creature);
		LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	} else {
		lua_pushnil(L);
	}
	LuaScriptInterface::pushPosition(L, tile->getPosition());

	scriptInterface->callFunction(2);
}

//**********************************************************//

void TargetCallback::onTargetCombat(const CreaturePtr& creature, const CreaturePtr& target) const
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
		LuaScriptInterface::pushSharedPtr(L, creature);
		LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	} else {
		lua_pushnil(L);
	}

	if (target) {
		LuaScriptInterface::pushSharedPtr(L, target);
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
	const int32_t dx = Position::getOffsetX(targetPos, centerPos);
	const int32_t dy = Position::getOffsetY(targetPos, centerPos);

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
		if (dx < 0 and dy < 0) {
			dir = DIRECTION_NORTHWEST;
		} else if (dx > 0 and dy < 0) {
			dir = DIRECTION_NORTHEAST;
		} else if (dx < 0 and dy > 0) {
			dir = DIRECTION_SOUTHWEST;
		} else if (dx > 0 and dy > 0) {
			dir = DIRECTION_SOUTHEAST;
		}
	}

	[[unlikely]]
	if (dir >= areas.size()) {
		// log location
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
		const int32_t mincol = cols - colSpread + 1;
		const int32_t maxcol = cols - (cols - colSpread);

		for (int32_t x = 1; x <= cols; ++x) {
			if (y == rows and x == ((cols - (cols % 2)) / 2) + 1) {
				vec.push_back(3);
			} else if (x >= mincol and x <= maxcol) {
				vec.push_back(1);
			} else {
				vec.push_back(0);
			}
		}

		if (spread > 0 and y % spread == 0) {
			--colSpread;
		}
	}

	setupArea(vec, rows);
}

void AreaCombat::setupArea(int32_t radius)
{
	constexpr int32_t area[13][13] = {
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
	for (const auto& row : area) {
		for (const auto& cell : row) {
			if (cell == 1) {
				vec.push_back(3);
			} else if (cell > 0 and cell <= radius) {
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

void MagicField::onStepInField(const CreaturePtr& creature)
{
	//remove magic walls/wild growth
	if (id == ITEM_MAGICWALL or id == ITEM_WILDGROWTH or id == ITEM_MAGICWALL_SAFE or id == ITEM_WILDGROWTH_SAFE or isBlocking()) {
		if (!creature->isInGhostMode()) {
			g_game.internalRemoveItem(getItem(), 1);
		}

		return;
	}

	//remove magic walls/wild growth (only nopvp tiles/world)
	if (id == ITEM_MAGICWALL_NOPVP or id == ITEM_WILDGROWTH_NOPVP) {
		if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or getTile()->hasFlag(TILESTATE_NOPVPZONE)) {
			g_game.internalRemoveItem(getItem(), 1);
		}
		return;
	}

	const ItemType& it = items[getID()];
	if (it.conditionDamage) {
		auto conditionCopy = it.conditionDamage->clone();
		const uint32_t ownerId = getOwner();
		if (ownerId) {
			bool harmfulField = true;

			if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or getTile()->hasFlag(TILESTATE_NOPVPZONE)) {
				if (const auto& owner = g_game.getCreatureByID(ownerId)) {
					if (owner->getPlayer() or (owner->isSummon() and owner->getMaster()->getPlayer())) {
						harmfulField = false;
					}
				}
			}

			if (const auto& targetPlayer = creature->getPlayer()) {
				if (const auto& attackerPlayer = g_game.getPlayerByID(ownerId)) {
					if (Combat::isProtected(attackerPlayer, targetPlayer)) {
						harmfulField = false;
					}
				}
			}

			if (!harmfulField or (OTSYS_TIME() - createTime <= 5000) or creature->hasBeenAttacked(ownerId)) {
				conditionCopy->setParam(CONDITION_PARAM_OWNER, ownerId);
			}
		}

		creature->addCondition(conditionCopy);
	}
}
