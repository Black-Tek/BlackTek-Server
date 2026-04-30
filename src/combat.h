// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "thing.h"
#include "condition.h"
#include "map.h"
#include "baseevents.h"
#include "tools.h"
#include "matrixarea.h"
#include "damagemodifier.h"
#include "creature.h"
#include "intrusive.h"

#include <utility>
#include <optional>
#include <functional>
#include <span>
#include <gtl/phmap.hpp>
#include <bitset>
#include <memory_resource>
#include <atomic>
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
	};


	class AreaCombat
	{
	public:
		void setupArea(const std::vector<uint32_t>& vec, uint32_t rows);
		void setupArea(int32_t length, int32_t spread);
		void setupArea(int32_t radius);
		void setupExtArea(const std::vector<uint32_t>& vec, uint32_t rows);
		const MatrixArea& getArea(const Position& centerPos, const Position& targetPos) const;

		[[nodiscard]] CombatArea GetCombatArea() const;
		[[nodiscard]] CombatArea GetExtCombatArea() const;

	private:
		std::vector<MatrixArea> areas;
		bool hasExtArea = false;
	};


	// Todo: Create a struct for "CombatTable" which will be a specific defined and handled lua table able to be passed for construction of combat objects

	// Forward declarations — full definitions appear after CombatRegistry.
	struct SituationFormulas;
	enum class FormulaStage : uint8_t;
	struct FormulaCallbacks;

	class Combat;
	using CombatHandle = intrusive_ptr<Combat>;

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
			Augment,
			Absorb,
			Restore,
			Reflect,
			Deflect,
			Ricochet,
			Piercing,

			Origins = 12
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

		enum Situaion : uint8_t
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
		// Plug in the relevant stat (e.g. level*2 + magLevel*3 for Tibia) at call time.

		enum class OutputFormula : uint8_t
		{
			Flat,          // normal_random(min_value, max_value) — Diablo / classic ARPG
			Linear,        // stat * scaling + base — deterministic, e.g. LoL AD/AP
			LinearRange,   // normal_random(stat*min_scale+min_base, stat*max_scale+max_base) — Tibia
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
			LinearRandom,  // uniform_random(stat*min_scale+min_base, stat*max_scale+max_base) — Tibia defense
			Parity,        // Tibia armor: stat>threshold → range; else flat
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
			Subtractive,      // max(floor, output - resistance) — Tibia / Fire Emblem
			RatioMitigation,  // output * constant / (constant + resistance) — LoL / WoW
			ScaledDivision,   // (output * multiplier / resistance) + addend — Pokémon
			Layered,          // max(floor, output*(1 - resistance/100) - flat_reduction) — Dark Souls
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
			Valid,
			YouAreInProtectionZone,
			CanNotAttackThisPlayer,
			TargetIsInProtectionZone,
			CanNotAttackThisMonster,
			PlayerIsUnMarked,
			CanNotWieldWeapon,
			NotEnoughRoom,
			FirstGoUpStairs,
			FirstGoDownStairs
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
			MultiLevel,			// this is a game changer, we will add after the main work is all done
			Aggressive,
			IgnoreBarriers,		// ignores walls for line of sight and such on the combat
			UseCharges,			// I think this one is used by weapons to reduce ammo
			HasArea,
			Critical,
			Leech,
			AttackModified,
			DefenseModified,	// don't think we need this one, but just in case..
			IsUtility,
			HasCondition,
			HasPvPFormula,
			HasPvMFormula,
			HasMvPFormula,
			HasMvMFormula,
			HasStages,			// this can come later, but the idea is to reduce the need of creating 10 combats per spell just to get it to do cool things..
		};


		// ── Tibia ────────────────────────────────────────────────────────────────
		// stat = level * 2 + magicLevel * 3; output rolls from 0 to stat
		static constexpr OutputFactors TibiaOutput
		{
			.min_scale = 0.0f,
			.max_scale = 1.0f,
			.formula_type = OutputFormula::LinearRange
		};

		// uniform_random(defense / 2, defense)
		static constexpr ResistanceFactors TibiaDefense
		{
			.min_scale = 0.5f,
			.max_scale = 1.0f,
			.formula_type = ResistanceFormula::LinearRandom
		};

		// stat > 3: uniform_random(stat / 2, stat - (stat % 2 + 1)); stat in [1, 3]: flat 1
		static constexpr ResistanceFactors TibiaArmor
		{
			.min_scale    = 0.5f,
			.threshold    = 3.0f,
			.flat_amount  = 1.0f,
			.parity_offset = 1,
			.formula_type = ResistanceFormula::Parity
		};

		// max(0, output - resistance_roll)
		static constexpr ResolutionFactors TibiaResolution
		{
			.floor = 0.0f,
			.formula_type = ResolutionFormula::Subtractive
		};

		// ── League of Legends ─────────────────────────────────────────────────
		// stat = AD or AP value; output = base + stat * scaling
		static constexpr OutputFactors LoLOutput
		{
			.scaling = 1.0f,
			.formula_type = OutputFormula::Linear
		};

		// armor / defense passed through unchanged; ratio applied in resolution
		static constexpr ResistanceFactors LoLResistance
		{
			.formula_type = ResistanceFormula::Identity
		};

		// output * 100 / (100 + armor)
		static constexpr ResolutionFactors LoLResolution
		{
			.constant = 100.0f,
			.formula_type = ResolutionFormula::RatioMitigation
		};

		// ── Pokémon ───────────────────────────────────────────────────────────
		// stat = pre-computed attack component (level, power, attack folded in by caller)
		static constexpr OutputFactors PokemonOutput
		{
			.scaling = 1.0f,
			.formula_type = OutputFormula::Linear
		};

		// defense stat passed through; ScaledDivision divides output by it
		static constexpr ResistanceFactors PokemonResistance
		{
			.formula_type = ResistanceFormula::Identity
		};

		// (output / defense) + 2
		static constexpr ResolutionFactors PokemonResolution
		{
			.multiplier = 1.0f,
			.addend     = 2.0f,
			.formula_type = ResolutionFormula::ScaledDivision
		};

		// ── Dark Souls ────────────────────────────────────────────────────────
		// weapon AR rolled flat; set min_value / max_value per weapon at call time
		static constexpr OutputFactors DarkSoulsOutput
		{
			.min_value = 0.0f,
			.max_value = 1.0f,
			.formula_type = OutputFormula::Flat
		};

		// absorption % = stat * 100 / (stat + 100)
		static constexpr ResistanceFactors DarkSoulsResistance
		{
			.constant = 100.0f,
			.formula_type = ResistanceFormula::Percent
		};

		// max(1, output * (1 - absorption / 100))
		static constexpr ResolutionFactors DarkSoulsResolution
		{
			.floor = 1.0f,
			.formula_type = ResolutionFormula::Layered
		};

		// ── D&D / Classic Tabletop ────────────────────────────────────────────
		// stat = ability modifier; normal_random(stat + 1, stat + die_max)
		// Both bounds shift with the modifier — unlike Tibia where stat IS the ceiling.
		// Override max_base per weapon (8 = d8, 12 = d12, 6 = d6, etc.)
		static constexpr OutputFactors DnDOutput
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

		// ── Monster Hunter ────────────────────────────────────────────────────
		// stat = pre-computed (effective_attack * motion_value); resistance = effective_defense
		// Pure division with no addend, unlike Pokémon which adds 2 after dividing.
		static constexpr ResolutionFactors MonsterHunterResolution
		{
			.multiplier = 1.0f,
			.addend     = 0.0f,
			.formula_type = ResolutionFormula::ScaledDivision
		};

		// ── Genshin Impact ────────────────────────────────────────────────────
		// output * 950 / (950 + DEF) 
		// Same shape as LoL but the 10x larger constant means mitigation grows
		// much more slowly; a character needs ~950 DEF to reach 50% reduction.
		static constexpr ResolutionFactors GenshinResolution
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
		[[nodiscard]] uint32_t handle_conversion(std::ranges::input_range auto&& modifiers, auto attacker, auto victim);
		[[nodiscard]] CombatHandle penetrateDamage(const uint32_t percent, const uint32_t flat) noexcept;
		[[nodiscard]] CombatHandle clone() const noexcept;

		void applyCrit(const uint32_t percent, const uint32_t flat);
		void post_damage(const PlayerPtr& caster, const CreaturePtr& victim, LeechData&& leech_data) noexcept;
		void strike_target(const PlayerPtr& caster, const PlayerPtr& victim) noexcept;
		void strike_target(const PlayerPtr& caster, const MonsterPtr& victim) noexcept;
		void strike_target(const MonsterPtr& attacker, const PlayerPtr& victim) noexcept;
		void strike_target(const MonsterPtr& attacker, const MonsterPtr& victim) noexcept;
		void strike_target(const CreaturePtr& attacker, const CreaturePtr& defender) noexcept;
		void execute(const CreaturePtr& caster, const Position& center) noexcept;
		[[nodiscard]] static bool sameInstance(const CreatureConstPtr& first, const CreatureConstPtr& second);
		void defense_block_effect(const Position& target_position) const noexcept;
		void armor_block_effect(const Position& target_position) const noexcept;
		void notify_players();
		void apply_mana_damage(const CreaturePtr& attacker, const PlayerPtr& target, bool manashield) noexcept;

		[[nodiscard]] uint32_t collect_notice_data(const CreaturePtr& target) const noexcept;
		[[nodiscard]] uint8_t immunity_block_effect() const noexcept;
		[[nodiscard]] BlockType block(const CreaturePtr& attacker, const PlayerPtr& target) noexcept;
		[[nodiscard]] BlockType block(const CreaturePtr& attacker, const MonsterPtr& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const PlayerPtr& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const MonsterPtr& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const Position& target_position) noexcept;

		[[nodiscard]]
		inline bool hasArea() const noexcept 
		{
			auto extIt = combat_ext_area_map.find(combat_id);
			if (extIt != combat_ext_area_map.end())
				return true;
			return false;
		}

		[[nodiscard]] static constexpr int32_t calculate_output(const OutputFactors& factors, int32_t stat) noexcept
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

		[[nodiscard]] static constexpr int32_t calculate_resistance(const ResistanceFactors& factors, int32_t stat) noexcept
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

		void SetSituationFormulas(uint8_t index, SituationFormulas&& formulas) noexcept;
		void SetFormulaCallback(uint8_t index, FormulaStage stage, int32_t lua_ref) noexcept;

		[[nodiscard]] int64_t id() const noexcept { return combat_id; }
		void set_id(int64_t new_id) { combat_id = new_id; }

	private:
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, const TilePtr& tile);
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles);
		const DamageArea getAreaPositions(const Position& casterPos, const Position& targetPos);

		// Ensures this combat has a unique ID usable as a formula/callback map key.
		// Lua-managed combats start with combat_id == -1; this assigns a negative ID.
		int64_t EnsureFormulaId() noexcept;

		// Returns the ID to use when looking up formula/callback maps.
		// Clones set formula_source_id to the parent's ID so they share its entries
		// without taking ownership — the parent remains responsible for cleanup.
		[[nodiscard]] int64_t formula_key() const noexcept
		{
			return formula_source_id != -1 ? formula_source_id : combat_id;
		}

		int64_t combat_id = -1;
		int64_t formula_source_id = -1;
		mutable std::atomic<int32_t> ref_count{ 0 };

		uint32_t damage = 0;
		std::bitset<32> config;
		uint16_t defense_charge_cost = 0;
		uint16_t armor_charge_cost = 0;
		uint16_t augment_charge_cost = 0;
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

	inline void intrusive_ptr_add_ref(const Combat * p) noexcept
	{
		p->ref_count.fetch_add(1, std::memory_order_relaxed);
	}

	void intrusive_ptr_release(const Combat * p) noexcept;


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
		std::atomic<int64_t>                     next_id_{ 1 };

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

	extern std::pmr::unordered_map<int64_t, CombatArea> combat_area_map;
	extern std::pmr::unordered_map<int64_t, CombatArea> combat_ext_area_map;

	// ── Formula Override System ───────────────────────────────────────────────
	// Three-tier resolution per damage-block step:
	//   Level 0 — no config:      Tibia defaults (zero overhead)
	//   Level 1 — TOML / Lua:     C++ factor params from formula map
	//   Level 2 — Lua callback:   full Lua function (registered per sit/stage)
	//
	// Situation indices: 0=PvP, 1=PvM, 2=MvP, 3=MvM

	struct SituationFormulas
	{
		Combat::OutputFactors     output     = Combat::TibiaOutput;
		Combat::ResistanceFactors defense    = Combat::TibiaDefense;
		Combat::ResistanceFactors armor      = Combat::TibiaArmor;
		Combat::ResolutionFactors resolution = Combat::TibiaResolution;
	};

	enum class FormulaStage : uint8_t
	{
		Output     = 0,
		Defense    = 1,
		Armor      = 2,
		Resolution = 3
	};

	struct FormulaCallbacks
	{
		static constexpr int32_t NoRef = -1;
		int32_t refs[4][4];

		FormulaCallbacks()
		{
			for (auto& row : refs)
				for (auto& r : row)
					r = NoRef;
		}
	};

	// Global per-situation defaults — populated from [formulas] in config/combat.toml.
	// Initialises to Tibia presets; absent TOML keys leave the Tibia value in place.
	extern std::array<SituationFormulas, 4> g_default_situation_formulas;

	// Per-combat Level 1 overrides.  Only combats that call SetSituationFormulas appear here.
	extern std::pmr::unordered_map<int64_t, std::array<SituationFormulas, 4>> combat_formula_map;

	// Per-combat Level 2 Lua callbacks.  Only combats that call SetFormulaCallback appear here.
	extern std::pmr::unordered_map<int64_t, FormulaCallbacks> combat_callback_map;

	// Apply a named formula preset string to a factor struct.
	// Unrecognised names are silently ignored (existing value unchanged).
	void ApplyOutputPreset(Combat::OutputFactors& out, std::string_view preset) noexcept;
	void ApplyDefensePreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept;
	void ApplyArmorPreset(Combat::ResistanceFactors& out, std::string_view preset) noexcept;
	void ApplyResolutionPreset(Combat::ResolutionFactors& out, std::string_view preset) noexcept;

	// Called from ConfigManager::Load() for each of the four situations.
	// Absent / unrecognised preset strings leave that stage at the Tibia default.
	void LoadFormulaDefaults(
		uint8_t          sit_idx,
		std::string_view out_preset,
		std::string_view def_preset,
		std::string_view arm_preset,
		std::string_view res_preset
	) noexcept;
}

class MagicField final : public Item
{
public:
	explicit MagicField(uint16_t type) : Item(type), createTime(OTSYS_TIME())
	{
		thing_subtype = ThingSubType::MagicField;
		item_subtype = ItemSubType::MagicField;
	}

	MagicFieldPtr getMagicField() override {
		return static_shared_this<MagicField>();
	}

	MagicFieldConstPtr getMagicField() const override {
		return static_shared_this<MagicField>();
	}

	bool isReplaceable() const {
		return Item::items[getID()].replaceable;
	}

	CombatType_t getCombatType() const {
		const ItemType& it = items[getID()];
		return it.combatType;
	}

	int32_t getDamage() const {
		const ItemType& it = items[getID()];
		if (it.conditionDamage) {
			return it.conditionDamage->getTotalDamage();
		}
		return 0;
	}
	void onStepInField(const CreaturePtr& creature);

private:
	int64_t createTime;
};