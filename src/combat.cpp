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
#include <immintrin.h>

using namespace BlackTek::Constant;
using BlackTek::MatrixArea;
using BlackTek::CreateArea;

extern Game g_game;
extern Weapons* g_weapons;
extern ConfigManager g_config;
extern Events* g_events;

thread_local std::vector<TilePtr> area_tile_buffer;
thread_local std::vector<Position> area_position_buffer;

template<typename T>
concept ByteLike = std::is_integral_v<T> or std::is_enum_v<T>;

struct NoticeData
{
	TextColor_t color = TEXTCOLOR_NONE;
	MagicEffectClasses effect = CONST_ME_NONE;
	FluidTypes_t fluid = FLUID_NONE;
	uint8_t padding = 0;
};

template<ByteLike T1, ByteLike T2, ByteLike T3 = FluidTypes_t>
uint32_t PackNotice(T1 color, T2 effect, T3 fluid = FLUID_NONE)
{
	NoticeData data
	{
		.color = static_cast<TextColor_t>(color),
		.effect = static_cast<MagicEffectClasses>(effect),
		.fluid = static_cast<FluidTypes_t>(fluid),
		.padding = 0
	};
	return std::bit_cast<uint32_t>(data);
}

NoticeData UnPackNotice(uint32_t packedValue) { return std::bit_cast<NoticeData>(packedValue); }

static const std::vector<TilePtr>& getList(const MatrixArea& area, const Position& targetPos, const Direction dir)
{
	const Position casterPos = getNextPosition(dir, targetPos);

	const uint32_t rows = area.GetRows();
	const uint32_t cols = area.GetCols();
	const auto& center = area.GetCenter();

	const int32_t startX = targetPos.x - static_cast<int32_t>(center.first);
	const int32_t startY = targetPos.y - static_cast<int32_t>(center.second);
	const uint32_t z = targetPos.z;

	area_tile_buffer.clear();
	area_tile_buffer.reserve(rows * cols);

	Position pos(0, 0, z);

	for (uint32_t row = 0; row < rows; ++row) 
	{
		pos.y = startY + static_cast<int32_t>(row);

		for (uint32_t col = 0; col < cols; ++col) 
		{
			[[unlikely]]
			if (not area(row, col))  
			{
				continue;
			}

			pos.x = startX + static_cast<int32_t>(col);

			[[unlikely]]
			if (not g_game.isSightClear(casterPos, pos, true))
			{
				continue;
			}

			auto tile = g_game.map.getTile(pos);
			[[unlikely]]
			if (not tile)
			{
				tile = std::make_shared<Tile>(pos.x, pos.y, z);
				g_game.map.setTile(pos, tile);
			}

			area_tile_buffer.push_back(tile);
		}
	}

	return area_tile_buffer;
}

const std::vector<TilePtr>& getCombatArea(const Position& centerPos, const Position& targetPos, const AreaCombat* area)
{
	[[unlikely]]
	if (targetPos.z >= MAP_MAX_LAYERS)
	{
		area_tile_buffer.clear();
		return area_tile_buffer;
	}

	[[likely]]
	if (area)
	{
		return getList(area->getArea(centerPos, targetPos), targetPos, getDirectionTo(targetPos, centerPos));
	}

	auto tile = g_game.map.getTile(targetPos);
	[[unlikely]]
	if (not tile)
	{
		tile = std::make_shared<Tile>(targetPos.x, targetPos.y, targetPos.z);
		g_game.map.setTile(targetPos, tile);
	}
	area_tile_buffer.clear();
	area_tile_buffer.push_back(tile);
	return area_tile_buffer;
}

// Todo: Change the matrix area and combat area so that it stores instead a flat array of structs which contain the math needed
// to be applied on the center location, in order to get the other positions, then this method can return those positions by applying
// that struct's math to the center position to create all the new positions... which should be far more effecient. Since we currently
// do not support "multi-level" combats, this can exclude the z for now... but we definitely need to implement multi-level combat as a thing.
const DamageArea Combat::getAreaPositions(const Position& casterPos, const Position& targetPos)
{
	area_position_buffer.clear();

	[[unlikely]]
	if (targetPos.z >= MAP_MAX_LAYERS)
		return { area_position_buffer, 0 };

	if (not area.get())
	{
		area_position_buffer.emplace_back(targetPos);
		auto distance = std::min(Position::getDistanceX(targetPos, casterPos), Position::getDistanceY(targetPos, casterPos));
		return { area_position_buffer, distance };
	}

	const auto& matrixArea  = area->getArea(casterPos, targetPos);
	const Position sightOrigin = getNextPosition(getDirectionTo(targetPos, casterPos), targetPos);

	const uint32_t rows = matrixArea.GetRows();
	const uint32_t cols = matrixArea.GetCols();
	const auto [centerX, centerY] = matrixArea.GetCenter();

	const int32_t startX = targetPos.x - static_cast<int32_t>(centerX);
	const int32_t startY = targetPos.y - static_cast<int32_t>(centerY);
	const uint8_t z      = targetPos.z;

	int32_t distance = 0;
	area_position_buffer.reserve(rows * cols);

	for (uint32_t row = 0; row < rows; ++row)
	{
		const int32_t py = startY + static_cast<int32_t>(row);

		for (uint32_t col = 0; col < cols; ++col)
		{
			[[unlikely]]
			if (not matrixArea(row, col))
				continue;

			const Position pos
			{
				static_cast<uint16_t>(startX + static_cast<int32_t>(col)),
				static_cast<uint16_t>(py),
				z
			};

			[[likely]]
			if (g_game.isSightClear(sightOrigin, pos, true))
			{
				auto difference = std::min(Position::getDistanceX(sightOrigin, pos), Position::getDistanceY(sightOrigin, pos));
				distance = (difference > distance) ? difference : distance;
				area_position_buffer.emplace_back(pos);
			}
		}
	}

	return { area_position_buffer, distance };
}

CombatDamage Combat::getCombatDamage(const CreaturePtr& creature, const CreaturePtr& target) const
{
	// todo rewrite this into an itilized list constructor
	CombatDamage damage;
	damage.origin = params.origin;
	damage.primary.type = params.combatType;

	// early return direct damage calculations
	if (formulaType == COMBAT_FORMULA_DAMAGE) 
	{
		damage.primary.value = normal_random(
			static_cast<int32_t>(mina),
			static_cast<int32_t>(maxa)
		);
		return damage;
	}

	[[unlikely]]
	if (not creature)
	{
		return damage;
	}

	int32_t min, max;
	// early return for monsters specifically
	if (creature->getCombatValues(min, max)) 
	{
		damage.primary.value = normal_random(min, max);
		return damage;
	}

	// should for sure be a player at this point (only because NPC's don't do damage)
	const auto& player = creature->getPlayer();
	[[unlikely]]
	if (not player) 
	{
		return damage;
	}

	if (params.valueCallback) 
	{
		params.valueCallback->getMinMaxValues(player, damage, target);
		return damage;
	}

	// We can check specific formula's now without concern
	// because if there were callbacks they would have returned already
	if (formulaType == COMBAT_FORMULA_LEVELMAGIC) 
	{
		const int32_t levelFormula = player->getLevel() * 2 + player->getMagicLevel() * 3;
		damage.primary.value = normal_random(
			std::fma(levelFormula, mina, minb),
			std::fma(levelFormula, maxa, maxb)
		);
		return damage;
	}

	if (formulaType == COMBAT_FORMULA_SKILL) 
	{
		const auto& tool = player->getWeapon();
		const Weapon* weapon = g_weapons->getWeapon(tool);
		[[likely]]
		if (weapon) 
		{
			damage.primary.value = normal_random(
				minb,
				std::fma(weapon->getWeaponDamage(player, target, tool, true), maxa, maxb)
			);
			damage.secondary.type = weapon->getElementType();
			damage.secondary.value = weapon->getElementDamage(player, target, tool);
		}
		else [[unlikely]]
		{
			damage.primary.value = normal_random(minb, maxb);
		}
	}

	return damage;
}

Combat::Combat(std::optional<CreaturePtr> attacker, CombatDamage&& combatDamage, CombatParams&& combatParams)
    : params(std::move(combatParams)),
      damage(std::move(combatDamage))
{
}

Combat::Combat(const CreaturePtr& attacker, const CreaturePtr& defender, CombatDamage&& combatDamage, CombatParams&& combatParams)
    : params(std::move(combatParams)),
      damage(std::move(combatDamage))
{
}

Combat Combat::makeCombat(std::optional<CreaturePtr> attacker, CombatDamage&& damage, CombatParams&& combatParams)
{
    return Combat(std::move(attacker), std::move(damage), std::move(combatParams));
}

Combat Combat::makeCombat(const CreaturePtr& attacker, const CreaturePtr& defender, CombatDamage&& damage, CombatParams&& combatParams)
{
    return Combat(attacker, defender, std::move(damage), std::move(combatParams));
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
	return target->getPlayer() or target->isSummon() and target->getMaster()->getPlayer();
}

ReturnValue Combat::canTargetCreature(const PlayerPtr& attacker, const CreaturePtr& target)
{
	if (attacker == target) {
		return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
	}

	if (not attacker->hasFlag(PlayerFlag_IgnoreProtectionZone)) 
	{
		//pz-zone
		if (attacker->getZone() == ZONE_PROTECTION) 
		{
			return RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE;
		}

		if (target->getZone() == ZONE_PROTECTION) 
		{
			return RETURNVALUE_ACTIONNOTPERMITTEDINPROTECTIONZONE;
		}

		//nopvp-zone
		if (isPlayerCombat(target)) 
		{
			if (attacker->getZone() == ZONE_NOPVP) 
			{
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}

			if (target->getZone() == ZONE_NOPVP) 
			{
				return RETURNVALUE_YOUMAYNOTATTACKAPERSONINPROTECTIONZONE;
			}
		}
	}

	if (attacker->hasFlag(PlayerFlag_CannotUseCombat) or not target->isAttackable()) 
	{
		if (target->getPlayer()) 
		{
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		} 
		else 
		{
			return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
		}
	}

	// we explicitly define in order to implicitly convert here for constness
	if (const PlayerConstPtr& p_target = target->getPlayer()) 
	{
		if (isProtected(attacker, p_target)) 
		{
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;
		}

		if (attacker->hasSecureMode() and not Combat::isInPvpZone(attacker, target) and attacker->getSkullClient(target->getPlayer()) == SKULL_NONE) 
		{
			return RETURNVALUE_TURNSECUREMODETOATTACKUNMARKEDPLAYERS;
		}
	}

	return Combat::canDoCombat(attacker, target);
}

