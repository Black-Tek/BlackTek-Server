// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#ifndef FS_DAMAGEMODIFIER_H
#define FS_DAMAGEMODIFIER_H

#include "otpch.h"
#include "tools.h"
#include "const.h"
#include "fileloader.h"

struct ModifierTotals {
	ModifierTotals() = default;
	ModifierTotals(uint8_t flat, uint8_t percent) : flatTotal(flat), percentTotal(percent) {}
	uint8_t flatTotal = 0;
	uint8_t percentTotal = 0;
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

	ATTACK_MODIFIER_LAST
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


	DEFENSE_MODIFIER_LAST
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
	auto operator<=>(const DamageModifier&) const = default;

	void initializeSharedPointer() { m_shared_this = shared_from_this(); }

	DamageModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factorType, uint8_t chance, CombatType_t combatType = COMBAT_NONE , CombatOrigin source = ORIGIN_NONE, CreatureType_t creatureType = CREATURETYPE_ATTACKABLE,  RaceType_t race = RACE_NONE, std::string creatureName = "none") :
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

	std::shared_ptr<DamageModifier>& getModifier();
	static std::shared_ptr<DamageModifier> makeModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factorType, uint8_t chance, CombatType_t combatType = COMBAT_NONE, CombatOrigin source = ORIGIN_NONE, CreatureType_t creatureType = CREATURETYPE_ATTACKABLE, RaceType_t race = RACE_NONE, std::string_view creatureName = "none");

	const uint8_t getStance() const;
	const uint8_t getType() const;
	const uint8_t getValue() const;
	const uint16_t getChance() const;

	const CombatType_t getDamageType() const;
	const CombatOrigin getOriginType() const;

	const bool isPercent() const;
	const bool isFlatValue() const;
	const bool appliesToDamage(const CombatType_t damageType) const;
	const bool appliesToOrigin(const CombatOrigin origin) const;
	const bool appliesToTarget(const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName) const;
	const bool isAttackStance() const;
	const bool isDefenseStance() const;
	const std::string getMonsterName() const;
	const CombatType_t conversionType() const;

	void serialize(PropWriteStream& propWriteStream) const;
	bool unserialize(PropStream& propReadStream);

	void setValue(uint16_t amount);
	void setFactor(uint8_t factor);
	void setCombatType(CombatType_t combatType);
	void setOriginType(CombatOrigin origin);
	void increaseValue(uint16_t amount);
	void decreaseValue(uint16_t amount);
	void setTransformDamageType(CombatType_t damageType);
	void setCreatureName(std::string_view creatureName);

private:
	std::shared_ptr<DamageModifier> m_shared_this;
	uint8_t m_mod_stance = 0;						// 0 = none, 1 = attack, 2 = defense;
	uint8_t m_mod_type = 0;
	uint16_t m_value = 0;
	uint8_t m_factor = 0;
	uint8_t m_chance = 0;
	CombatType_t m_damage_type = COMBAT_NONE;
	CombatOrigin m_origin_type = ORIGIN_NONE;
	CreatureType_t m_creature_type = CREATURETYPE_ATTACKABLE;
	RaceType_t m_race_type = RACE_NONE;
	std::string m_creature_name = "none";
	std::any m_aux_attribute;						// can be used for determining a type of damage, when transforming from one type to another.
};

class ModifierList : public std::enable_shared_from_this<ModifierList> {

public:

	ModifierList() { }
	auto operator<=>(const ModifierList&) const = default;


	void initializeSharedPointer() { m_shared_this = shared_from_this(); }

	//void serialize(PropWriteStream& propWriteStream) const {
	//	if (!m_modifiers.empty()) {
	//		propWriteStream.write<uint16_t>(m_modifiers.size());
	//		for (const auto& modifier : m_modifiers) {
	//			modifier->serialize(propWriteStream);
	//		}
	//	}
	//}

