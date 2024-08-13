#include "damagemodifier.h"

std::shared_ptr<DamageModifier> DamageModifier::makeModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factor, uint8_t chance, CombatType_t combatType, CombatOrigin source, CreatureType_t creatureType, RaceType_t race, std::string_view creatureName) {
	auto mod = std::make_shared<DamageModifier>(stance, modType, amount, factor, chance, combatType, source, creatureType, race, creatureName.data());
	return mod;
}

const std::string DamageModifier::getMonsterName() const
{	// Dangerous. Please only use if confirmed it is a Monster or Boss Based Mod.
	if (m_aux_attribute.has_value() && m_aux_attribute.type() == typeid(std::string)) {
		return any_cast<const std::string>(m_aux_attribute);
	}
	return std::string("ERROR");
}

const CombatType_t DamageModifier::conversionType() const
{
	if ((isAttackStance() && m_mod_type == ATTACK_MODIFIER_CONVERSION) || (isDefenseStance() && m_mod_type == DEFENSE_MODIFIER_REFORM)) {
		try {
			auto value = std::any_cast<CombatType_t>(m_aux_attribute);
			return value;
		} // to-do : make use of 'e' exception data here
		catch (const std::bad_any_cast& e) {
			std::cout << "[Error] Damage Modifier has invalid auxillary value for Conversion Type. Must be uint8_t ranged numerical value! Returning 0\n";
		}
	}
	return COMBAT_NONE;
}

// Note : Make sure to confirm the mod type is a conversion type
void DamageModifier::setTransformDamageType(CombatType_t damageType) {
	m_aux_attribute = std::any_cast<CombatType_t>(damageType);
}

void DamageModifier::increaseValue(uint16_t amount) {
	if ((m_value + amount) <= std::numeric_limits<uint16_t>::max()) {
		m_value += amount;
	} else {
		m_value = std::numeric_limits<uint16_t>::max();
		std::cout << "[WARNING] Amount exceded numeric limits for uint16_t. m_value set to limit." << "\n";
	}
}

void DamageModifier::decreaseValue(uint16_t amount) {
	if (m_value >= amount) {
		m_value -= amount;
	}else {
		m_value = 0;
		std::cout << "[WARNING] Amount is greater than m_value. m_value set to zero. " << "\n";
	}
}