ReturnValue Combat::canDoCombat(const CreaturePtr& caster, const TilePtr& tile, bool aggressive)
{
	if (tile->hasProperty(CONST_PROP_BLOCKPROJECTILE) or tile->hasFlag(TILESTATE_FLOORCHANGE) or tile->getTeleportItem()) 
	{
		return RETURNVALUE_NOTENOUGHROOM;
	}

	if (caster) 
	{
		const Position& casterPosition = caster->getPosition();
		const Position& tilePosition = tile->getPosition();

		if (casterPosition.z < tilePosition.z) 
		{
			return RETURNVALUE_FIRSTGODOWNSTAIRS;
		} 
		else if (casterPosition.z > tilePosition.z) 
		{
			return RETURNVALUE_FIRSTGOUPSTAIRS;
		}

		if (const auto& player = caster->getPlayer()) 
		{
			if (player->hasFlag(PlayerFlag_IgnoreProtectionZone)) 
			{
				return RETURNVALUE_NOERROR;
			}
		}
	}

	//pz-zone
	if (aggressive and tile->hasFlag(TILESTATE_PROTECTIONZONE)) 
	{
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
	uint32_t protectionLevel = g_config.GetNumber(ConfigManager::PROTECTION_LEVEL);
	if (target->getLevel() < protectionLevel or attacker->getLevel() < protectionLevel) 
	{
		return true;
	}

	if (not attacker->getVocation()->allowsPvp() or not target->getVocation()->allowsPvp()) 
	{
		return true;
	}

	if (attacker->getSkull() == SKULL_BLACK and attacker->getSkullClient(target) == SKULL_NONE) 
	{
		return true;
	}

	return false;
}

ReturnValue Combat::canDoCombat(const CreaturePtr& attacker, const CreaturePtr& target)
{
	if (not attacker)
		return g_events->eventCreatureOnTargetCombat(attacker, target);

	// Todo: This is a redundancy most likely, so ensure it is, then remove it.
	if (target and not attacker->compareInstance(target->getInstanceID()))
		return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;

	// Todo: Branchless this shz, using the RTTI enums for the creatures and the new masking trick with a switch.
	const auto pTargetPlayer = target->getPlayer();
	const auto pAttackerPlayer = attacker->getPlayer();

	const bool targetIsSummon = target->isSummon();
	const bool attackerIsSummon = attacker->isSummon();

	const auto pTargetMaster = targetIsSummon ? target->getMaster() : nullptr;
	const auto pAttackerMaster = attackerIsSummon ? attacker->getMaster() : nullptr;


	if (pTargetPlayer)
	{
		if (pTargetPlayer->hasFlag(PlayerFlag_CannotBeAttacked))
			return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;

		if (pAttackerPlayer)
		{
			const bool cannotAttack = pAttackerPlayer->hasFlag(PlayerFlag_CannotAttackPlayer) | isProtected(pAttackerPlayer, pTargetPlayer);

			if (cannotAttack)
				return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;

			const auto tTarget = pTargetPlayer->getTile();
			const auto tAttacker = pAttackerPlayer->getTile();

			const bool targetInNoPvp = tTarget->hasFlag(TILESTATE_NOPVPZONE);
			const bool attackerInNoPvp = tAttacker->hasFlag(TILESTATE_NOPVPZONE);
			const bool targetProtected = tTarget->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE);

			if (targetInNoPvp | (attackerInNoPvp & not targetProtected))
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
		}

		if (attackerIsSummon and pAttackerMaster)
		{
			if (const auto pMasterPlayer = pAttackerMaster->getPlayer())
			{
				const bool cannotAttack = pMasterPlayer->hasFlag(PlayerFlag_CannotAttackPlayer)	| isProtected(pMasterPlayer, pTargetPlayer);

				if (cannotAttack)
					return RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER;

				if (pTargetPlayer->getTile()->hasFlag(TILESTATE_NOPVPZONE))
					return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
			}
		}
	}
	else if (target->getMonster())
	{
		if (pAttackerPlayer)
		{
			if (pAttackerPlayer->hasFlag(PlayerFlag_CannotAttackMonster))
				return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;

			if (targetIsSummon and pTargetMaster and pTargetMaster->getPlayer() and target->getZone() == ZONE_NOPVP)
				return RETURNVALUE_ACTIONNOTPERMITTEDINANOPVPZONE;
		}
		else if (attacker->getMonster())
		{
			const bool targetHasPlayerMaster = pTargetMaster and pTargetMaster->getPlayer();
			const bool attackerHasPlayerMaster = pAttackerMaster and pAttackerMaster->getPlayer();

			if (not targetHasPlayerMaster and not attackerHasPlayerMaster)
				return RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
		}
	}

	if (g_game.getWorldType() == WORLD_TYPE_NO_PVP)
	{
		const bool isAttackerPlayerOrPlayerSummon = pAttackerPlayer or (attackerIsSummon and pAttackerMaster and pAttackerMaster->getPlayer());
		const bool isTargetPlayerOrPlayerSummon = pTargetPlayer or (targetIsSummon and pTargetMaster and pTargetMaster->getPlayer());

		if (isAttackerPlayerOrPlayerSummon and isTargetPlayerOrPlayerSummon)
		{
			if (not isInPvpZone(attacker, target))
			{
				return pTargetPlayer ? RETURNVALUE_YOUMAYNOTATTACKTHISPLAYER : RETURNVALUE_YOUMAYNOTATTACKTHISCREATURE;
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

		case CALLBACK_PARAM_DAMAGEVALUE:
        {
            params.valueCallback.reset(new ValueCallback(COMBAT_FORMULA_TARGET));
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

		case CALLBACK_PARAM_DAMAGEVALUE:
		{
			return params.valueCallback.get();
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
			item->setInstanceID(caster->getInstanceID());
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
		if (caster) {
			Game::addMagicEffect(spectators, tile->getPosition(), params.impactEffect, caster->getInstanceID());
		} else {
			Game::addMagicEffect(spectators, tile->getPosition(), params.impactEffect);
		}
	}
}

namespace
{
    [[nodiscard]] constexpr uint16_t resolve_persistent_item(uint16_t itemId) noexcept
    {
        switch (itemId)
        {
            case ITEM_FIREFIELD_PERSISTENT_FULL:   return ITEM_FIREFIELD_PVP_FULL;
            case ITEM_FIREFIELD_PERSISTENT_MEDIUM: return ITEM_FIREFIELD_PVP_MEDIUM;
            case ITEM_FIREFIELD_PERSISTENT_SMALL:  return ITEM_FIREFIELD_PVP_SMALL;
            case ITEM_ENERGYFIELD_PERSISTENT:      return ITEM_ENERGYFIELD_PVP;
            case ITEM_POISONFIELD_PERSISTENT:      return ITEM_POISONFIELD_PVP;
            case ITEM_MAGICWALL_PERSISTENT:        return ITEM_MAGICWALL;
            case ITEM_WILDGROWTH_PERSISTENT:       return ITEM_WILDGROWTH;
            default:                               return itemId;
        }
    }

    [[nodiscard]] constexpr uint16_t resolve_nopvp_item(uint16_t itemId) noexcept
    {
        switch (itemId)
        {
            case ITEM_FIREFIELD_PVP_FULL: return ITEM_FIREFIELD_NOPVP;
            case ITEM_POISONFIELD_PVP:    return ITEM_POISONFIELD_NOPVP;
            case ITEM_ENERGYFIELD_PVP:    return ITEM_ENERGYFIELD_NOPVP;
            case ITEM_MAGICWALL:          return ITEM_MAGICWALL_NOPVP;
            case ITEM_WILDGROWTH:         return ITEM_WILDGROWTH_NOPVP;
            default:                      return itemId;
        }
    }

    [[nodiscard]] constexpr bool triggers_in_fight(uint16_t itemId) noexcept
    {
        return itemId == ITEM_FIREFIELD_PVP_FULL
            or itemId == ITEM_POISONFIELD_PVP
            or itemId == ITEM_ENERGYFIELD_PVP;
    }
}

void Combat::apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, const TilePtr& tile)
{
	if (params.impactEffect != CONST_ME_NONE)
		Game::addMagicEffect(spectators, tile->getPosition(), params.impactEffect, caster->getInstanceID());

	if (params.itemId == 0)
		return;

    uint16_t itemId = resolve_persistent_item(params.itemId);
	auto casterPlayer = (caster->isSummon()) ? caster->getMaster()->getPlayer() : caster->getPlayer();

    if (casterPlayer)
    {
        if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or tile->hasFlag(TILESTATE_NOPVPZONE)) [[unlikely]]
        {
            itemId = resolve_nopvp_item(itemId);
        }
        else if (triggers_in_fight(itemId)) [[unlikely]]
        {
            casterPlayer->addInFightTicks();
        }
    }

	auto item = Item::CreateItem(itemId);

	if (not item) [[unlikely]]
		return;
	// should these "items" be owned by the summon or summoner
	// in the case that the caster is a summon? Perhaps a config to decide?
	item->setOwner(caster->getID());
	item->setInstanceID(caster->getInstanceID());

    CylinderPtr holder = tile;
    ReturnValue ret = g_game.internalAddItem(holder, item);
	auto decay = (ret == RETURNVALUE_NOERROR) and item->canDecay();

    if (decay) [[likely]]
        g_game.startDecay(item);

    if (params.tileCallback) [[unlikely]]
        params.tileCallback->onTileCombat(caster, tile);
}

void Combat::apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles)
{
    if (tiles.empty()) [[unlikely]]
        return;

    const uint16_t baseItemId	= (params.itemId != 0) ? resolve_persistent_item(params.itemId) : 0;
    const bool hasItem			= baseItemId != 0;
    const bool hasCallback		= params.tileCallback != nullptr;
    const bool hasEffect		= params.impactEffect != CONST_ME_NONE;
    const bool canFight			= hasItem and triggers_in_fight(baseItemId);

    PlayerPtr casterPlayer;
    bool isNoPvpWorld			= false;
    bool inFightConsumed		= false;
    uint32_t casterID			= 0;
    uint32_t casterInstanceID	= 0;

    casterPlayer      = caster->isSummon() ? caster->getMaster()->getPlayer() : caster->getPlayer();
    isNoPvpWorld      = (g_game.getWorldType() == WORLD_TYPE_NO_PVP);
    casterID          = caster->getID();
    casterInstanceID  = caster->getInstanceID();

    for (const auto& tile : tiles)
    {
        if (hasItem)
        {
            uint16_t itemId = baseItemId;

            if (casterPlayer)
            {
                if (isNoPvpWorld or tile->hasFlag(TILESTATE_NOPVPZONE)) [[unlikely]]
                {
                    itemId = resolve_nopvp_item(itemId);
                }
                else if (canFight and not inFightConsumed) [[unlikely]]
                {
                    casterPlayer->addInFightTicks();
                    inFightConsumed = true;
                }
            }

            auto item = Item::CreateItem(itemId);

			if (not item) [[unlikely]]
				return;

			item->setOwner(casterID);
			item->setInstanceID(casterInstanceID);

            CylinderPtr holder = tile;
            ReturnValue ret = g_game.internalAddItem(holder, item);
			auto decay = (ret == RETURNVALUE_NOERROR) and item->canDecay();

            if (decay) [[likely]]
                g_game.startDecay(item);
        }

        if (hasCallback) [[unlikely]]
            params.tileCallback->onTileCombat(caster, tile);

		if (hasEffect)
			Game::addMagicEffect(spectators, tile->getPosition(), params.impactEffect, casterInstanceID);
    }
}

void Combat::processImbuements(const CreaturePtr& caster, const CreaturePtr& target, CombatDamage& damage)
{
	if (not caster->is_player() and not target->is_player())
		return;

	static constexpr auto FireCombat = 720904U;
	static constexpr auto EarthCombat = 786436U;
	static constexpr auto IceCombat = 852480;
	static constexpr auto EnergyCombat = 917506U;
	static constexpr auto HolyCombat = 984064;
	static constexpr auto DeathCombat = 1050624;

	bool processAttackImbuements = caster->is_player() and (damage.origin == ORIGIN_MELEE or damage.origin == ORIGIN_RANGED) and (damage.primary.type != COMBAT_HEALING and damage.primary.type != COMBAT_MANADRAIN);
	bool processDefenseImbuements = target->is_player() and damage.primary.type != COMBAT_HEALING and damage.primary.type != COMBAT_MANADRAIN;

	if (processAttackImbuements or processDefenseImbuements)
	{
		const auto& attackerPlayer = processAttackImbuements ? caster->getPlayer() : nullptr;
		const auto& defenderPlayer = processDefenseImbuements ? target->getPlayer() : nullptr;
		const auto combatType = damage.primary.type;

		processAttackImbuements = processAttackImbuements and attackerPlayer /* and attackerPlayer->hasImbuements() or hasAttackImbuements() */;
		processDefenseImbuements = processDefenseImbuements and defenderPlayer /* and defenderPlayer->hasImbuements() or hasDefenseImbuements() */;

		for (int32_t slot = CONST_SLOT_FIRST; slot <= CONST_SLOT_LAST; ++slot)
		{
			if (processAttackImbuements)
			{
				auto item = attackerPlayer->getInventoryItem(slot);

				if (item and item->hasImbuements())
				{
					auto& imbues = item->getImbuements();

					for (auto& imbuement : *imbues)
					{
						if (not imbuement->value)
							continue;

						const auto originalDamage = abs(damage.primary.value);
						const auto conversionAmount = (originalDamage * imbuement->value) / 100;
						const int32_t difference = (originalDamage - conversionAmount);

						CombatDamage imbueDamage;
						imbueDamage.primary.type = combatType;
						imbueDamage.blockType = BLOCK_NONE;
						imbueDamage.origin = ORIGIN_IMBUEMENT;

						switch (imbuement->imbuetype)
						{
							case IMBUEMENT_TYPE_FIRE_DAMAGE:	imbueDamage.primary.type = COMBAT_FIREDAMAGE;		break;
							case IMBUEMENT_TYPE_ENERGY_DAMAGE:	imbueDamage.primary.type = COMBAT_ENERGYDAMAGE;		break;
							case IMBUEMENT_TYPE_EARTH_DAMAGE:	imbueDamage.primary.type = COMBAT_EARTHDAMAGE;		break;
							case IMBUEMENT_TYPE_ICE_DAMAGE:		imbueDamage.primary.type = COMBAT_ICEDAMAGE;		break;
							case IMBUEMENT_TYPE_HOLY_DAMAGE:	imbueDamage.primary.type = COMBAT_HOLYDAMAGE;		break;
							case IMBUEMENT_TYPE_DEATH_DAMAGE:	imbueDamage.primary.type = COMBAT_DEATHDAMAGE;		break;
							default: [[unlikely]]
								break;
						}

						if (difference < 0)
						{
							// this hack here technically secretly makes defense imbuements more powerful and here is why
							// because if we were to not cap this and the damage amount became a negative value that we held
							// as a temporary variable and then we applied the defense imbuements, we have a chance of allowing them
							// defense imbues to bring that damage back up to zero or above, and without doing that, we could be
							// potentially having that start value higher (at zero instead of a negative value) to start with on calculations
							// thereby giving defense an unfair advantage... however, as I am writing this, I'm considering the implications of
							// the inverse of this logic would likely make attack modifiers more powerful? 

							// I think what I should be doing instead is making it a secondary damage if the change is not the same type as the primary,
							// and there is not a secondary or the secondary is a matching type... perhaps we only create a new combat object if both those things are failures?
							imbueDamage.primary.value -= originalDamage;
							g_game.combatChangeHealth(caster, target, imbueDamage);
							break;
						}
						imbueDamage.primary.value -= conversionAmount;
						g_game.combatChangeHealth(caster, target, imbueDamage);
					}
				}
			}

			if (processDefenseImbuements)
			{
				auto item = defenderPlayer->getInventoryItem(slot);

				if (item and item->hasImbuements())
				{
					auto& imbues = item->getImbuements();
					for (const auto& imbuement : *imbues)
					{
						const auto originalDamage = abs(damage.primary.value);
						const auto resistance = (originalDamage * imbuement->value) / 100;
						const int32_t difference = (originalDamage - resistance);
						const auto switch_mask = (static_cast<uint32_t>(imbuement->imbuetype) << 16) | static_cast<uint32_t>(combatType);

						if (difference < 0)
						{
							damage.primary.value = 0;
						}
						else
						{
							switch (switch_mask)
							{
								case FireCombat:	damage.primary.value += difference;	break;
								case EarthCombat:	damage.primary.value += difference; break;
								case IceCombat:		damage.primary.value += difference;	break;
								case EnergyCombat:	damage.primary.value += difference;	break;
								case HolyCombat:	damage.primary.value += difference;	break;
								case DeathCombat:	damage.primary.value += difference;	break;
								default: [[unlikely]]
									break;
							}
						}
					}
				}
			}
		}
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
		if (caster) {
			SpectatorVec spectators;
			SpectatorVec toPosSpectators;
			g_game.map.getSpectators(spectators, fromPos, true, true);
			g_game.map.getSpectators(toPosSpectators, toPos, true, true);
			spectators.addSpectators(toPosSpectators);
			g_game.addDistanceEffect(spectators, fromPos, toPos, effect, caster->getInstanceID());
		} else {
			g_game.addDistanceEffect(fromPos, toPos, effect);
		}
	}
}

void Combat::doCombat(const CreaturePtr& caster, const CreaturePtr& target) const
{
	const auto& p = params;

	if (p.combatType == COMBAT_NONE and
		p.conditionList.empty() and
		p.dispelType == 0 and
		p.impactEffect == CONST_ME_NONE and
		p.distanceEffect == CONST_ANI_NONE and
		p.aggressive == 1) {
		return;
	}

	bool canCombat = not p.aggressive;
	if (not canCombat and caster != target) 
	{
		canCombat = (Combat::canDoCombat(caster, target) == RETURNVALUE_NOERROR);
	}

	if (p.combatType != COMBAT_NONE) 
	{
		CombatDamage damage = getCombatDamage(caster, target);

		if (p.impactEffect != CONST_ME_NONE && (caster == target or canCombat)) 
		{
			SpectatorVec spectators;
			g_game.map.getSpectators(spectators, target->getPosition(), true, true);
			Game::addMagicEffect(spectators, target->getPosition(), p.impactEffect, target->getInstanceID());
		}

		if (canCombat) 
		{
			doTargetCombat(caster, target, damage, p);
		}
	}
	else if (canCombat) 
	{ 
		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, target->getPosition(), true, true);

		if (p.origin != ORIGIN_MELEE) 
		{
			for (const auto& condition : p.conditionList) 
			{
				if (caster == target or not target->isImmune(condition->getType())) 
				{
					auto local_condition = condition->clone();

					if (local_condition)
					{
						local_condition->setParam(CONDITION_PARAM_OWNER, caster->getID());
						target->addCombatCondition(std::move(local_condition));
					}
				}
			}
		}


		if (p.dispelType & CONDITION_PARALYZE) 
		{
			target->removeCondition(CONDITION_PARALYZE);
		}
		else if (p.dispelType != 0) 
		{
			target->removeCombatCondition(p.dispelType);
		}

		combatTileEffects(spectators, caster, target->getTile(), p);

		if (p.targetCallback) 
		{
			p.targetCallback->onTargetCombat(caster, target);
		}

		if (caster and p.distanceEffect != CONST_ANI_NONE) 
		{
			addDistanceEffect(caster, caster->getPosition(), target->getPosition(), p.distanceEffect);
		}
	}
}