	//bool unserialize(PropStream& propReadStream) {
	//	uint16_t size = 0;
	//	if (!propReadStream.read<uint16_t>(size)) {
	//		return false;
	//	}

	//	m_modifiers.clear();
	//	for (uint16_t i = 0; i < size; ++i) {
	//		auto modifier = std::make_shared<DamageModifier>();
	//		if (!modifier->unserialize(propReadStream)) {
	//			return false;
	//		}
	//		m_modifiers.push_back(modifier);
	//	}
	//	return true;
	//}

	void addModifier(std::shared_ptr<DamageModifier>& mod);
	void removeModifier(std::shared_ptr<DamageModifier>& mod);

	std::vector<std::shared_ptr<DamageModifier>>& getAttackModifiers();
	std::vector<std::shared_ptr<DamageModifier>>& getDefenseModifiers();

	std::vector<std::shared_ptr<DamageModifier>>& getAttackModifiers(uint8_t modType);
	std::vector<std::shared_ptr<DamageModifier>>& getDefenseModifiers(uint8_t modType);

private:
	std::shared_ptr<ModifierList> m_shared_this;
	std::vector<std::shared_ptr<DamageModifier>> m_attack_modifiers;
	std::vector<std::shared_ptr<DamageModifier>> m_defense_modifiers;
};


/// Inline Methods' Definitions

inline void DamageModifier::setValue(uint16_t amount) {
	m_value = amount;
}

inline void DamageModifier::setFactor(uint8_t factor)
{
	m_factor = std::bit_cast<ModFactor>(factor);
}

inline void DamageModifier::setCombatType(CombatType_t combatType) {
	m_damage_type = combatType;
}

inline void DamageModifier::setOriginType(CombatOrigin origin) {
	m_origin_type = origin;
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

inline const bool DamageModifier::appliesToOrigin(const CombatOrigin origin) const {
	return m_origin_type == ORIGIN_NONE || m_origin_type == origin;
}

inline const bool DamageModifier::appliesToTarget(const CreatureType_t creatureType, const RaceType_t race, const std::string_view creatureName) const {
	bool matchesType = (m_creature_type == CREATURETYPE_ATTACKABLE || m_creature_type == creatureType);
	bool isValidTarget =
		(m_creature_type == CREATURETYPE_MONSTER || m_creature_type == CREATURETYPE_SUMMON_ALL && (
			creatureType == CREATURETYPE_MONSTER ||
			creatureType == CREATURETYPE_SUMMON_ALL ||
			creatureType == CREATURETYPE_SUMMON_OWN ||
			creatureType == CREATURETYPE_SUMMON_GUILD ||
			creatureType == CREATURETYPE_SUMMON_OTHERS ||
			creatureType == CREATURETYPE_SUMMON_PARTY
			));
	bool matchesRace = (m_race_type == RACE_NONE || m_race_type == race);

	bool attackableTarget = false;

	if (isValidTarget && matchesType && matchesRace) {

		if (m_creature_name.empty() || m_creature_name == "none") {
			attackableTarget = true;
		}else {
			attackableTarget = (m_creature_name == creatureName.data());
		}
	}
	return attackableTarget;
}

inline const uint8_t DamageModifier::getStance() const {
	return m_mod_stance;
}

inline const uint8_t DamageModifier::getType() const {
	return m_mod_type;
}

inline const uint8_t DamageModifier::getValue() const {
	return m_value;
}

inline const uint16_t DamageModifier::getChance() const {
	return m_chance;
}

inline const CombatType_t DamageModifier::getDamageType() const {
	return m_damage_type;
}

inline const CombatOrigin DamageModifier::getOriginType() const {
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

/// ModifierList Inline Functions

inline std::vector<std::shared_ptr<DamageModifier>>& ModifierList::getAttackModifiers() {
	return m_attack_modifiers;
}

inline std::vector<std::shared_ptr<DamageModifier>>& ModifierList::getDefenseModifiers() {
	return m_defense_modifiers;
}
#endif