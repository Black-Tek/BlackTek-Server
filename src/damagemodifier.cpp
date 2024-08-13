#include "damagemodifier.h"

std::shared_ptr<DamageModifier> DamageModifier::makeModifier(uint8_t stance, uint8_t modType, uint16_t amount, ModFactor factor, uint8_t chance, CombatType_t combatType, CombatOrigin source, CreatureType_t creatureType, RaceType_t race, std::string_view creatureName) {
	auto mod = std::make_shared<DamageModifier>(stance, modType, amount, factor, chance, combatType, source, creatureType, race, creatureName.data());
	mod->initializeSharedPointer();
	return mod;
}

std::shared_ptr<DamageModifier>& DamageModifier::getModifier() {
	return m_shared_this;
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

void DamageModifier::serialize(PropWriteStream& propWriteStream) const {
	propWriteStream.write<uint8_t>(m_mod_stance);
	propWriteStream.write<uint8_t>(m_mod_type);
	propWriteStream.write<uint16_t>(m_value);
	propWriteStream.write<uint8_t>(m_chance);
	propWriteStream.write<uint16_t>(m_damage_type);
	propWriteStream.write<uint8_t>(m_origin_type);
	/// To-do : add missing member data
}

bool DamageModifier::unserialize(PropStream& propReadStream) {
	uint8_t stance, modType, chance, originType = 0;
	bool flatRate, onAny, origin = false;
	uint16_t  damageType, value = 0;

	if (!propReadStream.read<uint8_t>(stance) ||
		!propReadStream.read<uint8_t>(modType) ||
		!propReadStream.read<uint16_t>(value) ||
		!propReadStream.read<uint8_t>(chance) ||
		!propReadStream.read<uint16_t>(damageType) ||
		!propReadStream.read<uint8_t>(originType) ||
		!propReadStream.read<bool>(flatRate) ||
		!propReadStream.read<bool>(onAny) ||
		!propReadStream.read<bool>(origin)) {
		return false;
	}

	m_mod_stance = stance;
	m_mod_type = modType;
	m_value = value;
	m_chance = chance;
	m_damage_type = static_cast<CombatType_t>(damageType);
	m_origin_type = static_cast<CombatOrigin>(originType);

	return true;
}

//////// ModifierList Members \\\\\\\\\\

void ModifierList::addModifier(std::shared_ptr<DamageModifier>& mod) {
	if (mod->getStance() == ATTACK_MOD) {
		m_attack_modifiers.push_back(std::move(mod));
	}
	else if (mod->getStance() == DEFENSE_MOD) {
		m_defense_modifiers.push_back(std::move(mod));
	}
}

void ModifierList::removeModifier(std::shared_ptr<DamageModifier>& mod) {
	if (mod->getStance() == ATTACK_MOD) {
		m_attack_modifiers.erase(std::remove(m_attack_modifiers.begin(), m_attack_modifiers.end(), mod), m_attack_modifiers.end());
	}
	else if (mod->getStance() == DEFENSE_MOD) {
		m_defense_modifiers.erase(std::remove(m_defense_modifiers.begin(), m_defense_modifiers.end(), mod), m_defense_modifiers.end());
	}
}

std::vector<std::shared_ptr<DamageModifier>>& ModifierList::getAttackModifiers(uint8_t modType) {
	static std::vector<std::shared_ptr<DamageModifier>> modifiers;
	modifiers.clear();
	for (auto& mod : m_attack_modifiers) {

		if (mod->getType() == modType) {
			modifiers.push_back(mod);
		}
	}
	return modifiers;
}

std::vector<std::shared_ptr<DamageModifier>>& ModifierList::getDefenseModifiers(uint8_t modType) {
	static std::vector<std::shared_ptr<DamageModifier>> modifiers;
	modifiers.clear();
	for (auto& mod : m_defense_modifiers) {

		if (mod->getType() == modType) {
			modifiers.push_back(mod);
		}
	}
	return modifiers;
}