void Combat::doCombat(const CreaturePtr& caster, const Position& position) const
{
	const auto& p = params;

	if (p.combatType != COMBAT_NONE) 
	{
		CombatDamage damage = getCombatDamage(caster, nullptr);
		doAreaCombat(caster, position, area.get(), damage, p);
	}
	else 
	{
		const auto& currentArea = area.get();
		const auto& tiles = caster ? getCombatArea(caster->getPosition(), position, currentArea) : getCombatArea(position, position, currentArea);

		if (tiles.empty()) 
		{
			return;
		}

		uint32_t maxX = 0;
		uint32_t maxY = 0;
		for (const auto& tile : tiles) 
		{
			const Position& tilePos = tile->getPosition();
			maxX = std::max<int32_t>(maxX, Position::getDistanceX(tilePos, position));
			maxY = std::max<int32_t>(maxY, Position::getDistanceY(tilePos, position));
		}

		const int32_t rangeX = maxX + Map::maxViewportX;
		const int32_t rangeY = maxY + Map::maxViewportY;
		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, position, true, true, rangeX, rangeX, rangeY, rangeY);
		postCombatEffects(caster, position, p);

		for (const auto& tile : tiles) 
		{
			if (canDoCombat(caster, tile, p.aggressive) != RETURNVALUE_NOERROR) 
			{
				continue;
			}

			combatTileEffects(spectators, caster, tile, p);

			if (const auto& creatures = tile->getCreatures()) 
			{
				const auto& topCreature = p.targetCasterOrTopMost ? tile->getTopCreature() : nullptr;
				for (const auto& creature : *creatures) 
				{
					if (p.targetCasterOrTopMost)
					{
						if (caster && caster->getTile() == tile)
						{
							if (creature != caster)
							{
								continue;
							}
						}
						else if (creature != topCreature)
						{
							continue;
						}
					}

					bool creatureCanCombat = not p.aggressive or (caster != creature and Combat::canDoCombat(caster, creature) == RETURNVALUE_NOERROR);

					if (creatureCanCombat) 
					{
						for (const auto& condition : p.conditionList) 
						{
							if (caster == creature || !creature->isImmune(condition->getType())) 
							{
								auto conditionCopy = condition->clone();
								if (conditionCopy) 
								{
									if (caster) 
									{
										conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());
									}
									creature->addCombatCondition(std::move(conditionCopy));
								}
							}
						}
					}

					if (p.dispelType & CONDITION_PARALYZE) 
					{
						creature->removeCondition(CONDITION_PARALYZE);
					}
					else if (p.dispelType != 0) 
					{
						creature->removeCombatCondition(p.dispelType);
					}

					if (p.targetCallback) 
					{
						p.targetCallback->onTargetCombat(caster, creature);
					}

					if (p.targetCasterOrTopMost) 
					{
						break;
					}
				}
			}
		}
	}
}

