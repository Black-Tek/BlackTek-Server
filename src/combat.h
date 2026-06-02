// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "thing.h"
#include "condition.h"
#include "map.h"
#include "tools.h"
#include "matrixarea.h"
#include "damagemodifier.h"
#include "creature.h"
#include "intrusive.h"

#include <utility>
#include <optional>
#include <expected>
#include <functional>
#include <memory>
#include <span>
#include <gtl/phmap.hpp>
#include <bitset>
#include <memory_resource>
#include <array>

class Condition;
class Item;
struct Position;

namespace BlackTek 
{
	PlayerPtr PlayerCast(auto creature)
	{
		return std::static_pointer_cast<Player>(creature);
	}

	MonsterPtr MonsterCast(auto creature)
	{
		return std::static_pointer_cast<Monster>(creature);
	}

	namespace Constant
	{
		auto constexpr Player_Vs_Player = (static_cast<uint32_t>(CreatureSubType::Player) << 16 | static_cast<uint32_t>(CreatureSubType::Player) << 8);
		auto constexpr Player_Vs_Monster = (static_cast<uint32_t>(CreatureSubType::Player) << 16 | static_cast<uint32_t>(CreatureSubType::Monster) << 8);
		auto constexpr Monster_Vs_Player = (static_cast<uint32_t>(CreatureSubType::Monster) << 16 | static_cast<uint32_t>(CreatureSubType::Player) << 8);
		auto constexpr Monster_Vs_Monster = (static_cast<uint32_t>(CreatureSubType::Monster) << 16 | static_cast<uint32_t>(CreatureSubType::Monster) << 8);
	}

	struct LeechData
	{
		int32_t percent_health = 0;
		int32_t percent_mana = 0;
		int32_t percent_stamina = 0;
		int32_t percent_soul = 0;
		int32_t flat_health = 0;
		int32_t flat_mana = 0;
		int32_t flat_stamina = 0;
		int32_t flat_soul = 0;
	};

	struct DamageArea
	{
		std::vector<Position> positions;
		int32_t distance;
		int32_t max_extent = 0;
	};

	// Pre-rotated offset from a target position.  After baking direction into the
	// stored values: spread = dx (east positive), forward = dy (south positive).
	// Apply directly: world = { target.x + spread, target.y + forward }.
	struct DamageLocation
	{
		int16_t spread;  // dx — east positive, west negative
		int16_t forward; // dy — south positive, north negative
	};

	// Holds four pre-rotated DamageLocation sets, one per cardinal direction.
	// Indexed by Direction enum: NORTH=0, EAST=1, SOUTH=2, WEST=3.
	// Built once at area-setup time; execution just picks the slot and iterates.
	struct CombatArea
	{
		std::array<std::vector<DamageLocation>, 4> directions;
		int32_t max_extent = 0; // max(abs(spread), abs(forward)) across all directions — pre-computed for spectator radius
	};


	class AreaCombat
	{
	public:
		void setupArea(const std::vector<uint32_t>& vec, uint32_t rows);
		void setupExtArea(const std::vector<uint32_t>& vec, uint32_t rows);

		[[nodiscard]] CombatArea GetCombatArea() const;
		[[nodiscard]] CombatArea GetExtCombatArea() const;

	private:
		std::vector<MatrixArea> areas;
		bool hasExtArea = false;
	};


	using RawArea = std::vector<uint32_t>;
	using RawAreaVec = std::vector<RawArea>;
	using DeflectionEffectMap = gtl::flat_hash_map<int, RawAreaVec>;
	using DeflectAreaMap = gtl::flat_hash_map<Direction, const DeflectionEffectMap>;


	// Todo: Create a struct for "CombatTable" which will be a specific defined and handled lua table able to be passed for construction of combat objects

	struct SituationFormulas;
	enum class FormulaStage : uint8_t;
	struct FormulaContext;
	struct CompiledFormulaSlots;
	struct CombatFormulaCache;

	class Combat;
	using CombatHandle = intrusive_ptr<Combat>;
	using CompiledFormula = std::function<double(const FormulaContext&)>;

	// Holds both cardinal and diagonal CombatArea data for one Combat object.
	// Stored by value in combat_area_pair_map; cached_areas on Combat points into it.
	struct CombatAreaPair
	{
		CombatArea area;
		CombatArea ext_area;
	};

	extern std::pmr::unordered_map<int64_t, CombatAreaPair> combat_area_pair_map;

	class Combat
	{
	public:
		Combat() = default;
		~Combat();

		// non-copyable
		Combat(const Combat&) = delete;
		Combat& operator=(const Combat&) = delete;

		enum Origin : uint8_t
		{
			None,
			Condition,
			Spell,
			Melee,
			Ranged,
			Fist,
			Sword,
			Axe,
			Club,
			Wand,
			Rod,
			Bow,
			Crossbow,
			Throwable, // stars, knives, stones, spears, ect.
			Augment,
			Absorb,
			Restore,
			Replenish,
			Revive,
			Reflect,
			Deflect,
			Ricochet,
			Piercing,
			LifeSteal,
			ManaSteal,
			StaminaSteal,
			SoulSteal,

