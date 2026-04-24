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

namespace BlackTek
{
    CombatRegistry g_combat_registry;

	std::pmr::unordered_map<int64_t, CombatArea> combat_area_map{ BlackTek::g_combat_registry.Allocator() };
	std::pmr::unordered_map<int64_t, CombatArea> combat_ext_area_map{ BlackTek::g_combat_registry.Allocator() };

	void intrusive_ptr_release(const Combat* p) noexcept
	{
		if (p->ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			BlackTek::g_combat_registry.Release(p->combat_id);
		}
	}

	CombatHandle CombatRegistry::Create()
	{
		const int64_t newId = next_id_.fetch_add(1, std::memory_order_relaxed);
		auto [it, ok] = table_.try_emplace(newId);
		it->second.set_id(newId);
		return CombatHandle(&it->second);   // add_ref fires here so ref_count becomes 1
	}

	CombatHandle CombatRegistry::Create(uint16_t type, uint32_t dmg)
	{
		auto handle = Create();
		handle->damage_type = type;
		handle->damage = dmg;
		return handle;
	}

	void CombatRegistry::Release(int64_t id)
	{
		table_.erase(id);   // hopefully destructs Combat in-place and returns the memory block back to the pool as expected
	}

	Combat* CombatRegistry::Get(int64_t id) noexcept
	{
		auto it = table_.find(id);
		return (it != table_.end()) ? &it->second : nullptr;
	}

	const Combat* CombatRegistry::Get(int64_t id) const noexcept
	{
		auto it = table_.find(id);
		return (it != table_.end()) ? &it->second : nullptr;
	}

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

	static NoticeData UnPackNotice(uint32_t packedValue) { return std::bit_cast<NoticeData>(packedValue); }

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

	// Extracts DamageLocations from an already-rotated MatrixArea.
	// After rotation, each active cell's offsets from the center ARE the world dx/dy:
	//   spread  = col - cx  →  dx (east positive)
	//   forward = row - cy  →  dy (south positive)
	// With this new way of handling it, the caller is now responsible for passing the correctly-rotated matrix for each direction,
	// but we have the absolute minimal data required as integer math applicable to target location, which should drastically reduce
	// the actual workload taking place during combat execution... the future improvements to this design will be to retrieve it as a
	// constant read only view (std::span) of the contiguous data for use during execution
	static std::vector<DamageLocation> CreateDamageLocations(const MatrixArea& area)
	{
		if (area.GetRows() == 0 or area.GetCols() == 0)
			return {};

		const auto [cx, cy] = area.GetCenter();

		std::vector<DamageLocation> locations;

		for (uint32_t row = 0; row < area.GetRows(); ++row)
		{
			for (uint32_t col = 0; col < area.GetCols(); ++col)
			{
				if (area(row, col))
				{
					locations.push_back({
						.spread  = static_cast<int16_t>(static_cast<int32_t>(col) - static_cast<int32_t>(cx)),
						.forward = static_cast<int16_t>(static_cast<int32_t>(row) - static_cast<int32_t>(cy))
					});
				}
			}
		}

		return locations;
	}

	const DamageArea Combat::getAreaPositions(const Position& casterPos, const Position& targetPos)
	{
		area_position_buffer.clear();

		const int32_t dx = Position::getOffsetX(targetPos, casterPos);
		const int32_t dy = Position::getOffsetY(targetPos, casterPos);
		const std::vector<DamageLocation>* locations = nullptr;

		if ((dx != 0) and (dy != 0))
		{
			auto extIt = combat_ext_area_map.find(combat_id);
			if (extIt != combat_ext_area_map.end())
			{
				// Diagonal direction → strip the diagonal mask bit to get a 0-3 slot index.
				// SW=4→0, SE=5→1, NW=6→2, NE=7→3 (matches how GetExtCombatArea stores them).
				Direction diagDir;
				if      (dx < 0 and dy < 0) diagDir = DIRECTION_NORTHWEST;
				else if (dx > 0 and dy < 0) diagDir = DIRECTION_NORTHEAST;
				else if (dx < 0)            diagDir = DIRECTION_SOUTHWEST;
				else                        diagDir = DIRECTION_SOUTHEAST;

				const uint8_t slot = static_cast<uint8_t>(diagDir) ^ DIRECTION_DIAGONAL_MASK;
				locations = &extIt->second.directions[slot];
			}
		}

		if (not locations)
		{
			auto it = combat_area_map.find(combat_id);
			if (it == combat_area_map.end())
				return { area_position_buffer, 0 };

			Direction cardDir;
			if      (dx < 0) cardDir = DIRECTION_WEST;
			else if (dx > 0) cardDir = DIRECTION_EAST;
			else if (dy < 0) cardDir = DIRECTION_NORTH;
			else             cardDir = DIRECTION_SOUTH;

			locations = &it->second.directions[cardDir];
		}

		if (locations->empty())
			return { area_position_buffer, 0 };

		area_position_buffer.reserve(locations->size());

		for (const auto& loc : *locations)
			area_position_buffer.emplace_back(static_cast<uint16_t>(targetPos.x + loc.spread), static_cast<uint16_t>(targetPos.y + loc.forward), targetPos.z);

		const int32_t distance = std::max(std::abs(dx), std::abs(dy));
		return { area_position_buffer, distance };
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
		if (impactEffect != CONST_ME_NONE)
			Game::addMagicEffect(spectators, tile->getPosition(), impactEffect, caster->getInstanceID());

		if (itemId == 0)
			return;

		uint16_t resolvedItemId = resolve_persistent_item(itemId);
		auto casterPlayer = (caster->isSummon()) ? caster->getMaster()->getPlayer() : caster->getPlayer();

		if (casterPlayer)
		{
			if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or tile->hasFlag(TILESTATE_NOPVPZONE)) [[unlikely]]
			{
				resolvedItemId = resolve_nopvp_item(resolvedItemId);
			}
			else if (triggers_in_fight(resolvedItemId)) [[unlikely]]
			{
				casterPlayer->addInFightTicks();
			}
		}