CombatCancelCode Combat::target(const PlayerPtr& caster, const PlayerPtr& victim) const noexcept
{
	// for now we will still hardcode all of these restrictions, but eventually, they can all be moved to lua.
	const bool attack_blocked = caster->hasFlag(PlayerFlag_CannotAttackPlayer) | isProtected(caster, victim);

	if (attack_blocked)
		return CombatCancelCode::CanNotAttackThisPlayer;

	const auto victim_tile = victim->getTile();
	const auto attacker_tile = caster->getTile();

	const bool attacker_blocked = attacker_tile->hasFlag(TILESTATE_NOPVPZONE);
	const bool target_protected = victim_tile->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE);

	if (attacker_blocked | target_protected)
		return CombatCancelCode::TargetIsInProtectionZone;

	if (caster->getSkull() == SKULL_BLACK and caster->getSkullClient(victim) == SKULL_NONE)
	{
		// todo: add cancel code for blackskull
		return CombatCancelCode::CanNotAttackThisPlayer;
	}

	// if (victim has creature event tryTargetCombat() ) then

	return CombatCancelCode::None;
}

CombatCancelCode Combat::target(const PlayerPtr& caster, const MonsterPtr& victim) const noexcept
{
	if (caster->hasFlag(PlayerFlag_CannotAttackMonster))
		return CombatCancelCode::CanNotAttackThisMonster;

	if (victim->isSummon() and victim->getMaster()->getPlayer() and victim->getZone() == ZONE_NOPVP)
		return CombatCancelCode::TargetIsInProtectionZone;

	return CombatCancelCode::None;
}

CombatCancelCode Combat::target(const MonsterPtr& attacker, const PlayerPtr& victim) const noexcept
{
	const auto& master = attacker->getMaster();
	const auto& commander = (master) ? ((master->getCreatureSubType() == CreatureSubType::Player) ? master->getPlayer() : nullptr) : nullptr;

	if (commander)
	{
		const bool attack_blocked = commander->hasFlag(PlayerFlag_CannotAttackPlayer) | isProtected(commander, victim);

		if (attack_blocked)
			return CombatCancelCode::CanNotAttackThisPlayer;

		if (victim->getTile()->hasFlag(TILESTATE_NOPVPZONE))
			return CombatCancelCode::TargetIsInProtectionZone;

	}
	return CombatCancelCode::None;
}

CombatCancelCode Combat::target(const MonsterPtr& attacker, const MonsterPtr& victim) const noexcept
{
	const auto& attacker_master = attacker->getMaster();
	const auto& defender_master = victim->getMaster();
	const auto& commander = attacker_master->getPlayer();
	const auto& ward = defender_master->getPlayer();

	if (not commander and not ward)
		return CombatCancelCode::CanNotAttackThisMonster;

	return CombatCancelCode::None;
}

CombatCancelCode Combat::target(const PlayerPtr& attacker, const Position& target_location) const noexcept
{
	const auto tile = g_game.map.getTile(target_location);

	if (not tile)
		return CombatCancelCode::NotEnoughRoom;

	if (tile->hasProperty(CONST_PROP_BLOCKPROJECTILE) or tile->hasFlag(TILESTATE_FLOORCHANGE) or tile->getTeleportItem())
		return CombatCancelCode::NotEnoughRoom;

	const auto& attacker_pos = attacker->getPosition();

	if (attacker_pos.z < target_location.z)
		return CombatCancelCode::FirstGoDownStairs;

	if (attacker_pos.z > target_location.z)
		return CombatCancelCode::FirstGoUpStairs;

	if (attacker->hasFlag(PlayerFlag_IgnoreProtectionZone))
		return CombatCancelCode::None;

	if (params.aggressive and tile->hasFlag(TILESTATE_PROTECTIONZONE))
		return CombatCancelCode::TargetIsInProtectionZone;

	return CombatCancelCode::None;
}

CombatCancelCode Combat::target(const MonsterPtr& attacker, const Position& target_location) const noexcept
{
	const auto tile = g_game.map.getTile(target_location);

	if (not tile)
		return CombatCancelCode::NotEnoughRoom;

	if (tile->hasProperty(CONST_PROP_BLOCKPROJECTILE) or tile->hasFlag(TILESTATE_FLOORCHANGE) or tile->getTeleportItem())
		return CombatCancelCode::NotEnoughRoom;

	const auto& attacker_pos = attacker->getPosition();

	if (attacker_pos.z < target_location.z)
		return CombatCancelCode::FirstGoDownStairs;

	if (attacker_pos.z > target_location.z)
		return CombatCancelCode::FirstGoUpStairs;

	if (params.aggressive and tile->hasFlag(TILESTATE_PROTECTIONZONE))
		return CombatCancelCode::TargetIsInProtectionZone;

	return CombatCancelCode::None;
}

void Combat::attack_augment(const PlayerPtr caster, const CreaturePtr victim, const std::string& victim_name, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept
{
	const auto [piercingFlatTotal, piercingPercentTotal] = modifier_map[ATTACK_MODIFIER_PIERCING];
	int32_t piercingDamage = 0;
	const auto originalDamage = std::abs(damage.primary.value);

	if (piercingPercentTotal)
	{
		const auto piercePercent = static_cast<int32_t>(piercingPercentTotal);
		piercingDamage = (piercingPercentTotal <= 100) ? (originalDamage * piercePercent / 100) : damage.primary.value;
	}

	if (piercingFlatTotal)
		piercingDamage += static_cast<int32_t>(piercingFlatTotal);

	if (piercingDamage)
	{
		piercingDamage = std::min<int32_t>(piercingDamage, originalDamage);
		damage.primary.value += piercingDamage;
		auto piercing = CombatDamage(COMBAT_UNDEFINEDDAMAGE, ORIGIN_PIERCING, BLOCK_NONE, (0 - piercingDamage), false, true, true);
		caster->sendTextMessage(MESSAGE_EVENT_DEFAULT, "You pierced " + victim_name + " for " + std::to_string(piercingDamage) + " damage!");
		return; /*combat->execute(caster, victim);*/
	}

	if (not damage.critical)
	{
		auto percentTotal = static_cast<int32_t>(modifier_map[ATTACK_MODIFIER_CRITICAL].percentTotal);
		auto flatTotal = static_cast<int32_t>(modifier_map[ATTACK_MODIFIER_CRITICAL].flatTotal);

		// we do percent based crits first, so that the flat damage doesn't add to the percent increase.
		if (percentTotal)
		{
			const auto damageIncrease = std::abs(damage.primary.value * percentTotal / 100);
			damage.primary.value -= damageIncrease;
			damage.critical = true;
			damage.augmented = true;
		}

		if (flatTotal)
		{
			damage.primary.value -= flatTotal;
			damage.critical = true;
			damage.augmented = true;
		}
	}
}

// Todo: we no longer need the caster for doing defense augments I don't think... at most, only their position for the deflect and reflect, which we could just split..
// investigate this situation and get it cleared up.
void Combat::defense_augment(const CreaturePtr caster, const PlayerPtr victim, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept
{
	for (const auto& [modkind, modTotals] : modifier_map)
	{
		if (modTotals.percentTotal or modTotals.flatTotal)
		{
			applyDamageReductionModifier(modkind, damage, victim, caster, static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);

			if (damage.primary.value == 0)
				return;
		}
	}
}

void Combat::conversion_augment(const PlayerPtr caster, const std::string& victim_name, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept
{
	auto iter = modifier_map.begin();

	while (damage.primary.value < 0 and iter != modifier_map.end())
	{
		const auto combatType = indexToCombatType(iter->first);
		const auto& totals = iter->second;
		auto convertedDamage = 0;
		const auto percent = static_cast<int32_t>(totals.percentTotal);
		const auto flat = static_cast<int32_t>(totals.flatTotal);
		const auto originalDamageValue = std::abs(damage.primary.value);

		if (percent)
			convertedDamage += originalDamageValue * percent / 100;

		if (flat)
			convertedDamage += flat;

		if (convertedDamage != 0)
		{

		}
		++iter;
	}
}

void Combat::reform_augment(const PlayerPtr victim, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept
{
	auto iter = modifier_map.begin();

	while (damage.primary.value < 0 and iter != modifier_map.end())
	{
		auto combatType = indexToCombatType(iter->first);
		const auto& totals = iter->second;
		auto reformedDamage = 0;
		auto percent = static_cast<int32_t>(totals.percentTotal);
		auto flat = static_cast<int32_t>(totals.flatTotal);
		const auto originalDamageValue = std::abs(damage.primary.value);

		if (percent)
			reformedDamage += originalDamageValue * percent / 100;

		if (flat)
			reformedDamage += flat;

		if (reformedDamage)
		{

		}

		++iter;
	}

}

void Combat::post_damage(const PlayerPtr& caster, const CreaturePtr& victim, LeechData&& leech) noexcept
{
	if (params.impactEffect != CONST_ME_NONE)
		g_game.addMagicEffect(victim->getPosition(), params.impactEffect);

	if (damage.critical)
	{
		if ((damage.augmented and g_config.GetBoolean(ConfigManager::AUGMENT_CRITICAL_ANIMATION)) or not (damage.augmented))
			g_game.addMagicEffect(victim->getPosition(), CONST_ME_CRITICAL_DAMAGE);
	}

	for (const auto& condition : params.conditionList)
	{
		if (not victim->isImmune(condition->getType()))
		{
			Condition* conditionCopy = condition->clone();

			if (caster)
				conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());

			//TODO: infight condition until all aggressive conditions has ended
			victim->addCombatCondition(conditionCopy);
		}
	}

	auto primary_conditions = (not damage.leeched and damage.primary.type != COMBAT_HEALING and caster->is_player() and damage.origin != ORIGIN_CONDITION);

	if (primary_conditions)
	{
		const auto& casterPlayer = caster->getPlayer();
		const auto totalDamage = std::abs(damage.primary.value + damage.secondary.value);

		if (leech.percent_health > 0 or leech.flat_health > 0)
		{
			int32_t life = 0;

			if (leech.percent_health)
				life += totalDamage * leech.percent_health / 100;

			if (leech.flat_health)
				life += leech.flat_health;

			if (life)
			{
				auto local_damage = CombatDamage(COMBAT_LIFEDRAIN, ORIGIN_AUGMENT, BLOCK_NONE, life, damage.critical, true, true);
			}
		}
		
		if (leech.percent_mana > 0 or leech.flat_mana > 0)
		{
			int32_t mana = 0;

			if (leech.percent_mana)
				mana += totalDamage * leech.percent_mana / 100;

			if (leech.flat_mana)
				mana += leech.flat_mana;

			if (mana)
			{
				auto local_damage = CombatDamage(COMBAT_MANADRAIN, ORIGIN_AUGMENT, BLOCK_NONE, mana, damage.critical, true, true);
			}
		}

		if (leech.percent_stamina > 0 or leech.flat_stamina > 0)
		{
			int32_t stamina = 0;
			
			if (leech.percent_stamina)
				stamina += totalDamage * leech.percent_stamina / 100;

			if (leech.flat_stamina)
				stamina += leech.flat_stamina;

			if (stamina)
			{
				stamina = (stamina > std::numeric_limits<uint16_t>::max()) ? std::numeric_limits<uint16_t>::max() : stamina;
				auto stamina_gained = g_config.GetBoolean(ConfigManager::AUGMENT_STAMINA_RULE) ? static_cast<uint16_t>(stamina) : static_cast<uint16_t>(stamina / 60);
				auto max_gain = MaximumStamina - caster->getStaminaMinutes();
				stamina_gained = (stamina_gained > max_gain) ? max_gain : stamina_gained;
			}
		}

		if (leech.percent_soul > 0 or leech.flat_soul > 0)
		{
			int32_t soul = 0;

			if (leech.percent_soul)
				soul += totalDamage * leech.percent_soul / 100;

			if (leech.flat_soul)
				soul += leech.flat_soul;

			if (soul)
			{
				soul = (soul > std::numeric_limits<uint8_t>::max()) ? std::numeric_limits<uint8_t>::max() : soul;
				auto soul_gained = static_cast<uint8_t>(soul);
				uint8_t max_gain = caster->getVocation()->getSoulMax() - caster->getSoul();
				soul_gained = (soul_gained > max_gain) ? max_gain : soul_gained;
			}
		}
	}

	// after looking into this nonsense, I have determined the best course is propagate the bool
	// variable for "force" in the removeCombatCondition call, and to use that one
	// for dispeling conditions in combat, using force == true, whats going on currently makes no sense

	//if (params.dispelType & CONDITION_PARALYZE)
	//target->removeCondition(CONDITION_PARALYZE);
	//else
	//target->removeCombatCondition(params.dispelType);

}

void Combat::strike_target(const PlayerPtr& caster, const PlayerPtr& victim) noexcept
{
	// todo: Determine if it's a non-aggressive combat first (like healing)
	// and save that result to a bool to gate the different paths and stop wasting
	// time and work on stuff that doesn't apply to healing.
	// Also need to add the "augment" validation to stop a chain of infinite augmentation from happening.

	// if (not can_target(caster, victim)) return;

	if (params.distanceEffect != CONST_ANI_NONE)
		addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), params.distanceEffect);

	if (params.origin != ORIGIN_PIERCING)
	{
		const auto blocked = g_game.combatBlockHit(damage, caster, victim, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);

		if (blocked)
			return;
	}

	LeechData leech_data {};

	if (caster->hasAttackModifiers())
	{
		const auto conversion_count = caster->conversion_mod_count();
		const auto victim_race = victim->getRace();
		const auto& victim_name = victim->getName();
		const auto moddable = damage.primary.type != COMBAT_MANADRAIN and damage.primary.type != COMBAT_HEALING;

		if (conversion_count > 0)
		{
			const auto& conversion_modifiers = caster->getConvertedTotals(ATTACK_MODIFIER_CONVERSION, damage.primary.type, damage.origin, CREATURETYPE_PLAYER, victim_race, victim_name);
			conversion_augment(caster, victim_name, conversion_modifiers);
		}

		if (moddable and caster->attack_mod_count() > conversion_count)
		{
			auto attack_modifiers = caster->getAttackModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, victim_race, victim_name);

			leech_data.percent_health		= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_LIFESTEAL].percentTotal);
			leech_data.percent_mana			= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_MANASTEAL].percentTotal);
			leech_data.percent_stamina		= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_STAMINASTEAL].percentTotal);
			leech_data.percent_soul			= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_SOULSTEAL].percentTotal);
			leech_data.flat_health			= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_LIFESTEAL].flatTotal);
			leech_data.flat_mana			= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_MANASTEAL].flatTotal);
			leech_data.flat_stamina			= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_STAMINASTEAL].flatTotal);
			leech_data.flat_soul			= static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_SOULSTEAL].flatTotal);
		
			attack_augment(caster, victim, victim_name, std::move(attack_modifiers));
		}
	}

	if (victim->hasDefenseModifiers())
	{
		const auto reform_count = victim->reform_mod_count();
		const auto attacker_race = caster->getRace();
		const auto& attacker_name = caster->getName();

		if (reform_count > 0)
		{
			const auto& reform_data = victim->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
			reform_augment(victim, reform_data);
		}

		if (victim->defense_mod_count() > reform_count)
		{
			const auto& defense_data = victim->getDefenseModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
			defense_augment(caster, victim, defense_data);
		}
	}

	const auto& success = (damage.primary.type == COMBAT_MANADRAIN) ? g_game.combatChangeMana(caster, victim, damage) : g_game.combatChangeHealth(caster, victim, damage);

	if (success)
		post_damage(caster, victim, std::move(leech_data));
}