			Origins = 27
		};

		enum DamageType : uint16_t
		{
			Unknown = 0,

			Physical = 1 << 0,
			Energy = 1 << 1,
			Earth = 1 << 2,
			Fire = 1 << 3,
			Undefined = 1 << 4,
			LifeDrain = 1 << 5,
			ManaDrain = 1 << 6,
			Healing = 1 << 7,
			Water = 1 << 8,
			Ice = 1 << 9,
			Holy = 1 << 10,
			Death = 1 << 11,

			DamageTypes = 12
		};

		enum BlockType : uint8_t 
		{
			NoBlock,
			Defensive,
			Armor,
			Immunity,
			Dodge
		};

		enum Situation : uint8_t
		{
			PlayerVsPlayer = 1 << 0,
			PlayerVsMonster = 1 << 1,
			MonsterVsPlayer = 1 << 2,
			MonsterVsMonster = 1 << 3
		};

		enum class BindSource : uint8_t
		{
			Caster,
			Target
		};

		enum class BindKey : uint8_t
		{
			Level,
			MagicLevel,
			SkillLevel,
			Attack,
			Defense,
			WeaponAttack,
			WeaponDefense,
			Health,
			MaxHealth,
			Mana,
			MaxMana,
			Soul,
			MaxSoul,
			Stamina,
		};

		// ── Damage Output ────────────────────────────────────────────────────────
		// How an attacker's stat translates into raw damage before any mitigation.
		// Plug in the relevant stat (e.g. level*2 + magLevel*3 in Classic mode) at call time.

		enum class OutputFormula : uint8_t
		{
			Flat,          // normal_random(min_value, max_value) — classic ARPG flat roll
			Linear,        // stat * scaling + base — deterministic AD/AP scaling
			LinearRange,   // normal_random(stat*min_scale+min_base, stat*max_scale+max_base) — Classic
			Power,         // stat^exponent * scaling + base — super-linear RPG scaling
		};

		struct [[nodiscard]] OutputFactors
		{
			float scaling   = 1.0f;    // Linear / Power: multiplier on stat
			float base      = 0.0f;    // Linear / Power: flat addition
			float min_scale = 0.0f;    // LinearRange: lower-bound stat multiplier
			float min_base  = 0.0f;    // LinearRange: lower-bound flat
			float max_scale = 1.0f;    // LinearRange: upper-bound stat multiplier
			float max_base  = 0.0f;    // LinearRange: upper-bound flat
			float min_value = 0.0f;    // Flat: lower bound
			float max_value = 1.0f;    // Flat: upper bound
			float exponent  = 1.0f;    // Power: exponent applied to stat before scaling
			OutputFormula formula_type = OutputFormula::Linear;
		};

		// ── Damage Resistance ─────────────────────────────────────────────────────
		// How a defender's stat translates into a resistance value.
		// The meaning of the returned int depends on the paired ResolutionFormula.

		enum class ResistanceFormula : uint8_t
		{
			Identity,      // returns stat unchanged — resolution formula handles mitigation
			LinearRandom,  // uniform_random(stat*min_scale+min_base, stat*max_scale+max_base) — Classic defense
			Parity,        // Classic armor: stat>threshold → range; else flat
			Percent,       // (stat*100)/(stat+constant) → integer [0–100] for Layered resolution
		};

		struct [[nodiscard]] ResistanceFactors
		{
			float min_scale      = 0.5f;   // LinearRandom / Parity: lower-bound stat multiplier
			float max_scale      = 1.0f;   // LinearRandom: upper-bound stat multiplier
			float min_base       = 0.0f;   // LinearRandom: lower-bound flat
			float max_base       = 0.0f;   // LinearRandom: upper-bound flat
			float threshold      = 0.0f;   // Parity: stat must exceed this for range formula
			float flat_amount    = 1.0f;   // Parity / fallback: flat resistance when below threshold
			float constant       = 100.0f; // Percent: denominator addend — percent = stat*100/(stat+constant)
			uint8_t parity_offset = 0;     // Parity: hi = stat - (stat%2 + parity_offset)
			ResistanceFormula formula_type = ResistanceFormula::Identity;
		};

		// ── Damage Resolution ─────────────────────────────────────────────────────
		// How raw output and resistance combine into final damage.
		// output = calculate_output result; resistance = calculate_resistance result.

		enum class ResolutionFormula : uint8_t
		{
			Subtractive,      // max(floor, output - resistance) — Classic; output minus resistance, floored
			RatioMitigation,  // output * constant / (constant + resistance) — Scaled; ratio mitigation
			ScaledDivision,   // (output * multiplier / resistance) + addend — Balanced; division with addend
			Layered,          // max(floor, output*(1 - resistance/100) - flat_reduction) — Absorption; layered mitigation
		};

		struct [[nodiscard]] ResolutionFactors
		{
			float constant       = 100.0f; // RatioMitigation: K in output*K/(K+resistance)
			float multiplier     = 1.0f;   // ScaledDivision: scales output before dividing by resistance
			float addend         = 0.0f;   // ScaledDivision: bonus added after division
			float flat_reduction = 0.0f;   // Layered: subtracted after percent mitigation
			float floor          = 0.0f;   // Subtractive / Layered: minimum final damage
			ResolutionFormula formula_type = ResolutionFormula::Subtractive;
		};

