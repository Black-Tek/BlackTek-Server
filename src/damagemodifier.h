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
#include <gtl/phmap.hpp>

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

class DamageModifier : public std::enable_shared_from_this<DamageModifier> {

public:
	DamageModifier() = default;
	~DamageModifier() = default;

	// allow copying
	DamageModifier(const DamageModifier&) = default;
	DamageModifier& operator=(const DamageModifier&) = default;

	DamageModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factorType, uint8_t chance, CombatType_t combatType = COMBAT_NONE , uint8_t source = 0, CreatureType_t creatureType = CREATURETYPE_ATTACKABLE,  RaceType_t race = RACE_NONE, const std::string& creatureName = "none") :
		m_mod_stance(stance),					// attack / defense
		m_mod_type(modType),					// the enum specific type
		m_value(amount),						// value to modify; default = percent
		m_factor(factorType),					// flat or percent based? defaults to percent.
		m_chance(chance),						// chance; if chance is 0, chance is not used.
		m_damage_type(combatType),				// if none, defaults to all damage types
		m_origin_type(source),					// if none, is used on all origin types
		m_creature_type(creatureType),			// defaults to all creatures if not set
		m_race_type(race),						// if none, all races.
		m_creature_name(creatureName)			// if none, all creatures.
	{}

	static std::shared_ptr<DamageModifier> makeModifier() { return std::make_shared<DamageModifier>(); }
	static std::shared_ptr<DamageModifier> makeModifier(const DamageModifier& mod) { return std::make_shared<DamageModifier>(mod); }
	static std::shared_ptr<DamageModifier> makeModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factorType, uint8_t chance, CombatType_t combatType = COMBAT_NONE, uint8_t source = 0, CreatureType_t creatureType = CREATURETYPE_ATTACKABLE, RaceType_t race = RACE_NONE, std::string_view creatureName = "none");

	const uint8_t& getStance() const;
	const uint8_t& getType() const;
	const uint16_t& getValue() const;
	const uint8_t& getChance() const;

	const CombatType_t& getDamageType() const;
	const uint8_t getOriginType() const;
	const CreatureType_t& getCreatureType() const;
	const RaceType_t& getRaceType() const;

	const bool isPercent() const;
	const bool isFlatValue() const;
	const bool appliesToDamage(const CombatType_t damageType) const;
	const bool appliesToOrigin(const uint8_t origin) const;
	const bool appliesToTarget(const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName) const;
	const bool isAttackStance() const;
	const bool isDefenseStance() const;
	const std::string& getMonsterName() const;
	const CombatType_t& getConversionType() const;

	void setType(uint8_t modType);
	void setStance(uint8_t stance);
	void setValue(uint16_t amount); 
	void setChance(uint8_t chance);
	void setFactor(uint8_t factor);
	void setCombatType(CombatType_t combatType);
	void setOriginType(uint8_t origin);
	void setRaceType(RaceType_t race);
	void setCreatureType(CreatureType_t c_type);
	void increaseValue(uint16_t amount);
	void decreaseValue(uint16_t amount);
	void setTransformDamageType(CombatType_t damageType);
	void setCreatureName(std::string_view creatureName);

	void serialize(PropWriteStream& propWriteStream) const {
		// Serialize regular fields
		propWriteStream.write<uint8_t>(m_mod_stance);
		propWriteStream.write<uint8_t>(m_mod_type);
		propWriteStream.write<uint16_t>(m_value);
		propWriteStream.write<uint8_t>(m_factor);
		propWriteStream.write<uint8_t>(m_chance);
		propWriteStream.write<CombatType_t>(m_damage_type);
		propWriteStream.write<CombatType_t>(m_to_damage_type);
		propWriteStream.write<uint8_t>(m_origin_type);
		propWriteStream.write<CreatureType_t>(m_creature_type);
		propWriteStream.write<RaceType_t>(m_race_type);
		propWriteStream.writeString(m_creature_name);

	}

	bool unserialize(PropStream& propReadStream) {
		if (!propReadStream.read<uint8_t>(m_mod_stance)) return false;
		if (!propReadStream.read<uint8_t>(m_mod_type)) return false;
		if (!propReadStream.read<uint16_t>(m_value)) return false;
		if (!propReadStream.read<uint8_t>(m_factor)) return false;
		if (!propReadStream.read<uint8_t>(m_chance)) return false;

		if (!propReadStream.read<CombatType_t>(m_damage_type)) return false;
		if (!propReadStream.read<CombatType_t>(m_to_damage_type)) return false;
		if (!propReadStream.read<uint8_t>(m_origin_type)) return false;
		if (!propReadStream.read<CreatureType_t>(m_creature_type)) return false;
		if (!propReadStream.read<RaceType_t>(m_race_type)) return false;

		auto [creatureName, success] = propReadStream.readString();
		if (!success) return false;
		m_creature_name = std::string(creatureName);

		return true;
	}