void Combat::strike_target(const PlayerPtr& caster, const MonsterPtr& victim) noexcept
{
	// if (not can_target(caster, victim)) return;

	if (params.distanceEffect != CONST_ANI_NONE)
		addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), params.distanceEffect);

	// todo: Wrap this in combat::block_hit() (and later refine into seperate, defense reduction, and predamage calculation, methods
	if (params.origin != ORIGIN_PIERCING)
	{
		const auto blocked = g_game.combatBlockHit(damage, caster, victim, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);

		if (blocked)
			return;
	}

	LeechData leech_data{};

	if (caster->hasAttackModifiers())
	{
		const auto conversion_count = caster->conversion_mod_count();
		const auto victim_race = victim->getRace();
		const auto& victim_name = victim->getName();
		const auto moddable = damage.primary.type != COMBAT_MANADRAIN and damage.primary.type != COMBAT_HEALING;
		const auto victim_type = (victim->isBoss() or victim->isRewardBoss()) ? CREATURETYPE_BOSS : CREATURETYPE_MONSTER;

		if (conversion_count > 0)
		{
			const auto& conversion_modifiers = caster->getConvertedTotals(ATTACK_MODIFIER_CONVERSION, damage.primary.type, damage.origin, victim_type, victim_race, victim_name);
			conversion_augment(caster, victim_name, conversion_modifiers);
		}

		if (moddable and caster->attack_mod_count() > conversion_count)
		{
			auto attack_modifiers = caster->getAttackModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, victim_race, victim_name);

			leech_data.percent_health = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_LIFESTEAL].percentTotal);
			leech_data.percent_mana = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_MANASTEAL].percentTotal);
			leech_data.percent_stamina = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_STAMINASTEAL].percentTotal);
			leech_data.percent_soul = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_SOULSTEAL].percentTotal);
			leech_data.flat_health = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_LIFESTEAL].flatTotal);
			leech_data.flat_mana = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_MANASTEAL].flatTotal);
			leech_data.flat_stamina = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_STAMINASTEAL].flatTotal);
			leech_data.flat_soul = static_cast<int32_t>(attack_modifiers[ATTACK_MODIFIER_SOULSTEAL].flatTotal);

			attack_augment(caster, victim, victim_name, std::move(attack_modifiers));
		}
	}

	const auto& success = (damage.primary.type == COMBAT_MANADRAIN) ? g_game.combatChangeMana(caster, victim, damage) : g_game.combatChangeHealth(caster, victim, damage);

	if (success)
		post_damage(caster, victim, std::move(leech_data));
}

void Combat::strike_target(const MonsterPtr& attacker, const PlayerPtr& victim) noexcept
{
	// if (not can_target(caster, victim)) return;

	if (params.distanceEffect != CONST_ANI_NONE)
		addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), params.distanceEffect);

	if (params.origin != ORIGIN_PIERCING)
	{
		const auto blocked = g_game.combatBlockHit(damage, attacker, victim, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);

		if (blocked)
			return;
	}

	if (victim->hasDefenseModifiers())
	{
		const auto reform_count = victim->reform_mod_count();
		const auto attacker_race = attacker->getRace();
		const auto& attacker_name = attacker->getName();

		if (reform_count > 0)
		{
			const auto& reform_data = victim->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, CREATURETYPE_PLAYER, attacker->getRace(), attacker->getName());
			reform_augment(victim, reform_data);
		}

		if (victim->defense_mod_count() > reform_count)
		{
			const auto& defense_data = victim->getDefenseModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, attacker->getRace(), attacker->getName());
			defense_augment(attacker, victim, defense_data);
		}
	}

	const auto& success = (damage.primary.type == COMBAT_MANADRAIN) ? g_game.combatChangeMana(attacker, victim, damage) : g_game.combatChangeHealth(attacker, victim, damage);

	if (not success)
	{
		// some kind of log here ?
	}
}

void Combat::strike_target(const MonsterPtr& attacker, const MonsterPtr& victim) noexcept
{
	// if (not can_execute(caster, victim)) return;

	if (params.distanceEffect != CONST_ANI_NONE)
		addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), params.distanceEffect);

	const auto blocked = g_game.combatBlockHit(damage, attacker, victim, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);

	if (blocked)
		return;

	// Do we do anything here for summons attacking or being attacked or anything like that? Perhaps in the future when we allow passing
	// either augments or modifiers directly with combat this version of combat becomes useful, or maybe we revamp the summon system and finally
	// make summon an actual "thing" type derived from monster like I always thought it should be... I just feel like I'm forgetting something obvious right now

	const auto& success = (damage.primary.type == COMBAT_MANADRAIN) ? g_game.combatChangeMana(attacker, victim, damage) : g_game.combatChangeHealth(attacker, victim, damage);

	if (not success)
	{
		// some kind of log here ?
	}
}

void Combat::strike_target(const CreaturePtr& attacker, const CreaturePtr& defender) noexcept
{
	using namespace BlackTek;
	auto switch_mask = (static_cast<uint32_t>(attacker->getCreatureSubType()) << 16 | static_cast<uint32_t>(defender->getCreatureSubType()) << 8);

	switch (switch_mask)
	{
		case Constant::Player_Vs_Player: strike_target(PlayerCast(attacker), PlayerCast(defender)); break;
		case Constant::Player_Vs_Monster: strike_target(PlayerCast(attacker), MonsterCast(defender)); break;
		case Constant::Monster_Vs_Player: strike_target(MonsterCast(attacker), PlayerCast(defender)); break;
		case Constant::Monster_Vs_Monster: strike_target(MonsterCast(attacker), MonsterCast(defender)); break;
		default: [[unlikely]]
			break;
	}
}

