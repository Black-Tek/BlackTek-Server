// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licensed MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#ifndef FS_DAMAGEMODIFIER_H
#define FS_DAMAGEMODIFIER_H

#include "otpch.h"
#include "tools.h"
#include "fileloader.h"
#include <span>
#include <string_view>
#include <memory>
#include <type_traits>
#include <gtl/phmap.hpp>
#include <atomic>

//template <typename T>
//requires std::is_trivially_copyable_v<T>
//const std::string_view to_blob_view(const T& obj) 
//{
//	return { reinterpret_cast<const char*>(&obj), sizeof(T) };
//}
//
//template <typename T>
//requires std::is_trivially_copyable_v<T>
//const T* from_blob_inplace(std::string_view data) 
//{
//	if (data.size() < sizeof(T)) return nullptr;
//	return std::start_lifetime_as<T>(data.data());
//}

struct ModifierTotals {
	ModifierTotals() = default;
	ModifierTotals(uint16_t flat, uint16_t percent) : flatTotal(flat), percentTotal(percent) {}
	uint16_t flatTotal = 0;
	uint16_t percentTotal = 0;

	ModifierTotals operator+(const ModifierTotals& other) const {
		return ModifierTotals(flatTotal + other.flatTotal, percentTotal + other.percentTotal);
	}
	
	ModifierTotals& operator+=(const ModifierTotals& other) {
		flatTotal += other.flatTotal;
		percentTotal = std::min<uint16_t>(percentTotal + other.percentTotal, 100);
		return *this;
	}
};

struct WeildModifier {
	uint8_t main = 0; // stat / skill 
	uint8_t sub = 0; // health / sword
	uint8_t value = 0;
	// Percent & Flat can be out-of-banded into being each their own containers;
};

enum StatModifier : uint8_t {
	MAX_HEALTH,
	HEALTH_REGEN,
	MAX_MANA,
	MANA_REGEN,
	MAX_SOUL,
	SOUL_REGEN,
	MAX_STAMINA,
	STAMINA_REGEN,
	/// Entries below this line are to be added as future features.
	MOVEMENT_SPEED,
	CASTING_SPEED,
	ATTACK_SPEED,
};

enum SkillModifier : uint8_t {
	MELEE_SKILL,
	FIST_SKILL,
	SWORD_SKILL,
	AXE_SKILL,
	CLUB_SKILL,
	WAND_SKILL,
	ROD_SKILL,
	MAGIC_SKILL,
	MAGIC_WEAPON_SKILL,
	DISTANCE_SKILL,
	SHIELD_SKILL,
	FISHING_SKILL,
};

enum KillModifier : uint8_t {
	AOE_DAMAGE,
	BONUS_LOOT, // flat
	BONUS_EXP,  // flat
	LOOT_GAIN,  // percent
	EXP_GAIN,   // percent
};

enum PassiveEffect : uint8_t {
	// all these applicable to specific creature.
	SCAVENGE, // increase chance for skinning
	GUTTING, // increased creature products w/ chance
	VOIDCALL, // increase mana leech if applicable
	VAMPIRIC, // increase life leech if applicable
	BLESS, // reduced death penalty
	CLEANSE, // removes one random active negative status effect and temporarily makes you immune against it
	ADDRENALINE, // chance to get temporary boosted movement speed after being attacked
	NUMB, // chance to paralyze attacker after an attack
	LOWBLOW, // increases crit hit chance if chance is already above 0
	DODGE, // chance to dodge an attack
};


enum ModifierAttackType : uint8_t {
	ATTACK_MODIFIER_NONE,				// default
	ATTACK_MODIFIER_LIFESTEAL,			// damage is converted to health
	ATTACK_MODIFIER_MANASTEAL,			// damage is converted to mana
	ATTACK_MODIFIER_STAMINASTEAL,		// damage is converted stamina
	ATTACK_MODIFIER_SOULSTEAL,			// damage is converted soul
	ATTACK_MODIFIER_CRITICAL,			// damage can critcally hit
	ATTACK_MODIFIER_PIERCING,			// damage ignores defenses
	ATTACK_MODIFIER_CONVERSION,			// damage is converted to different type
	// ATTACK_MODIFIER_CRIPPLE,			// new modifier for paralyzing target

	ATTACK_MODIFIER_FIRST = ATTACK_MODIFIER_LIFESTEAL,
	ATTACK_MODIFIER_LAST = ATTACK_MODIFIER_CONVERSION,
};

enum ModifierDefenseType : uint8_t {
	DEFENSE_MODIFIER_NONE,				// default
	DEFENSE_MODIFIER_ABSORB,			// damage is converted to health
	DEFENSE_MODIFIER_RESTORE,			// damage is converted to mana 
	DEFENSE_MODIFIER_REPLENISH,			// damage is converted to stamina
	DEFENSE_MODIFIER_REVIVE,			// damage is converted to soul
	DEFENSE_MODIFIER_REFLECT,			// damage is reduced on defender and returns to attacker
	DEFENSE_MODIFIER_DEFLECT,			// damage is negated on defender but hits all nearby enemies
	DEFENSE_MODIFIER_RICOCHET,			// damage is negated on defender but hits one random enemy
	DEFENSE_MODIFIER_RESIST,			// damage reduction
	DEFENSE_MODIFIER_REFORM,			// convert damage to another type
	DEFENSE_MODIFIER_WEAKNESS,			// increases damage taken

