#include "damagemodifier.h"

std::shared_ptr<DamageModifier> DamageModifier::makeModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factor, uint8_t chance, CombatType_t combatType, CombatOrigin source, CreatureType_t creatureType, RaceType_t race, std::string_view creatureName) {
	auto mod = std::make_shared<DamageModifier>(stance, modType, amount, factor, chance, combatType, source, creatureType, race, creatureName.data());
	return mod;
}

void DamageModifier::setTransformDamageType(CombatType_t damageType) {
	m_to_damage_type = damageType;
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
