// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "combat.h"
#include "game.h"
#include "weapons.h"
#include "configmanager.h"
#include "events.h"
#include "monster.h"
#include "luascript.h"

#include <optional>
#include <immintrin.h>

extern LuaEnvironment g_luaEnvironment;

using namespace BlackTek::Constant;
using BlackTek::MatrixArea;
using BlackTek::CreateArea;

extern Game g_game;
extern Weapons* g_weapons;
extern ConfigManager g_config;
extern Events* g_events;

namespace BlackTek
{
	// Formula maps defined BEFORE g_combat_registry so they are destroyed AFTER it.
	// Combat destructors (triggered by registry teardown) still reference these maps,
	// so they must remain alive for the full lifetime of the registry.
	// new_delete_resource() avoids any dependency on the registry allocator.
	std::array<SituationFormulas, SituationFormulas::Total>											g_default_situation_formulas = {};
	std::pmr::unordered_map<int64_t, std::array<SituationFormulas, SituationFormulas::Total>>		combat_formula_map{ std::pmr::new_delete_resource() };
	std::pmr::unordered_map<int64_t, FormulaCallbacks>												combat_callback_map{ std::pmr::new_delete_resource() };

	static std::atomic<int64_t> lua_formula_id_counter_{ -2 };

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

	CombatHandle CombatRegistry::Clone(const Combat& src) noexcept
	{
		auto handle = Create();
		Combat& dst = *handle;

		dst.damage              = src.damage;
		dst.config              = src.config;
		dst.defense_charge_cost = src.defense_charge_cost;
		dst.armor_charge_cost   = src.armor_charge_cost;
		dst.augment_charge_cost = src.augment_charge_cost;
		dst.itemId              = src.itemId;
		dst.damage_type         = src.damage_type;
		dst.blockType           = src.blockType;
		dst.origin              = src.origin;
		dst.impactEffect        = src.impactEffect;
		dst.distanceEffect      = src.distanceEffect;

		// Borrow the parent's formula/callback map entries without taking ownership.
		dst.formula_source_id = src.formula_key();

		return handle;
	}

	CombatHandle Combat::clone() const noexcept
	{
		return g_combat_registry.Clone(*this);
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

	// Returns 0 on error (Lua error is reported; damage is not modified).
	static int32_t ReductionCallback(int32_t ref, int32_t stat) noexcept
	{
		lua_State* L = g_luaEnvironment.getLuaState();
		if (not L)
			return 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_pushinteger(L, stat);
		if (LuaScriptInterface::protectedCall(L, 1, 1) != 0)
		{
			LuaScriptInterface::reportError(nullptr, LuaScriptInterface::getString(L, -1));
			lua_pop(L, 1);
			return 0;
		}
		const int32_t result = static_cast<int32_t>(lua_tointeger(L, -1));
		lua_pop(L, 1);
		return result;
	}

	static int32_t ResolutionCallback(int32_t ref, int32_t output, int32_t resistance) noexcept
	{
		lua_State* L = g_luaEnvironment.getLuaState();
		if (not L)
			return 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_pushinteger(L, output);
		lua_pushinteger(L, resistance);
		if (LuaScriptInterface::protectedCall(L, 2, 1) != 0)
		{
			LuaScriptInterface::reportError(nullptr, LuaScriptInterface::getString(L, -1));
			lua_pop(L, 1);
			return 0;
		}
		const int32_t result = static_cast<int32_t>(lua_tointeger(L, -1));
		lua_pop(L, 1);
		return result;
	}

	Combat::~Combat()
	{
		if (combat_id == -1)
			return;

		// Clones borrow the parent's formula/callback entries — parent owns them.
		if (formula_source_id != -1)
			return;

		combat_formula_map.erase(combat_id);

		auto cbIt = combat_callback_map.find(combat_id);
		if (cbIt != combat_callback_map.end())
		{
			lua_State* L = g_luaEnvironment.getLuaState();
			if (L)
			{
				for (auto& row : cbIt->second.refs)
					for (auto& r : row)
						if (r != FormulaCallbacks::NoRef)
							luaL_unref(L, LUA_REGISTRYINDEX, r);
			}
			combat_callback_map.erase(cbIt);
		}
	}

	// ── Formula override methods ──────────────────────────────────────────────

	int64_t Combat::ensureFormulaId() noexcept
	{
		if (combat_id == -1)
			combat_id = lua_formula_id_counter_.fetch_sub(1, std::memory_order_relaxed);
		return combat_id;
	}

	void Combat::setSituationFormulas(uint8_t sit_idx, SituationFormulas&& formulas) noexcept
	{
		if (sit_idx >= 4)
			return;

		const int64_t fid = ensureFormulaId();

		auto it = combat_formula_map.find(fid);
		if (it == combat_formula_map.end())
		{
			auto [newIt, ok] = combat_formula_map.emplace(fid, g_default_situation_formulas);
			it = newIt;
		}

		it->second[sit_idx] = std::move(formulas);

		static constexpr Config formula_flags[4] = {Config::HasPvPFormula, Config::HasPvMFormula, Config::HasMvPFormula, Config::HasMvMFormula};
		config.set(formula_flags[sit_idx]);
	}

	void Combat::setFormulaCallback(uint8_t sit_idx, FormulaStage stage, int32_t lua_ref) noexcept
	{
		if (sit_idx >= 4 or lua_ref == FormulaCallbacks::NoRef)
			return;

		const int64_t fid = ensureFormulaId();

		auto it = combat_callback_map.find(fid);
		if (it == combat_callback_map.end())
		{
			auto [newIt, ok] = combat_callback_map.emplace(fid, FormulaCallbacks{});
			it = newIt;
		}

		it->second.refs[sit_idx][static_cast<uint8_t>(stage)] = lua_ref;
	}

	void ApplyOutputPreset(Combat::OutputFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Tibia")			out = Combat::TibiaOutput;
		else if (preset == "LoL")			out = Combat::LoLOutput;
		else if (preset == "Pokemon")		out = Combat::PokemonOutput;
		else if (preset == "DarkSouls")		out = Combat::DarkSoulsOutput;
		else if (preset == "DnD")			out = Combat::DnDOutput;
		else if (preset == "Exponential")	out = Combat::ExponentialOutput;
	}

	void ApplyDefensePreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Tibia")			out = Combat::TibiaDefense;
		else if (preset == "LoL")			out = Combat::LoLResistance;
		else if (preset == "Pokemon")		out = Combat::PokemonResistance;
		else if (preset == "DarkSouls")		out = Combat::DarkSoulsResistance;
	}