		enum TargetCode : uint8_t
		{
			UnknownFailure,
			Valid,
			YouAreInProtectionZone,
			CanNotAttackThisPlayer,
			TargetIsInProtectionZone,
			CanNotAttackThisMonster,
			PlayerIsUnMarked,
			CanNotWieldWeapon,
			NotEnoughRoom,
			FirstGoUpStairs,
			FirstGoDownStairs,
		};

		enum Config : uint8_t
		{
			PlaceHolder,		// equal to None
			TrueDamage,			// piercing is this by default, but combat can have it assigned
			BlockedByArmor,
			BlockedByDefense,
			TopTargetOnly,		// without this it does all creatures in a tile stack
			SelfOnly,			// a combat which only affects the user
			FriendlyParty,		// using this enables party only combat objects for the casters party
			EnemyParty,			// same as above but for target's party
			FraggedOnly,		// using this targets only people with skulls
			MultiLevel,			// hits across multiple z-levels; floors with ground items act as barriers
			IgnoreGround,		// only meaningful with MultiLevel: skip the ground-barrier check and hit all floors in the range
			Aggressive,
			IgnoreBarriers,		// ignores walls for line of sight and such on the combat
			UseCharges,			// I think this one is used by weapons to reduce ammo
			HasArea,            // set by setArea() when any area data is registered
			HasExtArea,         // set by setArea() when diagonal (ext) area data is non-empty
			HasFormulaCache,    // set when formula_cache is first allocated (any formula override)
			HasCompiledFormula, // set when formula_cache->compiled is allocated (Lua callbacks)
			Critical,
			Leech,
			AttackModified,
			DefenseModified,	// don't think we need this one, but just in case..
			IsUtility,
			HealthTarget,		// we use these stat based targets along with "healing" type, to determine which stat to heal, this one is hp
			ManaTarget,			// and this one is mana
			SoulTarget,			// soul is healed by this one
			StaminaTarget,		// and yes we even heal stamina if you want
			HasCondition,
			HasPvPFormula,
			HasPvMFormula,
			HasMvPFormula,
			HasMvMFormula,
			HasStages,			// this can come later, but the idea is to reduce the need of creating 10 combats per spell just to get it to do cool things..
			SecondaryAttack,	// set when this combat originates from the off-hand weapon in a dual wield attack
			IsReflect,			// combat was spawned by a reflect modifier — suppresses further bounce triggers
			IsDeflect,			// combat was spawned by a deflect modifier — suppresses further bounce triggers
			IsRicochet,			// combat was spawned by a ricochet modifier — suppresses further bounce triggers
		};


		// ── Classic ──────────────────────────────────────────────────────────────
		// stat = level * 2 + magicLevel * 3; output rolls from 0 to stat
		static constexpr OutputFactors ClassicOutput
		{
			.min_scale = 0.0f,
			.max_scale = 1.0f,
			.formula_type = OutputFormula::LinearRange
		};

		// uniform_random(defense / 2, defense)
		static constexpr ResistanceFactors ClassicDefense
		{
			.min_scale = 0.5f,
			.max_scale = 1.0f,
			.formula_type = ResistanceFormula::LinearRandom
		};

		// stat > 3: uniform_random(stat / 2, stat - (stat % 2 + 1)); stat in [1, 3]: flat 1
		static constexpr ResistanceFactors ClassicArmor
		{
			.min_scale    = 0.5f,
			.threshold    = 3.0f,
			.flat_amount  = 1.0f,
			.parity_offset = 1,
			.formula_type = ResistanceFormula::Parity
		};

		// max(0, output - resistance_roll)
		static constexpr ResolutionFactors ClassicResolution
		{
			.floor = 0.0f,
			.formula_type = ResolutionFormula::Subtractive
		};

		// ── Scaled ────────────────────────────────────────────────────────────
		// stat = attack value; output = base + stat * scaling
		static constexpr OutputFactors ScaledOutput
		{
			.scaling = 1.0f,
			.formula_type = OutputFormula::Linear
		};

		// armor / defense passed through unchanged; ratio applied in resolution
		static constexpr ResistanceFactors ScaledResistance
		{
			.formula_type = ResistanceFormula::Identity
		};

		// output * 100 / (100 + armor)
		static constexpr ResolutionFactors ScaledResolution
		{
			.constant = 100.0f,
			.formula_type = ResolutionFormula::RatioMitigation
		};

		// ── Balanced ──────────────────────────────────────────────────────────
		// stat = pre-computed attack component (level, power, attack folded in by caller)
		static constexpr OutputFactors BalancedOutput
		{
			.scaling = 1.0f,
			.formula_type = OutputFormula::Linear
		};

		// defense stat passed through; ScaledDivision divides output by it
		static constexpr ResistanceFactors BalancedResistance
		{
			.formula_type = ResistanceFormula::Identity
		};

		// (output / defense) + 2
		static constexpr ResolutionFactors BalancedResolution
		{
			.multiplier = 1.0f,
			.addend     = 2.0f,
			.formula_type = ResolutionFormula::ScaledDivision
		};