void Combat::strike_location(const PlayerPtr& caster, const Position& center) noexcept
{
	// todo: I have forgotten to ensure absolute value in all the comparisions which were used for
	// calculating the distance, I need to go back and change that, in order to be sure it's positive for it's usage here
	const auto& [positions, distance ] = getAreaPositions(caster->getPosition(), center);
	const size_t n = std::min(static_cast<size_t>(distance), static_cast<size_t>(256));

	if (distance == 0)
		return;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, center, true, true,
		distance + Map::maxViewportX,
		distance + Map::maxViewportX,
		distance + Map::maxViewportY,
		distance + Map::maxViewportY);

	postCombatEffects(caster, center, params);

	// Phase 1: Vectorized tile validation via AVX2.
	// z-values come from positions directly — no tile dereference in the pack loop.
	// One tile lookup per position extracts flags; null tiles use ~0u to auto-reject.
	// tile_cache carries the pointer forward to phase 2 with no second lookup.

	static constexpr size_t MAX_TILES = 256;

	alignas(32) uint32_t packed_flags[MAX_TILES];
	alignas(32) uint8_t  valid_mask[MAX_TILES];
	TilePtr              tile_cache[MAX_TILES];

	for (size_t i = 0; i < n; ++i)
	{
		tile_cache[i]   = g_game.map.getTile(positions[i]);
		packed_flags[i] = tile_cache[i] ? tile_cache[i]->getFlags() : ~0u;
	}

	std::memset(valid_mask, 0xFF, n);

	// Flag-rejection pass: 8 uint32_t at a time.
	// IgnoreProtectionZone: fold into flag_reject constant once — zero per-position branch cost.
	const uint32_t flag_reject =
		TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT |
		(params.aggressive and not caster->hasFlag(PlayerFlag_IgnoreProtectionZone)
			? static_cast<uint32_t>(TILESTATE_PROTECTIONZONE) : 0u);

	const __m256i vReject = _mm256_set1_epi32(static_cast<int32_t>(flag_reject));
	const __m256i vZero   = _mm256_setzero_si256();

	size_t i = 0;
	for (; i + 8 <= n; i += 8)
	{
		const __m256i vFlags    = _mm256_load_si256(reinterpret_cast<const __m256i*>(packed_flags + i));
		const __m256i vHit      = _mm256_and_si256(vFlags, vReject);
		const __m256i vPassMask = _mm256_cmpeq_epi32(vHit, vZero);
		alignas(32) int32_t lane[8];
		_mm256_store_si256(reinterpret_cast<__m256i*>(lane), vPassMask);
		for (size_t j = 0; j < 8; ++j)
			valid_mask[i + j] &= lane[j] ? 0xFF : 0x00;
	}
	for (; i < n; ++i)
		valid_mask[i] &= ((packed_flags[i] & flag_reject) == 0u) ? 0xFF : 0x00;

	// Phase 2: Pointer-chasing on SIMD-passed positions only.
	// tile_cache[idx] is guaranteed non-null here — null tiles produced ~0u flags and were rejected.
	// Creature targeting dispatches through the new target() overloads on creature subtype.

	// Todo: biggest limiting factor on the SIMD is that hasProperty(CONST_PROP_BLOCKPROJECTILE) requires item traversal
	// and so we need to create a mask on loading the map for this and apply it to tile, updating as needed for things like magic wall.
	std::vector<CreaturePtr> toDamageCreatures;
	toDamageCreatures.reserve(n * 2);

	auto valid_tiles = std::views::iota(size_t{0}, n)
		| std::views::filter([&](size_t idx) { return static_cast<bool>(valid_mask[idx]); })
		| std::views::filter([&](size_t idx) { return not tile_cache[idx]->hasProperty(CONST_PROP_BLOCKPROJECTILE); });

	for (size_t idx : valid_tiles)
	{
		const auto& tile = tile_cache[idx];

		combatTileEffects(spectators, caster, tile, params);

		const auto& creaturesOnTile = tile->getCreatures();
		if (not creaturesOnTile)
			continue;

		const auto& topCreature = params.targetCasterOrTopMost ? tile->getTopCreature() : nullptr;
		const bool onCasterTile = (caster->getTile() == tile);

		auto strikeable = *creaturesOnTile
			| std::views::filter([&](const CreaturePtr& creature) -> bool {
				if (not params.targetCasterOrTopMost)
					return true;
				return onCasterTile ? (creature == caster) : (creature == topCreature);
			})
			| std::views::filter([&](const CreaturePtr& creature) -> bool {
				return not (params.aggressive and caster == creature);
			})
			| std::views::filter([&](const CreaturePtr& creature) -> bool {
				if (not params.aggressive)
					return true;

				switch (creature->getCreatureSubType())
				{
					case CreatureSubType::Player: return target(caster, PlayerCast(creature)) == CombatCancelCode::None;
					case CreatureSubType::Monster: return target(caster, MonsterCast(creature)) == CombatCancelCode::None;
					default: [[unlikely]]
						return false;
				}
			});

		for (const auto& creature : strikeable)
			toDamageCreatures.push_back(creature);
	}

	// Phase 3: Apply damage across all gathered creatures.
	for (const auto& target_creature : toDamageCreatures)
	{
		damage = getCombatDamage(caster, target_creature);
		strike_target(caster, target_creature);
	}
}

void Combat::strike_location(const MonsterPtr& attacker, const Position& center) noexcept
{
	// todo: I have forgotten to ensure absolute value in all the comparisions which were used for
	// calculating the distance, I need to go back and change that, in order to be sure it's positive for it's usage here
	const auto& [positions, distance] = getAreaPositions(attacker->getPosition(), center);
	const size_t n = std::min(static_cast<size_t>(distance), static_cast<size_t>(256));

	// todo: put a log here
	if (distance == 0)
		return;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, center, true, true,
		distance + Map::maxViewportX,
		distance + Map::maxViewportX,
		distance + Map::maxViewportY,
		distance + Map::maxViewportY);

	postCombatEffects(attacker, center, params);


	static constexpr size_t MAX_TILES = 256;

	alignas(32) uint32_t packed_flags[MAX_TILES];
	alignas(32) uint8_t  valid_mask[MAX_TILES];
	TilePtr              tile_cache[MAX_TILES];

	for (size_t i = 0; i < n; ++i)
	{
		tile_cache[i]   = g_game.map.getTile(positions[i]);
		packed_flags[i] = tile_cache[i] ? tile_cache[i]->getFlags() : ~0u;
	}

	std::memset(valid_mask, 0xFF, n);

	const uint32_t flag_reject =
		TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT |
		(params.aggressive ? static_cast<uint32_t>(TILESTATE_PROTECTIONZONE) : 0u);

	const __m256i vReject = _mm256_set1_epi32(static_cast<int32_t>(flag_reject));
	const __m256i vZero   = _mm256_setzero_si256();

	size_t i = 0;
	for (; i + 8 <= n; i += 8)
	{
		const __m256i vFlags    = _mm256_load_si256(reinterpret_cast<const __m256i*>(packed_flags + i));
		const __m256i vHit      = _mm256_and_si256(vFlags, vReject);
		const __m256i vPassMask = _mm256_cmpeq_epi32(vHit, vZero);
		alignas(32) int32_t lane[8];
		_mm256_store_si256(reinterpret_cast<__m256i*>(lane), vPassMask);
		for (size_t j = 0; j < 8; ++j)
			valid_mask[i + j] &= lane[j] ? 0xFF : 0x00;
	}
	for (; i < n; ++i)
		valid_mask[i] &= ((packed_flags[i] & flag_reject) == 0u) ? 0xFF : 0x00;

	// Phase 2: Pointer-chasing on SIMD-passed positions only.

	std::vector<CreaturePtr> toDamageCreatures;
	toDamageCreatures.reserve(n * 2);

	auto valid_tiles = std::views::iota(size_t{0}, n)
		| std::views::filter([&](size_t idx) { return static_cast<bool>(valid_mask[idx]); })
		| std::views::filter([&](size_t idx) { return not tile_cache[idx]->hasProperty(CONST_PROP_BLOCKPROJECTILE); });

	for (size_t idx : valid_tiles)
	{
		const auto& tile = tile_cache[idx];

		combatTileEffects(spectators, attacker, tile, params);

		const auto& creaturesOnTile = tile->getCreatures();
		if (not creaturesOnTile)
			continue;

		const auto& topCreature = params.targetCasterOrTopMost ? tile->getTopCreature() : nullptr;
		const bool onAttackerTile = (attacker->getTile() == tile);

		auto strikeable = *creaturesOnTile
			| std::views::filter([&](const CreaturePtr& creature) -> bool {
				if (not params.targetCasterOrTopMost)
					return true;
				return onAttackerTile ? (creature == attacker) : (creature == topCreature);
			})
			| std::views::filter([&](const CreaturePtr& creature) -> bool {
				return not (params.aggressive and attacker == creature);
			})
			| std::views::filter([&](const CreaturePtr& creature) -> bool {
				if (not params.aggressive)
					return true;
				switch (static_cast<uint32_t>(creature->getCreatureSubType()))
				{
					case static_cast<uint32_t>(CreatureSubType::Player):
						return target(attacker, PlayerCast(creature)) == CombatCancelCode::None;
					case static_cast<uint32_t>(CreatureSubType::Monster):
						return target(attacker, MonsterCast(creature)) == CombatCancelCode::None;
					default: [[unlikely]]
						return false;
				}
			});

		for (const auto& creature : strikeable)
			toDamageCreatures.push_back(creature);
	}

	// Phase 3: Apply damage across all gathered creatures.
	for (const auto& target_creature : toDamageCreatures)
	{
		damage = getCombatDamage(attacker, target_creature);
		strike_target(attacker, target_creature);
	}
}