		auto item = Item::CreateItem(resolvedItemId);

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
	}

	void Combat::apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles)
	{
		if (tiles.empty()) [[unlikely]]
			return;

		const uint16_t baseItemId	= (itemId != 0) ? resolve_persistent_item(itemId) : 0;
		const bool hasItem			= baseItemId != 0;
		//const bool hasCallback		= tileCallback != nullptr;
		const bool hasEffect		= impactEffect != CONST_ME_NONE;
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

			if (hasEffect)
				Game::addMagicEffect(spectators, tile->getPosition(), impactEffect, casterInstanceID);
		}
	}


	void Combat::postCombatEffects(const CreaturePtr& caster, const Position& pos, const Combat& combat)
	{
		if (caster and (combat.distanceEffect != CONST_ANI_NONE)) {
			addDistanceEffect(caster, caster->getPosition(), pos, combat.distanceEffect);
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

	Combat::TargetCode Combat::target(const PlayerPtr& caster, const PlayerPtr& victim) const noexcept
	{
		// for now we will still hardcode all of these restrictions, but eventually, they can all be moved to lua.
		if (damage_type == DamageType::Healing)
			return Combat::TargetCode::Valid;

		const bool attack_blocked = caster->hasFlag(PlayerFlag_CannotAttackPlayer) | isProtected(caster, victim) | victim->hasFlag(PlayerFlag_CannotBeAttacked);

		if (attack_blocked)
			return Combat::TargetCode::CanNotAttackThisPlayer;

		const auto victim_tile = victim->getTile();
		const auto attacker_tile = caster->getTile();

		const bool attacker_blocked = attacker_tile->hasFlag(TILESTATE_NOPVPZONE);
		const bool target_protected = victim_tile->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE);

		if (attacker_blocked | target_protected)
			return Combat::TargetCode::TargetIsInProtectionZone;

		// if (victim has creature event tryTargetCombat() ) then

		return Combat::TargetCode::Valid;
	}

	Combat::TargetCode Combat::target(const PlayerPtr& caster, const MonsterPtr& victim) const noexcept
	{
		if (damage_type == DamageType::Healing)
			return Combat::TargetCode::Valid;

		if (caster->hasFlag(PlayerFlag_CannotAttackMonster))
			return Combat::TargetCode::CanNotAttackThisMonster;

		if (victim->isSummon() and victim->getMaster()->getPlayer() and victim->getZone() == ZONE_NOPVP)
			return Combat::TargetCode::TargetIsInProtectionZone;

		return Combat::TargetCode::Valid;
	}

	Combat::TargetCode Combat::target(const MonsterPtr& attacker, const PlayerPtr& victim) const noexcept
	{
		if (damage_type == DamageType::Healing)
			return Combat::TargetCode::Valid;

		const auto& master = attacker->getMaster();
		const auto& commander = (master) ? ((master->is_player()) ? master->getPlayer() : nullptr) : nullptr;

		if (commander)
		{
			const bool attack_blocked = commander->hasFlag(PlayerFlag_CannotAttackPlayer) | isProtected(commander, victim) | victim->hasFlag(PlayerFlag_CannotBeAttacked);

			if (attack_blocked)
				return Combat::TargetCode::CanNotAttackThisPlayer;

			if (victim->getTile()->hasFlag(TILESTATE_NOPVPZONE))
				return Combat::TargetCode::TargetIsInProtectionZone;

		}
		return Combat::TargetCode::Valid;
	}

	Combat::TargetCode Combat::target(const MonsterPtr& attacker, const MonsterPtr& victim) const noexcept
	{
		if (damage_type == DamageType::Healing) // probably instances a monster shouldn't be able to heal a monster?
			return Combat::TargetCode::Valid;

		const bool commander = attacker->getMaster() and attacker->getMaster()->is_player();
		const bool ward = victim->getMaster() and victim->getMaster()->is_player();

		if (not commander and not ward)
			return Combat::TargetCode::CanNotAttackThisMonster; // this is tibia style rules, monsters don't harm other monsters in general, we will want this to be the users decision

		return Combat::TargetCode::Valid;
	}

	Combat::TargetCode Combat::target(const PlayerPtr& attacker, const Position& target_location) const noexcept
	{
		const auto tile = g_game.map.getTile(target_location);

		if (not tile)
			return Combat::TargetCode::NotEnoughRoom;

		if (tile->hasProperty(CONST_PROP_BLOCKPROJECTILE) or tile->hasFlag(TILESTATE_FLOORCHANGE) or tile->getTeleportItem())
			return Combat::TargetCode::NotEnoughRoom;

		const auto& attacker_pos = attacker->getPosition();

		if (attacker_pos.z < target_location.z)
			return Combat::TargetCode::FirstGoDownStairs;

		if (attacker_pos.z > target_location.z)
			return Combat::TargetCode::FirstGoUpStairs;

		if (attacker->hasFlag(PlayerFlag_IgnoreProtectionZone))
			return Combat::TargetCode::Valid;

		if (config.test(Config::Aggressive) and tile->hasFlag(TILESTATE_PROTECTIONZONE))
			return Combat::TargetCode::TargetIsInProtectionZone;

		return Combat::TargetCode::Valid;
	}

	Combat::TargetCode Combat::target(const MonsterPtr& attacker, const Position& target_location) const noexcept
	{
		const auto tile = g_game.map.getTile(target_location);

		if (not tile)
			return Combat::TargetCode::NotEnoughRoom;

		if (tile->hasProperty(CONST_PROP_BLOCKPROJECTILE) or tile->hasFlag(TILESTATE_FLOORCHANGE) or tile->getTeleportItem())
			return Combat::TargetCode::NotEnoughRoom;

		const auto& attacker_pos = attacker->getPosition();

		if (attacker_pos.z < target_location.z)
			return Combat::TargetCode::FirstGoDownStairs;

		if (attacker_pos.z > target_location.z)
			return Combat::TargetCode::FirstGoUpStairs;

		if (config.test(Config::Aggressive) and tile->hasFlag(TILESTATE_PROTECTIONZONE))
			return Combat::TargetCode::TargetIsInProtectionZone;

		return Combat::TargetCode::Valid;
	}

	CombatHandle Combat::transformDamage(const uint16_t damage_type, const uint32_t amount)
	{
		return g_combat_registry.Create(damage_type, amount);
	}

	inline void Combat::applyCrit(const uint32_t percent, const uint32_t flat)
	{
		if (percent > 0)
			damage += damage * percent / 100u;

		if (flat > 0)
			damage += flat;

		if (percent or flat)
			config.set(Combat::Config::Critical);
	}

	CombatHandle Combat::penetrateDamage(const uint32_t percent, const uint32_t flat)
	{
		uint32_t penetration_damage = 0;

		if (percent > 0)
			penetration_damage += damage * percent / 100u;

		if (flat > 0)
			penetration_damage += flat;

		penetration_damage = (penetration_damage > damage) ? damage : penetration_damage;
		damage -= penetration_damage;

		auto handle = g_combat_registry.Create(std::to_underlying(Combat::DamageType::Undefined), penetration_damage);
		handle->config.set(Config::TrueDamage);
		return handle;
	}

	void Combat::post_damage(const PlayerPtr& caster, const CreaturePtr& victim, LeechData&& leech) noexcept
	{
		if (impactEffect != CONST_ME_NONE)
			g_game.addMagicEffect(victim->getPosition(), impactEffect);

		if (config.test(Config::Critical))
		{
			if ((config.test(Config::AttackModified) and g_config.GetBoolean(ConfigManager::AUGMENT_CRITICAL_ANIMATION)) or not config.test(Config::AttackModified))
				g_game.addMagicEffect(victim->getPosition(), CONST_ME_CRITICAL_DAMAGE);
		}

		auto primary_conditions = (not config.test(Config::Leech) and damage != COMBAT_HEALING and caster->is_player() and origin != Origin::Condition);

		if (primary_conditions)
		{
			const auto& casterPlayer = caster->getPlayer();
			const auto totalDamage = damage;

			if (leech.percent_health > 0 or leech.flat_health > 0)
			{
				int32_t life = 0;

				if (leech.percent_health)
					life += totalDamage * leech.percent_health / 100;

				if (leech.flat_health)
					life += leech.flat_health;

				if (life)
				{
					// auto local_damage = CombatDamage(COMBAT_LIFEDRAIN, ORIGIN_AUGMENT, BLOCK_NONE, life, critical, true, true);
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
					// auto local_damage = CombatDamage(COMBAT_MANADRAIN, ORIGIN_AUGMENT, BLOCK_NONE, mana, critical, true, true);
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
	}


// ATTENTION 
// ----------
// This method does not care about conversion output damage being more than 
// it's input damage. IT WILL PRODUCE MORE DAMAGE if you do not manage the usage
// of convesion in either value or quantaties (ways to get it, or eq it's on, ect)
// writing a version which caps this would actually be extreme congestive for this
// particular call stack and I prefer to keep it streamlined while allowing
// end users like yourself the ability to abuse this as a feature if desired
uint32_t Combat::handle_conversion(std::ranges::input_range auto&& modifiers, auto attacker, auto victim)
{
    uint32_t physical = 0, energy = 0, earth = 0, fire = 0;
	uint32_t undefined = 0, lifedrain = 0, manadrain = 0;
	uint32_t healing = 0, water = 0, ice = 0;
	uint32_t holy = 0, death = 0, total = 0;

    for (const auto& modifier : modifiers)
    {
        const uint32_t idx = std::countr_zero(modifier.to_damage_type);
        const uint32_t value = modifier.isFlatValue() ? modifier.value : (damage * modifier.value * 1374389535ULL) >> 37;
		total += value;

        switch (idx)
        {
            case 0:  physical   += value; break;
            case 1:  energy     += value; break;
            case 2:  earth      += value; break;
            case 3:  fire       += value; break;
            case 4:  undefined  += value; break;
            case 5:  lifedrain  += value; break;
            case 6:  manadrain  += value; break;
            case 7:  healing    += value; break;
            case 8:  water      += value; break;
            case 9:  ice        += value; break;
            case 10: holy       += value; break;
            case 11: death      += value; break;
            default: [[unlikely]] break;
        }

    }

    if (physical)	Combat::transformDamage(Combat::DamageType::Physical,  physical)	->strike_target(attacker, victim);
    if (energy)		Combat::transformDamage(Combat::DamageType::Energy,    energy)		->strike_target(attacker, victim);
    if (earth)		Combat::transformDamage(Combat::DamageType::Earth,     earth)		->strike_target(attacker, victim);
    if (fire)		Combat::transformDamage(Combat::DamageType::Fire,      fire)		->strike_target(attacker, victim);
    if (undefined)	Combat::transformDamage(Combat::DamageType::Undefined, undefined)	->strike_target(attacker, victim);
    if (lifedrain)	Combat::transformDamage(Combat::DamageType::LifeDrain, lifedrain)	->strike_target(attacker, victim);
    if (manadrain)	Combat::transformDamage(Combat::DamageType::ManaDrain, manadrain)	->strike_target(attacker, victim);
    if (healing)	Combat::transformDamage(Combat::DamageType::Healing,   healing)		->strike_target(attacker, victim);
    if (water)		Combat::transformDamage(Combat::DamageType::Water,     water)		->strike_target(attacker, victim);
    if (ice)		Combat::transformDamage(Combat::DamageType::Ice,       ice)			->strike_target(attacker, victim);
    if (holy)		Combat::transformDamage(Combat::DamageType::Holy,      holy)		->strike_target(attacker, victim);
    if (death)		Combat::transformDamage(Combat::DamageType::Death,     death)		->strike_target(attacker, victim);

    return total;
}

	void Combat::strike_target(const PlayerPtr& caster, const PlayerPtr& victim) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		 if (not target(caster, victim))
		 { 
			 // handle error code and send player a message
			 return;
		 }
		
		// if (not non_aggressive())
		//		if (healing) doHealing()
		//      else
		//		postConditions or w/e reason this combat would exist as non-aggressive but isn't healing damage type

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			const auto blocked = block(caster, victim);

			if (blocked)
				return;
		}

		LeechData leech_data {};

		if (caster->hasAttackModifiers() and not config.test(Config::AttackModified))
		{
			const auto conversion_count = caster->conversion_mod_count();
			const auto victim_race = victim->getRace();
			const auto& victim_name = victim->getName();
			const auto moddable = damage_type != DamageType::ManaDrain and damage_type != DamageType::Healing;

			auto applied = [&](const auto& modifier) 
			{
				return modifier.applies( damage_type, CreatureType_t::CREATURETYPE_PLAYER, origin, victim_race, victim_name);
			};

			if (conversion_count > 0)
			{
				const auto& conversion_modifiers = caster->getConversionMods();
				auto converted_damage = handle_conversion(conversion_modifiers | std::views::filter(applied), caster, victim);

				if (converted_damage)
				{
					damage = converted_damage >= damage ? 0 : damage - converted_damage;
					if (damage == 0) return; // instead of doing animations or w/e here, we send the text message to players at the end of the converted damage calculation.
				}
			}

			if (moddable and caster->attack_mod_count() > conversion_count)
			{
				const auto& main_attack_sums = caster->getMainAttackModSums();
				const auto& main_postattack_sums = caster->getMainAttackModPostSums();

				auto percent_crit		= main_attack_sums[std::to_underlying(DamageModifier::AttackType::Critical)].percent;
				auto flat_crit			= main_attack_sums[std::to_underlying(DamageModifier::AttackType::Critical)].flat;
				auto percent_pierce		= main_attack_sums[std::to_underlying(DamageModifier::AttackType::Piercing)].percent;
				auto flat_pierce		= main_attack_sums[std::to_underlying(DamageModifier::AttackType::Piercing)].flat;
				
				if (caster->hasFilteredAttackMods())
				{
					const auto& filtered_attack_mods = caster->getFilteredAttackMods();

					for (const auto& modifier : filtered_attack_mods | std::views::filter(applied))
					{
						const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);

						if (mod_type == DamageModifier::AttackType::Critical)
						{
							percent_crit	+= modifier.isFlatValue() ? 0 : modifier.getValue();
							flat_crit		+= modifier.isFlatValue() ? modifier.getValue() : 0;
						}
						else if (mod_type == DamageModifier::AttackType::Piercing)
						{
							percent_pierce	+= modifier.isFlatValue() ? 0 : modifier.getValue();
							flat_pierce		+= modifier.isFlatValue() ? modifier.getValue() : 0;
						}
					}
				}

				if (caster->hasFilteredAttackPostMods())
				{
					const auto& filtered_post_attack_mods = caster->getFilteredAttackPostMods();

					for (const auto& modifier : filtered_post_attack_mods | std::views::filter(applied))
					{
						const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);

						switch (mod_type)
						{
							case DamageModifier::AttackType::Lifesteal:
							{
								leech_data.percent_health	+= modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_health		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Manasteal:
							{
								leech_data.percent_mana		+= modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_mana		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Staminasteal:
							{
								leech_data.percent_stamina	+= modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_stamina		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Soulsteal:
							{
								leech_data.percent_soul		+= modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_soul		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							default: [[unlikely]]
							{
								// log it
								break;
							}
						}
					}
				}

				if (percent_pierce or flat_pierce)
				{
					penetrateDamage(percent_pierce, flat_pierce);
				}

				if (percent_crit or flat_crit)
				{
					applyCrit(percent_crit, flat_crit);
				}
			}
		}

		if (victim->hasDefenseModifiers())
		{
			const auto reform_count = victim->reform_mod_count();
			const auto attacker_race = caster->getRace();
			const auto& attacker_name = caster->getName();

			if (reform_count > 0)
			{
				//const auto& reform_data = victim->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage_type, origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
				//reform_augment(victim, reform_data);
			}

			if (victim->defense_mod_count() > reform_count)
			{
				//const auto& defense_data = victim->getDefenseModifierTotals(damage_type, origin, CREATURETYPE_PLAYER, caster->getRace(), caster->getName());
				//defense_augment(caster, victim, defense_data);
			}
		}

		const auto& success = apply_damage(caster, victim);

		if (success)
			post_damage(caster, victim, std::move(leech_data));
	}

	void Combat::strike_target(const PlayerPtr& caster, const MonsterPtr& victim) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		if (not target(caster, victim))
		{
			// handle error code and send player a message
			return;
		}

		// if (not non_aggressive())
		//		if (healing) doHealing()
		//      else
		//		postConditions or w/e reason this combat would exist as non-aggressive but isn't healing damage type

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), distanceEffect);

		if (origin != Origin::Piercing)
		{
			const auto blocked = block(caster, victim);

			if (blocked)
				return;
		}

		LeechData leech_data{};

		if (caster->hasAttackModifiers() and not config.test(Config::AttackModified))
		{
			const auto conversion_count = caster->conversion_mod_count();
			const auto victim_race = victim->getRace();
			const auto& victim_name = victim->getName();
			const auto moddable = damage_type != DamageType::ManaDrain and damage_type != DamageType::Healing;

			auto applied = [&](const auto& modifier)
				{
					return modifier.applies(damage_type, CreatureType_t::CREATURETYPE_PLAYER, origin, victim_race, victim_name);
				};

			if (conversion_count > 0)
			{
				const auto& conversion_modifiers = caster->getConversionMods();
				auto converted_damage = handle_conversion(conversion_modifiers | std::views::filter(applied), caster, victim);

				if (converted_damage)
				{
					damage = converted_damage >= damage ? 0 : damage - converted_damage;
					// pretty sure we are good to return now so that's what we will do for now
					if (damage == 0) return;
				}
			}

			if (moddable and caster->attack_mod_count() > conversion_count)
			{
				const auto& main_attack_sums = caster->getMainAttackModSums();
				const auto& main_postattack_sums = caster->getMainAttackModPostSums();

				auto percent_crit = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Critical)].percent;
				auto flat_crit = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Critical)].flat;
				auto percent_pierce = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Piercing)].percent;
				auto flat_pierce = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Piercing)].flat;

				if (caster->hasFilteredAttackMods())
				{
					const auto& filtered_attack_mods = caster->getFilteredAttackMods();

					for (const auto& modifier : filtered_attack_mods | std::views::filter(applied))
					{
						const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);

						if (mod_type == DamageModifier::AttackType::Critical)
						{
							percent_crit += modifier.isFlatValue() ? 0 : modifier.getValue();
							flat_crit += modifier.isFlatValue() ? modifier.getValue() : 0;
						}
						else if (mod_type == DamageModifier::AttackType::Piercing)
						{
							percent_pierce += modifier.isFlatValue() ? 0 : modifier.getValue();
							flat_pierce += modifier.isFlatValue() ? modifier.getValue() : 0;
						}
					}
				}

				if (caster->hasFilteredAttackPostMods())
				{
					const auto& filtered_post_attack_mods = caster->getFilteredAttackPostMods();

					for (const auto& modifier : filtered_post_attack_mods | std::views::filter(applied))
					{
						const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);

						switch (mod_type)
						{
						case DamageModifier::AttackType::Lifesteal:
						{
							leech_data.percent_health += modifier.isFlatValue() ? 0 : modifier.getValue();
							leech_data.flat_health += modifier.isFlatValue() ? modifier.getValue() : 0;
							break;
						}
						case DamageModifier::AttackType::Manasteal:
						{
							leech_data.percent_mana += modifier.isFlatValue() ? 0 : modifier.getValue();
							leech_data.flat_mana += modifier.isFlatValue() ? modifier.getValue() : 0;
							break;
						}
						case DamageModifier::AttackType::Staminasteal:
						{
							leech_data.percent_stamina += modifier.isFlatValue() ? 0 : modifier.getValue();
							leech_data.flat_stamina += modifier.isFlatValue() ? modifier.getValue() : 0;
							break;
						}
						case DamageModifier::AttackType::Soulsteal:
						{
							leech_data.percent_soul += modifier.isFlatValue() ? 0 : modifier.getValue();
							leech_data.flat_soul += modifier.isFlatValue() ? modifier.getValue() : 0;
							break;
						}
						default: [[unlikely]]
						{
							// log it
							break;
						}
						}
					}
				}

				if (percent_pierce or flat_pierce)
				{
					penetrateDamage(percent_pierce, flat_pierce);
				}

				if (percent_crit or flat_crit)
				{
					applyCrit(percent_crit, flat_crit);
				}
			}
		}

		const auto& success = (damage_type == COMBAT_MANADRAIN) ? apply_damage(caster, victim) : apply_damage(caster, victim);

		if (success)
			post_damage(caster, victim, std::move(leech_data));
	}

	void Combat::strike_target(const MonsterPtr& attacker, const PlayerPtr& victim) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		if (not target(attacker, victim))
		{
			// handle error code and send player a message
			return;
		}

		// if (not non_aggressive())
		//		if (healing) doHealing()
		//      else
		//		postConditions or w/e reason this combat would exist as non-aggressive but isn't healing damage type

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), distanceEffect);

		if (origin != Origin::Piercing)
		{
			const auto blocked = block(attacker, victim);

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
				//const auto& reform_data = victim->getConvertedTotals(DEFENSE_MODIFIER_REFORM, damage_type, origin, CREATURETYPE_PLAYER, attacker->getRace(), attacker->getName());
				//reform_augment(victim, reform_data);
			}

			if (victim->defense_mod_count() > reform_count)
			{
				//const auto& defense_data = victim->getDefenseModifierTotals(damage_type, origin, CREATURETYPE_PLAYER, attacker->getRace(), attacker->getName());
				//defense_augment(attacker, victim, defense_data);
			}
		}

		const auto& success = (damage_type == DamageType::ManaDrain) ? apply_damage(attacker, victim) : apply_damage(attacker, victim);

		if (not success)
		{
			// some kind of log here ?
		}
	}

	void Combat::strike_target(const MonsterPtr& attacker, const MonsterPtr& victim) noexcept
	{
		// if (not can_execute(caster, victim)) return;

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), distanceEffect);

		const auto blocked = block(attacker, victim);

		if (blocked)
			return;

		// Do we do anything here for summons attacking or being attacked or anything like that? Perhaps in the future when we allow passing
		// either augments or modifiers directly with combat this version of combat becomes useful, or maybe we revamp the summon system and finally
		// make summon an actual "thing" type derived from monster like I always thought it should be... I just feel like I'm forgetting something obvious right now

		// about the above comment, I think perhaps we can allow multiple formula's for different target creature types, monster, npc, boss or player.

		const auto& success = (damage_type == DamageType::ManaDrain) ? apply_damage(attacker, victim) : apply_damage(attacker, victim);

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

		postCombatEffects(caster, center, *this);

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
			(config.test(Config::Aggressive) and not caster->hasFlag(PlayerFlag_IgnoreProtectionZone)
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
		// Also, it shouldn't be blockprojectile we are checking here, as this occurs after line of sight which should be what considers that
		// so we could likely get away with a dedicated toggle for tiles with acces via "tile->isBlocking()" and passing it to the branchless
		// tile validation above
		std::vector<CreaturePtr> toDamageCreatures;
		toDamageCreatures.reserve(n * 2);

		auto valid_tiles = std::views::iota(size_t{0}, n)
			| std::views::filter([&](size_t idx) { return static_cast<bool>(valid_mask[idx]); })
			| std::views::filter([&](size_t idx) { return not tile_cache[idx]->hasProperty(CONST_PROP_BLOCKPROJECTILE); });

		for (size_t idx : valid_tiles)
		{
			const auto& tile = tile_cache[idx];

			// combatTileEffects(spectators, caster, tile, *this);

			const auto& creaturesOnTile = tile->getCreatures();
			if (not creaturesOnTile)
				continue;

			const auto& topCreature = config.test(Config::TopTargetOnly) ? tile->getTopCreature() : nullptr;
			const bool onCasterTile = (caster->getTile() == tile);

			auto strikeable = *creaturesOnTile
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::TopTargetOnly))
						return true;
					return onCasterTile ? (creature == caster) : (creature == topCreature);
				})
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					return not (config.test(Config::Aggressive) and caster == creature);
				})
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::Aggressive))
						return true;

					switch (creature->getCreatureSubType())
					{
						case CreatureSubType::Player: return target(caster, PlayerCast(creature)) == Combat::TargetCode::Valid;
						case CreatureSubType::Monster: return target(caster, MonsterCast(creature)) == Combat::TargetCode::Valid;
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
			//auto cd = getCombatDamage(caster, target_creature);
			//primary = cd.primary;
			//secondary = cd.secondary;
			//blockType = cd.blockType;
			//critical = cd.critical;
			//leeched = cd.leeched;
			//augmented = cd.augmented;
			//isUtility = cd.isUtility;
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

		postCombatEffects(attacker, center, *this);

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
			(config.test(Config::Aggressive) ? static_cast<uint32_t>(TILESTATE_PROTECTIONZONE) : 0u);

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

			//combatTileEffects(spectators, attacker, tile, *this);

			const auto& creaturesOnTile = tile->getCreatures();
			if (not creaturesOnTile)
				continue;

			const auto& topCreature = config.test(Config::TopTargetOnly) ? tile->getTopCreature() : nullptr;
			const bool onAttackerTile = (attacker->getTile() == tile);

			auto strikeable = *creaturesOnTile
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::TopTargetOnly))
						return true;
					return onAttackerTile ? (creature == attacker) : (creature == topCreature);
				})
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					return not (config.test(Config::Aggressive) and attacker == creature);
				})
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::Aggressive))
						return true;
					switch (static_cast<uint32_t>(creature->getCreatureSubType()))
					{
						case static_cast<uint32_t>(CreatureSubType::Player):
							return target(attacker, PlayerCast(creature)) == Combat::TargetCode::Valid;
						case static_cast<uint32_t>(CreatureSubType::Monster):
							return target(attacker, MonsterCast(creature)) == Combat::TargetCode::Valid;
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
			//auto cd = getCombatDamage(attacker, target_creature);
			//primary = cd.primary;
			//secondary = cd.secondary;
			//blockType = cd.blockType;
			//critical = cd.critical;
			//leeched = cd.leeched;
			//augmented = cd.augmented;
			//isUtility = cd.isUtility;
			strike_target(attacker, target_creature);
		}
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

	CombatArea AreaCombat::GetCombatArea() const
	{
		if (areas.size() < 4)
			return {};

		CombatArea result;
		result.directions[DIRECTION_NORTH] = CreateDamageLocations(areas[DIRECTION_NORTH]);
		result.directions[DIRECTION_EAST]  = CreateDamageLocations(areas[DIRECTION_EAST]);
		result.directions[DIRECTION_SOUTH] = CreateDamageLocations(areas[DIRECTION_SOUTH]);
		result.directions[DIRECTION_WEST]  = CreateDamageLocations(areas[DIRECTION_WEST]);
		return result;
	}

	CombatArea AreaCombat::GetExtCombatArea() const
	{
		if (not hasExtArea or areas.size() < 8)
			return {};

		// Slot layout mirrors the diagonal-mask-strip used in getAreaPositions:
		// direction ^ DIRECTION_DIAGONAL_MASK → SW=4^4=0, SE=5^4=1, NW=6^4=2, NE=7^4=3
		CombatArea result;
		result.directions[DIRECTION_SOUTHWEST ^ DIRECTION_DIAGONAL_MASK] = CreateDamageLocations(areas[DIRECTION_SOUTHWEST]);
		result.directions[DIRECTION_SOUTHEAST ^ DIRECTION_DIAGONAL_MASK] = CreateDamageLocations(areas[DIRECTION_SOUTHEAST]);
		result.directions[DIRECTION_NORTHWEST ^ DIRECTION_DIAGONAL_MASK] = CreateDamageLocations(areas[DIRECTION_NORTHWEST]);
		result.directions[DIRECTION_NORTHEAST ^ DIRECTION_DIAGONAL_MASK] = CreateDamageLocations(areas[DIRECTION_NORTHEAST]);
		return result;
	}

	static bool combatCanInteractInSameInstance(const CreatureConstPtr& first, const CreatureConstPtr& second)
	{
		return first and second and first->compareInstance(second->getInstanceID());
	}


	void Combat::defense_block_effect(const Position& target_position) const noexcept
	{
		//Game::addMagicEffect(localSpectators, targetPos, CONST_ME_POFF, instanceId);
	}

	void Combat::armor_block_effect(const Position& target_position) const noexcept
	{
		//Game::addMagicEffect(localSpectators, targetPos, CONST_ME_BLOCKHIT, instanceId);
	}

	void Combat::immunity_block_effect(const Position& target_position) const noexcept
	{
		uint8_t hitEffect = 0;

		switch (damage_type)
		{
			case DamageType::Undefined:
				return;

			case DamageType::Energy:
			case DamageType::Fire:
			case DamageType::Physical:
			case DamageType::Ice:
			case DamageType::Death:
				hitEffect = CONST_ME_BLOCKHIT;
				break;

			case DamageType::Earth:
				hitEffect = CONST_ME_GREEN_RINGS;
				break;

			case DamageType::Holy:
				hitEffect = CONST_ME_HOLYDAMAGE;
				break;

			default:
				hitEffect = CONST_ME_POFF;
				break;
		}
	}

	Combat::BlockType Combat::block(const CreaturePtr& attacker, const PlayerPtr& target) noexcept
	{
		BlockType blockType = BlockType::NoBlock;

		bool apply_armor_reduction = config.test(BlockedByArmor);
		bool apply_defense_reduction = config.test(Config::BlockedByDefense);

		const float def_mult = target->get_defense_charge_cost_multiplier();
		const float arm_mult = target->get_armor_charge_cost_multiplier();
		const float def_base = static_cast<float>(apply_defense_reduction ? defense_charge_cost : g_config.GetNumber(ConfigManager::DEFAULT_DEFENSE_CHARGE_COST));
		const float arm_base = static_cast<float>(apply_armor_reduction ? armor_charge_cost   : g_config.GetNumber(ConfigManager::DEFAULT_ARMOR_CHARGE_COST));

		const uint32_t defense_cost = static_cast<uint32_t>(std::round(def_base * def_mult));
		const uint32_t armor_cost   = static_cast<uint32_t>(std::round(arm_base * arm_mult));

		if (config.test(Config::BlockedByDefense) and target->can_use_defense()
		    and (defense_cost == 0 or target->get_defense_charges() >= defense_cost))
		{
			if (defense_cost > 0)
				target->set_defense_charges(target->get_defense_charges() - defense_cost);

			int32_t defense = target->getDefense(); // we need to hook here
			damage -= uniform_random(defense / 2, defense); // and this should be a predefined option

			if (damage <= 0)
			{
				damage = 0;
				blockType = BlockType::Defensive;
				apply_armor_reduction = false;
			}

			// This is a wasteful function as it's just a virtual function with player having the
			// one and only overload, which does nothing more than reduce shield block count of the player
			// and increase the skill points for shielding if player has shield equipped (which needs adjusted too)
			target->onBlockHit(); 
		}

		if (apply_armor_reduction and (armor_cost == 0 or target->get_armor_charges() >= armor_cost))
		{
			if (armor_cost > 0)
				target->set_armor_charges(target->get_armor_charges() - armor_cost);

			int32_t armor = target->getArmor(); // this method is ineffective like others in combat, as it iterates over every possible slot rather than only the ones active

			// here, this "threshold" should be configurable (the > 3), and the deduction should be one of our plug in predefines for calculation
			if (armor > 3)
			{
				damage -= uniform_random(armor / 2, armor - (armor % 2 + 1));
			}
			else if (armor > 0)
			{
				--damage;
			}

			if (damage <= 0)
			{
				damage = 0;
				blockType = BlockType::Armor;
			}
		}

		if (attacker and damage_type != DamageType::Healing)
		{
			attacker->onAttackedCreature(target);
			// Warning! We are using old blocktype here with a static cast to get rid of any errors, if we eliminate old blocktype this needs updated!
			attacker->onAttackedCreatureBlockHit(static_cast<BlockType_t>(blockType));

			if (attacker->getMaster() and attacker->getMaster()->getPlayer())
				attacker->getMaster()->getPlayer()->onAttackedCreature(target);
		}

		target->onAttacked();

		if (attacker and damage_type != DamageType::Healing)
			target->sendCreatureSquare(attacker, SQ_COLOR_BLACK);

		return blockType;
	}

	Combat::BlockType Combat::block(const CreaturePtr& attacker, const MonsterPtr& target) noexcept
	{
		BlockType blockType = BlockType::NoBlock;

		bool apply_armor_reduction = config.test(BlockedByArmor);
		bool apply_defense_reduction = config.test(Config::BlockedByDefense);

		const float def_mult = target->get_defense_charge_cost_multiplier();
		const float arm_mult = target->get_armor_charge_cost_multiplier();
		const float def_base = static_cast<float>(apply_defense_reduction ? defense_charge_cost : g_config.GetNumber(ConfigManager::DEFAULT_DEFENSE_CHARGE_COST));
		const float arm_base = static_cast<float>(apply_armor_reduction ? armor_charge_cost : g_config.GetNumber(ConfigManager::DEFAULT_ARMOR_CHARGE_COST));

		const uint32_t defense_cost = static_cast<uint32_t>(std::round(def_base * def_mult));
		const uint32_t armor_cost = static_cast<uint32_t>(std::round(arm_base * arm_mult));

		if (config.test(Config::BlockedByDefense) and target->can_use_defense()
			and (defense_cost == 0 or target->get_defense_charges() >= defense_cost))
		{
			if (defense_cost > 0)
				target->set_defense_charges(target->get_defense_charges() - defense_cost);

			int32_t defense = target->getDefense(); // we need to hook here
			damage -= uniform_random(defense / 2, defense); // and this should be a predefined option

			if (damage <= 0)
			{
				damage = 0;
				blockType = BlockType::Defensive;
				apply_armor_reduction = false;
			}
		}

		if (apply_armor_reduction and (armor_cost == 0 or target->get_armor_charges() >= armor_cost))
		{
			if (armor_cost > 0)
				target->set_armor_charges(target->get_armor_charges() - armor_cost);

			int32_t armor = target->getArmor();

			// here, this "threshold" should be configurable (the > 3), and the deduction should be one of our plug in predefines for calculation
			if (armor > 3)
			{
				damage -= uniform_random(armor / 2, armor - (armor % 2 + 1));
			}
			else if (armor > 0)
			{
				--damage;
			}

			if (damage <= 0)
			{
				damage = 0;
				blockType = BlockType::Armor;
			}
		}

		if (attacker and damage_type != DamageType::Healing)
		{
			attacker->onAttackedCreature(target);
			// Warning! We are using old blocktype here with a static cast to get rid of any errors, if we eliminate old blocktype this needs updated!
			attacker->onAttackedCreatureBlockHit(static_cast<BlockType_t>(blockType));

			if (attacker->isSummon() and attacker->getMaster()->getPlayer())
				attacker->getMaster()->getPlayer()->onAttackedCreature(target);
		}

		target->onAttacked();

		return blockType;
	}

	uint32_t Combat::collect_notice_data(const CreaturePtr& target) const noexcept
	{
		const uint16_t combat_key = (static_cast<uint16_t>(damage_type) << 8) | (damage_type == DamageType::Physical ? static_cast<uint8_t>(target->getRace()) : 0);

		// Todo: all of these should be configurable by end user
		switch (combat_key)
		{
			case (DamageType::Physical << 8) | RACE_VENOM:		return PackNotice(TEXTCOLOR_LIGHTGREEN, CONST_ME_HITBYPOISON, FLUID_SLIME);
			case (DamageType::Physical << 8) | RACE_BLOOD:		return PackNotice(TEXTCOLOR_RED, CONST_ME_DRAWBLOOD, FLUID_BLOOD);
			case (DamageType::Physical << 8) | RACE_UNDEAD:		return PackNotice(TEXTCOLOR_LIGHTGREY, CONST_ME_HITAREA);
			case (DamageType::Physical << 8) | RACE_FIRE:		return PackNotice(TEXTCOLOR_ORANGE, CONST_ME_DRAWBLOOD);
			case (DamageType::Physical << 8) | RACE_ENERGY:		return PackNotice(TEXTCOLOR_ELECTRICPURPLE, CONST_ME_ENERGYHIT);
			case (DamageType::Energy << 8):						return PackNotice(TEXTCOLOR_ELECTRICPURPLE, CONST_ME_ENERGYHIT);
			case (DamageType::Earth << 8):						return PackNotice(TEXTCOLOR_LIGHTGREEN, CONST_ME_GREEN_RINGS);
			case (DamageType::Water << 8):						return PackNotice(TEXTCOLOR_LIGHTBLUE, CONST_ME_LOSEENERGY);
			case (DamageType::Fire << 8):						return PackNotice(TEXTCOLOR_ORANGE, CONST_ME_HITBYFIRE);
			case (DamageType::Undefined << 8):					return PackNotice(TEXTCOLOR_PASTELRED, CONST_ME_PURPLESMOKE);
			case (DamageType::Ice << 8):						return PackNotice(TEXTCOLOR_SKYBLUE, CONST_ME_ICEATTACK);
			case (DamageType::Holy << 8):						return PackNotice(TEXTCOLOR_YELLOW, CONST_ME_HOLYDAMAGE);
			case (DamageType::Death << 8):						return PackNotice(TEXTCOLOR_DARKRED, CONST_ME_SMALLCLOUDS);
			case (DamageType::LifeDrain << 8):					return PackNotice(TEXTCOLOR_RED, CONST_ME_MAGIC_RED);
			default: [[unlikely]] /* should never happen, log here */ return 0;
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

	// For those reading, there are two ways to trigger mana damage, mana-leech/drain/steal or
	// via manashield, we default to manashield as that will be the most common route, but when manashield
	// is false it's because we are doing a manadrain type of combat
	void Combat::apply_mana_damage(const CreaturePtr& attacker, const PlayerPtr& target, bool manashield) noexcept
	{

	}


	bool Combat::apply_damage(const CreaturePtr& attacker, const PlayerPtr& target) noexcept
	{
		// check for mana shield and mana drain

		TextMessage attacker_message;
		TextMessage defender_message;
		TextMessage observer_message;

		const auto notice = UnPackNotice(collect_notice_data(target));

		return true;
	}

	bool Combat::apply_damage(const CreaturePtr& attacker, const MonsterPtr& target) noexcept
	{


		return true;
	}

	bool Combat::apply_damage(const CreaturePtr& attacker, const Position& target_position) noexcept
	{

		return true;
	}

	//void Combat::absorbDamage(const std::optional<CreaturePtr> attacker, int32_t percent, int32_t flat)
	//{
	//	int32_t absorbDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//
	//	if (percent)
	//		absorbDamage += originalDamageValue * percent / 100;
	//
	//	if (flat)
	//		absorbDamage += flat;
	//
	//
	//	if (absorbDamage != 0) {
	//		absorbDamage = std::min<int32_t>(absorbDamage, originalDamageValue);
	//		originalDamage.primary.value += absorbDamage;
	//
	//		auto absorb = CombatDamage{};
	//		absorb.leeched = true;
	//		absorb.origin = ORIGIN_AUGMENT;
	//		absorb.primary.type = COMBAT_HEALING;
	//		absorb.primary.value = absorbDamage;
	//		absorb.augmented = true;
	//
	//		Combat absorbParams;
	//		absorbParams.setOrigin(ORIGIN_AUGMENT);
	//		absorbParams.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(COMBAT_HEALING));
	//		absorbParams.setParam(COMBAT_PARAM_EFFECT, CONST_ME_MAGIC_RED);
	//		absorbParams.setParam(COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_NONE);
	//
	//		if (not attacker.has_value()) 
	//		{
	//			Combat::doTargetCombat(nullptr, this->getPlayer(), absorb, absorbParams);
	//			return;
	//		}
	//
	//		Combat::doTargetCombat(attacker.value(), this->getPlayer(), absorb, absorbParams);
	//	}
	//}
	//
	//void Combat::restoreManaFromDamage(std::optional<CreaturePtr> attacker,	int32_t percent, int32_t flat)
	//{
	//	int32_t restoreDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//
	//	if (percent)
	//		restoreDamage += originalDamageValue * percent / 100;
	//
	//	if (flat)
	//		restoreDamage += flat;
	//
	//	if (restoreDamage != 0)
	//	{
	//		restoreDamage = std::min<int32_t>(restoreDamage, originalDamageValue);
	//		originalDamage.primary.value += restoreDamage;
	//
	//		auto restore = CombatDamage{};
	//		restore.leeched = true;
	//		restore.origin = ORIGIN_AUGMENT;
	//		restore.primary.type = COMBAT_MANADRAIN;
	//		restore.primary.value = restoreDamage;
	//		restore.augmented = true;
	//
	//		Combat restoreParams;
	//		restoreParams.setOrigin(ORIGIN_AUGMENT);
	//		restoreParams.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(COMBAT_MANADRAIN));
	//		restoreParams.setParam(COMBAT_PARAM_EFFECT, CONST_ME_ENERGYHIT);
	//		restoreParams.setParam(COMBAT_PARAM_DISTANCEEFFECT, CONST_ANI_NONE);
	//
	//		if (not attacker.has_value())
	//		{
	//			Combat::doTargetCombat(nullptr, this->getPlayer(), restore, restoreParams);
	//			return;
	//		}
	//
	//		Combat::doTargetCombat(attacker.value(), this->getPlayer(), restore, restoreParams);
	//	}
	//}
	//
	//void Combat::reviveSoulFromDamage(std::optional<CreaturePtr> attacker, int32_t percent,	int32_t flat)
	//{
	//	int32_t reviveDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//
	//	if (percent)
	//		reviveDamage += originalDamageValue * percent / 100;
	//	
	//	if (flat)
	//		reviveDamage += flat;
	//
	//	if (reviveDamage != 0) {
	//		reviveDamage = std::min<int32_t>(reviveDamage, originalDamageValue);
	//		originalDamage.primary.value += reviveDamage;
	//
	//		auto message = (attacker.has_value()) ?
	//			"You gained " + std::to_string(reviveDamage) + " soul from " + attacker.value()->getName() + "'s attack." :
	//			"You gained " + std::to_string(reviveDamage) + " soul from revival.";
	//
	//		sendTextMessage(MESSAGE_HEALED, message);
	//		changeSoul(reviveDamage);
	//	}
	//}
	//
	//void Combat::replenishStaminaFromDamage(std::optional<CreaturePtr> attacker, int32_t percent, int32_t flat)
	//{
	//	int32_t replenishDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//	if (percent)
	//		replenishDamage += originalDamageValue * percent / 100;
	//
	//	if (flat)
	//		replenishDamage += flat;
	//
	//	if (replenishDamage != 0) 
	//	{
	//		replenishDamage = std::min<int32_t>(replenishDamage, originalDamageValue);
	//		originalDamage.primary.value += replenishDamage;
	//
	//		if (not g_config.GetBoolean(ConfigManager::AUGMENT_STAMINA_RULE))
	//			replenishDamage = replenishDamage / 60;
	//
	//		auto message = (attacker.has_value()) ?
	//			"You gained " + std::to_string(replenishDamage) + " stamina from " + attacker.value()->getName() + "'s attack." :
	//			"You gained " + std::to_string(replenishDamage) + " stamina from replenishment.";
	//
	//		sendTextMessage(MESSAGE_HEALED, message);
	//		addStamina(static_cast<uint16_t>(replenishDamage));
	//	}
	//}
	//
	//void Combat::resistDamage(std::optional<CreaturePtr> attacker,
	//	CombatDamage& originalDamage,
	//	int32_t percent,
	//	int32_t flat) const
	//{
	//	int32_t resistDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//	if (percent) {
	//		resistDamage += originalDamageValue * percent / 100;
	//	}
	//	if (flat) {
	//		resistDamage += flat;
	//	}
	//
	//	if (resistDamage != 0) {
	//		resistDamage = std::min<int32_t>(resistDamage, originalDamageValue);
	//		originalDamage.primary.value += resistDamage;
	//
	//		auto message = (attacker.has_value()) ?
	//			"You resisted " + std::to_string(resistDamage) + " damage from " + attacker.value()->getName() + "'s attack." :
	//			"You resisted " + std::to_string(resistDamage) + " damage.";
	//
	//		sendTextMessage(MESSAGE_HEALED, message);
	//	}
	//}
	//
	//void Combat::reflectDamage(std::optional<CreaturePtr> attacker,
	//	CombatDamage& originalDamage,
	//	int32_t percent,
	//	int32_t flat,
	//	uint8_t areaEffect,
	//	uint8_t distanceEffect) {
	//
	//	if (!attacker.has_value()) {
	//		return;
	//	}
	//
	//	int32_t reflectDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//	if (percent) {
	//		reflectDamage += originalDamageValue * percent / 100;
	//	}
	//	if (flat) {
	//		reflectDamage += flat;
	//	}
	//
	//	if (reflectDamage != 0) {
	//		const auto& target = attacker.value();
	//		reflectDamage = std::min<int32_t>(reflectDamage, originalDamageValue);
	//		originalDamage.primary.value += reflectDamage;
	//
	//		auto reflect = CombatDamage{};
	//		reflect.primary.type = originalDamage.primary.type;
	//		reflect.primary.value = (0 - reflectDamage);
	//		reflect.origin = ORIGIN_AUGMENT;
	//		reflect.augmented = true;
	//
	//		Combat params;
	//		params.setParam(COMBAT_PARAM_DISTANCEEFFECT, distanceEffect);
	//		params.setParam(COMBAT_PARAM_EFFECT, areaEffect);
	//		params.setOrigin(ORIGIN_AUGMENT);
	//		params.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(originalDamage.primary.type));
	//
	//		sendTextMessage(
	//			MESSAGE_DAMAGE_DEALT,
	//			"You reflected " + std::to_string(reflectDamage) + " damage from " + target->getName() + "'s attack back at them."
	//		);
	//
	//		Combat::doTargetCombat(this->getPlayer(), target, reflect, params);
	//	}
	//}
	//
	//void Combat::deflectDamage(std::optional<CreaturePtr> attackerOpt,
	//	CombatDamage& originalDamage,
	//	int32_t percent,
	//	int32_t flat,
	//	CombatOrigin paramOrigin,
	//	uint8_t areaEffect,
	//	uint8_t distanceEffect) {
	//
	//	int32_t deflectDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//
	//	if (percent) {
	//		deflectDamage += originalDamageValue * percent / 100;
	//	}
	//	if (flat) {
	//		deflectDamage += flat;
	//	}
	//
	//	if (deflectDamage > 0) {
	//		deflectDamage = std::min(deflectDamage, originalDamageValue);
	//		originalDamage.primary.value += deflectDamage;
	//		constexpr int32_t DAMAGE_DIVIDER = 50.0; // Should be moved to global config
	//		constexpr int32_t MAX_TARGETS = 6.0;
	//		const int32_t calculatedTargets = std::min<int32_t>(
	//			std::round<int32_t>((deflectDamage) / DAMAGE_DIVIDER) + 1,
	//			MAX_TARGETS
	//		);
	//
	//		auto defensePos = getPosition();
	//		const auto attackPos = generateAttackPosition(attackerOpt, defensePos, paramOrigin);
	//		const auto damageArea = generateDeflectArea(attackerOpt, calculatedTargets);
	//
	//		auto deflect = CombatDamage{};
	//		deflect.primary.type = originalDamage.primary.type;
	//		deflect.origin = ORIGIN_AUGMENT;
	//		deflect.primary.value = -1 * std::round<int32_t>(deflectDamage / calculatedTargets);
	//		deflect.augmented = true;
	//
	//		Combat params;
	//		params.setOrigin(ORIGIN_AUGMENT);
	//		params.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(originalDamage.primary.type));
	//		params.setParam(COMBAT_PARAM_DISTANCEEFFECT, distanceEffect);
	//		params.setParam(COMBAT_PARAM_TARGETCASTERORTOPMOST, 1);
	//		params.setParam(COMBAT_PARAM_EFFECT, (areaEffect == CONST_ME_NONE)
	//			? CombatTypeToAreaEffect(originalDamage.primary.type)
	//			: areaEffect);
	//
	//		sendTextMessage(
	//			MESSAGE_EVENT_DEFAULT,
	//			"You deflected " + std::to_string(deflectDamage) + " total damage."
	//		);
	//
	//		Combat::doAreaCombat(this->getPlayer(), attackPos, damageArea.get(), deflect, params);
	//	}
	//}
	//
	//void Combat::ricochetDamage(CombatDamage& originalDamage,
	//	int32_t percent,
	//	int32_t flat,
	//	uint8_t areaEffect,
	//	uint8_t distanceEffect) {
	//
	//	int32_t ricochetDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//
	//	if (percent) {
	//		ricochetDamage += originalDamageValue * percent / 100;
	//	}
	//	if (flat) {
	//		ricochetDamage += flat;
	//	}
	//
	//	auto targetList = getOpenPositionsInRadius(3);
	//
	//	if (ricochetDamage != 0 && targetList.size() > 0) {
	//		const auto& targetPos = targetList[uniform_random(0, targetList.size() - 1)];
	//		ricochetDamage = std::min(ricochetDamage, originalDamageValue);
	//		originalDamage.primary.value += ricochetDamage;
	//
	//		auto message = "An attack on you ricocheted " + std::to_string(ricochetDamage) + " damage.";
	//		sendTextMessage(MESSAGE_EVENT_ADVANCE, message);
	//
	//		auto ricochet = CombatDamage{};
	//		ricochet.primary.type = originalDamage.primary.type;
	//		ricochet.primary.value = (0 - ricochetDamage);
	//		ricochet.origin = ORIGIN_AUGMENT;
	//		ricochet.augmented = true;
	//
	//		Combat params;
	//		params.setOrigin(ORIGIN_AUGMENT);
	//		params.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(originalDamage.primary.type));
	//		params.setParam(COMBAT_PARAM_DISTANCEEFFECT, distanceEffect);
	//		params.setParam(COMBAT_PARAM_TARGETCASTERORTOPMOST, 1);
	//		params.setParam(COMBAT_PARAM_EFFECT, (areaEffect == CONST_ME_NONE) ? CombatTypeToAreaEffect(originalDamage.primary.type) : areaEffect);
	//
	//		const auto& damageArea = std::make_unique<AreaCombat>();
	//		damageArea->setupArea(Deflect1xArea, 5);
	//		Combat::doAreaCombat(this->getPlayer(), targetPos, damageArea.get(), ricochet, params);
	//	}
	//}
	//
	//void Combat::convertDamage(const CreaturePtr& target, CombatDamage& originalDamage, gtl::node_hash_map<uint8_t, ModifierTotals> conversionList) {
	//	auto iter = conversionList.begin();
	//
	//	while (originalDamage.primary.value < 0 && iter != conversionList.end()) {
	//
	//		const CombatType_t combatType = indexToCombatType(iter->first);
	//		const ModifierTotals& totals = iter->second;
	//
	//		int32_t convertedDamage = 0;
	//		const int32_t percent = static_cast<int32_t>(totals.percentTotal);
	//		const int32_t flat = static_cast<int32_t>(totals.flatTotal);
	//		const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//		if (percent) {
	//			convertedDamage += originalDamageValue * percent / 100;
	//		}
	//		if (flat) {
	//			convertedDamage += flat;
	//		}
	//
	//		if (convertedDamage != 0 && target) {
	//			convertedDamage = std::min<int32_t>(convertedDamage, originalDamageValue);
	//			originalDamage.primary.value += convertedDamage;
	//
	//			auto converted = CombatDamage{};
	//			converted.primary.type = combatType;
	//			converted.primary.value = (0 - convertedDamage);
	//			converted.origin = ORIGIN_AUGMENT;
	//			converted.augmented = true;
	//
	//			Combat params;
	//			params.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(combatType));
	//			params.setOrigin(ORIGIN_AUGMENT);
	//
	//			auto message = "You converted " + std::to_string(convertedDamage) + " " + getCombatName(originalDamage.primary.type) + " damage to " + getCombatName(combatType) + " during an attack on " + target->getName() + ".";
	//			sendTextMessage(MESSAGE_DAMAGE_DEALT, message);
	//			Combat::doTargetCombat(this->getPlayer(), target, converted, params);
	//		}
	//		++iter;
	//	}
	//}
	//
	//void Combat::reformDamage(std::optional<CreaturePtr> attacker, CombatDamage& originalDamage, gtl::node_hash_map<uint8_t, ModifierTotals> conversionList) {
	//	auto iter = conversionList.begin();
	//
	//	while (originalDamage.primary.value < 0 && iter != conversionList.end()) {
	//
	//		CombatType_t combatType = indexToCombatType(iter->first);
	//		const ModifierTotals& totals = iter->second;
	//
	//		int32_t reformedDamage = 0;
	//		int32_t percent = static_cast<int32_t>(totals.percentTotal);
	//		int32_t flat = static_cast<int32_t>(totals.flatTotal);
	//		const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//		if (percent) {
	//			reformedDamage += originalDamageValue * percent / 100;
	//		}
	//		if (flat) {
	//			reformedDamage += flat;
	//		}
	//
	//		if (reformedDamage) {
	//			reformedDamage = std::min<int32_t>(reformedDamage, originalDamageValue);
	//			originalDamage.primary.value += reformedDamage;
	//
	//			auto reform = CombatDamage{};
	//			reform.primary.type = combatType;
	//			reform.primary.value = (0 - reformedDamage);
	//			reform.origin = ORIGIN_AUGMENT;
	//			reform.augmented = true;
	//
	//			Combat params;
	//			params.setParam(COMBAT_PARAM_TYPE, static_cast<uint32_t>(combatType));
	//			params.setOrigin(ORIGIN_AUGMENT);
	//
	//			auto message = (attacker.has_value()) ?
	//				"You reformed " + std::to_string(reformedDamage) + " " + getCombatName(originalDamage.primary.type) + " damage from " + getCombatName(combatType) + " during an attack on you by " + attacker.value()->getName() + "." :
	//				"You reformed " + std::to_string(reformedDamage) + " " + getCombatName(originalDamage.primary.type) + " damage from " + getCombatName(combatType) + ".";
	//
	//			sendTextMessage(MESSAGE_DAMAGE_DEALT, message);
	//			auto target = (attacker.has_value()) ? attacker.value() : nullptr;
	//			Combat::doTargetCombat(target, this->getPlayer(), reform, params);
	//		}
	//		++iter;
	//	}
	//}

	//void Combat::increaseDamage(std::optional<CreaturePtr> attacker,
	//	CombatDamage& originalDamage,
	//	int32_t percent,
	//	int32_t flat) const
	//{
	//	int32_t increasedDamage = 0;
	//	const int32_t originalDamageValue = std::abs(originalDamage.primary.value);
	//	if (percent) {
	//		increasedDamage += originalDamageValue * percent / 100;
	//	}
	//
	//	if (flat) {
	//		increasedDamage += flat;
	//	}
	//
	//	if (increasedDamage != 0) {
	//		increasedDamage = std::min<int32_t>(increasedDamage, originalDamageValue);
	//		originalDamage.primary.value -= increasedDamage;
	//
	//		auto message = (attacker.has_value()) ?
	//			"You took an additional " + std::to_string(increasedDamage) + " damage from " + attacker.value()->getName() + "'s attack." :
	//			"You took an additional " + std::to_string(increasedDamage) + " damage.";
	//
	//		sendTextMessage(MESSAGE_DAMAGE_RECEIVED, message);
	//	}
	//}

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
					//if (Combat::isProtected(attackerPlayer, targetPlayer)) {
					//	harmfulField = false;
					//}
				}
			}

			if (!harmfulField or (OTSYS_TIME() - createTime <= 5000) or creature->hasBeenAttacked(ownerId)) {
				conditionCopy->setParam(CONDITION_PARAM_OWNER, ownerId);
			}
		}

		creature->addCondition(conditionCopy);
	}
}