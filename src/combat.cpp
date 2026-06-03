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
#include "spells.h"

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
	// Formula maps defined BEFORE g_combat_registry so they are destroyed AFTER it.
	// Combat destructors (triggered by registry teardown) still reference these maps,
	// so they must remain alive for the full lifetime of the registry.
	// new_delete_resource() avoids any dependency on the registry allocator.
	std::array<SituationFormulas, SituationFormulas::Total> g_default_situation_formulas = {};
	// Defined before g_combat_registry so it outlives the registry (destructors reference it).
	std::unordered_map<int64_t, std::unique_ptr<CombatFormulaCache>> combat_formula_cache_map;

	static int64_t lua_formula_id_counter_{ -2 };

    CombatRegistry g_combat_registry;

	std::pmr::unordered_map<int64_t, CombatAreaPair> combat_area_pair_map{ BlackTek::g_combat_registry.Allocator() };

	void intrusive_ptr_release(const Combat* p) noexcept
	{
		if (--p->ref_count == 0)
		{
			BlackTek::g_combat_registry.Release(p->combat_id);
		}
	}

	CombatHandle CombatRegistry::Create()
	{
		const int64_t newId = next_id_++;
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

	Combat::~Combat()
	{
		if (combat_id == -1)
			return;

		combat_formula_cache_map.erase(combat_id);
	}

	// ── Formula override methods ──────────────────────────────────────────────

	int64_t Combat::ensureFormulaId() noexcept
	{
		if (combat_id == -1)
			combat_id = lua_formula_id_counter_--;
		return combat_id;
	}

	void Combat::SetSituationFormulas(uint8_t sit_idx, SituationFormulas&& formulas) noexcept
	{
		if (sit_idx >= 4)
			return;

		const int64_t fid = ensureFormulaId();

		if (not formula_cache)
		{
			auto [it, ok] = combat_formula_cache_map.emplace(fid, std::make_unique<CombatFormulaCache>());
			formula_cache = it->second.get();
			config.set(Config::HasFormulaCache);
		}

		formula_cache->situations[sit_idx] = std::move(formulas);

		config.set(k_formula_flags[sit_idx]);
	}

	void Combat::RegisterCompiledFormula(uint8_t sit_idx, FormulaStage stage, CompiledFormula fn) noexcept
	{
		if (sit_idx >= 4 or not fn)
			return;

		if (not formula_cache)
		{
			const int64_t fid = ensureFormulaId();
			auto [it, ok] = combat_formula_cache_map.emplace(fid, std::make_unique<CombatFormulaCache>());
			formula_cache = it->second.get();
			config.set(Config::HasFormulaCache);
		}

		if (not formula_cache->compiled)
		{
			formula_cache->compiled = std::make_unique<CompiledFormulaSlots>();
			config.set(Config::HasCompiledFormula);
		}

		formula_cache->compiled->set(sit_idx, static_cast<uint8_t>(stage), std::move(fn));
	}

	// ── BindKey resolution ────────────────────────────────────────────────────
	// Maps a BindKey to a concrete integer stat read directly from a C++ creature.
	// Called exclusively by FormulaNode closures at combat execution time.

	int32_t resolve_bind_key(Combat::BindKey key, const CreaturePtr& creature) noexcept
	{
		if (not creature)
			return 0;

		const bool is_player = creature->is_player();
		const PlayerPtr player = is_player ? creature->getPlayer() : nullptr;

		switch (key)
		{
			case Combat::BindKey::Level:
				return player ? static_cast<int32_t>(player->getLevel()) : 1;

			case Combat::BindKey::MagicLevel:
				return player ? static_cast<int32_t>(player->getMagicLevel()) : 0;

			case Combat::BindKey::SkillLevel:
				return player ? static_cast<int32_t>(player->getSkillLevel(SKILL_FIST)) : 0;

			case Combat::BindKey::Attack:
			{
				if (not player)
					return 0;
				const auto weapon = player->getWeapon(CONST_SLOT_RIGHT, false);
				return weapon ? static_cast<int32_t>(weapon->getAttack()) : static_cast<int32_t>(player->getSkillLevel(SKILL_FIST));
			}

			case Combat::BindKey::Defense:
				return static_cast<int32_t>(creature->getDefense());

			case Combat::BindKey::WeaponAttack:
			{
				if (not player)
					return 0;
				const auto weapon = player->getWeapon(CONST_SLOT_RIGHT, false);
				return weapon ? static_cast<int32_t>(weapon->getAttack()) : 0;
			}

			case Combat::BindKey::WeaponDefense:
			{
				if (not player)
					return static_cast<int32_t>(creature->getArmor());
				const auto shield = player->getWeapon(CONST_SLOT_LEFT, false);
				return shield ? static_cast<int32_t>(shield->getArmor()) : 0;
			}

			case Combat::BindKey::WeaponSkill:
			{
				if (not player)
					return 0;
				const auto weapon = player->getWeapon(CONST_SLOT_RIGHT, false);
				if (not weapon)
					return static_cast<int32_t>(player->getSkillLevel(SKILL_FIST));
				switch (weapon->getWeaponType())
				{
					case WEAPON_SWORD:    return static_cast<int32_t>(player->getSkillLevel(SKILL_SWORD));
					case WEAPON_AXE:      return static_cast<int32_t>(player->getSkillLevel(SKILL_AXE));
					case WEAPON_CLUB:     return static_cast<int32_t>(player->getSkillLevel(SKILL_CLUB));
					case WEAPON_DISTANCE: return static_cast<int32_t>(player->getSkillLevel(SKILL_DISTANCE));
					default:              return static_cast<int32_t>(player->getSkillLevel(SKILL_FIST));
				}
			}

			case Combat::BindKey::Health:
				return static_cast<int32_t>(creature->getHealth());

			case Combat::BindKey::MaxHealth:
				return static_cast<int32_t>(creature->getMaxHealth());

			case Combat::BindKey::Mana:
				return player ? static_cast<int32_t>(player->getMana()) : 0;

			case Combat::BindKey::MaxMana:
				return player ? static_cast<int32_t>(player->getMaxMana()) : 0;

			case Combat::BindKey::Soul:
				return player ? static_cast<int32_t>(player->getSoul()) : 0;

			case Combat::BindKey::MaxSoul:
				return player ? static_cast<int32_t>(player->getVocation()->getSoulMax()) : 0;

			case Combat::BindKey::Stamina:
				return player ? static_cast<int32_t>(player->getStaminaMinutes()) : 0;

			default: [[unlikely]]
				return 0;
		}
	}

	void Combat::AddCondition(ConditionHandle cond)
	{
		if (cond)
		{
			if (not condition_list)
				condition_list = std::make_unique<std::vector<ConditionHandle>>();
			condition_list->push_back(std::move(cond));
			config.set(Config::HasCondition);
		}
	}

	void Combat::ClearConditions() noexcept
	{
		condition_list.reset();
		config.set(Config::HasCondition, false);
	}

	void ApplyOutputPreset(Combat::OutputFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Classic")		out = Combat::ClassicOutput;
		else if (preset == "Scaled")		out = Combat::ScaledOutput;
		else if (preset == "Balanced")		out = Combat::BalancedOutput;
		else if (preset == "Absorption")	out = Combat::AbsorptionOutput;
		else if (preset == "Tabletop")		out = Combat::TabletopOutput;
		else if (preset == "Exponential")	out = Combat::ExponentialOutput;
	}

	void ApplyDefensePreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Classic")		out = Combat::ClassicDefense;
		else if (preset == "Scaled")		out = Combat::ScaledResistance;
		else if (preset == "Balanced")		out = Combat::BalancedResistance;
		else if (preset == "Absorption")	out = Combat::AbsorptionResistance;
	}

	void ApplyArmorPreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Classic")		out = Combat::ClassicArmor;
		else if (preset == "Absorption")	out = Combat::AbsorptionResistance;
	}

	void ApplyResolutionPreset(Combat::ResolutionFactors& out, std::string_view preset) noexcept
	{
		if      (preset == "Classic")       out = Combat::ClassicResolution;
		else if (preset == "Scaled")        out = Combat::ScaledResolution;
		else if (preset == "Balanced")      out = Combat::BalancedResolution;
		else if (preset == "Absorption")    out = Combat::AbsorptionResolution;
		else if (preset == "Proportional")  out = Combat::ProportionalResolution;
		else if (preset == "Gradual")       out = Combat::GradualResolution;
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

	thread_local std::vector<TilePtr>     area_tile_buffer;
	thread_local std::vector<Position>    area_position_buffer;
	thread_local std::vector<CreaturePtr> area_creature_buffer;
	thread_local std::vector<TilePtr>     area_valid_tile_buffer;

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
				if (const auto it = _StandardDeflectionMap.find(targetCount); it != _StandardDeflectionMap.end() and not it->second.empty())
				{
					const auto& targetAreas = it->second;
					const auto index = static_cast<size_t>(uniform_random(0, static_cast<int32_t>(targetAreas.size()) - 1));
					combatArea->setupArea(targetAreas[index], 5);
				}
				break;
			}
			case DIRECTION_SOUTHWEST:	[[fallthrough]];
			case DIRECTION_SOUTHEAST:	[[fallthrough]];
			case DIRECTION_NORTHWEST:	[[fallthrough]];
			case DIRECTION_NORTHEAST:
			{
				if (const auto it = _DiagonalDeflectionMap.find(targetCount); it != _DiagonalDeflectionMap.end() and not it->second.empty())
				{
					const auto& targetAreas = it->second;
					const auto index = static_cast<size_t>(uniform_random(0, static_cast<int32_t>(targetAreas.size()) - 1));
					combatArea->setupExtArea(targetAreas[index], 5);
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
		deflect->config.set(Config::IsDeflect);
		deflect->config.set(Config::AttackModified);
		deflect->config.set(Config::DefenseModified);
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
			ricochet->config.set(Config::IsRicochet);
			ricochet->config.set(Config::AttackModified);
			ricochet->config.set(Config::DefenseModified);
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
	static std::pair<std::vector<DamageLocation>, int32_t> CreateDamageLocations(const MatrixArea& area)
	{
		if (area.GetRows() == 0 or area.GetCols() == 0)
			return {};

		const auto [cx, cy] = area.GetCenter();

		std::vector<DamageLocation> locations;
		int32_t max_extent = 0;

		for (uint32_t row = 0; row < area.GetRows(); ++row)
		{
			for (uint32_t col = 0; col < area.GetCols(); ++col)
			{
				if (area(row, col))
				{
					const int16_t spread  = static_cast<int16_t>(static_cast<int32_t>(col) - static_cast<int32_t>(cx));
					const int16_t forward = static_cast<int16_t>(static_cast<int32_t>(row) - static_cast<int32_t>(cy));
					locations.push_back({ .spread = spread, .forward = forward });
					max_extent = std::max(max_extent, std::max(std::abs(static_cast<int32_t>(spread)), std::abs(static_cast<int32_t>(forward))));
				}
			}
		}

		return { std::move(locations), max_extent };
	}

	void Combat::setArea(AreaCombat* area)
	{
		if (not area)
			return;

		CombatArea cardinal = area->GetCombatArea();
		CombatArea ext      = area->GetExtCombatArea();

		const bool hasCard = std::ranges::any_of(cardinal.directions, [](const auto& d) { return not d.empty(); });
		const bool hasExt  = std::ranges::any_of(ext.directions,      [](const auto& d) { return not d.empty(); });

		if (hasCard or hasExt)
		{
			auto& pair = combat_area_pair_map[combat_id];
			if (hasCard) pair.area     = std::move(cardinal);
			if (hasExt)  pair.ext_area = std::move(ext);
			cached_areas = &pair;
			config.set(Config::HasArea);
			if (hasExt) config.set(Config::HasExtArea);
		}

		delete area;
	}

	void Combat::setArea(std::unique_ptr<AreaCombat> const area)
	{
		if (not area)
			return;

		CombatArea cardinal = area->GetCombatArea();
		CombatArea ext      = area->GetExtCombatArea();

		const bool hasCard = std::ranges::any_of(cardinal.directions, [](const auto& d) { return not d.empty(); });
		const bool hasExt  = std::ranges::any_of(ext.directions,      [](const auto& d) { return not d.empty(); });

		if (hasCard or hasExt)
		{
			auto& pair = combat_area_pair_map[combat_id];
			if (hasCard) pair.area     = std::move(cardinal);
			if (hasExt)  pair.ext_area = std::move(ext);
			cached_areas = &pair;
			config.set(Config::HasArea);
			if (hasExt) config.set(Config::HasExtArea);
		}
	}

	const DamageArea Combat::getAreaPositions(const Position& casterPos, const Position& targetPos)
	{
		area_position_buffer.clear();
		area_tile_buffer.clear();

		const int32_t dx = Position::getOffsetX(targetPos, casterPos);
		const int32_t dy = Position::getOffsetY(targetPos, casterPos);
		const std::vector<DamageLocation>* locations = nullptr;
		int32_t stored_max_extent = 0;

		// Flags set at setArea() time — no pointer or container checks needed here.
		if (config.test(Config::HasArea))
		{
			if ((dx != 0) and (dy != 0) and config.test(Config::HasExtArea))
			{
				Direction diagDir;
				if      (dx < 0 and dy < 0) diagDir = DIRECTION_NORTHWEST;
				else if (dx > 0 and dy < 0) diagDir = DIRECTION_NORTHEAST;
				else if (dx < 0)            diagDir = DIRECTION_SOUTHWEST;
				else                        diagDir = DIRECTION_SOUTHEAST;

				const uint8_t slot = static_cast<uint8_t>(diagDir) ^ DIRECTION_DIAGONAL_MASK;
				locations = &cached_areas->ext_area.directions[slot];
				stored_max_extent = cached_areas->ext_area.max_extent;
			}

			if (not locations)
			{
				Direction cardDir;
				if      (dx < 0) cardDir = DIRECTION_WEST;
				else if (dx > 0) cardDir = DIRECTION_EAST;
				else if (dy < 0) cardDir = DIRECTION_NORTH;
				else             cardDir = DIRECTION_SOUTH;

				locations = &cached_areas->area.directions[cardDir];
				stored_max_extent = cached_areas->area.max_extent;
			}
		}

		if (not locations or locations->empty())
			return { area_position_buffer, 0, 0 };

		const size_t loc_count      = locations->size();
		const DamageLocation* locs  = locations->data();
		const uint16_t base_x       = targetPos.x;
		const uint16_t base_y       = targetPos.y;

		if (config.test(Config::MultiLevel))
		{
			const int casterZ          = static_cast<int>(casterPos.z);
			const bool ignore_ground   = config.test(Config::IgnoreGround);
			static const int floor_range = g_config.GetNumber(ConfigManager::MULTILEVEL_FLOOR_RANGE);

			const uint8_t minZ = ignore_ground
				? static_cast<uint8_t>(std::max(0, casterZ - floor_range))
				: static_cast<uint8_t>(casterZ);
			const uint8_t maxZ = static_cast<uint8_t>(std::min(MAP_MAX_LAYERS - 1, casterZ + floor_range));

			if (ignore_ground)
			{
				// No barrier: emit every floor in the visible range.
				// Pre-size the buffer so all writes go directly to contiguous storage.
				const size_t total = loc_count * static_cast<size_t>(maxZ - minZ + 1);
				area_position_buffer.resize(total);
				Position* dst = area_position_buffer.data();

				for (uint8_t z = minZ; z <= maxZ; ++z)
				{
					for (size_t k = 0; k < loc_count; ++k)
					{
						*dst++ = Position(
							static_cast<uint16_t>(base_x + locs[k].spread),
							static_cast<uint16_t>(base_y + locs[k].forward),
							z);
					}
				}
			}
			else
			{
				// Per-column ground barrier: iterate downward from the caster's floor.
				// The first tile that has a ground item stops propagation for that column.
				// The resolved TilePtr is pushed to area_tile_buffer in lock-step with
				// area_position_buffer, so execute() can skip the second getTile() call.
				area_position_buffer.reserve(loc_count);
				area_tile_buffer.reserve(loc_count);

				for (size_t k = 0; k < loc_count; ++k)
				{
					const uint16_t wx = static_cast<uint16_t>(base_x + locs[k].spread);
					const uint16_t wy = static_cast<uint16_t>(base_y + locs[k].forward);

					for (uint8_t z = minZ; z <= maxZ; ++z)
					{
						const auto tile = g_game.map.getTile(wx, wy, z);
						if (tile and tile->getGround())
						{
							area_position_buffer.emplace_back(wx, wy, z);
							area_tile_buffer.push_back(tile);
							break;
						}
					}
				}
			}
		}
		else
		{
			// Single-floor path: pre-size and write directly into contiguous storage.
			area_position_buffer.resize(loc_count);
			Position* dst = area_position_buffer.data();
			const uint8_t z = targetPos.z;

			for (size_t k = 0; k < loc_count; ++k)
			{
				dst[k] = Position(
					static_cast<uint16_t>(base_x + locs[k].spread),
					static_cast<uint16_t>(base_y + locs[k].forward),
					z);
			}
		}

		const int32_t distance = std::max(std::abs(dx), std::abs(dy));
		return { std::move(area_position_buffer), distance, stored_max_extent };
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
			return Combat::TargetCode::CanNotAttackThisMonster; // this is classic style rules, monsters don't harm other monsters in general, we will want this to be the users decision

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

	uint32_t Combat::applyCrit(uint32_t currentDamage, const uint32_t percent, const uint32_t flat) noexcept
	{
		if (percent > 0)
			currentDamage += currentDamage * percent / 100u;

		if (flat > 0)
			currentDamage += flat;

		if (percent or flat)
			config.set(Combat::Config::Critical);

		return currentDamage;
	}

	std::pair<CombatHandle, uint32_t> Combat::penetrateDamage(uint32_t currentDamage, const uint32_t percent, const uint32_t flat) noexcept
	{
		uint32_t penetration_damage = 0;

		if (percent > 0)
			penetration_damage += currentDamage * percent / 100u;

		if (flat > 0)
			penetration_damage += flat;

		penetration_damage = (penetration_damage > currentDamage) ? currentDamage : penetration_damage;
		currentDamage -= penetration_damage;

		auto handle = g_combat_registry.Create(std::to_underlying(DamageType::Undefined), penetration_damage);
		handle->origin = Origin::Piercing;
		handle->config.set(Config::TrueDamage);
		handle->config.set(Config::AttackModified);
		return { std::move(handle), currentDamage };
	}

	// Todo: Currently the way all the damage modifiers work in the augment and combat systems, is that it's percent is based on
	// the damage value, and that is actually not the most common, or at least not the only top common path... People will often
	// choose to target percent based on victim's stats.. example, 10 percent lifesteal would steal 10 percent of the victims overall health
	// rather than just 10 percent of the damage output. I don't know how I have never really realized this huge fallacy before now.
	// The 'Todo' here is to build another cache system strictly bound by "damage output" or "victim stat"

	uint32_t Combat::process_steal(const PlayerPtr& caster, const CreaturePtr& victim, const LeechData& steal, uint32_t currentDamage) noexcept
	{
		const auto totalDamage = currentDamage;

		if (steal.percent_health > 0 or steal.flat_health > 0)
		{
			uint32_t life = 0;

			if (steal.percent_health)
				life += totalDamage * steal.percent_health / 100;

			if (steal.flat_health)
				life += steal.flat_health;

			if (life)
			{
				const uint32_t stolen = std::min(life, currentDamage);
				currentDamage -= stolen;

				auto lifesteal = g_combat_registry.Create(DamageType::LifeDrain, stolen);
				lifesteal->origin = Origin::LifeSteal;
				lifesteal->config.set(Config::Leech);
				lifesteal->config.set(Config::TopTargetOnly);
				lifesteal->config.set(Config::AttackModified);
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
				const uint32_t stolen = std::min(mana, currentDamage);
				currentDamage -= stolen;

				auto manasteal = g_combat_registry.Create(DamageType::ManaDrain, stolen);
				manasteal->origin = Origin::ManaSteal;
				manasteal->config.set(Config::Leech);
				manasteal->config.set(Config::TopTargetOnly);
				manasteal->config.set(Config::AttackModified);
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
				const uint32_t stolen = std::min(stamina, currentDamage);
				currentDamage -= stolen;

				auto staminasteal = g_combat_registry.Create(DamageType::Undefined, stolen);
				staminasteal->origin = Origin::StaminaSteal;
				staminasteal->config.set(Config::Leech);
				staminasteal->config.set(Config::TopTargetOnly);
				staminasteal->config.set(Config::AttackModified);
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
				const uint32_t stolen = std::min(soul, currentDamage);
				currentDamage -= stolen;

				auto soulsteal = g_combat_registry.Create(DamageType::Undefined, stolen);
				soulsteal->origin = Origin::SoulSteal;
				soulsteal->config.set(Config::Leech);
				soulsteal->config.set(Config::TopTargetOnly);
				soulsteal->config.set(Config::AttackModified);
				soulsteal->config.set(Config::TrueDamage);
				soulsteal->damage = stolen;
				soulsteal->strike_target(caster, victim, true);
			}
		}

		return currentDamage;
	}

	void Combat::post_damage(const PlayerPtr& caster, const CreaturePtr& victim, uint32_t currentDamage, LeechData&& leech) noexcept
	{
		if (impactEffect != CONST_ME_NONE)
			g_game.addMagicEffect(victim->getPosition(), impactEffect);

		if (config.test(Config::Critical))
		{
			if ((config.test(Config::AttackModified) and g_config.GetBoolean(ConfigManager::AUGMENT_CRITICAL_ANIMATION)) or not config.test(Config::AttackModified))
				g_game.addMagicEffect(victim->getPosition(), CONST_ME_CRITICAL_DAMAGE);
		}

		auto primary_conditions = (not config.test(Config::Leech) and currentDamage != COMBAT_HEALING and caster->is_player() and origin != Origin::Condition);

		if (primary_conditions)
		{
			const auto totalDamage = currentDamage;

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
	template <std::ranges::input_range R>
		requires std::same_as<std::ranges::range_value_t<R>, DamageModifier>
	uint32_t Combat::handle_conversion(R&& modifiers, const CreaturePtr& attacker, const CreaturePtr& victim, uint32_t currentDamage, std::optional<std::span<const CreaturePtr>> spectators)
	{
		uint32_t accumulated[DamageType::DamageTypes] = {};
		uint32_t total = 0;

		for (const auto& modifier : modifiers)
		{
			const uint32_t index = std::countr_zero(modifier.to_damage_type);
			const uint32_t value = modifier.isFlatValue() ? static_cast<uint32_t>(modifier.value)
			                                              : static_cast<uint32_t>(static_cast<uint64_t>(currentDamage) * modifier.value / 100u);
			if (index < DamageType::DamageTypes) [[likely]]
				accumulated[index] += value;
			total += value;
		}

		for (uint32_t i = 0; i < DamageType::DamageTypes; ++i)
		{
			if (accumulated[i] == 0) continue;
			const auto type = static_cast<uint16_t>(1u << i);
			if (type == DamageType::Healing)
				Combat::transformDamage(type, accumulated[i])->heal_target(attacker, victim, true, spectators);
			else
				Combat::transformDamage(type, accumulated[i])->strike_target(attacker, victim, true, spectators);
		}

		return total;
	}

	uint32_t Combat::reform_augment(const CreaturePtr& attacker, const PlayerPtr& victim, uint32_t currentDamage) noexcept
	{
		const auto& reform_mods = victim->getReformMods();
		if (reform_mods.empty())
			return currentDamage;

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
				: static_cast<uint32_t>(static_cast<uint64_t>(currentDamage) * modifier.getValue() / 100u);

			to_amounts[index] += value;
			total_converted += value;
		}

		if (total_converted == 0)
			return currentDamage;

		total_converted = std::min(total_converted, currentDamage);
		currentDamage -= total_converted;

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

		return currentDamage;
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
	uint32_t Combat::defense_augment(const CreaturePtr& attacker, const PlayerPtr& victim, uint32_t currentDamage, const std::optional<std::span<const CreaturePtr>> spectators) noexcept
	{
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
			currentDamage += currentDamage * percent_weak / 100;

		if (flat_weak)
			currentDamage += flat_weak;

		if (percent_resist)
		{
			const uint32_t reduction = currentDamage * percent_resist / 100;
			currentDamage = (reduction >= currentDamage) ? 0 : currentDamage - reduction;
		}

		if (flat_resist)
			currentDamage = (flat_resist >= currentDamage) ? 0 : currentDamage - flat_resist;

		if (percent_absorb or flat_absorb)
		{
			uint32_t absorbed = (percent_absorb ? currentDamage * percent_absorb / 100 : 0) + flat_absorb;
			absorbed = std::min(absorbed, currentDamage);
			currentDamage -= absorbed;

			auto heal = g_combat_registry.Create(DamageType::Healing, absorbed);
			heal->origin = Origin::Absorb;
			heal->config.set(Config::TrueDamage);
			heal->config.set(Config::HealthTarget);
			heal->config.set(Config::DefenseModified);
			heal->heal_target(victim, victim, true, spectators);
		}

		if (percent_restore or flat_restore)
		{
			uint32_t restored = (percent_restore ? currentDamage * percent_restore / 100 : 0) + flat_restore;
			restored = std::min(restored, currentDamage);
			currentDamage -= restored;

			auto mana = g_combat_registry.Create(DamageType::Healing, restored);
			mana->origin = Origin::Restore;
			mana->config.set(Config::TrueDamage);
			mana->config.set(Config::ManaTarget);
			mana->config.set(Config::DefenseModified);
			mana->heal_target(victim, victim, true, spectators);
		}

		if (percent_replenish or flat_replenish)
		{
			uint32_t replenished = (percent_replenish ? currentDamage * percent_replenish / 100 : 0) + flat_replenish;
			replenished = std::min(replenished, currentDamage);
			currentDamage -= replenished;

			auto stamina = g_combat_registry.Create(DamageType::Healing, replenished);
			stamina->origin = Origin::Replenish;
			stamina->config.set(Config::TrueDamage);
			stamina->config.set(Config::StaminaTarget);
			stamina->config.set(Config::DefenseModified);
			stamina->heal_target(victim, victim, true, spectators);
		}

		if (percent_revive or flat_revive)
		{
			uint32_t revived = (percent_revive ? currentDamage * percent_revive / 100 : 0) + flat_revive;
			revived = std::min(revived, currentDamage);
			currentDamage -= revived;

			auto soul = g_combat_registry.Create(DamageType::Healing, revived);
			soul->origin = Origin::Revive;
			soul->config.set(Config::TrueDamage);
			soul->config.set(Config::SoulTarget);
			soul->config.set(Config::DefenseModified);
			soul->heal_target(victim, victim, true, spectators);
		}

		const bool is_bounce = config.test(Config::IsReflect) or config.test(Config::IsDeflect) or config.test(Config::IsRicochet);

		if (not is_bounce and (percent_reflect or flat_reflect) and attacker)
		{
			uint32_t reflected = (percent_reflect ? currentDamage * percent_reflect / 100 : 0) + flat_reflect;
			reflected = std::min(reflected, currentDamage);
			currentDamage -= reflected;

			auto reflect = g_combat_registry.Create(damage_type, reflected);
			reflect->origin = Origin::Reflect;
			reflect->config.set(Config::TrueDamage);
			reflect->config.set(Config::TopTargetOnly);
			reflect->config.set(Config::IsReflect);
			reflect->config.set(Config::AttackModified);
			reflect->config.set(Config::DefenseModified);
			reflect->strike_target(victim, attacker, true);
		}

		if (not is_bounce and (percent_deflect or flat_deflect) and attacker)
		{
			uint32_t deflected = (percent_deflect ? currentDamage * percent_deflect / 100 : 0) + flat_deflect;
			deflected = std::min(deflected, currentDamage);
			currentDamage -= deflected;
			deflect_damage(attacker, victim, deflected, damage_type, distanceEffect, impactEffect);
		}

		if (not is_bounce and (percent_ricochet or flat_ricochet) and attacker)
		{
			uint32_t ricocheted = (percent_ricochet ? currentDamage * percent_ricochet / 100 : 0) + flat_ricochet;
			ricocheted = std::min(ricocheted, currentDamage);
			currentDamage -= ricocheted;
			ricochetDamage(victim, ricocheted, damage_type, distanceEffect, impactEffect);
		}

		return currentDamage;
	}

	// Returns false if damage was fully consumed by conversion (caller should stop processing).
	template <typename VictimT>
	void Combat::accumulate_attack_mods(const PlayerPtr& caster, const VictimT& victim,
	                                     uint32_t& currentDamage, LeechData& leech_data, LeechData& steal_data,
	                                     const std::optional<std::span<const CreaturePtr>>& spectators) noexcept
	{
		if (not caster->hasAttackModifiers() or config.test(Config::AttackModified))
			return;

		const float atk_mult    = caster->get_atk_modifier_charge_cost_multiplier();
		const uint32_t atk_cost = static_cast<uint32_t>(std::round(static_cast<float>(atk_modifier_charge_cost) * atk_mult));

		if (atk_cost != 0 and caster->get_atk_modifier_charges() < atk_cost)
			return;

		if (atk_cost > 0)
			caster->set_atk_modifier_charges(caster->get_atk_modifier_charges() - atk_cost);

		const auto conversion_count = caster->conversion_mod_count();
		const auto victim_race      = victim->getRace();
		const auto& victim_name     = victim->getName();
		const auto moddable         = damage_type != DamageType::ManaDrain and damage_type != DamageType::Healing;

		auto applied = [&](const auto& modifier)
		{
			return modifier.applies(damage_type, victim->getType(), origin, victim_race, victim_name);
		};

		if (conversion_count > 0)
		{
			const auto& conversion_modifiers = caster->getConversionMods();
			auto conversion_view = conversion_modifiers | std::views::filter(applied);
			const auto converted_damage = handle_conversion(conversion_view, caster, victim, currentDamage, spectators);

			if (converted_damage)
			{
				currentDamage = converted_damage >= currentDamage ? 0 : currentDamage - converted_damage;
				if (currentDamage == 0)
					return;
			}
		}

		if (not moddable or caster->attack_mod_count() <= conversion_count)
			return;

		const auto& main_attack_sums = caster->getMainAttackModSums();

		auto percent_crit   = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Critical)].percent;
		auto flat_crit      = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Critical)].flat;
		auto percent_pierce = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Piercing)].percent;
		auto flat_pierce    = main_attack_sums[std::to_underlying(DamageModifier::AttackType::Piercing)].flat;

		if (caster->hasFilteredAttackMods())
		{
			auto attack_view = caster->getFilteredAttackMods() | std::views::filter(applied);
			for (const auto& modifier : attack_view)
			{
				const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);
				if (mod_type == DamageModifier::AttackType::Critical)
				{
					percent_crit += modifier.isFlatValue() ? 0 : modifier.getValue();
					flat_crit    += modifier.isFlatValue() ? modifier.getValue() : 0;
				}
				else if (mod_type == DamageModifier::AttackType::Piercing)
				{
					percent_pierce += modifier.isFlatValue() ? 0 : modifier.getValue();
					flat_pierce    += modifier.isFlatValue() ? modifier.getValue() : 0;
				}
			}
		}

		if (caster->hasFilteredAttackPostMods())
		{
			auto post_view = caster->getFilteredAttackPostMods() | std::views::filter(applied);
			for (const auto& modifier : post_view)
			{
				const auto mod_type = static_cast<DamageModifier::AttackType>(modifier.mod_type);
				auto& tgt = modifier.true_leech ? steal_data : leech_data;

				switch (mod_type)
				{
					case DamageModifier::AttackType::Lifesteal:
						tgt.percent_health += modifier.isFlatValue() ? 0 : modifier.getValue();
						tgt.flat_health    += modifier.isFlatValue() ? modifier.getValue() : 0;
						break;
					case DamageModifier::AttackType::Manasteal:
						tgt.percent_mana   += modifier.isFlatValue() ? 0 : modifier.getValue();
						tgt.flat_mana      += modifier.isFlatValue() ? modifier.getValue() : 0;
						break;
					case DamageModifier::AttackType::Staminasteal:
						tgt.percent_stamina += modifier.isFlatValue() ? 0 : modifier.getValue();
						tgt.flat_stamina    += modifier.isFlatValue() ? modifier.getValue() : 0;
						break;
					case DamageModifier::AttackType::Soulsteal:
						tgt.percent_soul += modifier.isFlatValue() ? 0 : modifier.getValue();
						tgt.flat_soul    += modifier.isFlatValue() ? modifier.getValue() : 0;
						break;
					default: [[unlikely]]
						break;
				}
			}
		}

		if (not config.test(Config::TrueDamage) and (percent_pierce or flat_pierce))
		{
			auto [trueHandle, remaining] = penetrateDamage(currentDamage, percent_pierce, flat_pierce);
			trueHandle->strike_target(caster, victim, true, spectators);
			currentDamage = remaining;
		}

		if (percent_crit or flat_crit)
			currentDamage = applyCrit(currentDamage, percent_crit, flat_crit);
	}

	template void Combat::accumulate_attack_mods<PlayerPtr>(const PlayerPtr&, const PlayerPtr&, uint32_t&, LeechData&, LeechData&, const std::optional<std::span<const CreaturePtr>>&);
	template void Combat::accumulate_attack_mods<MonsterPtr>(const PlayerPtr&, const MonsterPtr&, uint32_t&, LeechData&, LeechData&, const std::optional<std::span<const CreaturePtr>>&);

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

		uint32_t currentDamage = damage;

		if (not config.test(Config::TrueDamage))
		{
			const auto blockResult = block(caster, victim);

			if (not blockResult)
			{
				const auto blocked = blockResult.error();
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
			currentDamage = blockResult.value();
		}

		LeechData leech_data {};
		LeechData steal_data {};

		accumulate_attack_mods(caster, victim, currentDamage, leech_data, steal_data, spectators);
		if (currentDamage == 0)
			return;

		if (victim->hasDefenseModifiers() and not config.test(Config::DefenseModified))
		{
			const float def_mult = victim->get_def_modifier_charge_cost_multiplier();
			const uint32_t def_cost = static_cast<uint32_t>(std::round(static_cast<float>(def_modifier_charge_cost) * def_mult));

			if (def_cost == 0 or victim->get_def_modifier_charges() >= def_cost)
			{
				if (def_cost > 0)
					victim->set_def_modifier_charges(victim->get_def_modifier_charges() - def_cost);

				const auto reform_count = victim->reform_mod_count();

				if (reform_count > 0)
					currentDamage = reform_augment(caster, victim, currentDamage);

				if (victim->defense_mod_count() > reform_count)
					currentDamage = defense_augment(caster, victim, currentDamage);
			}
		}

		currentDamage = process_steal(caster, victim, steal_data, currentDamage);

		if (apply_damage(caster, victim, currentDamage, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			post_damage(caster, victim, currentDamage, std::move(leech_data));
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : *condition_list)
					victim->addCondition(cond->clone());
			}

			// UseCharges: on a successful hit, consume one charge/count from the caster's equipped weapon.
			if (config.test(Config::UseCharges))
			{
				if (const auto& weaponItem = caster->getWeapon(CONST_SLOT_RIGHT, false))
				{
					const uint16_t charges = weaponItem->getCharges();
					if (charges != 0 and g_config.GetBoolean(ConfigManager::REMOVE_WEAPON_CHARGES))
					{
						g_game.transformItem(weaponItem, weaponItem->getID(), charges - 1);
					}
					else if (weaponItem->getItemCount() > 1)
					{
						g_game.transformItem(weaponItem, weaponItem->getID(), weaponItem->getItemCount() - 1);
					}
					else
					{
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

		uint32_t currentDamage = damage;

		if (not config.test(Config::TrueDamage))
		{
			const auto blockResult = block(caster, victim);

			if (not blockResult)
			{
				const auto blocked = blockResult.error();
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
			currentDamage = blockResult.value();
		}

		LeechData leech_data {};
		LeechData steal_data {};

		accumulate_attack_mods(caster, victim, currentDamage, leech_data, steal_data, spectators);
		if (currentDamage == 0)
			return;

		currentDamage = process_steal(caster, victim, steal_data, currentDamage);

		if (apply_damage(caster, victim, currentDamage, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			post_damage(caster, victim, currentDamage, std::move(leech_data));
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : *condition_list)
					victim->addCondition(cond->clone());
			}

			// UseCharges: on a successful hit, consume one charge/count from the caster's equipped weapon.
			if (config.test(Config::UseCharges))
			{
				if (const auto& weaponItem = caster->getWeapon(CONST_SLOT_RIGHT, false))
				{
					const uint16_t charges = weaponItem->getCharges();
					if (charges != 0 and g_config.GetBoolean(ConfigManager::REMOVE_WEAPON_CHARGES))
					{
						g_game.transformItem(weaponItem, weaponItem->getID(), charges - 1);
					}
					else if (weaponItem->getItemCount() > 1)
					{
						g_game.transformItem(weaponItem, weaponItem->getID(), weaponItem->getItemCount() - 1);
					}
					else
					{
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

		uint32_t currentDamage = damage;

		if (not config.test(Config::TrueDamage))
		{
			const auto blockResult = block(attacker, victim);
			if (not blockResult)
			{
				const auto blocked = blockResult.error();
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
			currentDamage = blockResult.value();
		}

		if (victim->hasDefenseModifiers() and not config.test(Config::DefenseModified))
		{
			const float def_mult = victim->get_def_modifier_charge_cost_multiplier();
			const uint32_t def_cost = static_cast<uint32_t>(std::round(static_cast<float>(def_modifier_charge_cost) * def_mult));

			if (def_cost == 0 or victim->get_def_modifier_charges() >= def_cost)
			{
				if (def_cost > 0)
					victim->set_def_modifier_charges(victim->get_def_modifier_charges() - def_cost);

				const auto reform_count = victim->reform_mod_count();

				if (reform_count > 0)
					currentDamage = reform_augment(attacker, victim, currentDamage);

				if (victim->defense_mod_count() > reform_count)
					currentDamage = defense_augment(attacker, victim, currentDamage);
			}
		}

		if (apply_damage(attacker, victim, currentDamage, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : *condition_list)
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

		uint32_t currentDamage = damage;

		if (not config.test(Config::TrueDamage))
		{
			const auto blockResult = block(attacker, victim);
			if (not blockResult)
			{
				const auto blocked = blockResult.error();
				if (blocked == BlockType::Defensive)
					defense_block_effect(victim->getPosition());
				else if (blocked == BlockType::Armor)
					armor_block_effect(victim->getPosition());
				return;
			}
			currentDamage = blockResult.value();
		}

		// Do we do anything here for summons attacking or being attacked or anything like that? Perhaps in the future when we allow passing
		// either augments or modifiers directly with combat this version of combat becomes useful, or maybe we revamp the summon system and finally
		// make summon an actual "thing" type derived from monster like I always thought it should be... I just feel like I'm forgetting something obvious right now

		// about the above comment, I think perhaps we can allow multiple formula's for different target creature types, monster, npc, boss or player.

		if (apply_damage(attacker, victim, currentDamage, spectators) == 0)
		{
			// some kind of log here ?
		}
		else
		{
			if (config.test(Config::HasCondition))
			{
				for (const auto& cond : *condition_list)
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
		if (damage_type != DamageType::Healing)
			return;

		// Output stage: compiled formula → TOML/Lua factor params → fixed damage
		const bool caster_player = caster && caster->getCreatureSubType() == CreatureSubType::Player;
		const bool target_player = target && target->getCreatureSubType() == CreatureSubType::Player;
		const uint8_t sit_idx = caster_player
			? (target_player ? 0u : 1u)
			: (target_player ? 2u : 3u);

		const CompiledFormulaSlots* compiled = config.test(Config::HasCompiledFormula)
			? formula_cache->compiled.get()
			: nullptr;

		FormulaContext ctx;
		ctx.caster = caster;
		ctx.target = target;

		uint32_t heal_amount = damage;
		if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Output)) : nullptr)
		{
			heal_amount = static_cast<uint32_t>(std::abs((*fn)(ctx)));
		}
		else if (config.test(Config::HasFormulaCache) && caster && caster->is_player())
		{
			const SituationFormulas& formulas = formula_cache->situations[sit_idx];
			const int32_t stat = static_cast<int32_t>(caster->getPlayer()->getLevel())
			                   + static_cast<int32_t>(caster->getPlayer()->getMagicLevel()) * 3;
			heal_amount = static_cast<uint32_t>(std::abs(calculate_output(formulas.output, stat)));
		}

		if (heal_amount == 0)
			return;

		if (config.test(Config::HealthTarget))
		{
			const int32_t maxHeal = target->getMaxHealth() - target->getHealth();

			if (maxHeal <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), target);

			const uint32_t healed = std::min<uint32_t>(heal_amount, static_cast<uint32_t>(maxHeal));
			target->changeHealth(healed);
			heal_notification(caster, target, healed, spectators);
		}
		else if (config.test(Config::ManaTarget))
		{
			const auto player = target->getPlayer();

			if (not player)
				return;

			const int32_t maxMana = player->getMaxMana() - player->getMana();

			if (maxMana <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), player);

			const uint32_t healed = std::min<uint32_t>(heal_amount, static_cast<uint32_t>(maxMana));
			player->changeMana(static_cast<int32_t>(healed));
			heal_notification(caster, player, healed, spectators);
		}
		else if (config.test(Config::StaminaTarget))
		{
			const auto player = target->getPlayer();

			if (not player)
				return;

			constexpr int32_t maxStaminaMinutes = 2520;
			const int32_t remaining = maxStaminaMinutes - static_cast<int32_t>(player->getStaminaMinutes());

			if (remaining <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), player);

			const uint32_t healed = std::min<uint32_t>(heal_amount, static_cast<uint32_t>(remaining));
			player->changeStamina(static_cast<int32_t>(healed));
			heal_notification(caster, player, healed, spectators);
		}
		else if (config.test(Config::SoulTarget))
		{
			const auto player = target->getPlayer();

			if (not player)
				return;

			const int32_t maxSoul = static_cast<int32_t>(player->getVocation()->getSoulMax()) - static_cast<int32_t>(player->getSoul());

			if (maxSoul <= 0)
				return;

			if (caster->is_player())
				apply_healing_modifiers(std::static_pointer_cast<Player>(caster), player);

			const uint32_t healed = std::min<uint32_t>(heal_amount, static_cast<uint32_t>(maxSoul));
			player->changeSoul(static_cast<int32_t>(healed));
			heal_notification(caster, player, healed, spectators);
		}
	}

	void Combat::execute(const CreaturePtr& caster, const Position& center) noexcept
	{
		const auto& [positions, distance, stored_extent] = getAreaPositions(caster->getPosition(), center);

		if (positions.empty())
			return;

		static constexpr size_t MAX_TILES = 256;
		const size_t n = std::min(positions.size(), MAX_TILES);

		const int32_t area_extent = std::max(distance, stored_extent);

		auto spectators = g_game.map.fetchSpectators(center, true, true,
			area_extent + Map::maxViewportX, area_extent + Map::maxViewportX,
			area_extent + Map::maxViewportY, area_extent + Map::maxViewportY);

		if (caster and (distanceEffect != CONST_ANI_NONE))
			addDistanceEffect(caster, caster->getPosition(), center, distanceEffect);

		alignas(32) uint32_t packed_flags[MAX_TILES];
		alignas(32) uint8_t  valid_mask[MAX_TILES];
		TilePtr              tile_cache[MAX_TILES];

		// When the ground-barrier MultiLevel path ran in getAreaPositions(), tiles were
		// already fetched and stored in area_tile_buffer in lock-step with positions.
		// Re-use those directly to avoid a second getTile() call per position.
		const bool tiles_prefetched = area_tile_buffer.size() >= n;
		for (size_t i = 0; i < n; ++i)
		{
			tile_cache[i]   = tiles_prefetched ? area_tile_buffer[i] : g_game.map.getTile(positions[i]);
			packed_flags[i] = tile_cache[i] ? tile_cache[i]->getFlags() : ~0u;
		}
		// Release pre-fetched tile refs now — recursive execute() calls (deflect, ricochet)
		// must be able to use area_tile_buffer for their own getAreaPositions() pass.
		if (tiles_prefetched)
			area_tile_buffer.clear();

		std::memset(valid_mask, 0xFF, n);

		// Pre-compute all config flags used in the filter loops below.
		const bool cfg_aggressive      = config.test(Config::Aggressive);
		const bool cfg_ignore_barriers = config.test(Config::IgnoreBarriers);
		const bool cfg_top_target_only = config.test(Config::TopTargetOnly);
		const bool cfg_self_only       = config.test(Config::SelfOnly);
		const bool cfg_friendly_party  = config.test(Config::FriendlyParty);
		const bool cfg_enemy_party     = config.test(Config::EnemyParty);
		const bool cfg_fragged_only    = config.test(Config::FraggedOnly);

		const bool admin = caster->getCreatureSubType() == CreatureSubType::Player and caster->getPlayer()->hasFlag(PlayerFlag_IgnoreProtectionZone);
		const uint32_t flag_reject = TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT | (cfg_aggressive and not admin ? static_cast<uint32_t>(TILESTATE_PROTECTIONZONE) : 0u);

#ifdef __AVX2__
		{
			const __m256i vReject = _mm256_set1_epi32(static_cast<int32_t>(flag_reject));
			const __m256i vZero   = _mm256_setzero_si256();

			// Byte j of this mask has exactly bit j set, used to isolate one bit per byte
			// after broadcasting the movemask result — avoids the store+reload round-trip.
			alignas(16) static const uint8_t k_bit_isolate[16] = {
				0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
				0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
			};
			const __m128i vIsolate = _mm_load_si128(reinterpret_cast<const __m128i*>(k_bit_isolate));

			size_t i = 0;
			for (; i + 8 <= n; i += 8)
			{
				const __m256i vFlags    = _mm256_load_si256(reinterpret_cast<const __m256i*>(packed_flags + i));
				const __m256i vHit      = _mm256_and_si256(vFlags, vReject);
				const __m256i vPassMask = _mm256_cmpeq_epi32(vHit, vZero);

				// Extract sign bit of each 32-bit lane: 1 = pass (0xFFFFFFFF), 0 = reject.
				const int bits = _mm256_movemask_ps(_mm256_castsi256_ps(vPassMask));

				// Broadcast the 8-bit result, isolate one bit per byte, compare to zero
				// (inverted), then invert again — net result: 0xFF=pass, 0x00=reject per byte.
				const __m128i vBits     = _mm_set1_epi8(static_cast<char>(bits));
				const __m128i vTest     = _mm_and_si128(vBits, vIsolate);
				const __m128i vExpanded = _mm_xor_si128(_mm_cmpeq_epi8(vTest, _mm_setzero_si128()), _mm_set1_epi8(-1));

				const __m128i vValid = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(valid_mask + i));
				_mm_storel_epi64(reinterpret_cast<__m128i*>(valid_mask + i), _mm_and_si128(vValid, vExpanded));
			}

			for (; i < n; ++i)
				valid_mask[i] &= ((packed_flags[i] & flag_reject) == 0u) ? 0xFF : 0x00;
		}
#else
		for (size_t i = 0; i < n; ++i)
			valid_mask[i] &= ((packed_flags[i] & flag_reject) == 0u) ? 0xFF : 0x00;
#endif

		area_creature_buffer.clear();
		area_creature_buffer.reserve(n * 2);
		auto& toDamageCreatures = area_creature_buffer;

		area_valid_tile_buffer.clear();
		area_valid_tile_buffer.reserve(n);
		auto& valid_tile_list = area_valid_tile_buffer;

		auto valid_tiles = std::views::iota(size_t{0}, n) | std::views::filter([&](size_t idx) { return static_cast<bool>(valid_mask[idx]); })
			| std::views::filter([&](size_t idx) {
				// IgnoreBarriers bypasses line-of-sight walls for area combat.
				return cfg_ignore_barriers or not tile_cache[idx]->hasProperty(CONST_PROP_BLOCKPROJECTILE);
			});

		for (size_t idx : valid_tiles)
		{
			const auto& tile = tile_cache[idx];

			valid_tile_list.push_back(tile);

			const auto& creaturesOnTile = tile->getCreatures();
			if (not creaturesOnTile)
				continue;

			const auto& topCreature = cfg_top_target_only ? tile->getTopCreature() : nullptr;
			const bool onCasterTile = (caster->getTile() == tile);

			auto strikeable = *creaturesOnTile
				// Tile-top filter: when TopTargetOnly is set, only the top creature per tile (or caster on their own tile).
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not cfg_top_target_only)
						return true;
					return onCasterTile ? (creature == caster) : (creature == topCreature);
				})
				// SelfOnly: only the caster themselves is a valid target (used by self-cast spells/heals).
				// When set it overrides the aggressive self-exclusion below.
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not cfg_self_only)
						return true;
					return creature == caster;
				})
				// Aggressive self-exclusion: aggressive combats skip the caster unless SelfOnly is set.
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not cfg_aggressive or cfg_self_only)
						return true;
					return caster != creature;
				})
				// Standard target-validity check for aggressive combats.
				| std::views::filter([&](const CreaturePtr& creature) -> bool {
					if (not cfg_aggressive)
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
					if (not cfg_friendly_party)
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
					if (not cfg_enemy_party)
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
					if (not cfg_fragged_only)
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
			strike_target(caster, target_creature, true, spec_span);
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
		if (vec.empty())
		{
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
		auto assign = [&](size_t slot, const MatrixArea& src)
		{
			auto [locs, ext] = CreateDamageLocations(src);
			result.directions[slot] = std::move(locs);
			result.max_extent = std::max(result.max_extent, ext);
		};
		assign(DIRECTION_NORTH, areas[DIRECTION_NORTH]);
		assign(DIRECTION_EAST,  areas[DIRECTION_EAST]);
		assign(DIRECTION_SOUTH, areas[DIRECTION_SOUTH]);
		assign(DIRECTION_WEST,  areas[DIRECTION_WEST]);
		return result;
	}

	CombatArea AreaCombat::GetExtCombatArea() const
	{
		if (not hasExtArea or areas.size() < 8)
			return {};

		// Slot layout mirrors the diagonal-mask-strip used in getAreaPositions:
		// direction ^ DIRECTION_DIAGONAL_MASK → SW=4^4=0, SE=5^4=1, NW=6^4=2, NE=7^4=3
		CombatArea result;
		auto assign = [&](size_t slot, const MatrixArea& src)
		{
			auto [locs, ext] = CreateDamageLocations(src);
			result.directions[slot] = std::move(locs);
			result.max_extent = std::max(result.max_extent, ext);
		};
		assign(DIRECTION_SOUTHWEST ^ DIRECTION_DIAGONAL_MASK, areas[DIRECTION_SOUTHWEST]);
		assign(DIRECTION_SOUTHEAST ^ DIRECTION_DIAGONAL_MASK, areas[DIRECTION_SOUTHEAST]);
		assign(DIRECTION_NORTHWEST ^ DIRECTION_DIAGONAL_MASK, areas[DIRECTION_NORTHWEST]);
		assign(DIRECTION_NORTHEAST ^ DIRECTION_DIAGONAL_MASK, areas[DIRECTION_NORTHEAST]);
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
	}

	std::expected<uint32_t, Combat::BlockType> Combat::block(const CreaturePtr& attacker, const PlayerPtr& target) noexcept
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

		// Three-tier resolution: compiled formula (tier 2) → TOML factor params (tier 1) → defaults (tier 0)
		const CompiledFormulaSlots* compiled = config.test(Config::HasCompiledFormula)
			? formula_cache->compiled.get()
			: nullptr;

		const SituationFormulas& formulas = config.test(Config::HasFormulaCache)
			? formula_cache->situations[sit_idx]
			: g_default_situation_formulas[sit_idx];

		FormulaContext ctx;
		ctx.caster = attacker;
		ctx.target = target;

		// Output stage: compiled formula → TOML/Lua factor params → fixed damage
		uint32_t computed_damage = damage;
		if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Output)) : nullptr)
		{
			computed_damage = static_cast<uint32_t>(std::abs((*fn)(ctx)));
		}
		else if (config.test(Config::HasFormulaCache) && attacker && attacker->is_player())
		{
			const int32_t stat = static_cast<int32_t>(attacker->getPlayer()->getLevel())
			                   + static_cast<int32_t>(attacker->getPlayer()->getMagicLevel()) * 3;
			computed_damage = static_cast<uint32_t>(std::abs(calculate_output(formulas.output, stat)));
		}

		if (apply_defense_reduction and target->can_use_defense()
		    and (defense_cost == 0 or target->get_defense_charges() >= defense_cost))
		{
			if (defense_cost > 0)
				target->set_defense_charges(target->get_defense_charges() - defense_cost);

			int32_t defense_value = 0;
			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Defense)) : nullptr)
			{
				ctx.pipeline_a = target->getDefense();
				defense_value = static_cast<int32_t>((*fn)(ctx));
			}
			else
			{
				defense_value = calculate_resistance(formulas.defense, target->getDefense());
			}

			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Resolution)) : nullptr)
			{
				ctx.pipeline_a = static_cast<int32_t>(computed_damage);
				ctx.pipeline_b = defense_value;
				computed_damage = static_cast<uint32_t>((*fn)(ctx));
			}
			else
			{
				computed_damage = static_cast<uint32_t>(calculate_resolution(formulas.resolution, static_cast<int32_t>(computed_damage), defense_value));
			}

			if (computed_damage == 0)
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

			int32_t armor_value = 0;
			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Armor)) : nullptr)
			{
				ctx.pipeline_a = target->getArmor();
				armor_value = static_cast<int32_t>((*fn)(ctx));
			}
			else
			{
				armor_value = calculate_resistance(formulas.armor, target->getArmor());
			}

			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Resolution)) : nullptr)
			{
				ctx.pipeline_a = static_cast<int32_t>(computed_damage);
				ctx.pipeline_b = armor_value;
				computed_damage = static_cast<uint32_t>((*fn)(ctx));
			}
			else
			{
				computed_damage = static_cast<uint32_t>(calculate_resolution(formulas.resolution, static_cast<int32_t>(computed_damage), armor_value));
			}

			if (computed_damage == 0)
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

		if (blockType != BlockType::NoBlock)
			return std::unexpected(blockType);
		return computed_damage;
	}

	std::expected<uint32_t, Combat::BlockType> Combat::block(const CreaturePtr& attacker, const MonsterPtr& target) noexcept
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

		// Three-tier resolution: compiled formula (tier 2) → TOML factor params (tier 1) → defaults (tier 0)
		const CompiledFormulaSlots* compiled = config.test(Config::HasCompiledFormula)
			? formula_cache->compiled.get()
			: nullptr;

		const SituationFormulas& formulas = config.test(Config::HasFormulaCache)
			? formula_cache->situations[sit_idx]
			: g_default_situation_formulas[sit_idx];

		FormulaContext ctx;
		ctx.caster = attacker;
		ctx.target = target;

		// Output stage: compiled formula → TOML/Lua factor params → fixed damage
		uint32_t computed_damage = damage;
		if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Output)) : nullptr)
		{
			computed_damage = static_cast<uint32_t>(std::abs((*fn)(ctx)));
		}
		else if (config.test(Config::HasFormulaCache) && attacker && attacker->is_player())
		{
			const int32_t stat = static_cast<int32_t>(attacker->getPlayer()->getLevel())
			                   + static_cast<int32_t>(attacker->getPlayer()->getMagicLevel()) * 3;
			computed_damage = static_cast<uint32_t>(std::abs(calculate_output(formulas.output, stat)));
		}

		if (apply_defense_reduction and target->can_use_defense()
			and (defense_cost == 0 or target->get_defense_charges() >= defense_cost))
		{
			if (defense_cost > 0)
				target->set_defense_charges(target->get_defense_charges() - defense_cost);

			int32_t defense_value = 0;
			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Defense)) : nullptr)
			{
				ctx.pipeline_a = target->getDefense();
				defense_value = static_cast<int32_t>((*fn)(ctx));
			}
			else
			{
				defense_value = calculate_resistance(formulas.defense, target->getDefense());
			}

			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Resolution)) : nullptr)
			{
				ctx.pipeline_a = static_cast<int32_t>(computed_damage);
				ctx.pipeline_b = defense_value;
				computed_damage = static_cast<uint32_t>((*fn)(ctx));
			}
			else
			{
				computed_damage = static_cast<uint32_t>(calculate_resolution(formulas.resolution, static_cast<int32_t>(computed_damage), defense_value));
			}

			if (computed_damage == 0)
			{
				blockType = BlockType::Defensive;
				apply_armor_reduction = false;
			}
		}

		if (apply_armor_reduction and (armor_cost == 0 or target->get_armor_charges() >= armor_cost))
		{
			if (armor_cost > 0)
				target->set_armor_charges(target->get_armor_charges() - armor_cost);

			int32_t armor_value = 0;
			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Armor)) : nullptr)
			{
				ctx.pipeline_a = target->getArmor();
				armor_value = static_cast<int32_t>((*fn)(ctx));
			}
			else
			{
				armor_value = calculate_resistance(formulas.armor, target->getArmor());
			}

			if (const auto* fn = compiled ? compiled->get(sit_idx, static_cast<uint8_t>(FormulaStage::Resolution)) : nullptr)
			{
				ctx.pipeline_a = static_cast<int32_t>(computed_damage);
				ctx.pipeline_b = armor_value;
				computed_damage = static_cast<uint32_t>((*fn)(ctx));
			}
			else
			{
				computed_damage = static_cast<uint32_t>(calculate_resolution(formulas.resolution, static_cast<int32_t>(computed_damage), armor_value));
			}

			if (computed_damage == 0)
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

		if (blockType != BlockType::NoBlock)
			return std::unexpected(blockType);
		return computed_damage;
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
			default: [[unlikely]] return 0;
		}
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

		const bool send_notice_effect = (notice.effect != CONST_ME_NONE);
		const bool send_origin_effect = (origin_notice.effect != CONST_ME_NONE);

		auto observer_view = *spectators | std::views::filter(is_observer);
		for (const auto& spectator : observer_view)
		{
			auto* player = static_cast<Player*>(spectator.get());
			player->sendTextMessage(observer_message);
			if (send_notice_effect)
				player->sendMagicEffect(target_position, notice.effect);
			if (send_origin_effect)
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

		const bool send_origin_effect_mana = (origin_notice.effect != CONST_ME_NONE);

		auto observer_view = *spectators | std::views::filter(is_observer);
		for (const auto& spectator : observer_view)
		{
			auto* player = static_cast<Player*>(spectator.get());
			player->sendTextMessage(observer_message);
			player->sendMagicEffect(defender_position, CONST_ME_LOSEENERGY);
			if (send_origin_effect_mana)
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

		const bool send_dmg_notice_effect  = (notice.effect != CONST_ME_NONE);
		const bool send_dmg_origin_effect  = (origin_notice.effect != CONST_ME_NONE);

		auto observer_view = *spectators | std::views::filter(is_observer);
		for (const auto& spectator : observer_view)
		{
			auto* player = static_cast<Player*>(spectator.get());
			player->sendTextMessage(observer_message);
			if (send_dmg_notice_effect)
				player->sendMagicEffect(defender_position, notice.effect);
			if (send_dmg_origin_effect)
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

	uint32_t Combat::apply_damage(const CreaturePtr& attacker, const PlayerPtr& target, uint32_t currentDamage, std::optional<std::span<const CreaturePtr>> pre_cache) const noexcept
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
			damage_dealt = std::min(currentDamage, target_mana);
			target->drainMana(attacker, damage_dealt);

			if (not pre_cache) {
				owned_spectators = g_game.map.fetchSpectators(target->getPosition(), true, true);
				pre_cache.emplace(owned_spectators.begin(), owned_spectators.end());
			}

			manadamage_notification(attacker, target, damage_dealt, pre_cache);

			if (manadrain or damage_dealt == currentDamage)
				return damage_dealt;
		}

		damage_limit -= damage_dealt;

		if (currentDamage > damage_dealt)
		{
			const auto health_changed = std::min(currentDamage - damage_dealt, damage_limit);

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

	uint32_t Combat::apply_damage(const CreaturePtr& attacker, const MonsterPtr& target, uint32_t currentDamage, const std::optional<std::span<const CreaturePtr>> spectators) const noexcept
	{
		if (damage_type == Combat::DamageType::ManaDrain)
			return 0;

		const auto target_health  = static_cast<uint32_t>(target->getHealth());
		const auto target_position = target->getPosition();
		const auto health_changed  = std::min(currentDamage, target_health);

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
	if (id == ITEM_MAGICWALL or id == ITEM_WILDGROWTH or id == ITEM_MAGICWALL_SAFE or id == ITEM_WILDGROWTH_SAFE or isBlocking())
	{
		if (not creature->isInGhostMode())
		{
			g_game.internalRemoveItem(getItem(), 1);
		}

		return;
	}

	//remove magic walls/wild growth (only nopvp tiles/world)
	if (id == ITEM_MAGICWALL_NOPVP or id == ITEM_WILDGROWTH_NOPVP)
	{
		if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or getTile()->hasFlag(TILESTATE_NOPVPZONE))
		{
			g_game.internalRemoveItem(getItem(), 1);
		}
		return;
	}

	const ItemType& it = items[getID()];
	if (it.conditionDamage)
	{
		auto conditionCopy = it.conditionDamage->clone();
		const uint32_t ownerId = getOwner();
		if (ownerId)
		{
			bool harmfulField = true;

			if (g_game.getWorldType() == WORLD_TYPE_NO_PVP or getTile()->hasFlag(TILESTATE_NOPVPZONE))
			{
				if (const auto& owner = g_game.getCreatureByID(ownerId))
				{
					if (owner->getPlayer() or (owner->isSummon() and owner->getMaster()->getPlayer()))
					{
						harmfulField = false;
					}
				}
			}

			if (const auto& targetPlayer = creature->getPlayer())
			{
				if (const auto& attackerPlayer = g_game.getPlayerByID(ownerId))
				{
					if (Combat::isProtected(attackerPlayer, targetPlayer))
					{
						harmfulField = false;
					}
				}
			}

			if (not harmfulField or (OTSYS_TIME() - createTime <= 5000) or creature->hasBeenAttacked(ownerId))
			{
				conditionCopy->setParam(CONDITION_PARAM_OWNER, ownerId);
			}
		}

		creature->addCondition(std::move(conditionCopy));
	}
}