void Combat::doTargetCombat(const CreaturePtr& caster, const CreaturePtr& target, CombatDamage& damage, const CombatParams& params, bool sendDistanceEffect, const RawModifierMap* precomputedAttackMods)
{	
	// To-do : I need to properly handle augment based damage which requires entire reworking of this method.
	// The thing that needs to happen is for augment based damage should not interact again with other aumgent
	// based damage. Instead of using origin for this, would possibly be better as fields on the combat or combat params.

	if (params.distanceEffect != CONST_ANI_NONE and sendDistanceEffect)
		addDistanceEffect(caster, caster->getPosition(), target->getPosition(), params.distanceEffect);

	if (not target)
		return;

	gtl::node_hash_map<uint8_t, ModifierTotals> attackModData;
	bool isAugmented = damage.augmented;

	if (caster)
	{
		if (caster->is_player())
		{
			const auto& casterPlayer = caster->getPlayer();
			attackModData.reserve(ATTACK_MODIFIER_LAST);
			auto targetType = target->getMonster()	? casterPlayer->getCreatureType(target->getMonster()) : CREATURETYPE_PLAYER; // we can't target npc's right now so this is safe
			attackModData = precomputedAttackMods	? casterPlayer->getAttackModifierTotals(*precomputedAttackMods, damage.primary.type, damage.origin, targetType, target->getRace(), target->getName())
													: casterPlayer->getAttackModifierTotals(damage.primary.type, damage.origin, targetType, target->getRace(), target->getName());

			/// we do conversion here incase someone wants to convert say healing to mana or mana to death.
			if (casterPlayer->hasConversionModifiers() and not damage.augmented)
			{
				const auto& conversionTotals = casterPlayer->getConvertedTotals(ATTACK_MODIFIER_CONVERSION, damage.primary.type, damage.origin, targetType, target->getRace(), target->getName());
				casterPlayer->convertDamage(target->getCreature(), damage, conversionTotals);

				if (damage.primary.value == 0)
					return;
			}

			if (damage.primary.type != COMBAT_MANADRAIN and damage.primary.type != COMBAT_HEALING)
			{
				// to-do: checking against origin for augment is too limiting.. Lets make piercing like crit and leech, ect.

				if (not attackModData.empty() and params.origin != ORIGIN_PIERCING)
				{
					const auto [piercingFlatTotal, piercingPercentTotal] = attackModData[ATTACK_MODIFIER_PIERCING];
					int32_t piercingDamage = 0;
					const auto originalDamage = std::abs(damage.primary.value);

					if (piercingPercentTotal)
					{
						const auto piercePercent = static_cast<int32_t>(piercingPercentTotal);
						piercingDamage = (piercingPercentTotal <= 100) ? (originalDamage * piercePercent / 100)	: damage.primary.value;
					}

					if (piercingFlatTotal)
						piercingDamage += static_cast<int32_t>(piercingFlatTotal);

					if (piercingDamage)
					{
						piercingDamage = std::min<int32_t>(piercingDamage, originalDamage);
						damage.primary.value += piercingDamage;
						auto piercing = CombatDamage(COMBAT_UNDEFINEDDAMAGE, ORIGIN_AUGMENT, BLOCK_NONE, (0 - piercingDamage), false, true, true);
						casterPlayer->sendTextMessage(MESSAGE_EVENT_DEFAULT, "You pierced " + target->getName() + " for " + std::to_string(piercingDamage) + " damage!");
						g_game.combatChangeHealth(caster, target, piercing);

						if (damage.primary.value == 0)
							return;
					}
				}

				if (params.origin != ORIGIN_PIERCING)
				{
					const auto blocked = g_game.combatBlockHit(damage, caster, target, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances);

					if (blocked)
						return;
				}

				if (not damage.critical)
				{
					int32_t percentTotal = 0;
					int32_t flatTotal = 0;

					if (not attackModData.empty())
					{
						percentTotal = attackModData[ATTACK_MODIFIER_CRITICAL].percentTotal;
						flatTotal = attackModData[ATTACK_MODIFIER_CRITICAL].flatTotal;
					}

					// normal crits are the old ones and are percent based
					const auto normalCritChance = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_CRITICALHITCHANCE));
					const auto normalCritDamage = static_cast<int32_t>(casterPlayer->getSpecialSkill(SPECIALSKILL_CRITICALHITAMOUNT));

					// note : the way this works, its own damage increase is independent allowing for more than 100
					// and also at the same time, its chance is independent, so it doesn't add to augmented crit's chance.
					if ((normalCritChance and normalCritDamage) and (normal_random(1, 100) <= normalCritChance))
						percentTotal += normalCritDamage;

					// we do percent based crits first, so that the flat damage doesn't add to the percent increase.
					if (percentTotal)
					{
						const auto damageIncrease = std::abs(damage.primary.value * percentTotal / 100);
						damage.primary.value -= damageIncrease;
						damage.critical = true;
						damage.augmented = true;
					}

					if (flatTotal)
					{
						damage.primary.value -= flatTotal;
						damage.critical = true;
						damage.augmented = true;
					}
				}

				isAugmented = damage.augmented;

				if (target->is_player() and (caster != target) and not isAugmented)
				{
					const auto& targetPlayer = target->getPlayer();
					const auto& reformTotals = targetPlayer->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());

					if (not reformTotals.empty())
					{
						targetPlayer->reformDamage(casterPlayer->getCreature(), damage, reformTotals);

						if (damage.primary.value == 0)
							return;
					}

					const auto& defenseModData = targetPlayer->getDefenseModifierTotals(damage.primary.type, damage.origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());

					if (not defenseModData.empty())
					{
						for (const auto& [modkind, modTotals] : defenseModData)
						{
							if (modTotals.percentTotal or modTotals.flatTotal)
							{
								applyDamageReductionModifier(modkind, damage, targetPlayer, caster->getCreature(), static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);

								if (damage.primary.value == 0)
									return;
							}
						}
					}
				}
			}
		}
		else if (caster->is_monster())
		{
			const auto& casterMonster = caster->getMonster();

			if (g_game.combatBlockHit(damage, caster, target, params.blockedByShield, params.blockedByArmor, params.itemId != 0, params.ignoreResistances))
				return;

			if (target->is_player())
			{
				const auto& targetPlayer = target->getPlayer();
				const auto& attackerType = targetPlayer->getCreatureType(casterMonster->getMonster());
				
				if (targetPlayer->hasReformModifiers() and not isAugmented)
				{
					const auto& reformTotals = targetPlayer->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage.primary.type, damage.origin, attackerType, casterMonster->getRace(), casterMonster->getName());
					targetPlayer->reformDamage(casterMonster->getCreature(), damage, reformTotals);

					if (damage.primary.value == 0)
						return;
				}

				if (targetPlayer->hasDefenseModifiers() and not isAugmented)
				{
					const auto& defenseModData = targetPlayer->getDefenseModifierTotals(damage.primary.type, damage.origin, attackerType, casterMonster->getRace(), casterMonster->getName());

					for (const auto& [modkind, modTotals] : defenseModData)
					{
						if (modTotals.percentTotal or modTotals.flatTotal)
						{
							applyDamageReductionModifier(modkind, damage, targetPlayer, caster->getCreature(), static_cast<int32_t>(modTotals.percentTotal), static_cast<int32_t>(modTotals.flatTotal), params.origin, params.impactEffect, params.distanceEffect);

							if (damage.primary.value == 0)
								return;
						}
					}
				}
			}
		}
	}

	const auto& success = (damage.primary.type == COMBAT_MANADRAIN) ? g_game.combatChangeMana(caster, target, damage) :	g_game.combatChangeHealth(caster, target, damage);
	
	if (success)
	{
		if (target and caster and target != caster)
		{
			if (params.impactEffect != CONST_ME_NONE)
				g_game.addMagicEffect(target->getPosition(), params.impactEffect);

			if (damage.critical)
			{
				if ((damage.augmented and g_config.GetBoolean(ConfigManager::AUGMENT_CRITICAL_ANIMATION)) or not (damage.augmented))
					g_game.addMagicEffect(target->getPosition(), CONST_ME_CRITICAL_DAMAGE);
			}

			for (const auto& condition : params.conditionList)
			{
				if (!target->isImmune(condition->getType()))
				{
					Condition* conditionCopy = condition->clone();

					if (caster)
						conditionCopy->setParam(CONDITION_PARAM_OWNER, caster->getID());

					//TODO: infight condition until all aggressive conditions has ended
					target->addCombatCondition(conditionCopy);
				}
			}

			processImbuements(caster, target, damage);

			if (not damage.leeched 
				and damage.primary.type != COMBAT_HEALING 
				and caster->is_player()
				and damage.origin != ORIGIN_CONDITION)
			{
				const auto& casterPlayer = caster->getPlayer();
				const auto totalDamage = std::abs(damage.primary.value + damage.secondary.value);
				int32_t lifeStealPercentTotal = 0, manaStealPercentTotal = 0, staminaStealPercentTotal = 0, soulStealPercentTotal = 0;
				int32_t lifeStealFlatTotal = 0, manaStealFlatTotal = 0, staminaStealFlatTotal = 0, soulStealFlatTotal = 0;
				int32_t lifeStealGain = 0, manaStealGain = 0, soulGain = 0, staminaGain = 0;
				
				// Static cast everything to int32_t to ensure consistency
				if (not attackModData.empty() and not isAugmented)
				{
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
				if ((lifeLeechChance and lifeLeechAmount) and (normal_random(1, 100) <= lifeLeechChance))
					lifeStealGain += totalDamage * lifeLeechAmount / 100;

				if (lifeStealPercentTotal)
					lifeStealGain += totalDamage * lifeStealPercentTotal / 100;

				if (lifeStealFlatTotal)
					lifeStealGain += (lifeStealFlatTotal);

				if (lifeStealGain)
				{
					auto lifeStealCombat = CombatDamage(COMBAT_LIFEDRAIN, ORIGIN_AUGMENT, BLOCK_NONE, lifeStealGain, damage.critical, true, true);
					lifeStealCombat.primary.value = std::abs(lifeStealGain);
					g_game.combatChangeHealth(caster, target, lifeStealCombat);
				}

				/// Manasteal
				if ((manaLeechChance and manaLeechAmount) and (normal_random(1, 100) <= manaLeechChance))
					manaStealGain += totalDamage * manaLeechAmount / 100;

				if (manaStealPercentTotal)
					manaStealGain += totalDamage * manaStealPercentTotal / 100;

				if (manaStealFlatTotal)
					manaStealGain += manaStealFlatTotal;

				if (manaStealGain)
				{
					auto manaStealCombat = CombatDamage(COMBAT_MANADRAIN, ORIGIN_AUGMENT, BLOCK_NONE, manaStealGain, damage.critical, true, true);
					g_game.combatChangeMana(caster, target, manaStealCombat);
				}

				/// Staminasteal
				if (staminaStealPercentTotal)
					staminaGain += totalDamage * staminaStealPercentTotal / 100;

				if (staminaStealFlatTotal)
					staminaGain += staminaStealFlatTotal;

				SpectatorVec stealEffectSpectators;
				bool stealEffectSpectatorsFetched = false;

				if (staminaGain)
				{
					if (staminaGain <= std::numeric_limits<uint16_t>::max())
					{
						const uint16_t trueStaminaGain = g_config.GetBoolean(ConfigManager::AUGMENT_STAMINA_RULE) ?	static_cast<uint16_t>(staminaGain) : static_cast<uint16_t>(staminaGain / 60);
						const uint16_t currentStamina = casterPlayer->getStaminaMinutes();
						const uint16_t missingStamina = (MaximumStamina - currentStamina);

						if ((trueStaminaGain + currentStamina) >= missingStamina)
						{
							casterPlayer->addStamina(missingStamina);
						}
						else
						{
							casterPlayer->addStamina(trueStaminaGain);
						}
					}
					else
					{
						casterPlayer->addStamina(MaximumStamina - casterPlayer->getStaminaMinutes());
					}
					g_game.map.getSpectators(stealEffectSpectators, casterPlayer->getPosition(), true, true);
					stealEffectSpectatorsFetched = true;
					Game::addMagicEffect(stealEffectSpectators, casterPlayer->getPosition(), CONST_ME_YELLOWENERGY, casterPlayer->getInstanceID());
				}

				// Soulsteal
				if (soulStealPercentTotal)
					soulGain += totalDamage * soulStealPercentTotal /  100;

				if (soulStealFlatTotal)
					soulGain += soulStealFlatTotal;

				if (soulGain)
				{
					if (soulGain <= std::numeric_limits<uint8_t>::max())
					{
						const uint8_t trueSoulGain = static_cast<uint8_t>(soulGain);
						const uint8_t currentSoul = casterPlayer->getSoul();
						const uint8_t maxSoul = casterPlayer->getVocation()->getSoulMax();
						const uint8_t missingSoul = (maxSoul - currentSoul);

						if ((trueSoulGain + currentSoul) >= maxSoul)
						{
							casterPlayer->addSoul(missingSoul);
						}
						else
						{
							casterPlayer->addSoul(trueSoulGain);
						}
					}
					else
					{
						casterPlayer->addSoul(casterPlayer->getVocation()->getSoulMax() - casterPlayer->getSoul());
					}

					if (not stealEffectSpectatorsFetched)
						g_game.map.getSpectators(stealEffectSpectators, casterPlayer->getPosition(), true, true);

					Game::addMagicEffect(stealEffectSpectators, casterPlayer->getPosition(), CONST_ME_MAGIC_GREEN, casterPlayer->getInstanceID());
				}
			}
		}

		// after looking into this nonsense, I have determined the best course is propagate the bool
		// variable for "force" in the removeCombatCondition call, and to use that one
		// for dispeling conditions in combat, using force == true, whats going on currently makes no sense
		if (params.dispelType & CONDITION_PARALYZE)
			target->removeCondition(CONDITION_PARALYZE);
		else
			target->removeCombatCondition(params.dispelType);
	}

	if (params.targetCallback)
		params.targetCallback->onTargetCombat(caster, target);
}

