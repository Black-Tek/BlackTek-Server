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

class Condition;
class Item;
struct Position;

//for luascript callback
class ValueCallback final : public CallBack
{
	public:
		explicit ValueCallback(formulaType_t type): type(type) {}
		void getMinMaxValues(const PlayerPtr& player, CombatDamage& damage, const CreaturePtr& target) const;

	private:
		formulaType_t type;
};

class TileCallback final : public CallBack
{
	public:
		void onTileCombat(const CreaturePtr& creature, const TilePtr& tile) const;
};

class TargetCallback final : public CallBack
{
	public:
		void onTargetCombat(const CreaturePtr& creature, const CreaturePtr& target) const;
};

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

	enum class CombatCancelCode
	{
		None,
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

struct CombatParams
{
	std::forward_list<std::unique_ptr<const Condition>> conditionList;

	std::unique_ptr<ValueCallback> valueCallback;
	std::unique_ptr<TileCallback> tileCallback;
	std::unique_ptr<TargetCallback> targetCallback;

	uint16_t itemId = 0;

	ConditionType_t dispelType = CONDITION_NONE;
	CombatType_t combatType = COMBAT_NONE;
	CombatOrigin origin = ORIGIN_SPELL;

	uint8_t impactEffect = CONST_ME_NONE;
	uint8_t distanceEffect = CONST_ANI_NONE;

	bool blockedByArmor = false;
	bool blockedByShield = false;
	bool targetCasterOrTopMost = false;
	bool aggressive = true;
	bool useCharges = false;
	bool ignoreResistances = false;
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
		Combat(std::optional<CreaturePtr> attacker, CombatDamage&& combatDamage, CombatParams&& combatParams);
		Combat(const CreaturePtr& attacker, const CreaturePtr& defender, CombatDamage&& combatDamage, CombatParams&& combatParams);

		// non-copyable
		Combat(const Combat&) = delete;
		Combat& operator=(const Combat&) = delete;

		//static Combat create(CombatParams params, formulaType_t formula, CombatDamage damage, AreaCombat* area);
		//static Combat create(const Combat& other_combat);

		static Combat makeCombat(std::optional<CreaturePtr> attacker, CombatDamage&& damage, CombatParams&& params);
		static Combat makeCombat(const CreaturePtr& attacker, const CreaturePtr& defender, CombatDamage&& damage, CombatParams&& params);

		static bool isInPvpZone(const CreatureConstPtr& attacker, const CreatureConstPtr& target);
		static bool isProtected(const PlayerConstPtr& attacker, const PlayerConstPtr& target);
		static bool isPlayerCombat(const CreatureConstPtr& target);
		static CombatType_t ConditionToDamageType(ConditionType_t type);
		static ConditionType_t DamageToConditionType(CombatType_t type);
		// To-do : follow this call stack and improve it.
		// Here we have a method that is under-utilized. It should be used in combats
		// to remove those same checks out of game::combatHealthChange
		// with the breaking down of the smaller parts of combatHealthChange as well
		// we can eliminate the need in it all together and provide cleaner code.
		static ReturnValue canTargetCreature(const PlayerPtr& attacker, const CreaturePtr& target);
		static ReturnValue canDoCombat(const CreaturePtr& caster, const TilePtr& tile, bool aggressive);
		static ReturnValue canDoCombat(const CreaturePtr& attacker, const CreaturePtr& target);
		static void postCombatEffects(const CreaturePtr& caster, const Position& pos, const CombatParams& params);

		static void addDistanceEffect(const CreaturePtr& caster, const Position& fromPos, const Position& toPos, uint8_t effect);

		void doCombat(const CreaturePtr& caster,const CreaturePtr& target) const;
		void doCombat(const CreaturePtr& caster, const Position& position) const;

		[[nodiscard]] BlackTek::Constant::CombatCancelCode target(const PlayerPtr& attacker, const PlayerPtr& victim) const noexcept;
		[[nodiscard]] BlackTek::Constant::CombatCancelCode target(const PlayerPtr& attacker, const MonsterPtr& victim) const noexcept;
		[[nodiscard]] BlackTek::Constant::CombatCancelCode target(const MonsterPtr& attacker, const PlayerPtr& victim) const noexcept;
		[[nodiscard]] BlackTek::Constant::CombatCancelCode target(const MonsterPtr& attacker, const MonsterPtr& victim) const noexcept;
		[[nodiscard]] BlackTek::Constant::CombatCancelCode target(const PlayerPtr& attacker, const Position& target_location) const noexcept;
		[[nodiscard]] BlackTek::Constant::CombatCancelCode target(const MonsterPtr& attacker, const Position& target_location) const noexcept;

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

		static void doTargetCombat(const CreaturePtr& caster, const CreaturePtr& target, CombatDamage& damage, const CombatParams& params, bool sendDistanceEffect = true, const RawModifierMap* precomputedAttackMods = nullptr);
		static void doAreaCombat(const CreaturePtr& caster, const Position& position, const AreaCombat* area, const CombatDamage& damage, const CombatParams& params);

		static void applyDamageIncreaseModifier(uint8_t modifierType, CombatDamage& damage, int32_t percentValue, int32_t flatValue);
		static void applyDamageReductionModifier(uint8_t modifierType, CombatDamage& damage, const PlayerPtr& damageTarget, const std::optional<CreaturePtr>& attacker, int32_t percentValue, int32_t flatValue, CombatOrigin paramOrigin, uint8_t areaEffect = CONST_ME_NONE, uint8_t distanceEffect = CONST_ANI_NONE);

		void defense_block_effect(const Position& target_position) const noexcept;
		void armor_block_effect(const Position& target_position) const noexcept;
		void immunity_block_effect(const Position& target_position) const noexcept;

		[[nodiscard]] bool block(const CreaturePtr& attacker, const PlayerPtr& target) const noexcept;
		[[nodiscard]] bool block(const CreaturePtr& attacker, const MonsterPtr& target) const noexcept;
		[[nodiscard]] uint32_t collect_notice_data(const CreaturePtr& target) const noexcept;
		static void combatGetTypeInfo(CombatType_t combatType, const CreaturePtr& target, TextColor_t& color, uint8_t& effect);
		void notify_players();
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const PlayerPtr& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const Monster& target) noexcept;
		[[nodiscard]] bool apply_damage(const CreaturePtr& attacker, const Position& target_position) noexcept;

		bool setCallback(CallBackParam_t key);
		CallBack* getCallback(CallBackParam_t key) const;

		bool setParam(CombatParam_t param, uint32_t value);
		int32_t getParam(CombatParam_t param) const;

		void setArea(AreaCombat* area);

		[[nodiscard]] 
		inline bool hasArea() const noexcept {
			return area != nullptr;
		}
	
		void addCondition(Condition* condition) {
			params.conditionList.emplace_front(std::move(condition));
		}
	
		void clearConditions() {
			params.conditionList.clear();
		}
	
		void setPlayerCombatValues(formulaType_t formulaType, double mina, double minb, double maxa, double maxb);
	
		void postCombatEffects(CreaturePtr& caster, const Position& pos) const {
			postCombatEffects(caster, pos, params);
		}

		void setOrigin(CombatOrigin origin) {
			params.origin = origin;
		}

	private:
		static void combatTileEffects(const SpectatorVec& spectators,const CreaturePtr& caster, TilePtr tile, const CombatParams& params);
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, const TilePtr& tile);
		void apply_effects(const SpectatorVec& spectators, const CreaturePtr& caster, std::span<const TilePtr> tiles);
		static void processImbuements(const CreaturePtr& caster, const CreaturePtr& target, CombatDamage& damage);
		const DamageArea getAreaPositions(const Position& casterPos, const Position& targetPos);
		CombatDamage getCombatDamage(const CreaturePtr& creature, const CreaturePtr& target) const;

		//configurable
		CombatParams params;

		//formula variables
		formulaType_t formulaType = COMBAT_FORMULA_UNDEFINED;
		double mina = 0.0;
		double minb = 0.0;
		double maxa = 0.0;
		double maxb = 0.0;

		CombatDamage damage;

		std::unique_ptr<AreaCombat> area;
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