private:
	uint8_t m_mod_stance = 0;						// 0 = none, 1 = attack, 2 = defense;
	uint8_t m_mod_type = 0;
	uint16_t m_value = 0;
	uint8_t m_factor = 0;
	uint8_t m_chance = 0;
	CombatType_t m_damage_type = COMBAT_NONE;
	CombatType_t m_to_damage_type = COMBAT_NONE;
	uint8_t m_origin_type = 0;
	CreatureType_t m_creature_type = CREATURETYPE_ATTACKABLE;
	RaceType_t m_race_type = RACE_NONE;
	std::string m_creature_name = "none";
};

/// Inline Methods' Definitions

inline void DamageModifier::setType(uint8_t modType)
{
	m_mod_type = modType;
}

inline void DamageModifier::setStance(uint8_t stance)
{
	m_mod_stance = stance;
}

inline void DamageModifier::setValue(uint16_t amount) {
	m_value = amount;
}

inline void DamageModifier::setChance(uint8_t chance)
{
	m_chance = chance;
}

inline void DamageModifier::setFactor(uint8_t factor)
{
	m_factor = static_cast<ModFactor>(factor);
}

inline void DamageModifier::setCombatType(CombatType_t combatType) {
	m_damage_type = combatType;
}

inline void DamageModifier::setOriginType(uint8_t origin) {
	m_origin_type = origin;
}

inline void DamageModifier::setRaceType(RaceType_t race)
{
	m_race_type = race;
}

inline void DamageModifier::setCreatureType(CreatureType_t c_type)
{
	m_creature_type = c_type;
}

inline void DamageModifier::setCreatureName(std::string_view creatureName) {
	m_creature_name = creatureName.data();
}

inline const bool DamageModifier::isPercent() const {
	return m_factor == PERCENT_MODIFIER;
}

inline const bool DamageModifier::isFlatValue() const {
	return m_factor == FLAT_MODIFIER;
}

inline const bool DamageModifier::appliesToDamage(const CombatType_t damageType) const {
	return m_damage_type == COMBAT_NONE || m_damage_type == damageType;
}

inline const bool DamageModifier::appliesToOrigin(const uint8_t origin) const
{
	bool matches = (m_origin_type == 0 or m_origin_type == origin);
	bool applies = (m_origin_type == 12 and (origin == 5
			or origin == 6
			or origin == 7
			or origin == 8
			or origin == 9
			or origin == 10));
	
	return matches or applies;
}

inline const bool DamageModifier::appliesToTarget(const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName) const {
	bool matchesType = (m_creature_type == CREATURETYPE_ATTACKABLE || m_creature_type == creatureType);
	bool isValidTarget =
		((m_creature_type == CREATURETYPE_MONSTER || m_creature_type == CREATURETYPE_SUMMON_ALL) && (
			creatureType == CREATURETYPE_MONSTER ||
			creatureType == CREATURETYPE_SUMMON_ALL ||
			creatureType == CREATURETYPE_SUMMON_OWN ||
			creatureType == CREATURETYPE_SUMMON_GUILD ||
			creatureType == CREATURETYPE_SUMMON_HOSTILE ||
			creatureType == CREATURETYPE_SUMMON_PARTY
			));
	bool matchesRace = (m_race_type == RACE_NONE || m_race_type == race);

	bool attackableTarget = false;

	if ((matchesType || isValidTarget) && matchesRace) {

		if (m_creature_name.empty() || m_creature_name == "none") {
			attackableTarget = true;
		} else {
			attackableTarget = (m_creature_name == std::string(creatureName.data()));
		}
	}
	return attackableTarget;
}

inline const uint8_t& DamageModifier::getStance() const {
	return m_mod_stance;
}

inline const uint8_t& DamageModifier::getType() const {
	return m_mod_type;
}

inline const uint16_t& DamageModifier::getValue() const {
	return m_value;
}

inline const uint8_t& DamageModifier::getChance() const {
	return m_chance;
}

inline const CombatType_t& DamageModifier::getDamageType() const {
	return m_damage_type;
}

inline const uint8_t DamageModifier::getOriginType() const {
	return m_origin_type;
}

inline const bool DamageModifier::isAttackStance() const
{
	return m_mod_stance == ATTACK_MOD;
}

inline const bool DamageModifier::isDefenseStance() const
{
	return m_mod_stance == DEFENSE_MOD;
}

inline const std::string& DamageModifier::getMonsterName() const
{
	return m_creature_name;
}

inline const CombatType_t& DamageModifier::getConversionType() const
{
	return m_to_damage_type;
}

inline const CreatureType_t& DamageModifier::getCreatureType() const
{
	return m_creature_type;
}

inline const RaceType_t& DamageModifier::getRaceType() const
{
	return m_race_type;
}

// Pre-collected attack modifier list per modifier type, keyed by ModifierAttackType.
// Target-independent: built once from a player's augments and equipped items.
// Used to avoid re-scanning equipment on every target hit by an area spell.
using RawModifierMap = gtl::node_hash_map<uint8_t, std::vector<std::shared_ptr<DamageModifier>>>;

#endif