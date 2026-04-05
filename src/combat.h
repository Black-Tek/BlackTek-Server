// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_COMBAT_H
#define FS_COMBAT_H

#include "thing.h"
#include "condition.h"
#include "map.h"
#include "baseevents.h"
#include "tools.h"
#include "matrixarea.h"
#include "damagemodifier.h"
#include "creature.h"

#include <utility>
#include <optional>
#include <functional>
#include <span>
#include <gtl/phmap.hpp>
#include <bitset>

class Condition;
class Item;
struct Position;

PlayerPtr PlayerCast(auto creature)
{
	return std::static_pointer_cast<Player>(creature);
}

MonsterPtr MonsterCast(auto creature)
{
	return std::static_pointer_cast<Monster>(creature);
}

namespace BlackTek::Constant
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
		const BlackTek::MatrixArea& getArea(const Position& centerPos, const Position& targetPos) const;
		

	private:
		std::vector<BlackTek::MatrixArea> areas;
		bool hasExtArea = false;
};

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
			Absorb,
			Restore,
			Reflect,
			Deflect,
			Ricochet,
			Piercing,
			Imbuement,
			Augment,
			Last = Augment,
			Count = Last,
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
			BlockedByArmor,
			BlockedByShield,
			TopOnly,			// without this it does all creatures in a tile stack
			SelfOnly,			// not sure about this one yet
			FriendlyParty,		// using this enables party only combat objects for the casters party
			EnemyParty,			// same as above but for target's party
			FraggedOnly,		// using this targets only people with skulls
			MultiLevel,			// this is a game changer, we will add after the main work is all done
			Aggressive,
			UseCharges,			// I think this one is used by weapons to reduce ammo
			IgnoreResistances,
			Critical,
			Leeched,
			Augmented,
			IsUtility,
			HasPvPFormula,
			HasPvMFormula,
			HasMvPFormula,
			HasMvMFormula,
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

		bool setParam(CombatParam_t param, uint32_t value);
		int32_t getParam(CombatParam_t param) const;

		void setArea(AreaCombat* area);

		[[nodiscard]] 
		inline bool hasArea() const noexcept {
			return area != nullptr;
		}
	

		void postCombatEffects(CreaturePtr& caster, const Position& pos) const {
			postCombatEffects(caster, pos, *this);
		}

		void setOrigin(Origin o) {
			origin = o;
		}

		[[nodiscard]] CombatType_t getCombatType() const noexcept { return primary.type; }

	private:
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, const TilePtr& tile);
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles);
		const DamageArea getAreaPositions(const Position& casterPos, const Position& targetPos);

		std::unique_ptr<AreaCombat> area;

		struct { CombatType_t type = COMBAT_NONE; int32_t value = 0; } primary{};
		struct { CombatType_t type = COMBAT_NONE; int32_t value = 0; } secondary{};

		// todo: change from using int32_t to uint32_t for all damage. There is no reason to need a negative value when we can define healing by combat type
		float mina = 0.0f;
		float minb = 0.0f;
		float maxa = 0.0f;
		float maxb = 0.0f;

		std::bitset<32> config;

		ConditionType_t dispelType = CONDITION_NONE;
		formulaType_t formulaType = COMBAT_FORMULA_UNDEFINED;

		// --- 2-byte members ---
		uint16_t itemId = 0;

		// --- 1-byte members ---
		uint8_t impactEffect = CONST_ME_NONE;
		uint8_t distanceEffect = CONST_ANI_NONE;
		BlockType_t blockType = BLOCK_NONE;
		Origin origin = Origin::None;
		
		bool blockedByArmor = false;
		bool blockedByShield = false;
		bool targetCasterOrTopMost = false;
		bool aggressive = true;
		bool useCharges = false;
		bool ignoreResistances = false;
		bool critical = false;
		bool leeched = false;
		bool augmented = false;
		bool isUtility = false;
};

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


#endif