	DEFENSE_MODIFIER_FIRST = DEFENSE_MODIFIER_NONE,
	DEFENSE_MODIFIER_LAST = DEFENSE_MODIFIER_WEAKNESS
};

enum ModFactor : uint8_t {
	PERCENT_MODIFIER,
	FLAT_MODIFIER
};

enum ModifierStance : uint8_t {
	NO_MOD,
	ATTACK_MOD,
	DEFENSE_MOD
};


struct DamageModifier
{
	inline static uint64_t generateGUID()
	{
		static std::atomic<uint64_t> s_next_guid{ 1 };
		return s_next_guid.fetch_add(1, std::memory_order_relaxed);
	}

	DamageModifier() : guid(generateGUID()) {}
	~DamageModifier() = default;

	// guid is preserved on copy so the identity tracks the original
	DamageModifier(const DamageModifier&) = default;
	DamageModifier& operator=(const DamageModifier&) = default;

	enum Flag : uint16_t
	{
		InvalidFlag = 0,
		Attack = 1 << 0,
		Defense = 1 << 1,
		Damage = 1 << 2,
		Origin = 1 << 3,
		Creature = 1 << 4,
		Race = 1 << 5,
		Named = 1 << 6,
		Reformed = 1 << 7,
		Converted = 1 << 8,
	};

	enum class AttackType : uint8_t
	{
		None,				// default
		Lifesteal,			// damage is converted to health
		Manasteal,			// damage is converted to mana
		Staminasteal,		// damage is converted stamina
		Soulsteal,			// damage is converted soul
		Critical,			// damage can critcally hit
		Piercing,			// damage ignores defenses
		Conversion,			// damage is converted to different type
		// attack_modifier_cripple,			// new modifier for paralyzing target

		First = Lifesteal,
		Last = Conversion,
	};

	enum class DefenseType : uint8_t 
	{
		None,				// default
		Absorb,				// damage is converted to health
		Restore,			// damage is converted to mana 
		Replenish,			// damage is converted to stamina
		Revive,				// damage is converted to soul
		Reflect,			// damage is reduced on defender and returns to attacker
		Deflect,			// damage is negated on defender but hits all nearby enemies
		Ricochet,			// damage is negated on defender but hits one random enemy
		Resist,				// damage reduction
		Reform,				// convert damage to another type
		Weakness,			// increases damage taken

		First = None,
		Last = Weakness
	};

	DamageModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factorType, uint8_t chance, uint16_t combatType = COMBAT_NONE , uint8_t source = 0, uint8_t creatureType = CREATURETYPE_ATTACKABLE,  uint8_t race = RACE_NONE, const std::string& creatureName = "none") :
		guid(generateGUID()),
		damage_type(combatType),			// if none, defaults to all damage types
		to_damage_type(COMBAT_NONE),
		value(amount),						// value to modify; default = percent
		mod_stance(stance),					// attack / defense
		mod_type(modType),					// the enum specific type
		factor(factorType),					// flat or percent based? defaults to percent.
		chance(chance),						// chance; if chance is 0, chance is not used.
		origin_type(source),				// if none, is used on all origin types
		creature_type(creatureType),		// defaults to all creatures if not set
		race_type(race)						// if none, all races.
	{}

	const uint64_t getGUID() const;
	const uint8_t getStance() const;
	const uint8_t getType() const;
	const uint16_t getValue() const;
	const uint8_t getChance() const;

	const uint16_t getDamageType() const;
	const uint8_t getOriginType() const;
	const uint8_t getCreatureType() const;
	const uint8_t getRaceType() const;

	const bool isPercent() const;
	const bool isFlatValue() const;
	const bool appliesToDamage(const uint16_t damageType) const;
	const bool appliesToOrigin(const uint8_t origin) const;
	const bool appliesToTarget(const uint8_t creatureType, const uint8_t race, const std::string_view creatureName) const;
	const bool applies(uint16_t damageType, uint8_t creatureType, uint8_t origin, uint8_t race, const std::string_view creatureName) const;
	const bool isAttackStance() const;
	const bool isDefenseStance() const;
	//const bool hasCreatureName() const;
	//const std::string& getMonsterName() const;
	const uint16_t getConversionType() const;

	void setType(uint8_t modType);
	void setStance(uint8_t stance);
	void setValue(uint16_t amount); 
	void setChance(uint8_t chance);
	void setFactor(uint8_t factor);
	void setCombatType(uint16_t combatType);
	void setOriginType(uint8_t origin);
	void setRaceType(uint8_t race);
	void setCreatureType(uint8_t c_type);
	void increaseValue(uint16_t amount);
	void decreaseValue(uint16_t amount);
	void setTransformDamageType(uint16_t damageType);
	//void setCreatureName(std::string_view creatureName);

	bool operator==(const DamageModifier& other) const { return guid == other.guid; }

	uint64_t guid = 0;
	uint16_t damage_type = COMBAT_NONE;
	uint16_t to_damage_type = COMBAT_NONE;
	uint16_t value = 0;
	uint16_t filter_index = Flag::InvalidFlag;
	uint8_t mod_stance = 0;						// 0 = none, 1 = attack, 2 = defense;
	uint8_t mod_type = 0;
	uint8_t factor = 0;
	uint8_t chance = 0;
	uint8_t origin_type = 0;
	uint8_t creature_type = CREATURETYPE_ATTACKABLE;
	uint8_t race_type = RACE_NONE;
};