void Combat::doAreaCombat(const CreaturePtr& caster, const Position& position, const AreaCombat* area, const CombatDamage& damage, const CombatParams& params)
{
	const auto& p = params;
	const auto& tiles = caster ? getCombatArea(caster->getPosition(), position, area) :	getCombatArea(position, position, area);

	if (tiles.empty()) 
	{
		return;
	}

	const auto& casterPlayer = caster ? caster->getPlayer() : nullptr;

	uint32_t maxX = 0;
	uint32_t maxY = 0;
	for (const auto& tile : tiles) 
	{
		const Position& tilePos = tile->getPosition();
		maxX = std::max<int32_t>(maxX, Position::getDistanceX(tilePos, position));
		maxY = std::max<int32_t>(maxY, Position::getDistanceY(tilePos, position));
	}

	const int32_t rangeX = maxX + Map::maxViewportX;
	const int32_t rangeY = maxY + Map::maxViewportY;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, position, true, true, rangeX, rangeX, rangeY, rangeY);
	postCombatEffects(caster, position, p);
	std::vector<CreaturePtr> toDamageCreatures;
	toDamageCreatures.reserve(100);

	for (const auto& tile : tiles) 
	{
		if (canDoCombat(caster, tile, p.aggressive) != RETURNVALUE_NOERROR) 
		{
			continue;
		}

		combatTileEffects(spectators, caster, tile, p);

		if (const auto& creaturesOnTile = tile->getCreatures()) 
		{
			const auto& topCreature = p.targetCasterOrTopMost ? tile->getTopCreature() : nullptr;

			for (const auto& creature : *creaturesOnTile) 
			{
				if (p.targetCasterOrTopMost) 
				{
					if (caster && caster->getTile() == tile) 
					{
						if (creature != caster) 
						{
							continue;
						}
					}
					else if (creature != topCreature) 
					{
						continue;
					}
				}


				if (not p.aggressive or (caster != creature and Combat::canDoCombat(caster, creature) == RETURNVALUE_NOERROR)) 
				{
					toDamageCreatures.push_back(creature);

					if (p.targetCasterOrTopMost) 
					{
						break;
					}
				}
			}
		}
	}

	RawModifierMap areaAttackMods;
	const RawModifierMap* precomputedAttackMods = nullptr;

	if (casterPlayer)
	{
		areaAttackMods = casterPlayer->getAttackModifiers();
		precomputedAttackMods = &areaAttackMods;
	}

	for (const auto& target : toDamageCreatures)
	{
		CombatDamage local_damage = damage;
		Combat::doTargetCombat(caster, target, local_damage, p, false, precomputedAttackMods);
	}
}

void Combat::applyDamageIncreaseModifier
	(const uint8_t modifierType,
	CombatDamage& damage,
	const int32_t percentValue,
	const int32_t flatValue) {

	if (flatValue) {
		damage.primary.value += flatValue;
	}
	if (percentValue) {
		if (percentValue <= 100) {
			damage.primary.value += damage.primary.value * (percentValue / 100.0);
		} else {
			damage.primary.value *= 2;
		}
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

void ValueCallback::getMinMaxValues(const PlayerPtr& player, CombatDamage& damage, const CreaturePtr& target) const
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

		case COMBAT_FORMULA_TARGET:
        {
			// onGetPlayerMinMaxValues(player, target)
            LuaScriptInterface::pushSharedPtr(L, target);
            LuaScriptInterface::setCreatureMetatable(L, -1, target);
            parameters += 1;
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

const MatrixArea& AreaCombat::getArea(const Position& centerPos, const Position& targetPos) const
{
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
	auto area = CreateArea(vec, rows);
	if (areas.size() == 0) {
		areas.resize(4);
	}

	areas[DIRECTION_EAST] = area.Rotate90();
	areas[DIRECTION_SOUTH] = area.Rotate180();
	areas[DIRECTION_WEST] = area.Rotate270();
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
	auto area = CreateArea(vec, rows);
	areas.resize(8);
	areas[DIRECTION_NORTHEAST] = area.Rotate90();
	areas[DIRECTION_SOUTHEAST] = area.Rotate180();
	areas[DIRECTION_SOUTHWEST] = area.Rotate270();
	areas[DIRECTION_NORTHWEST] = std::move(area);
}

static bool combatCanInteractInSameInstance(const CreatureConstPtr& first, const CreatureConstPtr& second)
{
	return first and second and first->compareInstance(second->getInstanceID());
}

// todo: split into multiple class member functions, for the different blocktypes
static auto sendBlockEffect(const BlockType_t blockType, const CombatType_t combatType, const Position& targetPos, uint32_t instanceId)
{
	SpectatorVec localSpectators;
	g_game.map.getSpectators(localSpectators, targetPos, true, true);

	if (blockType == BLOCK_DEFENSE)
	{
		Game::addMagicEffect(localSpectators, targetPos, CONST_ME_POFF, instanceId);
	}
	else if (blockType == BLOCK_ARMOR)
	{
		Game::addMagicEffect(localSpectators, targetPos, CONST_ME_BLOCKHIT, instanceId);
	}
	else if (blockType == BLOCK_IMMUNITY)
	{
		uint8_t hitEffect = 0;

		switch (combatType)
		{
		case COMBAT_UNDEFINEDDAMAGE:
			return;

		case COMBAT_ENERGYDAMAGE:
		case COMBAT_FIREDAMAGE:
		case COMBAT_PHYSICALDAMAGE:
		case COMBAT_ICEDAMAGE:
		case COMBAT_DEATHDAMAGE:
			hitEffect = CONST_ME_BLOCKHIT;
			break;

		case COMBAT_EARTHDAMAGE:
			hitEffect = CONST_ME_GREEN_RINGS;
			break;

		case COMBAT_HOLYDAMAGE:
			hitEffect = CONST_ME_HOLYDAMAGE;
			break;

		default:
			hitEffect = CONST_ME_POFF;
			break;
		}

		Game::addMagicEffect(localSpectators, targetPos, hitEffect, instanceId);
	}
}

bool Combat::block(const CreaturePtr& attacker, const PlayerPtr& target) const noexcept
{


	return true;
}

bool Combat::block(const CreaturePtr& attacker, const MonsterPtr& target) const noexcept
{


	return true;
}


uint32_t Combat::collect_notice_data(const CreaturePtr& target) const noexcept
{
	const uint16_t combat_key = (static_cast<uint16_t>(params.combatType) << 8) | (params.combatType == COMBAT_PHYSICALDAMAGE ? static_cast<uint8_t>(target->getRace()) : 0);

	switch (combat_key)
	{
		case (COMBAT_PHYSICALDAMAGE << 8) | RACE_VENOM:		return PackNotice(TEXTCOLOR_LIGHTGREEN, CONST_ME_HITBYPOISON, FLUID_SLIME);
		case (COMBAT_PHYSICALDAMAGE << 8) | RACE_BLOOD:		return PackNotice(TEXTCOLOR_RED, CONST_ME_DRAWBLOOD, FLUID_BLOOD);
		case (COMBAT_PHYSICALDAMAGE << 8) | RACE_UNDEAD:	return PackNotice(TEXTCOLOR_LIGHTGREY, CONST_ME_HITAREA);
		case (COMBAT_PHYSICALDAMAGE << 8) | RACE_FIRE:		return PackNotice(TEXTCOLOR_ORANGE, CONST_ME_DRAWBLOOD);
		case (COMBAT_PHYSICALDAMAGE << 8) | RACE_ENERGY:	return PackNotice(TEXTCOLOR_ELECTRICPURPLE, CONST_ME_ENERGYHIT);
		case (COMBAT_ENERGYDAMAGE << 8):					return PackNotice(TEXTCOLOR_ELECTRICPURPLE, CONST_ME_ENERGYHIT);
		case (COMBAT_EARTHDAMAGE << 8):						return PackNotice(TEXTCOLOR_LIGHTGREEN, CONST_ME_GREEN_RINGS);
		case (COMBAT_DROWNDAMAGE << 8):						return PackNotice(TEXTCOLOR_LIGHTBLUE, CONST_ME_LOSEENERGY);
		case (COMBAT_FIREDAMAGE << 8):						return PackNotice(TEXTCOLOR_ORANGE, CONST_ME_HITBYFIRE);
		case (COMBAT_ICEDAMAGE << 8):						return PackNotice(TEXTCOLOR_SKYBLUE, CONST_ME_ICEATTACK);
		case (COMBAT_HOLYDAMAGE << 8):						return PackNotice(TEXTCOLOR_YELLOW, CONST_ME_HOLYDAMAGE);
		case (COMBAT_DEATHDAMAGE << 8):						return PackNotice(TEXTCOLOR_DARKRED, CONST_ME_SMALLCLOUDS);
		case (COMBAT_LIFEDRAIN << 8):						return PackNotice(TEXTCOLOR_RED, CONST_ME_MAGIC_RED);
	}
	// should never happen, log here..
	return 0;
}

void Combat::notify_players()
{
	// we need the three types of notifications
	// ourself, our target/attacker, and the observers
	// we should probably be able to apply intinsics here too

	// onTargetCombat()
	// onPositionCombat()
}


bool Combat::apply_damage(const CreaturePtr& attacker, const PlayerPtr& target) noexcept
{
	// if (not doTargetCombat(combat, attacker, target)) return false;

	TextMessage attacker_message;
	TextMessage defender_message;
	TextMessage observer_message;

	return true;
}

bool Combat::apply_damage(const CreaturePtr& attacker, const Monster& target) noexcept
{

	// if (not doTargetCombat(combat, attacker, target)) return false;


	return true;
}

bool Combat::apply_damage(const CreaturePtr& attacker, const Position& target_position) noexcept
{
	// if (not doPositionCombat(combat, attacker, position)) return false;


	return true;
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