		// ── Absorption ────────────────────────────────────────────────────────
		// weapon AR rolled flat; set min_value / max_value per weapon at call time
		static constexpr OutputFactors AbsorptionOutput
		{
			.min_value = 0.0f,
			.max_value = 1.0f,
			.formula_type = OutputFormula::Flat
		};

		// absorption % = stat * 100 / (stat + 100)
		static constexpr ResistanceFactors AbsorptionResistance
		{
			.constant = 100.0f,
			.formula_type = ResistanceFormula::Percent
		};

		// max(1, output * (1 - absorption / 100))
		static constexpr ResolutionFactors AbsorptionResolution
		{
			.floor = 1.0f,
			.formula_type = ResolutionFormula::Layered
		};

		// ── Tabletop ──────────────────────────────────────────────────────────
		// stat = ability modifier; normal_random(stat + 1, stat + die_max)
		// Both bounds shift with the modifier — unlike Classic where stat IS the ceiling.
		// Override max_base per weapon (8 = d8, 12 = d12, 6 = d6, etc.)
		static constexpr OutputFactors TabletopOutput
		{
			.min_scale = 1.0f,
			.min_base  = 1.0f,
			.max_scale = 1.0f,
			.max_base  = 8.0f,
			.formula_type = OutputFormula::LinearRange
		};

		// ── Exponential / Endgame RPG ─────────────────────────────────────────
		// stat^2 * scaling — quadratic growth rewards heavy stat investment.
		// Useful for high-end spells or ability-tree systems.
		static constexpr OutputFactors ExponentialOutput
		{
			.scaling  = 1.0f,
			.exponent = 2.0f,
			.formula_type = OutputFormula::Power
		};

		// ── Proportional ──────────────────────────────────────────────────────
		// stat = pre-computed (effective_attack * motion_value); resistance = effective_defense
		// Pure division with no addend, unlike Balanced which adds 2 after dividing.
		static constexpr ResolutionFactors ProportionalResolution
		{
			.multiplier = 1.0f,
			.addend     = 0.0f,
			.formula_type = ResolutionFormula::ScaledDivision
		};

		// ── Gradual ───────────────────────────────────────────────────────────
		// output * 950 / (950 + DEF)
		// Same shape as Scaled but the 10x larger constant means mitigation grows
		// much more slowly; a character needs ~950 DEF to reach 50% reduction.
		static constexpr ResolutionFactors GradualResolution
		{
			.constant = 950.0f,
			.formula_type = ResolutionFormula::RatioMitigation
		};


		static bool isProtected(const PlayerConstPtr& attacker, const PlayerConstPtr& target);
		static void postCombatEffects(const CreaturePtr& caster, const Position& pos, const Combat& combat);
		static void addDistanceEffect(const CreaturePtr& caster, const Position& fromPos, const Position& toPos, uint8_t effect);

		[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const PlayerPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const MonsterPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const MonsterPtr& attacker, const PlayerPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const MonsterPtr& attacker, const MonsterPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const CreaturePtr& attacker, const CreaturePtr& defender) const noexcept;
		[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const Position& target_location) const noexcept;
		[[nodiscard]] TargetCode target(const MonsterPtr& attacker, const Position& target_location) const noexcept;
		[[nodiscard]] CombatHandle transformDamage(const uint16_t damage_type, const uint32_t amount) noexcept;
		template <std::ranges::input_range R>
			requires std::same_as<std::ranges::range_value_t<R>, DamageModifier>
		[[nodiscard]] uint32_t handle_conversion(R&& modifiers, const CreaturePtr& attacker, const CreaturePtr& victim, uint32_t currentDamage, std::optional<std::span<const CreaturePtr>> spectators = std::nullopt);
		[[nodiscard]] std::pair<CombatHandle, uint32_t> penetrateDamage(uint32_t currentDamage, uint32_t percent, uint32_t flat) noexcept;
		[[nodiscard]] CombatHandle clone() const noexcept;