/// Inline Methods' Definitions
inline void DamageModifier::setType(uint8_t modType)						{ mod_type = modType; }
inline void DamageModifier::setStance(uint8_t stance)						{ mod_stance = stance; }
inline void DamageModifier::setValue(uint16_t amount)						{ value = amount; }
inline void DamageModifier::setChance(uint8_t chance)						{ this->chance = chance; }
inline void DamageModifier::setFactor(uint8_t factor)						{ this->factor = factor; }
inline void DamageModifier::setCombatType(uint16_t combatType)				{ damage_type = combatType; }
inline void DamageModifier::setOriginType(uint8_t origin)					{ origin_type = origin; }
inline void DamageModifier::setRaceType(uint8_t race)						{ race_type = race; }
inline void DamageModifier::setCreatureType(uint8_t c_type)					{ creature_type = c_type; }
//inline void DamageModifier::setCreatureName(std::string_view creatureName)	{ m_creature_name = creatureName.data(); }
inline const bool DamageModifier::isPercent() const							{ return factor == PERCENT_MODIFIER; }
inline const bool DamageModifier::isFlatValue() const						{ return factor == FLAT_MODIFIER; }

inline const bool DamageModifier::appliesToDamage(const uint16_t damageType) const
{
	return damage_type == COMBAT_NONE or damage_type == damageType;
}

inline const bool DamageModifier::appliesToOrigin(const uint8_t origin) const
{
	const bool matches = (origin_type == 0 or origin_type == origin);
	const bool applies = (origin_type == 12 and (origin == 5 or origin == 6 or origin == 7 or origin == 8 or origin == 9 or origin == 10)); // augment origin checked against different augment origins
	return matches or applies;
}

inline const bool DamageModifier::appliesToTarget(const uint8_t creatureType, const uint8_t race, const std::string_view creatureName) const
{
	bool valid_target = (creatureType == creature_type or creature_type == CREATURETYPE_ATTACKABLE);
	valid_target = valid_target or ((creature_type == CREATURETYPE_SUMMON_ALL) and ( creatureType == CREATURETYPE_SUMMON_OWN or creatureType == CREATURETYPE_SUMMON_GUILD or creatureType == CREATURETYPE_SUMMON_HOSTILE or creatureType == CREATURETYPE_SUMMON_PARTY));
	valid_target = valid_target and (race_type == RACE_NONE or race_type == race);

	if (filter_index & Flag::Named)
		valid_target = valid_target and creatureName == ""; // todo: has monster name, must look up name by modifier id from a map

	return valid_target;
}

inline const bool DamageModifier::applies(uint16_t damageType, uint8_t creatureType, uint8_t origin, uint8_t race, const std::string_view creatureName) const
{
	auto damage_counts = appliesToDamage(damageType);
	auto origin_counts = appliesToOrigin(origin);
	auto target_counts = appliesToTarget(creatureType, race, creatureName);
	return damage_counts and origin_counts and target_counts;
}

inline const uint64_t	DamageModifier::getGUID() const						{ return guid; }
inline const uint8_t	DamageModifier::getStance() const					{ return mod_stance; }
inline const uint8_t	DamageModifier::getType() const						{ return mod_type; }
inline const uint16_t	DamageModifier::getValue() const					{ return value; }
inline const uint8_t	DamageModifier::getChance() const					{ return chance; }
inline const uint16_t	DamageModifier::getDamageType() const				{ return damage_type; }
inline const uint8_t	DamageModifier::getOriginType() const				{ return origin_type; }
inline const bool		DamageModifier::isAttackStance() const				{ return mod_stance == ATTACK_MOD; }
inline const bool		DamageModifier::isDefenseStance() const				{ return mod_stance == DEFENSE_MOD; }
//inline const bool DamageModifier::hasCreatureName() const					{ return not m_creature_name.empty() and m_creature_name != "none"; }
//inline const std::string& DamageModifier::getMonsterName() const			{ return m_creature_name; }
inline const uint16_t	DamageModifier::getConversionType() const			{ return to_damage_type; }
inline const uint8_t	DamageModifier::getCreatureType() const				{ return creature_type; }
inline const uint8_t	DamageModifier::getRaceType() const					{ return race_type; }

#endif