	void ApplyArmorPreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Tibia")			out = Combat::TibiaArmor;
		else if (preset == "DarkSouls")		out = Combat::DarkSoulsResistance;
	}

	void ApplyResolutionPreset(Combat::ResolutionFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Tibia")         out = Combat::TibiaResolution;
		else if (preset == "LoL")           out = Combat::LoLResolution;
		else if (preset == "Pokemon")       out = Combat::PokemonResolution;
		else if (preset == "DarkSouls")     out = Combat::DarkSoulsResolution;
		else if (preset == "MonsterHunter") out = Combat::MonsterHunterResolution;
		else if (preset == "Genshin")       out = Combat::GenshinResolution;
	}

	void LoadFormulaDefaults(uint8_t sit_idx, std::string_view out_preset, std::string_view def_preset, std::string_view arm_preset, std::string_view res_preset) noexcept
	{
		if (sit_idx >= 4)
			return;

		auto& sf = g_default_situation_formulas[sit_idx];
		ApplyOutputPreset(sf.output, out_preset);
		ApplyDefensePreset(sf.defense, def_preset);
		ApplyArmorPreset(sf.armor, arm_preset);
		ApplyResolutionPreset(sf.resolution, res_preset);
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
	[[nodiscard]] constexpr uint32_t PackNotice(T1 color, T2 effect, T3 fluid = FLUID_NONE)
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

		[[nodiscard]] constexpr std::string resolve_block_code(Combat::BlockType code) noexcept
		{
			switch (code)
			{
				case Combat::BlockType::Defensive:	return "";
				case Combat::BlockType::Armor:		return "";
				case Combat::BlockType::Immunity:	return "";
				case Combat::BlockType::Dodge:		return ""; // dodge is not part of the system yet.
				default:							return "";
			}
		}

		[[nodiscard]] constexpr std::string resolve_target_code(Combat::TargetCode code) noexcept
		{
			switch (code)
			{
				case Combat::TargetCode::UnknownFailure:			return "Fail";
				case Combat::TargetCode::Valid:						return "Valid";
				case Combat::TargetCode::YouAreInProtectionZone:	return "";
				case Combat::TargetCode::CanNotAttackThisPlayer:	return "";
				case Combat::TargetCode::TargetIsInProtectionZone:	return "";
				case Combat::TargetCode::CanNotAttackThisMonster:	return "";
				case Combat::TargetCode::PlayerIsUnMarked:			return "";
				case Combat::TargetCode::CanNotWieldWeapon:			return "";
				case Combat::TargetCode::NotEnoughRoom:				return "";
				case Combat::TargetCode::FirstGoUpStairs:			return "";
				case Combat::TargetCode::FirstGoDownStairs:			return "";
				default:											return "Fail";
			}
		}
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

		casterPlayer      = caster->isSummon() ? caster->getMaster()->getPlayer() : caster->getPlayer();
		isNoPvpWorld      = (g_game.getWorldType() == WORLD_TYPE_NO_PVP);
		casterID          = caster->getID();

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

				if (not item) [[unlikely]] // this situation should never ever occur but definitely needs a log if it does
					continue;

				item->setOwner(casterID);

				CylinderPtr holder = tile;
				ReturnValue ret = g_game.internalAddItem(holder, item);
				auto decay = (ret == RETURNVALUE_NOERROR) and item->canDecay();

				if (decay) [[likely]]
					g_game.startDecay(item);
			}

			if (hasEffect)
				Game::addMagicEffect(spectators, tile->getPosition(), impactEffect);
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

	Combat::TargetCode Combat::target(const CreaturePtr& attacker, const CreaturePtr& defender) const
	{
		using namespace BlackTek;
		auto switch_mask = (static_cast<uint32_t>(attacker->getCreatureSubType()) << 16 | static_cast<uint32_t>(defender->getCreatureSubType()) << 8);

		switch (switch_mask)
		{
			case Constant::Player_Vs_Player:	return target(PlayerCast(attacker), PlayerCast(defender));
			case Constant::Player_Vs_Monster:	return target(PlayerCast(attacker), MonsterCast(defender));
			case Constant::Monster_Vs_Player:	return target(MonsterCast(attacker), PlayerCast(defender));
			case Constant::Monster_Vs_Monster:	return target(MonsterCast(attacker), MonsterCast(defender));
			default: [[unlikely]] // todo: log here
				break;
		}
		return TargetCode::UnknownFailure;
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

		auto handle = g_combat_registry.Create(std::to_underlying(DamageType::Undefined), penetration_damage);
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
			const uint32_t index = std::countr_zero(modifier.to_damage_type);
			const uint32_t value = modifier.isFlatValue() ? modifier.value : (damage * modifier.value * 1374389535ULL) >> 37;
			total += value;

			switch (index)
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

	void Combat::strike_target(const PlayerPtr& caster, const PlayerPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		const auto target_code = target(caster, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
		{ 
			auto message = resolve_target_code(target_code);
			caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
			return;
		}
		
		// if (not non_aggressive())
		//		if (healing) doHealing()
		//      else
		//		postConditions or w/e reason this combat would exist as non-aggressive but isn't healing damage type

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), distanceEffect); // we have an overload which uses spectators we need to pass our already existing spectators

		if (not config.test(Config::TrueDamage))
		{
			const auto blocked = block(caster, victim);

			if (blocked != BlockType::NoBlock)
			{
				auto message = resolve_block_code(blocked);
				caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
				return;
			}
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
					auto true_damage = penetrateDamage(percent_pierce, flat_pierce);
				}

				if (percent_crit or flat_crit)
					applyCrit(percent_crit, flat_crit);
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

	void Combat::strike_target(const PlayerPtr& caster, const MonsterPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		const auto target_code = target(caster, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
		{
			auto message = resolve_target_code(target_code);
			caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
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

			if (blocked != BlockType::NoBlock)
			{
				auto message = resolve_block_code(blocked);
				caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
				return;
			}
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
								leech_data.flat_health	 += modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Manasteal:
							{
								leech_data.percent_mana	 += modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_mana	 += modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Staminasteal:
							{
								leech_data.percent_stamina += modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_stamina	 += modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Soulsteal:
							{
								leech_data.percent_soul	 += modifier.isFlatValue() ? 0 : modifier.getValue();
								leech_data.flat_soul	 += modifier.isFlatValue() ? modifier.getValue() : 0;
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
					auto true_damage = penetrateDamage(percent_pierce, flat_pierce);
				}

				if (percent_crit or flat_crit)
					applyCrit(percent_crit, flat_crit);
			}
		}

		const auto& success = (damage_type == COMBAT_MANADRAIN) ? apply_damage(caster, victim) : apply_damage(caster, victim);

		if (success)
			post_damage(caster, victim, std::move(leech_data));
	}

	void Combat::strike_target(const MonsterPtr& attacker, const PlayerPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		const auto target_code = target(attacker, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
			return;

		// if (not non_aggressive())
		//		if (healing) doHealing()
		//      else
		//		postConditions or w/e reason this combat would exist as non-aggressive but isn't healing damage type

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			if (block(attacker, victim) != BlockType::NoBlock)
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

	void Combat::strike_target(const MonsterPtr& attacker, const MonsterPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		const auto target_code = target(attacker, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
			return;

		// if (not non_aggressive())
		//		if (healing) doHealing()
		//      else
		//		postConditions or w/e reason this combat would exist as non-aggressive but isn't healing damage type

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			if (block(attacker, victim) != BlockType::NoBlock)
				return;
		}

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

	void Combat::strike_target(const CreaturePtr& attacker, const CreaturePtr& defender, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		using namespace BlackTek;
		auto switch_mask = (static_cast<uint32_t>(attacker->getCreatureSubType()) << 16 | static_cast<uint32_t>(defender->getCreatureSubType()) << 8);

		switch (switch_mask)
		{
			case Constant::Player_Vs_Player:	strike_target(PlayerCast(attacker), PlayerCast(defender));		break;
			case Constant::Player_Vs_Monster:	strike_target(PlayerCast(attacker), MonsterCast(defender));		break;
			case Constant::Monster_Vs_Player:	strike_target(MonsterCast(attacker), PlayerCast(defender));		break;
			case Constant::Monster_Vs_Monster:	strike_target(MonsterCast(attacker), MonsterCast(defender));	break;
			default: [[unlikely]]
				break;
		}
	}

	void Combat::execute(const CreaturePtr& caster, const Position& center) noexcept
	{
		// todo: I have forgotten to ensure absolute value in all the comparisions which were used for
		// calculating the distance, I need to go back and change that, in order to be sure it's positive for it's usage here
		const auto& [positions, distance ] = getAreaPositions(caster->getPosition(), center);
		const size_t n = std::min(static_cast<size_t>(distance), static_cast<size_t>(Map::maxViewportX + Map::maxClientViewportX));

		if (distance == 0)
		{
			// handle for non pvp, ect.
			return;
		}

		auto spectators = g_game.map.fetchSpectators(center, true, true, distance + Map::maxViewportX, distance + Map::maxViewportX, distance + Map::maxViewportY, distance + Map::maxViewportY);

		if (caster and (distanceEffect != CONST_ANI_NONE))
			addDistanceEffect(caster, caster->getPosition(), center, distanceEffect);

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

		const bool admin = caster->getCreatureSubType() == CreatureSubType::Player and caster->getPlayer()->hasFlag(PlayerFlag_IgnoreProtectionZone);
		const uint32_t flag_reject = TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT | (config.test(Config::Aggressive) and not admin ? static_cast<uint32_t>(TILESTATE_PROTECTIONZONE) : 0u);
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

		std::vector<CreaturePtr> toDamageCreatures;
		toDamageCreatures.reserve(n * 2);

		std::vector<TilePtr> valid_tile_list;
		valid_tile_list.reserve(n);

		auto valid_tiles = std::views::iota(size_t{0}, n) | std::views::filter([&](size_t idx) { return static_cast<bool>(valid_mask[idx]); })
			| std::views::filter([&](size_t idx) { return not tile_cache[idx]->hasProperty(CONST_PROP_BLOCKPROJECTILE); });

		for (size_t idx : valid_tiles)
		{
			const auto& tile = tile_cache[idx];

			valid_tile_list.push_back(tile);

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

		apply_effects(spectators, caster, valid_tile_list);

		for (const auto& target_creature : toDamageCreatures)
		{
			auto single_combat = clone();
			single_combat->strike_target(caster, target_creature);
		}
	}

	const MatrixArea& AreaCombat::getArea(const Position& centerPos, const Position& targetPos) const
	{
		const int32_t dx = Position::getOffsetX(targetPos, centerPos);
		const int32_t dy = Position::getOffsetY(targetPos, centerPos);

		Direction dir;

		if (dx < 0)			dir = DIRECTION_WEST;
		else if (dx > 0)	dir = DIRECTION_EAST;
		else if (dy < 0)	dir = DIRECTION_NORTH;
		else				dir = DIRECTION_SOUTH;

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
		if (dir >= areas.size()) 
		{
			// log location
			static MatrixArea empty;
			return empty;
		}
		return areas[dir];
	}

	void AreaCombat::setupArea(const std::vector<uint32_t>& vec, uint32_t rows)
	{
		auto area = CreateArea(vec, rows);
		if (areas.size() == 0)
			areas.resize(4);

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

	void Combat::defense_block_effect(const Position& target_position) const noexcept
	{
	}

	void Combat::armor_block_effect(const Position& target_position) const noexcept
	{
	}

	uint8_t Combat::immunity_block_effect() const noexcept
	{
		switch (damage_type)
		{
			case DamageType::Undefined:		return CONST_ME_NONE;
			case DamageType::Energy:		[[fallthrough]]
			case DamageType::Fire:			[[fallthrough]]
			case DamageType::Physical:		[[fallthrough]]
			case DamageType::Ice:			[[fallthrough]]
			case DamageType::Death:			return CONST_ME_BLOCKHIT;
			case DamageType::Earth:			return CONST_ME_GREEN_RINGS;
			case DamageType::Holy:			return CONST_ME_HOLYDAMAGE;
			default:						return CONST_ME_NONE;
		}
		return CONST_ME_NONE;
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

		// Three-tier formula resolution:
		// PvP (idx 0) when attacker is a player; MvP (idx 2) when attacker is a monster.
		const uint8_t sit_idx = (attacker and attacker->getCreatureSubType() == CreatureSubType::Player) ? 0u : 2u;

		static constexpr Config formula_flags[4] = {Config::HasPvPFormula, Config::HasPvMFormula, Config::HasMvPFormula, Config::HasMvMFormula};

		const FormulaCallbacks* callbacks = nullptr;
		{
			auto cbIt = combat_callback_map.find(formula_key());
			if (cbIt != combat_callback_map.end())
				callbacks = &cbIt->second;
		}

		const SituationFormulas* formulas = nullptr;
		if (config.test(formula_flags[sit_idx]))
		{
			auto fIt = combat_formula_map.find(formula_key());
			if (fIt != combat_formula_map.end())
				formulas = &fIt->second[sit_idx];
		}

		if (not formulas)
			formulas = &g_default_situation_formulas[sit_idx];

		if (apply_defense_reduction and target->can_use_defense()
		    and (defense_cost == 0 or target->get_defense_charges() >= defense_cost))
		{
			if (defense_cost > 0)
				target->set_defense_charges(target->get_defense_charges() - defense_cost);

			const int32_t defense_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Defense)] : FormulaCallbacks::NoRef;
			const int32_t resolution_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Resolution)] : FormulaCallbacks::NoRef;

			const int32_t defense_value = (defense_type != FormulaCallbacks::NoRef)
				? ReductionCallback(defense_type, target->getDefense())
				: calculate_resistance(formulas->defense, target->getDefense());

			damage = static_cast<uint32_t>((resolution_type != FormulaCallbacks::NoRef)
				? ResolutionCallback(resolution_type, static_cast<int32_t>(damage), defense_value)
				: calculate_resolution(formulas->resolution, static_cast<int32_t>(damage), defense_value));

			if (damage == 0)
			{
				blockType = BlockType::Defensive;
				apply_armor_reduction = false;
			}

			// reduces shield block count and awards shielding skill if shield is equipped
			target->onBlockHit();
		}

		if (apply_armor_reduction and (armor_cost == 0 or target->get_armor_charges() >= armor_cost))
		{
			if (armor_cost > 0)
				target->set_armor_charges(target->get_armor_charges() - armor_cost);

			const int32_t armor_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Armor)] : FormulaCallbacks::NoRef;
			const int32_t armor_value = (armor_type != FormulaCallbacks::NoRef)
				? ReductionCallback(armor_type, target->getArmor())
				: calculate_resistance(formulas->armor, target->getArmor());

			const int32_t resolution_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Resolution)] : FormulaCallbacks::NoRef;
			damage = static_cast<uint32_t>((resolution_type != FormulaCallbacks::NoRef)
				? ResolutionCallback(resolution_type, static_cast<int32_t>(damage), armor_value)
				: calculate_resolution(formulas->resolution, static_cast<int32_t>(damage), armor_value));

			if (damage == 0)
				blockType = BlockType::Armor;
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
		const bool apply_defense_reduction = config.test(Config::BlockedByDefense);

		const float def_mult = target->get_defense_charge_cost_multiplier();
		const float arm_mult = target->get_armor_charge_cost_multiplier();
		const float def_base = static_cast<float>(apply_defense_reduction ? defense_charge_cost : g_config.GetNumber(ConfigManager::DEFAULT_DEFENSE_CHARGE_COST));
		const float arm_base = static_cast<float>(apply_armor_reduction ? armor_charge_cost : g_config.GetNumber(ConfigManager::DEFAULT_ARMOR_CHARGE_COST));

		const uint32_t defense_cost = static_cast<uint32_t>(std::round(def_base * def_mult));
		const uint32_t armor_cost = static_cast<uint32_t>(std::round(arm_base * arm_mult));

		// PvM (idx 1) when attacker is a player; MvM (idx 3) when attacker is a monster.
		const uint8_t sit_idx = (attacker and attacker->getCreatureSubType() == CreatureSubType::Player) ? 1u : 3u;

		static constexpr Config formula_flags[4] = {Config::HasPvPFormula, Config::HasPvMFormula, Config::HasMvPFormula, Config::HasMvMFormula};

		const FormulaCallbacks* callbacks = nullptr;
		{
			auto cbIt = combat_callback_map.find(formula_key());
			if (cbIt != combat_callback_map.end())
				callbacks = &cbIt->second;
		}

		const SituationFormulas* formulas = nullptr;

		if (config.test(formula_flags[sit_idx]))
		{
			auto fIt = combat_formula_map.find(formula_key());
			if (fIt != combat_formula_map.end())
				formulas = &fIt->second[sit_idx];
		}

		if (not formulas)
			formulas = &g_default_situation_formulas[sit_idx];

		if (apply_defense_reduction and target->can_use_defense()
			and (defense_cost == 0 or target->get_defense_charges() >= defense_cost))
		{
			if (defense_cost > 0)
				target->set_defense_charges(target->get_defense_charges() - defense_cost);

			const int32_t defense_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Defense)] : FormulaCallbacks::NoRef;
			const int32_t defense_value = (defense_type != FormulaCallbacks::NoRef)
				? ReductionCallback(defense_type, target->getDefense())
				: calculate_resistance(formulas->defense, target->getDefense());

			const int32_t resolution_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Resolution)] : FormulaCallbacks::NoRef;
			damage = static_cast<uint32_t>((resolution_type != FormulaCallbacks::NoRef)
				? ResolutionCallback(resolution_type, static_cast<int32_t>(damage), defense_value)
				: calculate_resolution(formulas->resolution, static_cast<int32_t>(damage), defense_value));

			if (damage == 0)
			{
				blockType = BlockType::Defensive;
				apply_armor_reduction = false;
			}
		}

		if (apply_armor_reduction and (armor_cost == 0 or target->get_armor_charges() >= armor_cost))
		{
			if (armor_cost > 0)
				target->set_armor_charges(target->get_armor_charges() - armor_cost);

			const int32_t armor_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Armor)] : FormulaCallbacks::NoRef;
			const int32_t armor_value = (armor_type != FormulaCallbacks::NoRef)
				? ReductionCallback(armor_type, target->getArmor())
				: calculate_resistance(formulas->armor, target->getArmor());

			const int32_t resolution_type = callbacks ? callbacks->refs[sit_idx][static_cast<uint8_t>(FormulaStage::Resolution)] : FormulaCallbacks::NoRef;
			damage = static_cast<uint32_t>((resolution_type != FormulaCallbacks::NoRef)
				? ResolutionCallback(resolution_type, static_cast<int32_t>(damage), armor_value)
				: calculate_resolution(formulas->resolution, static_cast<int32_t>(damage), armor_value));

			if (damage == 0)
				blockType = BlockType::Armor;
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
	}

	uint32_t Combat::apply_damage(const CreaturePtr& attacker, const PlayerPtr& target, std::optional<std::span<const CreaturePtr>> pre_cache) const noexcept
	{
		const auto target_mana = target->getMana();
		const auto target_health = static_cast<uint32_t>(target->getHealth());
		const auto target_position = target->getPosition();
		const auto& target_name = target->getName();
		const auto& attacker_name = attacker->getNameDescription(); // we use name description here (for now) as it could be a monster
		const auto manashield = target->hasCondition(CONDITION_MANASHIELD);
		const auto manadrain = damage_type == Combat::DamageType::ManaDrain;
		const auto self_harm = attacker == target;
		auto damage_limit = (manashield or manadrain) ? (manadrain ? target_mana : target_mana + target_health) : target_health;
		const auto notice = UnPackNotice(collect_notice_data(target));
		uint32_t damage_dealt = 0;

		if (manadrain or manashield)
		{
			damage_dealt = std::min(damage, target_mana);
			target->drainMana(attacker, damage_dealt);

			TextMessage defender_message,	observer_message				= {};
			defender_message.position,		observer_message.position		= target_position;
			defender_message.primary.color, observer_message.primary.color	= TEXTCOLOR_BLUE;
			defender_message.primary.value, observer_message.primary.value	= damage_dealt;

			defender_message.type = MESSAGE_DAMAGE_RECEIVED;
			observer_message.type = MESSAGE_DAMAGE_OTHERS;

			defender_message.text = self_harm	? "You lose " + std::to_string(damage_dealt) + " mana due to your own attack." : "You lose " + std::to_string(damage_dealt) + " mana due to an attack by " + attacker_name + ".";
			observer_message.text = target_name + " loses " + std::to_string(damage_dealt) + " mana due to an attack by " + attacker_name + ".";

			if (not pre_cache)
				pre_cache = std::make_optional<std::span<const CreaturePtr>>(g_game.map.fetchSpectators(target->getPosition(), true, true));

			auto observer = [&](const auto& spectator) {return spectator != target and spectator != attacker;};

			target->sendTextMessage(defender_message);
			target->sendMagicEffect(target_position, CONST_ME_LOSEENERGY);

			if (not self_harm and attacker->is_player())
			{
				TextMessage attacker_message { MESSAGE_DAMAGE_DEALT , target_name + " loses " + std::to_string(damage_dealt) + " mana due to your attack." };
				attacker_message.primary.color	= TEXTCOLOR_BLUE;
				attacker_message.primary.value	= damage_dealt;
				attacker_message.position		= target_position;

				auto* caster = static_cast<Player*>(attacker.get());
				caster->sendTextMessage(attacker_message);
				caster->sendMagicEffect(target_position, CONST_ME_LOSEENERGY);
			}

			for (const auto& spectator : *pre_cache | std::views::filter(observer))
			{
				auto* player = static_cast<Player*>(spectator.get());
				player->sendTextMessage(observer_message);
				player->sendMagicEffect(target_position, CONST_ME_LOSEENERGY);
			}

			if (manadrain or damage_dealt == damage) return damage_dealt;
		}

		damage_limit -= damage_dealt;

		if (damage > damage_dealt)
		{
			auto health_changed = std::min(damage_limit, target_health);
			damage_dealt += health_changed;
			target->drainHealth(attacker, health_changed);

			TextMessage defender_message,		observer_message				= {};
			defender_message.position,			observer_message.position		= target_position;
			defender_message.primary.color,		observer_message.primary.color	= notice.color;
			defender_message.primary.value,		observer_message.primary.value	= health_changed;

			defender_message.type = MESSAGE_DAMAGE_RECEIVED;
			observer_message.type = MESSAGE_DAMAGE_OTHERS;

			defender_message.text = self_harm	? "You lose " + std::to_string(health_changed) + " health due to your own attack." : "You lose " + std::to_string(health_changed) + " health due to an attack by " + attacker_name + ".";
			observer_message.text = target_name + " loses " + std::to_string(health_changed) + " health due to an attack by " + attacker_name + ".";

			if (not pre_cache)
				pre_cache = std::make_optional<std::span<const CreaturePtr>>(g_game.map.fetchSpectators(target->getPosition(), true, true));

			auto observer = [&](const auto& spectator) {return spectator != target and spectator != attacker;};

			target->sendTextMessage(defender_message);
			target->sendMagicEffect(target_position, notice.effect);
			target->sendStats();

			if (not self_harm and attacker->is_player())
			{
				TextMessage attacker_message{ MESSAGE_DAMAGE_DEALT , target_name + " loses " + std::to_string(damage_dealt) + " mana due to your attack." };
				attacker_message.primary.color = notice.color;
				attacker_message.primary.value = health_changed;
				attacker_message.position = target_position;

				auto* caster = static_cast<Player*>(attacker.get());
				caster->sendTextMessage(attacker_message);
				caster->sendMagicEffect(target_position, notice.effect);
				caster->sendCreatureHealth(target);
			}

			for (const auto& spectator : *pre_cache | std::views::filter(observer))
			{
				auto* player = static_cast<Player*>(spectator.get());
				player->sendTextMessage(observer_message);
				player->sendMagicEffect(target_position, notice.effect);
				player->sendCreatureHealth(target);
			}

			if (notice.fluid != 0)
			{
				TilePtr tile = g_game.map.getTile(target_position);
				CylinderPtr c_tile = tile;

				auto fluid = Item::CreateItem(ITEM_SMALLSPLASH, notice.fluid);
				if (fluid) [[likely]]
				{
					// we could, and probably should, assign the owner of the blood to either the attacker or defender
					// this is something not done in OT servers, that could be quite cool in-game I think, lets make it happen
					// it should be a configuration option in toml somewhere
					g_game.internalAddItem(c_tile, fluid, INDEX_WHEREEVER, FLAG_NOLIMIT);
					g_game.startDecay(fluid);
				}
				// could do an "else" and log it, incase of failure
			}

			if (health_changed == target_health)
			{
				for (const auto& creatureEvent : target->getCreatureEvents(CREATURE_EVENT_PREPAREDEATH))
					if (not creatureEvent->executeOnPrepareDeath(target, attacker))
					{
						// in this situation, as is, the victim simply loses whatever mana was lost from manashield damage
						// I'm thinking we need to probably have better options for user on what happens specifically during
						// a return false for a preparedeath event, like for returning mana or health, how much, ect. 
						target->changeHealth(target_health, true);
						return 0;
					}
			}
		}

		return damage_dealt;
	}

	uint32_t Combat::apply_damage(const CreaturePtr& attacker, const MonsterPtr& target, const std::optional<std::span<const CreaturePtr>> spectators) const noexcept
	{
		if (damage_type == Combat::DamageType::ManaDrain)
			return 0;

		const auto target_health = static_cast<uint32_t>(target->getHealth());
		const auto target_position = target->getPosition();
		const auto& target_name = target->getName();
		const auto& attacker_name = attacker->getNameDescription();
		const auto notice = UnPackNotice(collect_notice_data(target));
		const auto health_changed = std::min(damage, target_health);

		if (health_changed == 0)
			return 0;

		target->drainHealth(attacker, health_changed);

		auto pre_cache = spectators;
		if (not pre_cache)
			pre_cache = std::make_optional<std::span<const CreaturePtr>>(g_game.map.fetchSpectators(target_position, true, true));

		auto is_observer = [&](const auto& spectator) { return spectator != attacker; };

		if (attacker->is_player())
		{
			auto* caster = static_cast<Player*>(attacker.get());
			TextMessage attacker_message = {};
			attacker_message.position = target_position;
			attacker_message.primary.color = notice.color;
			attacker_message.primary.value = health_changed;
			attacker_message.type = MESSAGE_DAMAGE_DEALT;
			attacker_message.text = target_name + " loses " + std::to_string(health_changed) + " health due to your attack.";
			caster->sendTextMessage(attacker_message);
			caster->sendMagicEffect(target_position, notice.effect);
			caster->sendCreatureHealth(target);
		}

		TextMessage observer_message = {};
		observer_message.position = target_position;
		observer_message.primary.color = notice.color;
		observer_message.primary.value = health_changed;
		observer_message.type = MESSAGE_DAMAGE_OTHERS;
		observer_message.text = target_name + " loses " + std::to_string(health_changed) + " health due to an attack by " + attacker_name + ".";

		for (const auto& spectator : *pre_cache | std::views::filter(is_observer))
		{
			auto* player = static_cast<Player*>(spectator.get());

			player->sendTextMessage(observer_message);
			player->sendMagicEffect(target_position, notice.effect);
			player->sendCreatureHealth(target);
		}

		if (notice.fluid != 0)
		{
			TilePtr tile = g_game.map.getTile(target_position);
			CylinderPtr c_tile = tile;
			auto fluid = Item::CreateItem(ITEM_SMALLSPLASH, notice.fluid);
			if (fluid) [[likely]]
			{
				g_game.internalAddItem(c_tile, fluid, INDEX_WHEREEVER, FLAG_NOLIMIT);
				g_game.startDecay(fluid);
			}
		}

		if (health_changed == target_health)
		{
			for (const auto& creatureEvent : target->getCreatureEvents(CREATURE_EVENT_PREPAREDEATH))
				if (not creatureEvent->executeOnPrepareDeath(target, attacker))
				{
					target->changeHealth(target_health, true);
					return 0;
				}
		}

		return health_changed;
	}

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