		[[nodiscard]] uint32_t applyCrit(uint32_t currentDamage, uint32_t percent, uint32_t flat) noexcept;
		[[nodiscard]] uint32_t process_steal(const PlayerPtr& caster, const CreaturePtr& victim, const LeechData& steal, uint32_t currentDamage) noexcept;
		void post_damage(const PlayerPtr& caster, const CreaturePtr& victim, uint32_t currentDamage, LeechData&& leech_data) noexcept;
		void strike_target(const PlayerPtr& caster, const PlayerPtr& victim, bool skip_validation = false, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;
		void strike_target(const PlayerPtr& caster, const MonsterPtr& victim, bool skip_validation = false, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;
		void strike_target(const MonsterPtr& attacker, const PlayerPtr& victim, bool skip_validation = false, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;
		void strike_target(const MonsterPtr& attacker, const MonsterPtr& victim, bool skip_validation = false, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;
		void strike_target(const CreaturePtr& attacker, const CreaturePtr& defender, bool skip_validation = false, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;
		void apply_healing_modifiers(const PlayerPtr& caster, const auto& target);
		void heal_target(const CreaturePtr& caster, const CreaturePtr& target, bool skip_validation = false, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;
		void execute(const CreaturePtr& caster, const Position& center) noexcept;
		void setArea(AreaCombat* area);
		void setArea(std::unique_ptr<AreaCombat> const area);
		void defense_block_effect(const Position& target_position) const noexcept;
		void armor_block_effect(const Position& target_position) const noexcept;
		void heal_notification(const CreaturePtr& caster, const CreaturePtr& target, uint32_t amount, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) const noexcept;
		void damage_notification(const CreaturePtr& attacker, const CreaturePtr& defender, uint32_t amount, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) const noexcept;
		void manadamage_notification(const CreaturePtr& attacker, const CreaturePtr& defender, uint32_t amount, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) const noexcept;

		struct OriginNotice
		{
			MagicEffectClasses effect = CONST_ME_NONE;
			std::string_view verb     = "an attack";
		};

		[[nodiscard]] uint32_t collect_notice_data(const CreaturePtr& target) const noexcept;
		[[nodiscard]] uint32_t collect_heal_notice_data() const noexcept;
		[[nodiscard]] static constexpr OriginNotice collect_origin_notice(Origin o) noexcept;
		[[nodiscard]] uint8_t immunity_block_effect() const noexcept;
		[[nodiscard]] std::expected<uint32_t, BlockType> block(const CreaturePtr& attacker, const PlayerPtr& target) noexcept;
		[[nodiscard]] std::expected<uint32_t, BlockType> block(const CreaturePtr& attacker, const MonsterPtr& target) noexcept;
		[[nodiscard]] uint32_t apply_damage(const CreaturePtr& attacker, const PlayerPtr& target, uint32_t currentDamage, std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) const noexcept;
		[[nodiscard]] uint32_t apply_damage(const CreaturePtr& attacker, const MonsterPtr& target, uint32_t currentDamage, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) const noexcept;

		[[nodiscard]]
		inline bool hasArea() const noexcept
		{
			return cached_areas != nullptr;
		}

		[[nodiscard]] static int32_t calculate_output(const OutputFactors& factors, int32_t stat) noexcept
		{
			switch (factors.formula_type)
			{
				case OutputFormula::Flat:
					return normal_random(static_cast<int32_t>(factors.min_value), static_cast<int32_t>(factors.max_value));

				case OutputFormula::Linear:
					return static_cast<int32_t>(std::max(0.0f, static_cast<float>(stat) * factors.scaling + factors.base));

				case OutputFormula::LinearRange:
				{
					int32_t lo = static_cast<int32_t>(static_cast<float>(stat) * factors.min_scale + factors.min_base);
					int32_t hi = static_cast<int32_t>(static_cast<float>(stat) * factors.max_scale + factors.max_base);
					return normal_random(lo, hi);
				}

				case OutputFormula::Power:
				{
					float powered = factors.exponent != 1.0f
						? std::pow(static_cast<float>(stat), factors.exponent)
						: static_cast<float>(stat);
					return static_cast<int32_t>(std::max(0.0f, powered * factors.scaling + factors.base));
				}

				default: [[unlikely]]
					return 0;
			}
		}

		[[nodiscard]] static int32_t calculate_resistance(const ResistanceFactors& factors, int32_t stat) noexcept
		{
			if (stat <= 0)
				return 0;

			switch (factors.formula_type)
			{
				case ResistanceFormula::Identity:
					return stat;

				case ResistanceFormula::LinearRandom:
				{
					int32_t lo = static_cast<int32_t>(static_cast<float>(stat) * factors.min_scale + factors.min_base);
					int32_t hi = static_cast<int32_t>(static_cast<float>(stat) * factors.max_scale + factors.max_base);
					return uniform_random(lo, hi);
				}

				case ResistanceFormula::Parity:
				{
					if (stat > static_cast<int32_t>(factors.threshold))
					{
						int32_t lo = static_cast<int32_t>(static_cast<float>(stat) * factors.min_scale);
						int32_t hi = stat - (stat % 2 + static_cast<int32_t>(factors.parity_offset));
						return uniform_random(lo, hi);
					}
					return static_cast<int32_t>(factors.flat_amount);
				}

				case ResistanceFormula::Percent:
					return (stat * 100) / (stat + static_cast<int32_t>(factors.constant));

				default: [[unlikely]]
					return 0;
			}
		}

		[[nodiscard]] static constexpr int32_t calculate_resolution(const ResolutionFactors& factors, int32_t output, int32_t resistance) noexcept
		{
			switch (factors.formula_type)
			{
				case ResolutionFormula::Subtractive:
					return static_cast<int32_t>(std::max(factors.floor, static_cast<float>(output - resistance)));

				case ResolutionFormula::RatioMitigation:
				{
					float r = resistance < 0 ? 0.0f : static_cast<float>(resistance);
					return static_cast<int32_t>(std::max(factors.floor, static_cast<float>(output) * factors.constant / (factors.constant + r)));
				}

				case ResolutionFormula::ScaledDivision:
					if (resistance <= 0) [[unlikely]]
						return static_cast<int32_t>(std::max(factors.floor, static_cast<float>(output) * factors.multiplier + factors.addend));
					return static_cast<int32_t>(std::max(factors.floor, static_cast<float>(output) * factors.multiplier / static_cast<float>(resistance) + factors.addend));

				case ResolutionFormula::Layered:
				{
					float mitigated = static_cast<float>(output) * (1.0f - static_cast<float>(resistance) / 100.0f);
					return static_cast<int32_t>(std::max(factors.floor, mitigated - factors.flat_reduction));
				}

				default: [[unlikely]]
					return 0;
			}
		}


		void postCombatEffects(CreaturePtr& caster, const Position& pos) const { postCombatEffects(caster, pos, *this); }
		void setOrigin(Origin o) { origin = o; }

		void SetDamageType(uint16_t type) noexcept { damage_type = type; }
		void SetDamage(uint32_t dmg) noexcept { damage = dmg; }
		void SetImpactEffect(uint8_t effect) noexcept { impactEffect = effect; }
		void SetDistanceEffect(uint8_t effect) noexcept { distanceEffect = effect; }
		void SetBlockType(uint8_t btype) noexcept { blockType = btype; }
		void SetItemId(uint16_t item) noexcept { itemId = item; }
		void SetConfig(Config flag, bool value = true) noexcept { config.set(flag, value); }
		[[nodiscard]] bool GetConfig(Config flag) const noexcept { return config.test(flag); }
		[[nodiscard]] uint16_t GetDamageType() const noexcept { return damage_type; }
		[[nodiscard]] uint32_t GetDamage() const noexcept { return damage; }
		[[nodiscard]] uint8_t GetImpactEffect() const noexcept { return impactEffect; }
		[[nodiscard]] uint8_t GetDistanceEffect() const noexcept { return distanceEffect; }
		[[nodiscard]] uint8_t GetBlockType() const noexcept { return blockType; }
		[[nodiscard]] uint16_t GetItemId() const noexcept { return itemId; }

		// ── Semantic Config flag wrappers ─────────────────────────────────────────
		// Setters
		void setTrueDamage(bool v)      noexcept { SetConfig(Config::TrueDamage, v);       }
		void setBlockedByArmor(bool v)  noexcept { SetConfig(Config::BlockedByArmor, v);   }
		void setBlockedByShield(bool v) noexcept { SetConfig(Config::BlockedByDefense, v); }
		void setTopTargetOnly(bool v)   noexcept { SetConfig(Config::TopTargetOnly, v);    }
		void setSelfOnly(bool v)        noexcept { SetConfig(Config::SelfOnly, v);         }
		void setFriendlyParty(bool v)   noexcept { SetConfig(Config::FriendlyParty, v);    }
		void setEnemyParty(bool v)      noexcept { SetConfig(Config::EnemyParty, v);       }
		void setFraggedOnly(bool v)     noexcept { SetConfig(Config::FraggedOnly, v);      }
		void setAggressive(bool v)      noexcept { SetConfig(Config::Aggressive, v);       }
		void setIgnoreBarriers(bool v)  noexcept { SetConfig(Config::IgnoreBarriers, v);   }
		void setUseCharges(bool v)      noexcept { SetConfig(Config::UseCharges, v);       }

		// Getters
		[[nodiscard]] bool isTrueDamage()      const noexcept { return GetConfig(Config::TrueDamage);       }
		[[nodiscard]] bool isBlockedByArmor()  const noexcept { return GetConfig(Config::BlockedByArmor);   }
		[[nodiscard]] bool isBlockedByShield() const noexcept { return GetConfig(Config::BlockedByDefense); }
		[[nodiscard]] bool isTopTargetOnly()   const noexcept { return GetConfig(Config::TopTargetOnly);    }
		[[nodiscard]] bool isSelfOnly()        const noexcept { return GetConfig(Config::SelfOnly);         }
		[[nodiscard]] bool isFriendlyParty()   const noexcept { return GetConfig(Config::FriendlyParty);    }
		[[nodiscard]] bool isEnemyParty()      const noexcept { return GetConfig(Config::EnemyParty);       }
		[[nodiscard]] bool isFraggedOnly()     const noexcept { return GetConfig(Config::FraggedOnly);      }
		[[nodiscard]] bool isAggressive()      const noexcept { return GetConfig(Config::Aggressive);       }
		[[nodiscard]] bool isIgnoreBarriers()  const noexcept { return GetConfig(Config::IgnoreBarriers);   }
		[[nodiscard]] bool usesCharges()       const noexcept { return GetConfig(Config::UseCharges);       }

		void AddCondition(ConditionHandle cond);
		void ClearConditions() noexcept;

		void SetSituationFormulas(uint8_t index, SituationFormulas&& formulas) noexcept;
		void RegisterCompiledFormula(uint8_t sit_idx, FormulaStage stage, CompiledFormula fn) noexcept;

		[[nodiscard]] static Position generateAttackPosition(const CreaturePtr& attacker, const PlayerPtr& defender) noexcept;
		[[nodiscard]] static std::unique_ptr<AreaCombat> generateDeflectArea(const CreaturePtr& attacker, const PlayerPtr& defender, int32_t targetCount) noexcept;
		static void getOpenPositionsInRadius(const PlayerPtr& defender, int radius, std::vector<Position>& out) noexcept;

		static void deflect_damage(const CreaturePtr& attacker, const PlayerPtr& defender, uint32_t amount, uint16_t damageType, uint8_t distanceEffect, uint8_t impactEffect) noexcept;

		static void ricochetDamage(const PlayerPtr& defender, uint32_t amount, uint16_t damageType, uint8_t distanceEffect, uint8_t impactEffect) noexcept;

		[[nodiscard]] int64_t id() const noexcept { return combat_id; }
		void set_id(int64_t new_id) { combat_id = new_id; }

		[[nodiscard]] static constexpr ConditionType_t DamageToConditionType(CombatType_t type) noexcept
		{
			switch (type)
			{
				case COMBAT_FIREDAMAGE:     return CONDITION_FIRE;
				case COMBAT_ENERGYDAMAGE:   return CONDITION_ENERGY;
				case COMBAT_EARTHDAMAGE:    return CONDITION_POISON;
				case COMBAT_DROWNDAMAGE:    return CONDITION_DROWN;
				case COMBAT_ICEDAMAGE:      return CONDITION_FREEZING;
				case COMBAT_HOLYDAMAGE:     return CONDITION_DAZZLED;
				case COMBAT_DEATHDAMAGE:    return CONDITION_CURSED;
				case COMBAT_PHYSICALDAMAGE: return CONDITION_BLEEDING;
				default:                    return CONDITION_NONE;
			}
		}

	private:
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles);
		const DamageArea getAreaPositions(const Position& casterPos, const Position& targetPos);

		// Ensures this combat has a unique ID usable as a formula map key.
		// Lua-managed combats start with combat_id == -1; this assigns a negative ID.
		int64_t ensureFormulaId() noexcept;

		[[nodiscard]] uint32_t reform_augment(const CreaturePtr& attacker, const PlayerPtr& victim, uint32_t currentDamage) noexcept;
		[[nodiscard]] uint32_t defense_augment(const CreaturePtr& attacker, const PlayerPtr& victim, uint32_t currentDamage, const std::optional<std::span<const CreaturePtr>> spectators = std::nullopt) noexcept;

		template <typename VictimT>
		void accumulate_attack_mods(const PlayerPtr& caster, const VictimT& victim, uint32_t& currentDamage, LeechData& leech_data, LeechData& steal_data, const std::optional<std::span<const CreaturePtr>>& spectators) noexcept;

		static constexpr Config k_formula_flags[4] = { Config::HasPvPFormula, Config::HasPvMFormula, Config::HasMvPFormula, Config::HasMvMFormula };

		int64_t combat_id = -1;
		std::bitset<64> config;
		CombatFormulaCache* formula_cache = nullptr;           // raw ptr into combat_formula_cache_map; not owned
		mutable const CombatAreaPair* cached_areas = nullptr; // raw ptr into combat_area_pair_map; set by setArea()
		std::unique_ptr<std::vector<ConditionHandle>> condition_list;

		mutable int32_t ref_count{ 0 };
		uint32_t damage = 0;

		uint16_t defense_charge_cost = 0;
		uint16_t armor_charge_cost = 0;
		uint16_t def_modifier_charge_cost = 0;
		uint16_t atk_modifier_charge_cost = 0;
		uint16_t itemId = 0;
		uint16_t damage_type = DamageType::Unknown;
		uint8_t blockType = BlockType::NoBlock;
		uint8_t origin = Origin::None;
		uint8_t impactEffect = CONST_ME_NONE;
		uint8_t distanceEffect = CONST_ANI_NONE;

		friend class CombatRegistry;
		friend void intrusive_ptr_add_ref(const Combat* p) noexcept;
		friend void intrusive_ptr_release(const Combat* p) noexcept;
	};

	inline void intrusive_ptr_add_ref(const Combat* p) noexcept
	{
		++p->ref_count;
	}

	void intrusive_ptr_release(const Combat* p) noexcept;

	class CombatRegistry
	{
		static constexpr std::size_t k_InitialBytes = 512 * 1024;
		static constexpr std::pmr::pool_options k_PoolOpts
		{
			.max_blocks_per_chunk = 128,
			.largest_required_pool_block = 256
		};

		alignas(std::max_align_t)
		std::array<std::byte, k_InitialBytes>    buffer_;
		std::pmr::monotonic_buffer_resource      upstream_;
		std::pmr::unsynchronized_pool_resource   pool_;
		std::pmr::unordered_map<int64_t, Combat> table_;
		int64_t                                  next_id_{ 1 };

	public:
		CombatRegistry()
			: upstream_(buffer_.data(), buffer_.size())
			, pool_(k_PoolOpts, &upstream_)
			, table_(&pool_)
		{
		}

		CombatRegistry(const CombatRegistry&) = delete;
		CombatRegistry& operator=(const CombatRegistry&) = delete;

		[[nodiscard]] CombatHandle Create();
		[[nodiscard]] CombatHandle Create(uint16_t type, uint32_t dmg);
		[[nodiscard]] CombatHandle Clone(const Combat& src) noexcept;

		void Release(int64_t id);

		[[nodiscard]] Combat* Get(int64_t id) noexcept;
		[[nodiscard]] const Combat* Get(int64_t id) const noexcept;
		[[nodiscard]] std::size_t	Size() const noexcept { return table_.size(); }

		// we need this so external PMR containers can share the same resource.
		// e.g. std::pmr::unordered_map<int64_t, AreaCombat>{ g_combat_registry.Allocator() }
		[[nodiscard]] std::pmr::memory_resource* Allocator() noexcept { return &pool_; }
	};
	extern CombatRegistry g_combat_registry;

	// ── Formula Override System ───────────────────────────────────────────────
	// Three-tier resolution per damage-block step:
	//   Level 0 — no config:      Classic defaults (zero overhead)
	//   Level 1 — TOML / Lua:     C++ factor params from formula map
	//   Level 2 — Lua callback:   full Lua function (registered per sit/stage)
	//
	// Situation indices: 0=PvP, 1=PvM, 2=MvP, 3=MvM

	struct SituationFormulas
	{
		Combat::OutputFactors     output     = Combat::ClassicOutput;
		Combat::ResistanceFactors defense    = Combat::ClassicDefense;
		Combat::ResistanceFactors armor      = Combat::ClassicArmor;
		Combat::ResolutionFactors resolution = Combat::ClassicResolution;

		static constexpr auto Total = 4;
	};

	enum class FormulaStage : uint8_t
	{
		Output     = 0,
		Defense    = 1,
		Armor      = 2,
		Resolution = 3
	};

	extern std::array<SituationFormulas, 4> g_default_situation_formulas;

	// ── Compiled Formula System ───────────────────────────────────────────────
	// Context passed to every compiled formula at execution time.
	// Stages that only need one or two values (reduction, resolution) read the
	// fields relevant to them; unused fields are left at their zero defaults.
	struct FormulaContext
	{
		CreaturePtr caster;
		CreaturePtr target;
		int32_t     pipeline_a = 0;  // output damage (resolution stage)
		int32_t     pipeline_b = 0;  // resistance value (resolution stage)
	};

	// Slot table: [situation_index 0-3][stage_index 0-3]
	struct CompiledFormulaSlots
	{
		CompiledFormula slots[4][4];
		std::bitset<16> active;

		[[nodiscard]] bool has(uint8_t sit, uint8_t stage) const noexcept
		{
			return active.test(static_cast<size_t>(sit) * 4 + stage);
		}

		void set(uint8_t sit, uint8_t stage, CompiledFormula fn) noexcept
		{
			slots[sit][stage] = std::move(fn);
			active.set(static_cast<size_t>(sit) * 4 + stage);
		}

		[[nodiscard]] const CompiledFormula* get(uint8_t sit, uint8_t stage) const noexcept
		{
			if (!has(sit, stage))
				return nullptr;
			return &slots[sit][stage];
		}
	};

	// Merges per-combat SituationFormulas overrides and compiled Lua formula callbacks
	// into one heap-allocated node. Combat::formula_cache points into this map.
	// unique_ptr nodes are heap-stable — the raw pointer cached on Combat never moves.
	struct CombatFormulaCache
	{
		std::array<SituationFormulas, 4>      situations{};  // defaults to Classic presets via SituationFormulas{}
		std::unique_ptr<CompiledFormulaSlots> compiled;
	};

	extern std::unordered_map<int64_t, std::unique_ptr<CombatFormulaCache>> combat_formula_cache_map;

	void ApplyOutputPreset(Combat::OutputFactors& out, std::string_view preset) noexcept;
	void ApplyDefensePreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept;
	void ApplyArmorPreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept;
	void ApplyResolutionPreset(Combat::ResolutionFactors& out, std::string_view preset) noexcept;
	void LoadFormulaDefaults(uint8_t sit_idx, std::string_view out_preset, std::string_view def_preset, std::string_view arm_preset, std::string_view res_preset) noexcept;

	// Used by FormulaNode closures built in Lua to read creature stats at combat time.
	int32_t resolve_bind_key(Combat::BindKey key, const CreaturePtr& creature) noexcept;

	class MagicField final : public Item
	{
	public:
		explicit MagicField(uint16_t type) : Item(type), createTime(OTSYS_TIME())
		{
			thing_subtype = ThingSubType::MagicField;
			item_subtype = ItemSubType::MagicField;
		}

		MagicFieldPtr getMagicField() override
		{
			return static_shared_this<MagicField>();
		}

		MagicFieldConstPtr getMagicField() const override
		{
			return static_shared_this<MagicField>();
		}

		bool isReplaceable() const
		{
			return Item::items[getID()].replaceable;
		}

		CombatType_t getCombatType() const
		{
			const ItemType& it = items[getID()];
			return it.combatType;
		}

		int32_t getDamage() const
		{
			const ItemType& it = items[getID()];
			if (it.conditionDamage)
			{
				return it.conditionDamage->getTotalDamage();
			}
			return 0;
		}
		void onStepInField(const CreaturePtr& creature);

	private:
		int64_t createTime;
	};
}