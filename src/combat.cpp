// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "combat.h"
#include "game.h"
#include "scheduler.h"
#include "weapons.h"
#include "configmanager.h"
#include "events.h"
#include "monster.h"
#include "luascript.h"
#include "spells.h"

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

	void Combat::SetSituationFormulas(uint8_t sit_idx, SituationFormulas&& formulas) noexcept
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

	void Combat::SetFormulaCallback(uint8_t sit_idx, FormulaStage stage, int32_t lua_ref) noexcept
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

	void Combat::AddCondition(ConditionHandle cond)
	{
		if (cond)
		{
			condition_list.push_front(std::move(cond));
			config.set(Config::HasCondition);
		}
	}

	void Combat::ClearConditions() noexcept
	{
		condition_list.clear();
		config.set(Config::HasCondition, false);
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

	struct HealNoticeData
	{
		TextColor_t        color   = TEXTCOLOR_NONE;
		MagicEffectClasses effect  = CONST_ME_NONE;
		Combat::Config     stat    = Combat::Config::PlaceHolder;
		uint8_t            padding = 0;
	};

	struct HealNotice
	{
		TextColor_t        color;
		MagicEffectClasses effect;
		std::string_view   stat_name;
	};

	template<ByteLike T1, ByteLike T2, ByteLike T3>
	[[nodiscard]] constexpr uint32_t PackHealNotice(T1 color, T2 effect, T3 stat)
	{
		HealNoticeData data
		{
			.color   = static_cast<TextColor_t>(color),
			.effect  = static_cast<MagicEffectClasses>(effect),
			.stat    = static_cast<Combat::Config>(stat),
			.padding = 0
		};
		return std::bit_cast<uint32_t>(data);
	}

	static HealNotice UnPackHealNotice(uint32_t packed)
	{
		const auto data = std::bit_cast<HealNoticeData>(packed);
		const std::string_view stat_name =
		    data.stat == Combat::Config::HealthTarget  ? "health"  :
		    data.stat == Combat::Config::ManaTarget    ? "mana"    :
		    data.stat == Combat::Config::StaminaTarget ? "stamina" :
		    data.stat == Combat::Config::SoulTarget    ? "soul"    : "";
		return { data.color, data.effect, stat_name };
	}

	namespace
	{
		const DeflectionEffectMap _StandardDeflectionMap =
		{
			{1, {{0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{2, {{0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 1, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 1, 2, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{3, {{0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{4, {{0, 0, 0, 0, 0,
				  0, 0, 1, 0, 0,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 1, 1, 1, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{5, {{0, 0, 0, 0, 0,
				  1, 0, 0, 0, 1,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 1, 0, 0,
				  0, 0, 1, 0, 0,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 1, 0, 0,
				  0, 1, 1, 1, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{6, {{0, 0, 0, 0, 0,
				  1, 0, 1, 0, 1,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 1, 0, 1, 0,
				  0, 0, 1, 0, 0,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 1, 0, 0,
				  0, 1, 3, 1, 0,
				  1, 0, 0, 0, 1,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 1, 1, 1, 0,
				  0, 1, 3, 1, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}}
		};

		const DeflectionEffectMap _DiagonalDeflectionMap =
		{
			{1,  {{0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{2, {{0, 0, 0, 0, 0,
				  0, 0, 1, 0, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 1, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 1, 0, 0, 0,
				  0, 0, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 2, 1, 0,
				  0, 0, 1, 0, 0,
				  0, 0, 0, 0, 0}}},
			{3, {{0, 0, 0, 0, 0,
				  0, 0, 1, 0, 0,
				  0, 1, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 0, 1, 0,
				  0, 0, 3, 0, 0,
				  0, 1, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 3, 1, 0,
				  0, 0, 1, 0, 0,
				  0, 0, 0, 0, 0}}},
			{4, {{0, 0, 0, 0, 0,
				  0, 1, 1, 0, 0,
				  0, 1, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 1, 0, 1, 0,
				  0, 0, 3, 0, 0,
				  0, 1, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {1, 0, 0, 0, 0,
				  0, 1, 1, 0, 0,
				  0, 1, 2, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{5, {{0, 0, 1, 0, 0,
				  0, 0, 1, 0, 0,
				  1, 1, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 0, 1, 1, 0,
				  0, 1, 3, 0, 0,
				  0, 1, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 1, 0, 0,
				  0, 1, 1, 0, 0,
				  1, 1, 2, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0}}},
			{6, {{0, 0, 1, 0, 0,
				  0, 1, 1, 0, 0,
				  1, 1, 3, 0, 0,
				  0, 0, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {0, 0, 0, 0, 0,
				  0, 1, 1, 1, 0,
				  0, 1, 3, 0, 0,
				  0, 1, 0, 0, 0,
				  0, 0, 0, 0, 0},
				 {1, 0, 0, 0, 0,
				  0, 1, 1, 1, 0,
				  0, 1, 2, 0, 0,
				  0, 1, 0, 0, 0,
				  0, 0, 0, 0, 0}}}
		};

		const RawArea Deflect1xArea =
		{
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 3, 0, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
		};

		const RawAreaVec Deflect2xAreas =
		{
			{0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 1, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 1, 2, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawArea Deflect3xArea =
		{
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0,
			0, 1, 3, 1, 0,
			0, 0, 0, 0, 0,
			0, 0, 0, 0, 0
		};

		const RawAreaVec Deflect4xAreas =
		{
			{0, 0, 0, 0, 0,
			 0, 0, 1, 0, 0,
			 0, 1, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 1, 1, 1, 0,
			 0, 0, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec Deflect5xAreas =
		{
			{0, 0, 0, 0, 0,
			 1, 0, 0, 0, 1,
			 0, 1, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 1, 0, 0,
			 0, 0, 1, 0, 0,
			 0, 1, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 1, 0, 0,
			 0, 1, 1, 1, 0,
			 0, 0, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec Deflect6xAreas =
		{
			{0, 0, 0, 0, 0,
			 1, 0, 1, 0, 1,
			 0, 1, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 1, 0, 1, 0,
			 0, 0, 1, 0, 0,
			 0, 1, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 1, 0, 0,
			 0, 1, 3, 1, 0,
			 0, 1, 0, 1, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 1, 1, 1, 0,
			 0, 1, 3, 1, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec DeflectDiagonal2xAreas =
		{
			{0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 3, 0, 0,
			 0, 0, 1, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 1, 0, 0,
			 0, 0, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 2, 1, 0,
			 0, 0, 1, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec DeflectDiagonal3xAreas =
		{
			{0, 0, 0, 0, 0,
			 0, 0, 1, 0, 0,
			 0, 1, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 3, 1, 0,
			 0, 0, 1, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec DeflectDiagonal4xAreas =
		{
			{0, 0, 0, 0, 0,
			 0, 1, 1, 0, 0,
			 0, 1, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 1, 0, 0, 0,
			 0, 0, 3, 1, 0,
			 0, 0, 1, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec DeflectDiagonal5xAreas =
		{
			{0, 0, 1, 0, 0,
			 0, 0, 1, 0, 0,
			 1, 1, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 1, 1, 0,
			 0, 1, 3, 0, 0,
			 0, 1, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 0, 1, 0,
			 0, 0, 3, 1, 0,
			 0, 1, 1, 0, 0,
			 0, 0, 0, 0, 0}
		};

		const RawAreaVec DeflectDiagonal6xAreas =
		{
			{0, 0, 1, 0, 0,
			 0, 1, 1, 0, 0,
			 1, 1, 3, 0, 0,
			 0, 0, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 1, 1, 0,
			 0, 1, 3, 0, 0,
			 0, 1, 0, 0, 0,
			 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0,
			 0, 0, 0, 1, 0,
			 0, 0, 3, 1, 0,
			 0, 1, 1, 0, 0,
			 0, 0, 0, 0, 0}
		};

		std::vector<uint32_t> GetDeflectArea(uint32_t targets)
		{
			switch (targets)
			{
				case 1:  return Deflect1xArea;
				case 2:  return Deflect2xAreas[uniform_random(0, static_cast<int32_t>(Deflect2xAreas.size()) - 1)];
				case 3:  return Deflect3xArea;
				case 4:  return Deflect4xAreas[uniform_random(0, static_cast<int32_t>(Deflect4xAreas.size()) - 1)];
				case 5:  return Deflect5xAreas[uniform_random(0, static_cast<int32_t>(Deflect5xAreas.size()) - 1)];
				default: return Deflect6xAreas[uniform_random(0, static_cast<int32_t>(Deflect6xAreas.size()) - 1)];
			}
		}

		std::vector<uint32_t> GetDiagonalDeflectArea(uint32_t targets)
		{
			switch (targets)
			{
				case 1:  return Deflect1xArea;
				case 2:  return DeflectDiagonal2xAreas[uniform_random(0, static_cast<int32_t>(DeflectDiagonal2xAreas.size()) - 1)];
				case 3:  return DeflectDiagonal3xAreas[uniform_random(0, static_cast<int32_t>(DeflectDiagonal3xAreas.size()) - 1)];
				case 4:  return DeflectDiagonal4xAreas[uniform_random(0, static_cast<int32_t>(DeflectDiagonal4xAreas.size()) - 1)];
				case 5:  return DeflectDiagonal5xAreas[uniform_random(0, static_cast<int32_t>(DeflectDiagonal5xAreas.size()) - 1)];
				default: return DeflectDiagonal6xAreas[uniform_random(0, static_cast<int32_t>(DeflectDiagonal6xAreas.size()) - 1)];
			}
		}
	}

	Position Combat::generateAttackPosition(const CreaturePtr& attacker, const PlayerPtr& defender) noexcept
	{
		const auto defense_position = defender->getPosition();
		const Direction attackDirection = getDirectionTo(defense_position, attacker->getPosition());

		// Offsets
		static constexpr std::array<std::array<int, 4>, 8> DIRECTION_PATTERNS = 
		{ {
				// x_start, x_end, y_start, y_end
				{-1, 1, -2, -1},		// NORTH:
				{1, 2, -1, 1},			// EAST:
				{-1, 1, 1, 2},			// SOUTH:
				{-2, -1, -1, 1},		// WEST:
				{-2, -1, 1, 2},			// SOUTHWEST:
				{1, 2, 1, 2},			// SOUTHEAST:
				{-2, -1, -2, -1},		// NORTHWEST:
				{1, 2, -2, -1}			// NORTHEAST:
		} };

		std::vector<Position> possibleTargets;
		possibleTargets.reserve(9);

		const auto& pattern = DIRECTION_PATTERNS[attackDirection & 0x7]; // Mask to handle diagonal directions

		auto addLocationInline = [&](int x, int y) 
		{
			Position targetLocation
			{
				static_cast<uint16_t>(defense_position.x + x),
				static_cast<uint16_t>(defense_position.y + y),
				defense_position.z
			};

			const auto& tile = g_game.map.getTile(targetLocation);
			const bool isValid = tile
				and g_game.canThrowObjectTo(defense_position, targetLocation)
				and tile->getZone() != ZONE_PROTECTION
				and not tile->hasFlag(TILESTATE_PROTECTIONZONE
					| TILESTATE_FLOORCHANGE
					| TILESTATE_TELEPORT
					| TILESTATE_IMMOVABLEBLOCKSOLID
					| TILESTATE_NOPVPZONE
					| TILESTATE_IMMOVABLEBLOCKPATH
					| TILESTATE_IMMOVABLENOFIELDBLOCKPATH);

			if (isValid)
				possibleTargets.emplace_back(targetLocation);
		};

		for (int x = pattern[0]; x <= pattern[1]; ++x)
			for (int y = pattern[2]; y <= pattern[3]; ++y)
				addLocationInline(x, y);

		const size_t vectorSize = possibleTargets.size();
		const size_t index = vectorSize ? static_cast<size_t>(uniform_random(0, static_cast<int32_t>(vectorSize) - 1)) : 0;
		return vectorSize ? possibleTargets[index] : Spells::getCasterPosition(defender, getOppositeDirection(defender->getDirection()));
	}

	std::unique_ptr<AreaCombat> Combat::generateDeflectArea(const CreaturePtr& attacker, const PlayerPtr& defender, int32_t targetCount) noexcept
	{
		auto combatArea = std::make_unique<AreaCombat>();
		const auto& defendersPosition = defender->getPosition();

		switch (const auto direction = getDirectionTo(defendersPosition, attacker->getPosition()))
		{
			case DIRECTION_NORTH:		[[fallthrough]];
			case DIRECTION_EAST:		[[fallthrough]];
			case DIRECTION_SOUTH:		[[fallthrough]];
			case DIRECTION_WEST:
			{
				const auto& targetAreas = _StandardDeflectionMap.find(targetCount)->second;
				if (not targetAreas.empty())
				{
					const auto index = static_cast<size_t>(uniform_random(0, static_cast<int32_t>(targetAreas.size()) - 1));
					const auto& area = targetAreas[index];
					combatArea->setupArea(area, 5);
				}
				break;
			}
			case DIRECTION_SOUTHWEST:	[[fallthrough]];
			case DIRECTION_SOUTHEAST:	[[fallthrough]];
			case DIRECTION_NORTHWEST:	[[fallthrough]];
			case DIRECTION_NORTHEAST:
			{
				if (const auto targetAreas = _DiagonalDeflectionMap.find(targetCount)->second; not targetAreas.empty())
				{
					const auto index = static_cast<size_t>(uniform_random(0, static_cast<int32_t>(targetAreas.size()) - 1));
					const auto& area = targetAreas[index];
					combatArea->setupExtArea(area, 5);
				}
				break;
			}
					
			default:[[unlikely]]
				// todo: log here
				//std::cerr << "Deflection area attempted to be generated from unknown direction!" << std::endl;
				break;
		}

		return combatArea;
	}

	void Combat::getOpenPositionsInRadius(const PlayerPtr& defender, int radius, std::vector<Position>& out) noexcept
	{
		const auto& center = defender->getPosition();
		for (int x = -radius; x <= radius; ++x)
		{
			for (int y = -radius; y <= radius; ++y)
			{
				Position pos(center.x + x, center.y + y, center.z);

				auto tile = g_game.map.getTile(pos);
				const bool isValid = tile
					and g_game.canThrowObjectTo(center, pos)
					and tile->getZone() != ZONE_PROTECTION
					and not tile->hasFlag(TILESTATE_PROTECTIONZONE
						| TILESTATE_FLOORCHANGE
						| TILESTATE_TELEPORT
						| TILESTATE_IMMOVABLEBLOCKSOLID
						| TILESTATE_NOPVPZONE
						| TILESTATE_IMMOVABLEBLOCKPATH
						| TILESTATE_IMMOVABLENOFIELDBLOCKPATH);

				if (isValid)
				{
					out.push_back(pos);
				}
			}
		}
	}
	
	// Note: We chose not to reduce the damage on the combat object calling this method, inside this method,
	// we chose to handle it this way so that we can make this a static method which is only responsible
	// for creating and executing new damages
	void Combat::deflect_damage(const CreaturePtr& attacker, const PlayerPtr& defender, uint32_t amount, uint16_t damageType, uint8_t distanceEffect, uint8_t impactEffect) noexcept
	{
		constexpr int32_t DAMAGE_DIVIDER = 50;
		constexpr int32_t MAX_TARGETS = 6;
		const int32_t calculatedTargets = std::min<int32_t>(static_cast<int32_t>(std::round(static_cast<double>(amount) / DAMAGE_DIVIDER)) + 1, MAX_TARGETS);

		const auto attackPos = generateAttackPosition(attacker, defender);
		auto damageArea = generateDeflectArea(attacker, defender, calculatedTargets);

		auto deflect = g_combat_registry.Create(damageType, static_cast<uint32_t>(std::round(static_cast<double>(amount) / calculatedTargets)));
		deflect->config.set(Config::Aggressive);
		deflect->config.set(Config::TopTargetOnly);
		deflect->config.set(Config::TrueDamage);
		deflect->config.set(Config::HasArea);
		deflect->distanceEffect = distanceEffect;
		deflect->origin = Origin::Deflect;
		deflect->impactEffect = (impactEffect == CONST_ME_NONE)	? CombatTypeToAreaEffect(static_cast<CombatType_t>(damageType)) : impactEffect;
		deflect->setArea(std::move(damageArea));
		deflect->execute(defender, attackPos);
	}

	void Combat::ricochetDamage(const PlayerPtr& defender, uint32_t amount, uint16_t damageType, uint8_t distanceEffect, uint8_t impactEffect) noexcept
	{
		std::vector<Position> targetList;
		getOpenPositionsInRadius(defender, 3, targetList);

		if (targetList.size() > 0)
		{
			const auto& targetPos = targetList[uniform_random(0, targetList.size() - 1)];

			auto damageArea = std::make_unique<AreaCombat>();
			damageArea->setupArea(Deflect1xArea, 5);

			auto ricochet = g_combat_registry.Create(damageType, amount);
			ricochet->config.set(Config::Aggressive);
			ricochet->config.set(Config::TopTargetOnly);
			ricochet->config.set(Config::TrueDamage);
			ricochet->config.set(Config::HasArea);
			ricochet->distanceEffect = distanceEffect;
			ricochet->origin = Origin::Ricochet;
			ricochet->impactEffect = (impactEffect == CONST_ME_NONE) ? CombatTypeToAreaEffect(static_cast<CombatType_t>(damageType)) : impactEffect;
			ricochet->setArea(std::move(damageArea));
			ricochet->execute(defender, targetPos);
		}
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

	void Combat::setArea(AreaCombat* area)
	{
		if (!area)
			return;

		CombatArea cardinal = area->GetCombatArea();
		const bool hasCard = std::ranges::any_of(cardinal.directions, [](const auto& d) { return !d.empty(); });
		if (hasCard)
			combat_area_map[combat_id] = std::move(cardinal);

		CombatArea ext = area->GetExtCombatArea();
		const bool hasExt = std::ranges::any_of(ext.directions, [](const auto& d) { return !d.empty(); });
		if (hasExt)
			combat_ext_area_map[combat_id] = std::move(ext);

		delete area;
	}

	void Combat::setArea(std::unique_ptr<AreaCombat> area)
	{
		if (!area)
			return;

		CombatArea cardinal = area->GetCombatArea();
		const bool hasCard = std::ranges::any_of(cardinal.directions, [](const auto& d) { return !d.empty(); });
		if (hasCard)
			combat_area_map[combat_id] = std::move(cardinal);

		CombatArea ext = area->GetExtCombatArea();
		const bool hasExt = std::ranges::any_of(ext.directions, [](const auto& d) { return !d.empty(); });
		if (hasExt)
			combat_ext_area_map[combat_id] = std::move(ext);
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
		return { std::move(area_position_buffer), distance };
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
				case Combat::TargetCode::UnknownFailure:			return "Failure";
				case Combat::TargetCode::Valid:						return "Valid";
				case Combat::TargetCode::YouAreInProtectionZone:	return "You are in protection zone";
				case Combat::TargetCode::CanNotAttackThisPlayer:	return "You can not attack this player";
				case Combat::TargetCode::TargetIsInProtectionZone:	return "Target is in protection zone";
				case Combat::TargetCode::CanNotAttackThisMonster:	return "Can not attack this monster";
				case Combat::TargetCode::PlayerIsUnMarked:			return "Player is UnMarked";
				case Combat::TargetCode::CanNotWieldWeapon:			return "You are unable to wield this weapon";
				case Combat::TargetCode::NotEnoughRoom:				return "There is not enough room";
				case Combat::TargetCode::FirstGoUpStairs:			return "First go upstairs";
				case Combat::TargetCode::FirstGoDownStairs:			return "First go downstairs";
				default:											return "Failure";
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

		const bool attack_blocked = caster->hasFlag(PlayerFlag_CannotAttackPlayer) or isProtected(caster, victim) or victim->hasFlag(PlayerFlag_CannotBeAttacked);

		if (attack_blocked)
			return Combat::TargetCode::CanNotAttackThisPlayer;

		const auto victim_tile = victim->getTile();
		const auto attacker_tile = caster->getTile();

		const bool attacker_blocked = attacker_tile->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE);
		const bool target_protected = victim_tile->hasFlag(TILESTATE_NOPVPZONE | TILESTATE_PROTECTIONZONE);

		if (attacker_blocked)
			return Combat::TargetCode::YouAreInProtectionZone;

		if (target_protected)
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
			const bool attack_blocked = commander->hasFlag(PlayerFlag_CannotAttackPlayer) or isProtected(commander, victim) or victim->hasFlag(PlayerFlag_CannotBeAttacked);

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

	Combat::TargetCode Combat::target(const CreaturePtr& attacker, const CreaturePtr& defender) const noexcept
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

	CombatHandle Combat::transformDamage(const uint16_t damage_type, const uint32_t amount) noexcept
	{
		auto handle = g_combat_registry.Create(damage_type, amount);
		handle->config.set(Config::AttackModified);
		if (damage_type == static_cast<uint16_t>(DamageType::Healing))
			handle->config.set(Config::HealthTarget);
		return handle;
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

	CombatHandle Combat::penetrateDamage(const uint32_t percent, const uint32_t flat) noexcept
	{
		uint32_t penetration_damage = 0;

		if (percent > 0)
			penetration_damage += damage * percent / 100u;

		if (flat > 0)
			penetration_damage += flat;

		penetration_damage = (penetration_damage > damage) ? damage : penetration_damage;
		damage -= penetration_damage;

		auto handle = g_combat_registry.Create(std::to_underlying(DamageType::Undefined), penetration_damage);
		handle->origin = Origin::Piercing;
		handle->config.set(Config::TrueDamage);
		return handle;
	}

	// Todo: Currently the way all the damage modifiers work in the augment and combat systems, is that it's percent is based on
	// the damage value, and that is actually not the most common, or at least not the only top common path... People will often
	// choose to target percent based on victim's stats.. example, 10 percent lifesteal would steal 10 percent of the victims overall health
	// rather than just 10 percent of the damage output. I don't know how I have never really realized this huge fallacy before now.
	// The 'Todo' here is to build another cache system strictly bound by "damage output" or "victim stat"

	void Combat::process_steal(const PlayerPtr& caster, const CreaturePtr& victim, const LeechData& steal) noexcept
	{
		const auto totalDamage = damage;

		if (steal.percent_health > 0 or steal.flat_health > 0)
		{
			uint32_t life = 0;

			if (steal.percent_health)
				life += totalDamage * steal.percent_health / 100;

			if (steal.flat_health)
				life += steal.flat_health;

			if (life)
			{
				const uint32_t stolen = std::min(life, damage);
				damage -= stolen;

				auto lifesteal = g_combat_registry.Create(DamageType::LifeDrain, stolen);
				lifesteal->origin = Origin::LifeSteal;
				lifesteal->config.set(Config::Leech);
				lifesteal->config.set(Config::TopTargetOnly);

				lifesteal->config.set(Config::TrueDamage);		// This is our new path for setting damage values directly
				lifesteal->damage = stolen;						// by setting it as true damage it skips the "block" call which does the callbacks for damage calculation
				lifesteal->strike_target(caster, victim, true);	// and calling the strike_target directly if we are not executing an area combat, execute if it is an area combat
			}
		}

		if (steal.percent_mana > 0 or steal.flat_mana > 0)
		{
			uint32_t mana = 0;

			if (steal.percent_mana)
				mana += totalDamage * steal.percent_mana / 100;

			if (steal.flat_mana)
				mana += steal.flat_mana;

			if (mana)
			{
				const uint32_t stolen = std::min(mana, damage);
				damage -= stolen;

				auto manasteal = g_combat_registry.Create(DamageType::ManaDrain, stolen);
				manasteal->origin = Origin::ManaSteal;
				manasteal->config.set(Config::Leech);
				manasteal->config.set(Config::TopTargetOnly);
				manasteal->config.set(Config::TrueDamage);
				manasteal->damage = stolen;
				manasteal->strike_target(caster, victim, true);
			}
		}

		if (steal.percent_stamina > 0 or steal.flat_stamina > 0)
		{
			uint32_t stamina = 0;

			if (steal.percent_stamina)
				stamina += totalDamage * steal.percent_stamina / 100;

			if (steal.flat_stamina)
				stamina += steal.flat_stamina;

			if (stamina)
			{
				const uint32_t stolen = std::min(stamina, damage);
				damage -= stolen;

				auto staminasteal = g_combat_registry.Create(DamageType::Undefined, stolen);
				staminasteal->origin = Origin::StaminaSteal;
				staminasteal->config.set(Config::Leech);
				staminasteal->config.set(Config::TopTargetOnly);
				staminasteal->config.set(Config::TrueDamage);
				staminasteal->damage = stolen;
				staminasteal->strike_target(caster, victim, true);
			}
		}

		if (steal.percent_soul > 0 or steal.flat_soul > 0)
		{
			uint32_t soul = 0;

			if (steal.percent_soul)
				soul += totalDamage * steal.percent_soul / 100;

			if (steal.flat_soul)
				soul += steal.flat_soul;

			if (soul)
			{
				const uint32_t stolen = std::min(soul, damage);
				damage -= stolen;

				auto soulsteal = g_combat_registry.Create(DamageType::Undefined, stolen);
				soulsteal->origin = Origin::SoulSteal;
				soulsteal->config.set(Config::Leech);
				soulsteal->config.set(Config::TopTargetOnly);
				soulsteal->config.set(Config::TrueDamage);
				soulsteal->damage = stolen;
				soulsteal->strike_target(caster, victim, true);
			}
		}
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
			const auto totalDamage = damage;

			if (leech.percent_health > 0 or leech.flat_health > 0)
			{
				uint32_t life = 0;

				if (leech.percent_health)
					life += totalDamage * leech.percent_health / 100;

				if (leech.flat_health)
					life += leech.flat_health;

				if (life)
				{
					auto hp_gain = g_combat_registry.Create(DamageType::Healing, life);
					hp_gain->origin = Origin::LifeSteal;
					hp_gain->config.set(Config::HealthTarget);
					hp_gain->config.set(Config::Leech);
					hp_gain->config.set(Config::TopTargetOnly);
					hp_gain->config.set(Config::TrueDamage);
					hp_gain->heal_target(caster, caster, true);
				}
			}

			if (leech.percent_mana > 0 or leech.flat_mana > 0)
			{
				uint32_t mana = 0;

				if (leech.percent_mana)
					mana += totalDamage * leech.percent_mana / 100;

				if (leech.flat_mana)
					mana += leech.flat_mana;

				if (mana)
				{
					auto mana_gain = g_combat_registry.Create(DamageType::Healing, mana);
					mana_gain->origin = Origin::ManaSteal;
					mana_gain->config.set(Config::ManaTarget);
					mana_gain->config.set(Config::Leech);
					mana_gain->config.set(Config::TopTargetOnly);
					mana_gain->config.set(Config::TrueDamage);
					mana_gain->heal_target(caster, caster, true);
				}
			}

			if (leech.percent_stamina > 0 or leech.flat_stamina > 0)
			{
				uint32_t stamina = 0;

				if (leech.percent_stamina)
					stamina += totalDamage * leech.percent_stamina / 100;

				if (leech.flat_stamina)
					stamina += leech.flat_stamina;

				if (stamina)
				{
					auto stamina_gain = g_combat_registry.Create(DamageType::Healing, stamina);
					stamina_gain->origin = Origin::StaminaSteal;
					stamina_gain->config.set(Config::StaminaTarget);
					stamina_gain->config.set(Config::Leech);
					stamina_gain->config.set(Config::TopTargetOnly);
					stamina_gain->config.set(Config::TrueDamage);
					stamina_gain->heal_target(caster, caster, true);
				}
			}

			if (leech.percent_soul > 0 or leech.flat_soul > 0)
			{
				uint32_t soul = 0;

				if (leech.percent_soul)
					soul += totalDamage * leech.percent_soul / 100;

				if (leech.flat_soul)
					soul += leech.flat_soul;

				if (soul)
				{
					auto soul_gain = g_combat_registry.Create(DamageType::Healing, soul);
					soul_gain->origin = Origin::SoulSteal;
					soul_gain->config.set(Config::SoulTarget);
					soul_gain->config.set(Config::Leech);
					soul_gain->config.set(Config::TopTargetOnly);
					soul_gain->config.set(Config::TrueDamage);
					soul_gain->heal_target(caster, caster, true);
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
	uint32_t Combat::handle_conversion(std::span<const DamageModifier> modifiers, const CreaturePtr& attacker, const CreaturePtr& victim)
	{
		uint32_t physical = 0, energy = 0, earth = 0, fire = 0;
		uint32_t undefined = 0, lifedrain = 0, manadrain = 0;
		uint32_t healing = 0, water = 0, ice = 0;
		uint32_t holy = 0, death = 0, total = 0;

		for (const auto& modifier : modifiers)
		{
			const uint32_t index = std::countr_zero(modifier.to_damage_type);
			const uint32_t value = modifier.isFlatValue() ? static_cast<uint32_t>(modifier.value)
			                                              : static_cast<uint32_t>(static_cast<uint64_t>(damage) * modifier.value / 100u);
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
		if (healing)	Combat::transformDamage(Combat::DamageType::Healing,   healing)		->heal_target(attacker, victim);
		if (water)		Combat::transformDamage(Combat::DamageType::Water,     water)		->strike_target(attacker, victim);
		if (ice)		Combat::transformDamage(Combat::DamageType::Ice,       ice)			->strike_target(attacker, victim);
		if (holy)		Combat::transformDamage(Combat::DamageType::Holy,      holy)		->strike_target(attacker, victim);
		if (death)		Combat::transformDamage(Combat::DamageType::Death,     death)		->strike_target(attacker, victim);

		return total;
	}

	void Combat::reform_augment(const CreaturePtr& attacker, const PlayerPtr& victim) noexcept
	{
		const auto& reform_mods = victim->getReformMods();
		if (reform_mods.empty())
			return;

		const auto attacker_type = attacker->getType();
		const auto attacker_race = attacker->getRace();
		const auto& attacker_name = attacker->getName();

		auto applied = [&](const auto& modifier)
		{
			return modifier.applies(damage_type, attacker_type, origin, attacker_race, attacker_name);
		};

		uint32_t to_amounts[DamageType::DamageTypes] = {};
		uint32_t total_converted = 0;

		auto reform_view = reform_mods | std::views::filter(applied);
		for (const auto& modifier : reform_view)
		{
			const uint32_t index = std::countr_zero(static_cast<uint32_t>(modifier.to_damage_type));
			if (index >= DamageType::DamageTypes) [[unlikely]]
				continue;

			const uint32_t value = modifier.isFlatValue()
				? modifier.getValue()
				: static_cast<uint32_t>(static_cast<uint64_t>(damage) * modifier.getValue() / 100u);

			to_amounts[index] += value;
			total_converted += value;
		}

		if (total_converted == 0)
			return;

		total_converted = std::min(total_converted, damage);
		damage -= total_converted;

		for (uint32_t i = 0; i < DamageType::DamageTypes; ++i)
		{
			if (to_amounts[i] == 0)
				continue;

			auto reformed = g_combat_registry.Create(static_cast<uint16_t>(1u << i), to_amounts[i]);
			reformed->origin = Origin::Augment;
			reformed->config.set(Config::AttackModified);
			reformed->config.set(Config::DefenseModified);
			reformed->strike_target(attacker, victim, true);
		}
	}

	// We use the TrueDamage config for all defensive modifiers in order to set the exact damage
	// which is used for the new combats.. in other words, so it's not affected by the formula's
	// which normally dictate how damage is generated and reduced, or calculated
	// It's also worth realizing that with our chosen path of controlled order of defensive modifier
	// application, we are reducing damage each step of the way, and so the damage value the later modifiers
	// are basing their values against will be reduced... this can result in something like 100 percent absorb blocking
	// any and all restore, resist and deflects for example. If this is not a desired behavior for you, raise hell about it
	// in the blactkek discord community, and I will pay attention. It's possible to write an entirely different version of this method
	// and in that method, it's handled where they are all based on "originating" damage, so you can have 100 percent absorb, 100 percent restore, and more
	// but since that seems illogical and unconventional to me, I am not doing it that way at this time
	void Combat::defense_augment(const CreaturePtr& attacker, const PlayerPtr& victim, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		const float aug_mult = victim->get_augment_charge_cost_multiplier();
		const uint32_t aug_cost = static_cast<uint32_t>(std::round(static_cast<float>(augment_charge_cost) * aug_mult));

		if (aug_cost > 0 and victim->get_augment_charges() < aug_cost)
			return;

		// here we are making another decision on behalf of the end user, to charge once for using damage modifiers, instead of 
		// once per modifier, or once per augment type.. once per modifier is out of question at this point in the stack call
		// so if this was something someone somewhere wants, I encourage you to go on blacktek discord and raise the issue.
		if (aug_cost > 0) 
			victim->set_augment_charges(victim->get_augment_charges() - aug_cost);

		const auto attacker_type = attacker->getType();
		const auto attacker_race = attacker->getRace();
		const auto& attacker_name = attacker->getName();

		auto applied = [&](const auto& modifier)
		{
			return modifier.applies(damage_type, attacker_type, origin, attacker_race, attacker_name);
		};

		const auto& main_sums = victim->getMainDefenseModSums();

		using DefenseType = DamageModifier::DefenseType;

		auto percent_resist    = main_sums[std::to_underlying(DefenseType::Resist)].percent;
		auto flat_resist       = main_sums[std::to_underlying(DefenseType::Resist)].flat;
		auto percent_weak      = main_sums[std::to_underlying(DefenseType::Weakness)].percent;
		auto flat_weak         = main_sums[std::to_underlying(DefenseType::Weakness)].flat;
		auto percent_absorb    = main_sums[std::to_underlying(DefenseType::Absorb)].percent;
		auto flat_absorb       = main_sums[std::to_underlying(DefenseType::Absorb)].flat;
		auto percent_restore   = main_sums[std::to_underlying(DefenseType::Restore)].percent;
		auto flat_restore      = main_sums[std::to_underlying(DefenseType::Restore)].flat;
		auto percent_replenish = main_sums[std::to_underlying(DefenseType::Replenish)].percent;
		auto flat_replenish    = main_sums[std::to_underlying(DefenseType::Replenish)].flat;
		auto percent_revive    = main_sums[std::to_underlying(DefenseType::Revive)].percent;
		auto flat_revive       = main_sums[std::to_underlying(DefenseType::Revive)].flat;
		auto percent_reflect   = main_sums[std::to_underlying(DefenseType::Reflect)].percent;
		auto flat_reflect      = main_sums[std::to_underlying(DefenseType::Reflect)].flat;
		auto percent_deflect   = main_sums[std::to_underlying(DefenseType::Deflect)].percent;
		auto flat_deflect      = main_sums[std::to_underlying(DefenseType::Deflect)].flat;
		auto percent_ricochet  = main_sums[std::to_underlying(DefenseType::Ricochet)].percent;
		auto flat_ricochet     = main_sums[std::to_underlying(DefenseType::Ricochet)].flat;

		auto accumulate = [&](const DamageModifier& modifier)
		{
			const auto def_type = static_cast<DefenseType>(modifier.mod_type);
			const bool is_flat  = modifier.isFlatValue();
			const uint32_t val  = modifier.getValue();

			switch (def_type)
			{
				case DefenseType::Resist:    (is_flat ? flat_resist    : percent_resist)    += val; break;
				case DefenseType::Weakness:  (is_flat ? flat_weak      : percent_weak)      += val; break;
				case DefenseType::Absorb:    (is_flat ? flat_absorb    : percent_absorb)    += val; break;
				case DefenseType::Restore:   (is_flat ? flat_restore   : percent_restore)   += val; break;
				case DefenseType::Replenish: (is_flat ? flat_replenish : percent_replenish) += val; break;
				case DefenseType::Revive:    (is_flat ? flat_revive    : percent_revive)    += val; break;
				case DefenseType::Reflect:   (is_flat ? flat_reflect   : percent_reflect)   += val; break;
				case DefenseType::Deflect:   (is_flat ? flat_deflect   : percent_deflect)   += val; break;
				case DefenseType::Ricochet:  (is_flat ? flat_ricochet  : percent_ricochet)  += val; break;
				default: [[unlikely]] break;
			}
		};

		if (victim->hasFilteredDefenseMods())
		{
			auto fv = victim->getFilteredDefenseMods() | std::views::filter(applied);
			for (const auto& modifier : fv)
				accumulate(modifier);
		}

		if (victim->hasNamedDefenseMods())
		{
			auto fv = victim->getNamedDefenseMods() | std::views::filter(applied);
			for (const auto& modifier : fv)
				accumulate(modifier);
		}

		if (percent_weak)
			damage += damage * percent_weak / 100;

		if (flat_weak)
			damage += flat_weak;

		if (percent_resist)
		{
			const uint32_t reduction = damage * percent_resist / 100;
			damage = (reduction >= damage) ? 0 : damage - reduction;
		}

		if (flat_resist)
			damage = (flat_resist >= damage) ? 0 : damage - flat_resist;

		if (percent_absorb or flat_absorb)
		{
			uint32_t absorbed = (percent_absorb ? damage * percent_absorb / 100 : 0) + flat_absorb;
			absorbed = std::min(absorbed, damage);
			damage -= absorbed;

			auto heal = g_combat_registry.Create(DamageType::Healing, absorbed);
			heal->origin = Origin::Absorb;
			heal->config.set(Config::TrueDamage);
			heal->config.set(Config::HealthTarget);
			heal->config.set(Config::DefenseModified);
			heal->heal_target(victim, victim, true, spectators);
		}

		if (percent_restore or flat_restore)
		{
			uint32_t restored = (percent_restore ? damage * percent_restore / 100 : 0) + flat_restore;
			restored = std::min(restored, damage);
			damage -= restored;

			auto mana = g_combat_registry.Create(DamageType::Healing, restored);
			mana->origin = Origin::Restore;
			mana->config.set(Config::TrueDamage);
			mana->config.set(Config::ManaTarget);
			mana->config.set(Config::DefenseModified);
			mana->heal_target(victim, victim, true, spectators);
		}

		if (percent_replenish or flat_replenish)
		{
			uint32_t replenished = (percent_replenish ? damage * percent_replenish / 100 : 0) + flat_replenish;
			replenished = std::min(replenished, damage);
			damage -= replenished;

			auto stamina = g_combat_registry.Create(DamageType::Healing, replenished);
			stamina->origin = Origin::Replenish;
			stamina->config.set(Config::TrueDamage);
			stamina->config.set(Config::StaminaTarget);
			stamina->config.set(Config::DefenseModified);
			stamina->heal_target(victim, victim, true, spectators);
		}

		if (percent_revive or flat_revive)
		{
			uint32_t revived = (percent_revive ? damage * percent_revive / 100 : 0) + flat_revive;
			revived = std::min(revived, damage);
			damage -= revived;

			auto soul = g_combat_registry.Create(DamageType::Healing, revived);
			soul->origin = Origin::Revive;
			soul->config.set(Config::TrueDamage);
			soul->config.set(Config::SoulTarget);
			soul->config.set(Config::DefenseModified);
			soul->heal_target(victim, victim, true, spectators);
		}

		if ((percent_reflect or flat_reflect) and attacker)
		{
			uint32_t reflected = (percent_reflect ? damage * percent_reflect / 100 : 0) + flat_reflect;
			reflected = std::min(reflected, damage);
			damage -= reflected;

			auto reflect = g_combat_registry.Create(damage_type, reflected);
			reflect->origin = Origin::Reflect;
			reflect->config.set(Config::TrueDamage);
			reflect->config.set(Config::TopTargetOnly);
			reflect->strike_target(victim, attacker, true);
		}

		if ((percent_deflect or flat_deflect) and attacker)
		{
			uint32_t deflected = (percent_deflect ? damage * percent_deflect / 100 : 0) + flat_deflect;
			deflected = std::min(deflected, damage);
			damage -= deflected;
			deflect_damage(attacker, victim, deflected, damage_type, distanceEffect, impactEffect);
		}

		if ((percent_ricochet or flat_ricochet) and attacker)
		{			
			uint32_t ricocheted = (percent_ricochet ? damage * percent_ricochet / 100 : 0) + flat_ricochet;
			ricocheted = std::min(ricocheted, damage);
			damage -= ricocheted;
			ricochetDamage(victim, ricocheted, damage_type, distanceEffect, impactEffect);
		}
	}

	void Combat::strike_target(const PlayerPtr& caster, const PlayerPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			return; // log here
		}

		if (damage_type == DamageType::Healing)
		{
			heal_target(caster, victim, skip_validation, spectators);
			return;
		}

		const auto target_code = target(caster, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
		{
			auto message = resolve_target_code(target_code);
			caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
			return;
		}

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			const auto blocked = block(caster, victim);

			if (blocked != BlockType::NoBlock)
			{
				auto message = resolve_block_code(blocked);
				caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
		}

		LeechData leech_data {};
		LeechData steal_data {};

		if (caster->hasAttackModifiers() and not config.test(Config::AttackModified))
		{
			const auto conversion_count = caster->conversion_mod_count();
			const auto victim_race = victim->getRace();
			const auto& victim_name = victim->getName();
			const auto moddable = damage_type != DamageType::ManaDrain and damage_type != DamageType::Healing;

			auto applied = [&](const auto& modifier)
			{
				return modifier.applies(damage_type, victim->getType(), origin, victim_race, victim_name);
			};

			if (conversion_count > 0)
			{
				const auto& conversion_modifiers = caster->getConversionMods();
				std::vector<DamageModifier> filtered_conversions;
				auto conversion_view = conversion_modifiers | std::views::filter(applied);
				for (const auto& m : conversion_view)
					filtered_conversions.push_back(m);
				auto converted_damage = handle_conversion(std::span<const DamageModifier>(filtered_conversions), caster, victim);

				if (converted_damage)
				{
					damage = converted_damage >= damage ? 0 : damage - converted_damage;
					if (damage == 0)
					{
						// notifyPlayers();
						return;
					}
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

					auto attack_view = filtered_attack_mods | std::views::filter(applied);
					for (const auto& modifier : attack_view)
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

					auto post_attack_view = filtered_post_attack_mods | std::views::filter(applied);
					for (const auto& modifier : post_attack_view)
					{
						const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);
						auto& target = modifier.true_leech ? steal_data : leech_data;

						switch (mod_type)
						{
							case DamageModifier::AttackType::Lifesteal:
							{
								target.percent_health	+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_health		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Manasteal:
							{
								target.percent_mana		+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_mana		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Staminasteal:
							{
								target.percent_stamina	+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_stamina		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Soulsteal:
							{
								target.percent_soul		+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_soul		+= modifier.isFlatValue() ? modifier.getValue() : 0;
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
					true_damage->strike_target(caster, victim, true, spectators);
				}

				if (percent_crit or flat_crit)
					applyCrit(percent_crit, flat_crit);
			}
		}

		if (victim->hasDefenseModifiers() and not config.test(Config::DefenseModified))
		{
			const auto reform_count = victim->reform_mod_count();

			if (reform_count > 0)
				reform_augment(caster, victim);

			if (victim->defense_mod_count() > reform_count)
				defense_augment(caster, victim);
		}

		process_steal(caster, victim, steal_data);

		if (apply_damage(caster, victim, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			post_damage(caster, victim, std::move(leech_data));
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : condition_list)
					victim->addCondition(cond->clone());
			}

			// UseCharges: on a successful hit, consume one charge/count from the caster's equipped weapon.
			if (config.test(Config::UseCharges)) {
				if (const auto& weaponItem = caster->getWeapon(CONST_SLOT_RIGHT, false)) {
					const uint16_t charges = weaponItem->getCharges();
					if (charges != 0 && g_config.GetBoolean(ConfigManager::REMOVE_WEAPON_CHARGES)) {
						g_game.transformItem(weaponItem, weaponItem->getID(), charges - 1);
					} else if (weaponItem->getItemCount() > 1) {
						g_game.transformItem(weaponItem, weaponItem->getID(), weaponItem->getItemCount() - 1);
					} else {
						g_game.internalRemoveItem(weaponItem);
					}
				}
			}
		}
	}

	void Combat::strike_target(const PlayerPtr& caster, const MonsterPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		if (damage_type == DamageType::Healing)
		{
			heal_target(caster, victim, skip_validation, spectators);
			return;
		}

		const auto target_code = target(caster, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
		{
			auto message = resolve_target_code(target_code);
			caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
			return;
		}

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(caster, caster->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			const auto blocked = block(caster, victim);

			if (blocked != BlockType::NoBlock)
			{
				auto message = resolve_block_code(blocked);
				caster->sendTextMessage(MessageClasses::MESSAGE_INFO_DESCR, message);
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
		}

		LeechData leech_data {};
		LeechData steal_data {};

		if (caster->hasAttackModifiers() and not config.test(Config::AttackModified))
		{
			const auto conversion_count = caster->conversion_mod_count();
			const auto victim_race = victim->getRace();
			const auto& victim_name = victim->getName();
			const auto moddable = damage_type != DamageType::ManaDrain and damage_type != DamageType::Healing;

			auto applied = [&](const auto& modifier)
			{
				return modifier.applies(damage_type, victim->getType(), origin, victim_race, victim_name);
			};

			if (conversion_count > 0)
			{
				const auto& conversion_modifiers = caster->getConversionMods();
				std::vector<DamageModifier> filtered_conversions;
				auto conversion_view = conversion_modifiers | std::views::filter(applied);
				for (const auto& m : conversion_view)
					filtered_conversions.push_back(m);
				auto converted_damage = handle_conversion(std::span<const DamageModifier>(filtered_conversions), caster, victim);

				if (converted_damage)
				{
					damage = converted_damage >= damage ? 0 : damage - converted_damage;
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

					auto attack_view = filtered_attack_mods | std::views::filter(applied);
					for (const auto& modifier : attack_view)
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

					auto post_attack_view = filtered_post_attack_mods | std::views::filter(applied);
					for (const auto& modifier : post_attack_view)
					{
						const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);
						auto& target = modifier.true_leech ? steal_data : leech_data;

						switch (mod_type)
						{
							case DamageModifier::AttackType::Lifesteal:
							{
								target.percent_health	+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_health		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Manasteal:
							{
								target.percent_mana		+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_mana		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Staminasteal:
							{
								target.percent_stamina	+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_stamina		+= modifier.isFlatValue() ? modifier.getValue() : 0;
								break;
							}
							case DamageModifier::AttackType::Soulsteal:
							{
								target.percent_soul		+= modifier.isFlatValue() ? 0 : modifier.getValue();
								target.flat_soul		+= modifier.isFlatValue() ? modifier.getValue() : 0;
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
					true_damage->strike_target(caster, victim, true, spectators);
				}

				if (percent_crit or flat_crit)
					applyCrit(percent_crit, flat_crit);
			}
		}

		process_steal(caster, victim, steal_data);

		if (apply_damage(caster, victim, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			post_damage(caster, victim, std::move(leech_data));
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : condition_list)
					victim->addCondition(cond->clone());
			}

			// UseCharges: on a successful hit, consume one charge/count from the caster's equipped weapon.
			if (config.test(Config::UseCharges)) {
				if (const auto& weaponItem = caster->getWeapon(CONST_SLOT_RIGHT, false)) {
					const uint16_t charges = weaponItem->getCharges();
					if (charges != 0 && g_config.GetBoolean(ConfigManager::REMOVE_WEAPON_CHARGES)) {
						g_game.transformItem(weaponItem, weaponItem->getID(), charges - 1);
					} else if (weaponItem->getItemCount() > 1) {
						g_game.transformItem(weaponItem, weaponItem->getID(), weaponItem->getItemCount() - 1);
					} else {
						g_game.internalRemoveItem(weaponItem);
					}
				}
			}
		}

	}

	void Combat::strike_target(const MonsterPtr& attacker, const PlayerPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		if (damage_type == DamageType::Healing)
		{
			heal_target(attacker, victim, skip_validation, spectators);
			return;
		}

		const auto target_code = target(attacker, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
			return;

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			const auto blocked = block(attacker, victim);
			if (blocked != BlockType::NoBlock)
			{
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
		}

		if (victim->hasDefenseModifiers() and not config.test(Config::DefenseModified))
		{
			const auto reform_count = victim->reform_mod_count();

			if (reform_count > 0)
				reform_augment(attacker, victim);

			if (victim->defense_mod_count() > reform_count)
				defense_augment(attacker, victim);
		}

		if (apply_damage(attacker, victim, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : condition_list)
					victim->addCondition(cond->clone());
			}
		}
	}

	void Combat::strike_target(const MonsterPtr& attacker, const MonsterPtr& victim, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Unknown)
		{
			// log this
			return;
		}

		if (damage_type == DamageType::Healing)
		{
			heal_target(attacker, victim, skip_validation, spectators);
			return;
		}

		const auto target_code = target(attacker, victim);

		if (not skip_validation and target_code != TargetCode::Valid)
			return;

		if (distanceEffect != CONST_ANI_NONE)
			addDistanceEffect(attacker, attacker->getPosition(), victim->getPosition(), distanceEffect);

		if (not config.test(Config::TrueDamage))
		{
			const auto blocked = block(attacker, victim);
			if (blocked != BlockType::NoBlock)
			{
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
		}

		// Do we do anything here for summons attacking or being attacked or anything like that? Perhaps in the future when we allow passing
		// either augments or modifiers directly with combat this version of combat becomes useful, or maybe we revamp the summon system and finally
		// make summon an actual "thing" type derived from monster like I always thought it should be... I just feel like I'm forgetting something obvious right now

		// about the above comment, I think perhaps we can allow multiple formula's for different target creature types, monster, npc, boss or player.

		if (apply_damage(attacker, victim, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : condition_list)
					victim->addCondition(cond->clone());
			}
		}
	}

	void Combat::strike_target(const CreaturePtr& attacker, const CreaturePtr& defender, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type == DamageType::Healing)
		{
			heal_target(attacker, defender, skip_validation, spectators);
			return;
		}

		using namespace BlackTek;
		auto switch_mask = (static_cast<uint32_t>(attacker->getCreatureSubType()) << 16 | static_cast<uint32_t>(defender->getCreatureSubType()) << 8);

		switch (switch_mask)
		{
			case Constant::Player_Vs_Player:	strike_target(PlayerCast(attacker), PlayerCast(defender), skip_validation, spectators);		break;
			case Constant::Player_Vs_Monster:	strike_target(PlayerCast(attacker), MonsterCast(defender), skip_validation, spectators);	break;
			case Constant::Monster_Vs_Player:	strike_target(MonsterCast(attacker), PlayerCast(defender), skip_validation, spectators);	break;
			case Constant::Monster_Vs_Monster:	strike_target(MonsterCast(attacker), MonsterCast(defender), skip_validation, spectators);	break;
			default: [[unlikely]]
				break;
		}
	}

	void Combat::apply_healing_modifiers(const PlayerPtr& caster, const auto& target)
	{
		if (not caster->hasHealingModifiers())
			return;

		const auto heal_type =
			config.test(Config::HealthTarget)   ? DamageModifier::AttackType::Regeneration  :
			config.test(Config::ManaTarget)     ? DamageModifier::AttackType::Attunement    :
			config.test(Config::StaminaTarget)  ? DamageModifier::AttackType::Vigor         :
			config.test(Config::SoulTarget)     ? DamageModifier::AttackType::Transcendence :
			                                      DamageModifier::AttackType::None;

		if (heal_type == DamageModifier::AttackType::None)
			return;

		const auto& main_sum    = caster->getMainHealingModSum(heal_type);
		const auto target_type  = target->getType();
		const auto target_race  = target->getRace();
		const auto& target_name = target->getName();
		const auto type_val     = std::to_underlying(heal_type);

		auto applied = [&](const auto& modifier)
		{
			return modifier.getType() == type_val
				and modifier.applies(damage_type, target_type, origin, target_race, target_name);
		};

		auto percent_boost = main_sum.percent;
		auto flat_boost    = main_sum.flat;

		if (caster->hasFilteredHealingMods())
		{
			auto fv = caster->getFilteredHealingMods() | std::views::filter(applied);
			for (const auto& modifier : fv)
			{
				percent_boost += modifier.isFlatValue() ? 0 : modifier.getValue();
				flat_boost    += modifier.isFlatValue() ? modifier.getValue() : 0;
			}
		}

		if (caster->hasNamedHealingMods())
		{
			auto fv = caster->getNamedHealingMods() | std::views::filter(applied);
			for (const auto& modifier : fv)
			{
				percent_boost += modifier.isFlatValue() ? 0 : modifier.getValue();
				flat_boost    += modifier.isFlatValue() ? modifier.getValue() : 0;
			}
		}

		if (percent_boost) damage += damage * percent_boost / 100;
		damage += flat_boost;
	}

	void Combat::heal_target(const CreaturePtr& caster, const CreaturePtr& target, bool skip_validation, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
		if (damage_type != DamageType::Healing or damage == 0)
			return;

		if (config.test(Config::HealthTarget))
		{
			const int32_t maxHeal = target->getMaxHealth() - target->getHealth();

			if (maxHeal <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), target);

			const uint32_t healed = std::min<uint32_t>(damage, static_cast<uint32_t>(maxHeal));
			target->changeHealth(healed);
			heal_notification(caster, target, healed, spectators);
		}
		else if (config.test(Config::ManaTarget))
		{
			const auto player = std::dynamic_pointer_cast<Player>(target);

			if (not player)
				return;

			const int32_t maxMana = player->getMaxMana() - player->getMana();

			if (maxMana <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), player);

			const uint32_t healed = std::min<uint32_t>(damage, static_cast<uint32_t>(maxMana));
			player->changeMana(static_cast<int32_t>(healed));
			heal_notification(caster, player, healed, spectators);
		}
		else if (config.test(Config::StaminaTarget))
		{
			const auto player = std::dynamic_pointer_cast<Player>(target);

			if (not player)
				return;

			constexpr int32_t maxStaminaMinutes = 2520;
			const int32_t remaining = maxStaminaMinutes - static_cast<int32_t>(player->getStaminaMinutes());

			if (remaining <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), player);

			const uint32_t healed = std::min<uint32_t>(damage, static_cast<uint32_t>(remaining));
			player->changeStamina(static_cast<int32_t>(healed));
			heal_notification(caster, player, healed, spectators);
		}
		else if (config.test(Config::SoulTarget))
		{
			const auto player = std::dynamic_pointer_cast<Player>(target);

			if (not player)
				return;

			const int32_t maxSoul = static_cast<int32_t>(player->getVocation()->getSoulMax()) - static_cast<int32_t>(player->getSoul());

			if (maxSoul <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), player);

			const uint32_t healed = std::min<uint32_t>(damage, static_cast<uint32_t>(maxSoul));
			player->changeSoul(static_cast<int32_t>(healed));
			heal_notification(caster, player, healed, spectators);
		}
	}

	void Combat::execute(const CreaturePtr& caster, const Position& center) noexcept
	{
		const auto& [positions, distance] = getAreaPositions(caster->getPosition(), center);

		if (positions.empty())
			return;

		static constexpr size_t MAX_TILES = 256;
		const size_t n = std::min(positions.size(), MAX_TILES);

		// Compute the furthest tile's reach from center so spectators at the area edge are covered.
		int32_t area_extent = distance;
		for (size_t k = 0; k < n; ++k)
		{
			area_extent = std::max(area_extent, std::abs(static_cast<int32_t>(positions[k].x) - static_cast<int32_t>(center.x)));
			area_extent = std::max(area_extent, std::abs(static_cast<int32_t>(positions[k].y) - static_cast<int32_t>(center.y)));
		}

		auto spectators = g_game.map.fetchSpectators(center, true, true,
			area_extent + Map::maxViewportX, area_extent + Map::maxViewportX,
			area_extent + Map::maxViewportY, area_extent + Map::maxViewportY);

		if (caster and (distanceEffect != CONST_ANI_NONE))
			addDistanceEffect(caster, caster->getPosition(), center, distanceEffect);

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

#ifdef __AVX2__
		{
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
		}
#else
		for (size_t i = 0; i < n; ++i)
			valid_mask[i] &= ((packed_flags[i] & flag_reject) == 0u) ? 0xFF : 0x00;
#endif

		std::vector<CreaturePtr> toDamageCreatures;
		toDamageCreatures.reserve(n * 2);

		std::vector<TilePtr> valid_tile_list;
		valid_tile_list.reserve(n);

		auto valid_tiles = std::views::iota(size_t{0}, n) | std::views::filter([&](size_t idx) { return static_cast<bool>(valid_mask[idx]); })
			| std::views::filter([&](size_t idx) {
				// IgnoreBarriers bypasses line-of-sight walls for area combat.
				return config.test(Config::IgnoreBarriers) or not tile_cache[idx]->hasProperty(CONST_PROP_BLOCKPROJECTILE);
			});

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
				// Tile-top filter: when TopTargetOnly is set, only the top creature per tile (or caster on their own tile).
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::TopTargetOnly))
						return true;
					return onCasterTile ? (creature == caster) : (creature == topCreature);
				})
				// SelfOnly: only the caster themselves is a valid target (used by self-cast spells/heals).
				// When set it overrides the aggressive self-exclusion below.
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::SelfOnly))
						return true;
					return creature == caster;
				})
				// Aggressive self-exclusion: aggressive combats skip the caster unless SelfOnly is set.
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::Aggressive) or config.test(Config::SelfOnly))
						return true;
					return caster != creature;
				})
				// Standard target-validity check for aggressive combats.
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
				})
				// FriendlyParty: only hit creatures that share the caster's party (or the caster themselves).
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::FriendlyParty))
						return true;
					if (creature == caster)
						return true;
					const auto casterPlayer = caster->getPlayer();
					if (not casterPlayer)
						return true; // non-player casters have no party concept
					const auto targetPlayer = creature->getPlayer();
					if (not targetPlayer)
						return false; // party filter only applies to players
					return casterPlayer->isPartner(targetPlayer);
				})
				// EnemyParty: skip creatures that are in the caster's party; always skips the caster.
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::EnemyParty))
						return true;
					if (creature == caster)
						return false;
					const auto casterPlayer = caster->getPlayer();
					if (not casterPlayer)
						return true;
					const auto targetPlayer = creature->getPlayer();
					if (not targetPlayer)
						return true; // monsters are always enemies
					return not casterPlayer->isPartner(targetPlayer);
				})
				// FraggedOnly: only target players that currently carry a skull (i.e., have killed others).
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not config.test(Config::FraggedOnly))
						return true;
					const auto targetPlayer = creature->getPlayer();
					if (not targetPlayer)
						return false; // FraggedOnly only applies to players
					return targetPlayer->getSkull() != SKULL_NONE;
				});

			for (const auto& creature : strikeable)
				toDamageCreatures.push_back(creature);
		}

		apply_effects(spectators, caster, valid_tile_list);

		const std::span<const CreaturePtr> spec_span(spectators.begin(), spectators.size());
		for (const auto& target_creature : toDamageCreatures)
		{
			auto single_combat = clone();
			single_combat->strike_target(caster, target_creature, false, spec_span);
		}
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

	void Combat::postCombatEffects(const CreaturePtr& caster, const Position& pos, const Combat& combat)
	{
		if (combat.impactEffect != CONST_ME_NONE)
			g_game.addMagicEffect(pos, combat.impactEffect);
	}

	void Combat::addDistanceEffect(const CreaturePtr& caster, const Position& fromPos, const Position& toPos, uint8_t effect)
	{
		if (effect == CONST_ANI_NONE)
			return;
		g_game.addDistanceEffect(fromPos, toPos, effect);
	}

	void Combat::defense_block_effect(const Position& target_position) const noexcept
	{
		g_game.addMagicEffect(target_position, CONST_ME_POFF);
	}

	void Combat::armor_block_effect(const Position& target_position) const noexcept
	{
		g_game.addMagicEffect(target_position, CONST_ME_BLOCKHIT);
	}

	uint8_t Combat::immunity_block_effect() const noexcept
	{
		switch (damage_type)
		{
			case DamageType::Undefined:		return CONST_ME_NONE;
			case DamageType::Energy:		[[fallthrough]];
			case DamageType::Fire:			[[fallthrough]];
			case DamageType::Physical:		[[fallthrough]];
			case DamageType::Ice:			[[fallthrough]];
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

		const int64_t fkey = formula_key();

		const FormulaCallbacks* callbacks = nullptr;
		{
			auto cbIt = combat_callback_map.find(fkey);
			if (cbIt != combat_callback_map.end())
				callbacks = &cbIt->second;
		}

		const SituationFormulas* formulas = nullptr;
		if (config.test(formula_flags[sit_idx]))
		{
			auto fIt = combat_formula_map.find(fkey);
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

		const int64_t fkey = formula_key();

		const FormulaCallbacks* callbacks = nullptr;
		{
			auto cbIt = combat_callback_map.find(fkey);
			if (cbIt != combat_callback_map.end())
				callbacks = &cbIt->second;
		}

		const SituationFormulas* formulas = nullptr;

		if (config.test(formula_flags[sit_idx]))
		{
			auto fIt = combat_formula_map.find(fkey);
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
		const uint32_t combat_key = (static_cast<uint32_t>(damage_type) << 8) | (damage_type == DamageType::Physical ? static_cast<uint8_t>(target->getRace()) : 0);

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

	uint32_t Combat::collect_heal_notice_data() const noexcept
	{
		if (config.test(Config::HealthTarget))  return PackHealNotice(TEXTCOLOR_LIGHTGREEN, CONST_ME_MAGIC_GREEN, Config::HealthTarget);
		if (config.test(Config::ManaTarget))    return PackHealNotice(TEXTCOLOR_BLUE,       CONST_ME_MAGIC_BLUE,  Config::ManaTarget);
		if (config.test(Config::StaminaTarget)) return PackHealNotice(TEXTCOLOR_ORANGE,     CONST_ME_MAGIC_GREEN, Config::StaminaTarget);
		if (config.test(Config::SoulTarget))    return PackHealNotice(TEXTCOLOR_PURPLE,       CONST_ME_MAGIC_GREEN, Config::SoulTarget);
		return 0;
	}

	constexpr Combat::OriginNotice Combat::collect_origin_notice(Origin o) noexcept
	{
		switch (o)
		{
			case Origin::Condition:    return { CONST_ME_NONE,         "a condition"             };
			case Origin::Spell:        return { CONST_ME_NONE,         "a spell"                 };
			case Origin::Melee:        return { CONST_ME_NONE,         "a melee attack"          };
			case Origin::Ranged:       return { CONST_ME_NONE,         "a ranged attack"         };
			case Origin::Fist:         return { CONST_ME_NONE,         "a fist strike"           };
			case Origin::Sword:        return { CONST_ME_NONE,         "a sword strike"          };
			case Origin::Axe:          return { CONST_ME_NONE,         "an axe blow"             };
			case Origin::Club:         return { CONST_ME_NONE,         "a club blow"             };
			case Origin::Wand:         return { CONST_ME_NONE,         "a wand attack"           };
			case Origin::Rod:          return { CONST_ME_NONE,         "a rod attack"            };
			case Origin::Bow:          return { CONST_ME_NONE,         "a bow shot"              };
			case Origin::Crossbow:     return { CONST_ME_NONE,         "a crossbow bolt"         };
			case Origin::Throwable:    return { CONST_ME_NONE,         "a thrown weapon"         };
			case Origin::Augment:      return { CONST_ME_NONE,         "an augmented attack"     };
			case Origin::Reflect:      return { CONST_ME_MAGIC_RED,    "a reflected attack"      };
			case Origin::Deflect:      return { CONST_ME_MAGIC_BLUE,   "a deflected attack"      };
			case Origin::Ricochet:     return { CONST_ME_MAGIC_BLUE,   "a ricocheting attack"    };
			case Origin::Piercing:     return { CONST_ME_PURPLESMOKE,  "a piercing blow"         };
			case Origin::LifeSteal:    return { CONST_ME_MAGIC_RED,    "a life drain"            };
			case Origin::ManaSteal:    return { CONST_ME_MAGIC_BLUE,   "a mana drain"            };
			case Origin::StaminaSteal: return { CONST_ME_MAGIC_GREEN,  "a stamina drain"         };
			case Origin::SoulSteal:    return { CONST_ME_MAGIC_GREEN,  "a soul drain"            };
			case Origin::Absorb:       return { CONST_ME_MAGIC_GREEN,  "absorption"              };
			case Origin::Restore:      return { CONST_ME_MAGIC_BLUE,   "mana restoration"        };
			case Origin::Replenish:    return { CONST_ME_MAGIC_GREEN,  "stamina replenishment"   };
			case Origin::Revive:       return { CONST_ME_MAGIC_GREEN,  "soul revival"            };
			default:                   return {};
		}
	}

	void Combat::heal_notification(const CreaturePtr& caster, const CreaturePtr& target, uint32_t amount, std::optional<std::span<const CreaturePtr>> spectators) const noexcept
	{
		if (amount == 0)
			return;

		const auto notice = UnPackHealNotice(collect_heal_notice_data());
		if (notice.stat_name.empty()) [[unlikely]]
			return;

		const bool self_target       = (caster == target);
		const auto target_position   = target->getPosition();
		const auto& target_name      = target->getName();
		const auto& caster_name      = caster->getNameDescription();
		const auto origin_notice      = collect_origin_notice(static_cast<Origin>(origin));
		const std::string amount_str  = std::to_string(amount);
		const std::string stat_str    = std::string(notice.stat_name);
		const std::string origin_verb = std::string(origin_notice.verb);

		SpectatorVec owned;
		if (not spectators) {
			owned = g_game.map.fetchSpectators(target_position, true, true);
			spectators.emplace(owned.begin(), owned.end());
		}

		auto is_observer = [&](const auto& spectator) { return spectator != target and spectator != caster and spectator->is_player(); };

		const bool is_defense_conversion = (origin == Origin::Absorb   or origin == Origin::Restore
		                                 or origin == Origin::Replenish or origin == Origin::Revive);
		const bool is_drain_gain         = (origin == Origin::LifeSteal   or origin == Origin::ManaSteal
		                                 or origin == Origin::StaminaSteal or origin == Origin::SoulSteal);

		const std::string target_text =
		    is_defense_conversion ? "Your " + origin_verb + " restored " + amount_str + " " + stat_str + "."
		  : is_drain_gain         ? "You drained " + amount_str + " " + stat_str + " from your target."
		  : self_target           ? "You heal yourself for " + amount_str + " " + stat_str + "."
		                          : "You are healed for " + amount_str + " " + stat_str + " by " + origin_verb + ".";

		const std::string caster_text =
		    is_defense_conversion ? target_name + "'s " + origin_verb + " restored " + amount_str + " " + stat_str + " from your attack."
		  : is_drain_gain         ? "You drained " + amount_str + " " + stat_str + " from " + target_name + "."
		                          : target_name + " is healed for " + amount_str + " " + stat_str + " by your " + origin_verb + ".";

		const std::string observer_text =
		    is_defense_conversion ? target_name + "'s " + origin_verb + " restored " + amount_str + " " + stat_str + "."
		  : is_drain_gain         ? caster_name + " drained " + amount_str + " " + stat_str + "."
		                          : target_name + " is healed for " + amount_str + " " + stat_str + " by " + origin_verb + ".";

		TextMessage observer_message = {};
		observer_message.position      = target_position;
		observer_message.primary.color = notice.color;
		observer_message.primary.value = amount;
		observer_message.type          = MESSAGE_HEALED_OTHERS;
		observer_message.text          = observer_text;

		if (target->is_player())
		{
			TextMessage target_message = {};
			target_message.position      = target_position;
			target_message.primary.color = notice.color;
			target_message.primary.value = amount;
			target_message.type          = MESSAGE_HEALED;
			target_message.text          = target_text;

			auto* player = static_cast<Player*>(target.get());
			player->sendTextMessage(target_message);
			if (notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(target_position, notice.effect);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(target_position, origin_notice.effect);
			player->sendStats();
		}

		if (not self_target and caster->is_player())
		{
			TextMessage caster_message = {};
			caster_message.position      = target_position;
			caster_message.primary.color = notice.color;
			caster_message.primary.value = amount;
			caster_message.type          = MESSAGE_HEALED_OTHERS;
			caster_message.text          = caster_text;

			auto* player = static_cast<Player*>(caster.get());
			player->sendTextMessage(caster_message);
			if (notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(target_position, notice.effect);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(target_position, origin_notice.effect);
		}

		auto observer_view = *spectators | std::views::filter(is_observer);
		for (const auto& spectator : observer_view)
		{
			auto* player = static_cast<Player*>(spectator.get());
			player->sendTextMessage(observer_message);
			if (notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(target_position, notice.effect);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(target_position, origin_notice.effect);
		}
	}

	void Combat::manadamage_notification(const CreaturePtr& attacker, const CreaturePtr& defender, uint32_t amount, std::optional<std::span<const CreaturePtr>> spectators) const noexcept
	{
		if (amount == 0)
			return;

		const bool self_target        = (attacker == defender);
		const auto defender_position  = defender->getPosition();
		const auto& defender_name     = defender->getName();
		const auto& attacker_name     = attacker->getNameDescription();
		const auto origin_notice       = collect_origin_notice(static_cast<Origin>(origin));
		const std::string amount_str   = std::to_string(amount);
		const std::string origin_verb  = std::string(origin_notice.verb);

		SpectatorVec owned;
		if (not spectators) {
			owned = g_game.map.fetchSpectators(defender_position, true, true);
			spectators.emplace(owned.begin(), owned.end());
		}

		auto is_observer = [&](const auto& spectator) { return spectator != defender and spectator != attacker and spectator->is_player(); };

		const bool is_drain = (origin == Origin::ManaSteal);

		const std::string defender_text =
		    is_drain    ? attacker_name + " drained " + amount_str + " mana from you."
		  : self_target ? "You lose " + amount_str + " mana due to your own " + origin_verb + "."
		                : "You lose " + amount_str + " mana due to " + origin_verb + " by " + attacker_name + ".";

		const std::string attacker_text =
		    is_drain ? "You drained " + amount_str + " mana from " + defender_name + "."
		             : defender_name + " loses " + amount_str + " mana due to your " + origin_verb + ".";

		const std::string observer_text =
		    is_drain ? attacker_name + " drained " + amount_str + " mana from " + defender_name + "."
		             : defender_name + " loses " + amount_str + " mana due to " + origin_verb + " by " + attacker_name + ".";

		TextMessage observer_message = {};
		observer_message.position      = defender_position;
		observer_message.primary.color = TEXTCOLOR_BLUE;
		observer_message.primary.value = amount;
		observer_message.type          = MESSAGE_DAMAGE_OTHERS;
		observer_message.text          = observer_text;

		if (defender->is_player())
		{
			TextMessage defender_message = {};
			defender_message.position      = defender_position;
			defender_message.primary.color = TEXTCOLOR_BLUE;
			defender_message.primary.value = amount;
			defender_message.type          = MESSAGE_DAMAGE_RECEIVED;
			defender_message.text          = defender_text;

			auto* player = static_cast<Player*>(defender.get());
			player->sendTextMessage(defender_message);
			player->sendMagicEffect(defender_position, CONST_ME_LOSEENERGY);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, origin_notice.effect);
		}

		if (not self_target and attacker->is_player())
		{
			TextMessage attacker_message = {};
			attacker_message.position      = defender_position;
			attacker_message.primary.color = TEXTCOLOR_BLUE;
			attacker_message.primary.value = amount;
			attacker_message.type          = MESSAGE_DAMAGE_DEALT;
			attacker_message.text          = attacker_text;

			auto* player = static_cast<Player*>(attacker.get());
			player->sendTextMessage(attacker_message);
			player->sendMagicEffect(defender_position, CONST_ME_LOSEENERGY);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, origin_notice.effect);
		}

		auto observer_view = *spectators | std::views::filter(is_observer);
		for (const auto& spectator : observer_view)
		{
			auto* player = static_cast<Player*>(spectator.get());
			player->sendTextMessage(observer_message);
			player->sendMagicEffect(defender_position, CONST_ME_LOSEENERGY);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, origin_notice.effect);
		}
	}

	void Combat::damage_notification(const CreaturePtr& attacker, const CreaturePtr& defender, uint32_t amount, std::optional<std::span<const CreaturePtr>> spectators) const noexcept
	{
		if (amount == 0)
			return;

		const bool self_target        = (attacker == defender);
		const auto defender_position  = defender->getPosition();
		const auto& defender_name     = defender->getName();
		const auto& attacker_name     = attacker->getNameDescription();
		const auto notice              = UnPackNotice(collect_notice_data(defender));
		const auto origin_notice       = collect_origin_notice(static_cast<Origin>(origin));
		const std::string amount_str   = std::to_string(amount);
		const std::string origin_verb  = std::string(origin_notice.verb);

		SpectatorVec owned;
		if (not spectators) {
			owned = g_game.map.fetchSpectators(defender_position, true, true);
			spectators.emplace(owned.begin(), owned.end());
		}

		auto is_observer = [&](const auto& spectator) { return spectator != defender and spectator != attacker and spectator->is_player(); };

		const bool is_reflect  = (origin == Origin::Reflect);
		const bool is_deflect  = (origin == Origin::Deflect);
		const bool is_ricochet = (origin == Origin::Ricochet);
		const bool is_piercing = (origin == Origin::Piercing);
		const bool is_drain    = (origin == Origin::LifeSteal   or origin == Origin::ManaSteal
		                       or origin == Origin::StaminaSteal or origin == Origin::SoulSteal);

		// attacker = source of the resistance for reflect/deflect; defender = recipient of secondary damage
		const std::string defender_text =
		    is_reflect  ? (self_target
		                    ? "You reflected " + amount_str + " damage from your own attack."
		                    : "You are struck by " + amount_str + " reflected damage from " + attacker_name + "'s resistance.")
		  : is_deflect  ? "You are hit by " + amount_str + " deflected damage from " + attacker_name + "'s resistance."
		  : is_ricochet ? "You are hit by " + amount_str + " ricocheting damage."
		  : is_piercing ? "You are struck by " + amount_str + " piercing damage from " + attacker_name + ", bypassing your defenses."
		  : is_drain    ? attacker_name + " drained " + amount_str + " health from you via " + origin_verb + "."
		  : self_target ? "You lose " + amount_str + " health due to your own " + origin_verb + "."
		                : "You lose " + amount_str + " health due to " + origin_verb + " by " + attacker_name + ".";

		const std::string attacker_text =
		    is_reflect  ? "Your resistance reflected " + amount_str + " damage to " + defender_name + "."
		  : is_deflect  ? "Your resistance deflected " + amount_str + " damage to " + defender_name + "."
		  : is_ricochet ? "Your attack ricocheted for " + amount_str + " damage."
		  : is_piercing ? "Your attack pierced through " + defender_name + "'s defenses for " + amount_str + " damage."
		  : is_drain    ? "You drained " + amount_str + " health from " + defender_name + " via " + origin_verb + "."
		                : defender_name + " loses " + amount_str + " health due to your " + origin_verb + ".";

		const std::string observer_text =
		    is_reflect  ? attacker_name + "'s resistance reflected " + amount_str + " damage to " + defender_name + "."
		  : is_deflect  ? attacker_name + "'s resistance deflected " + amount_str + " damage to " + defender_name + "."
		  : is_ricochet ? "A ricocheting attack struck " + defender_name + " for " + amount_str + " damage."
		  : is_piercing ? attacker_name + "'s attack pierced through " + defender_name + "'s defenses for " + amount_str + " damage."
		  : is_drain    ? attacker_name + " drained " + amount_str + " health from " + defender_name + "."
		                : defender_name + " loses " + amount_str + " health due to " + origin_verb + " by " + attacker_name + ".";

		TextMessage observer_message = {};
		observer_message.position      = defender_position;
		observer_message.primary.color = notice.color;
		observer_message.primary.value = amount;
		observer_message.type          = MESSAGE_DAMAGE_OTHERS;
		observer_message.text          = observer_text;

		if (defender->is_player())
		{
			TextMessage defender_message = {};
			defender_message.position      = defender_position;
			defender_message.primary.color = notice.color;
			defender_message.primary.value = amount;
			defender_message.type          = MESSAGE_DAMAGE_RECEIVED;
			defender_message.text          = defender_text;

			auto* player = static_cast<Player*>(defender.get());
			player->sendTextMessage(defender_message);
			if (notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, notice.effect);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, origin_notice.effect);
			player->sendStats();
		}

		if (not self_target and attacker->is_player())
		{
			TextMessage attacker_message = {};
			attacker_message.position      = defender_position;
			attacker_message.primary.color = notice.color;
			attacker_message.primary.value = amount;
			attacker_message.type          = MESSAGE_DAMAGE_DEALT;
			attacker_message.text          = attacker_text;

			auto* player = static_cast<Player*>(attacker.get());
			player->sendTextMessage(attacker_message);
			if (notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, notice.effect);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, origin_notice.effect);
			player->sendCreatureHealth(defender);
		}

		auto observer_view = *spectators | std::views::filter(is_observer);
		for (const auto& spectator : observer_view)
		{
			auto* player = static_cast<Player*>(spectator.get());
			player->sendTextMessage(observer_message);
			if (notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, notice.effect);
			if (origin_notice.effect != CONST_ME_NONE)
				player->sendMagicEffect(defender_position, origin_notice.effect);
			player->sendCreatureHealth(defender);
		}

		if (notice.fluid != FLUID_NONE)
		{
			TilePtr tile       = g_game.map.getTile(defender_position);
			CylinderPtr c_tile = tile;
			auto fluid = Item::CreateItem(ITEM_SMALLSPLASH, notice.fluid);
			if (fluid) [[likely]]
			{
				g_game.internalAddItem(c_tile, fluid, INDEX_WHEREEVER, FLAG_NOLIMIT);
				g_game.startDecay(fluid);
			}
		}
	}

	uint32_t Combat::apply_damage(const CreaturePtr& attacker, const PlayerPtr& target, std::optional<std::span<const CreaturePtr>> pre_cache) const noexcept
	{
		const auto target_mana   = target->getMana();
		const auto target_health = static_cast<uint32_t>(target->getHealth());
		const auto manashield    = target->hasCondition(CONDITION_MANASHIELD);
		const auto manadrain     = damage_type == Combat::DamageType::ManaDrain;
		auto damage_limit        = (manashield or manadrain) ? (manadrain ? target_mana : target_mana + target_health) : target_health;
		uint32_t damage_dealt    = 0;
		SpectatorVec owned_spectators;

		if (manadrain or manashield)
		{
			damage_dealt = std::min(damage, target_mana);
			target->drainMana(attacker, damage_dealt);

			if (not pre_cache) {
				owned_spectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
				pre_cache.emplace(owned_spectators.begin(), owned_spectators.end());
			}

			manadamage_notification(attacker, target, damage_dealt, pre_cache);

			if (manadrain or damage_dealt == damage)
				return damage_dealt;
		}

		damage_limit -= damage_dealt;

		if (damage > damage_dealt)
		{
			const auto health_changed = std::min(damage - damage_dealt, damage_limit);

			// Fire PREPAREDEATH before any drain so the creature is still alive during the event.
			// If any handler vetoes the death, cancel the damage entirely.
			if (health_changed == target_health)
			{
				for (const auto& creatureEvent : target->getCreatureEvents(CREATURE_EVENT_PREPAREDEATH))
					if (not creatureEvent->executeOnPrepareDeath(target, attacker))
						return 0;
			}

			damage_dealt += health_changed;
			target->drainHealth(attacker, health_changed);

			if (not pre_cache) {
				owned_spectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
				pre_cache.emplace(owned_spectators.begin(), owned_spectators.end());
			}

			damage_notification(attacker, target, health_changed, pre_cache);
		}

		return damage_dealt;
	}

	uint32_t Combat::apply_damage(const CreaturePtr& attacker, const MonsterPtr& target, const std::optional<std::span<const CreaturePtr>> spectators) const noexcept
	{
		if (damage_type == Combat::DamageType::ManaDrain)
			return 0;

		const auto target_health  = static_cast<uint32_t>(target->getHealth());
		const auto target_position = target->getPosition();
		const auto health_changed  = std::min(damage, target_health);

		if (health_changed == 0)
			return 0;

		// Fire PREPAREDEATH before any drain so the creature is still alive during the event.
		// If any handler vetoes the death, cancel the damage entirely.
		if (health_changed == target_health)
		{
			for (const auto& creatureEvent : target->getCreatureEvents(CREATURE_EVENT_PREPAREDEATH))
				if (not creatureEvent->executeOnPrepareDeath(target, attacker))
					return 0;
		}

		target->drainHealth(attacker, health_changed);

		auto pre_cache = spectators;
		SpectatorVec owned;
		if (not pre_cache) {
			owned = g_game.map.fetchSpectators(target_position, true, true);
			pre_cache.emplace(owned.begin(), owned.end());
		}

		damage_notification(attacker, target, health_changed, pre_cache);

		return health_changed;
	}
}

void BlackTek::MagicField::onStepInField(const CreaturePtr& creature)
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

		creature->addCondition(std::move(conditionCopy));
	}
}