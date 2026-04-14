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


	class AreaCombat
	{
	public:
		void setupArea(const std::vector<uint32_t>& vec, uint32_t rows);
		void setupArea(int32_t length, int32_t spread);
		void setupArea(int32_t radius);
		void setupExtArea(const std::vector<uint32_t>& vec, uint32_t rows);
		const MatrixArea& getArea(const Position& centerPos, const Position& targetPos) const;


	private:
		std::vector<MatrixArea> areas;
		bool hasExtArea = false;
	};


	// Todo: Create a struct for "CombatTable" which will be a specific defined and handled lua table able to be passed for construction of combat objects

	class Combat
	{
	public:
		Combat() = default;

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

		enum class Formula : uint8_t
		{
			Raw,
			Linear,         // Tibia style: (Stat * Scaling) + Base
			Subtractive,    // Fire Emblem style: (Stat + Base) - (Def * Weight)
			Multiplicative, // MOBA style: Base * (Constant / (Constant + Def * Weight))
			PowerRatio      // Pokemon style: ((Stat * (Att/Def)) / Constant) + Base
		};

		struct [[nodiscard]] CombatFactors
		{
			float scaling = 0.0f;
			float base = 0.0f;
			float ratioConstant = 1.0f;
			float exponent = 1.0f;
			float power = 0.0f;
			float resistance = 0.0f;
			Formula formula_type = Formula::Linear;
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
			IgnoreAllDefenses,	// piercing should be this by default
			BlockedByArmor,
			BlockedByShield,
			TopTargetOnly,		// without this it does all creatures in a tile stack
			SelfOnly,			// a combat which only affects the user
			FriendlyParty,		// using this enables party only combat objects for the casters party
			EnemyParty,			// same as above but for target's party
			FraggedOnly,		// using this targets only people with skulls
			MultiLevel,			// this is a game changer, we will add after the main work is all done
			Aggressive,
			IgnoreBarriers,		// ignores walls for line of sight and such on the combat
			UseCharges,			// I think this one is used by weapons to reduce ammo
			Critical,
			Leech,
			Augmented,
			IsUtility,
			HasCondition,
			HasPvPFormula,
			HasPvMFormula,
			HasMvPFormula,
			HasMvMFormula,
			HasStages,			// this can come later, but the idea is to reduce the need of creating 10 combats per spell just to get it to do cool things..
		};


		static bool isProtected(const PlayerConstPtr& attacker, const PlayerConstPtr& target);
		static CombatType_t ConditionToDamageType(ConditionType_t type);
		static ConditionType_t DamageToConditionType(CombatType_t type);
		static void postCombatEffects(const CreaturePtr& caster, const Position& pos, const Combat& combat);
		static void addDistanceEffect(const CreaturePtr& caster, const Position& fromPos, const Position& toPos, uint8_t effect);


		[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const PlayerPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const MonsterPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const MonsterPtr& attacker, const PlayerPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const MonsterPtr& attacker, const MonsterPtr& victim) const noexcept;
		[[nodiscard]] TargetCode target(const PlayerPtr& attacker, const Position& target_location) const noexcept;
		[[nodiscard]] TargetCode target(const MonsterPtr& attacker, const Position& target_location) const noexcept;

		void attack_augment(const PlayerPtr caster, const CreaturePtr victim, const std::string& victim_name, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept;
		void defense_augment(const CreaturePtr caster, const PlayerPtr victim, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept;
		void conversion_augment(const PlayerPtr caster, const std::string& victim_name, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept;
		void reform_augment(const PlayerPtr victim, gtl::node_hash_map<uint8_t, ModifierTotals> modifier_map) noexcept;

		void post_damage(const PlayerPtr& caster, const CreaturePtr& victim, LeechData&& leech_data) noexcept;

		void strike_target(const PlayerPtr& caster, const PlayerPtr& victim) noexcept;
		void strike_target(const PlayerPtr& caster, const MonsterPtr& victim) noexcept;
		void strike_target(const MonsterPtr& attacker, const PlayerPtr& victim) noexcept;
		void strike_target(const MonsterPtr& attacker, const MonsterPtr& victim) noexcept;
		void strike_target(const CreaturePtr& attacker, const CreaturePtr& defender) noexcept;
		void strike_location(const PlayerPtr& caster, const Position& center) noexcept;
		void strike_location(const MonsterPtr& attacker, const Position& center) noexcept;
		void defense_block_effect(const Position& target_position) const noexcept;
		void armor_block_effect(const Position& target_position) const noexcept;
		void immunity_block_effect(const Position& target_position) const noexcept;

		[[nodiscard]] bool block(const CreaturePtr& attacker, const PlayerPtr& target) const noexcept;
		[[nodiscard]] bool block(const CreaturePtr& attacker, const MonsterPtr& target) const noexcept;
		[[nodiscard]] uint32_t collect_notice_data(const CreaturePtr& target) const noexcept;
		void notify_players();
		void apply_mana_damage(const CreaturePtr& attacker, const PlayerPtr& target, bool manashield) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const PlayerPtr& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const MonsterPtr& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const Position& target_position) noexcept;

		[[nodiscard]]
		inline bool hasArea() const noexcept {
			//return area != nullptr;
			return false;
		}

		[[nodiscard]] constexpr uint32_t calculate_damage_factors(const CombatFactors& factors) const noexcept
		{
			float poweredStat = factors.exponent != 1.0f ? poweredStat = std::pow(factors.power, factors.exponent) : factors.power;

			switch (factors.formula_type)
			{
				case Formula::Raw:				return normal_random(static_cast<uint32_t>(factors.base), static_cast<uint32_t>(factors.power));
				case Formula::Linear:			return static_cast<uint32_t>(std::max(0.0f, (poweredStat * factors.scaling) + factors.base));
				case Formula::Subtractive:		return static_cast<uint32_t>(std::max(0.0f, (poweredStat + factors.base) - (factors.resistance * factors.scaling)));
				case Formula::Multiplicative:	return static_cast<uint32_t>(std::max(0.0f, factors.base * (factors.ratioConstant / (factors.ratioConstant + (factors.resistance * factors.scaling)))));

				case Formula::PowerRatio:
					if (factors.resistance == 0) [[unlikely]]
						return static_cast<uint32_t>(std::max(0.0f, poweredStat + factors.base));
					else
						return static_cast<uint32_t>(std::max(0.0f, ((poweredStat * (factors.power / factors.resistance)) / factors.ratioConstant) + factors.base));
					break;

				default: [[unlikely]] // log
					return 0;
			}
			// log
			return 0;
		}


		void postCombatEffects(CreaturePtr& caster, const Position& pos) const { postCombatEffects(caster, pos, *this); }
		void setOrigin(Origin o) { origin = o; }

		[[nodiscard]] int64_t id() const noexcept { return combat_id; }
		void set_id(int64_t new_id) { combat_id = new_id; }

	private:
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, const TilePtr& tile);
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles);
		const DamageArea getAreaPositions(const Position& casterPos, const Position& targetPos);

		int64_t combat_id = -1;
		mutable std::atomic<int32_t> ref_count{ 0 };

		uint32_t damage = 0;
		std::bitset<32> config;
		uint16_t itemId = 0;
		DamageType damage_type = DamageType::Unknown;
		BlockType_t blockType = BLOCK_NONE;
		Origin origin = Origin::None;
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

	using CombatHandle = intrusive_ptr<Combat>;


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

		void Release(int64_t id);

		[[nodiscard]] Combat* Get(int64_t id) noexcept;
		[[nodiscard]] const Combat* Get(int64_t id) const noexcept;
		[[nodiscard]] std::size_t	Size() const noexcept { return table_.size(); }

		// we need this so external PMR containers can share the same resource.
		// e.g. std::pmr::unordered_map<int64_t, AreaCombat>{ g_combat_registry.Allocator() }
		[[nodiscard]] std::pmr::memory_resource* Allocator() noexcept { return &pool_; }
	};
	extern CombatRegistry g_combat_registry;

	extern std::pmr::unordered_map<int64_t, AreaCombat> combat_area_map;
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