// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#include "augment.h"

Augment::Augment(std::string name, std::string description) : m_name(name), m_description(description) {
	
}

Augment::Augment(std::shared_ptr<Augment>& original) : m_name(original->m_name), m_description(original->m_description) {

    for (const auto& mod : original->m_attack_modifiers) {
        auto copiedMod = std::make_shared<DamageModifier>(*mod);
        m_attack_modifiers.push_back(copiedMod);
    }

    for (const auto& mod : original->m_defense_modifiers) {
        auto copiedMod = std::make_shared<DamageModifier>(*mod);
        m_defense_modifiers.push_back(copiedMod);
    }
}


std::vector<std::shared_ptr<DamageModifier>> Augment::getAttackModifiers(uint8_t modType) {
	std::vector<std::shared_ptr<DamageModifier>> modifiers;
	for (auto& mod : m_attack_modifiers) {

		if (mod->getType() == modType) {
			modifiers.emplace_back(mod);
		}
	}
	return modifiers;
}

std::vector<std::shared_ptr<DamageModifier>> Augment::getDefenseModifiers(uint8_t modType) {
	std::vector<std::shared_ptr<DamageModifier>> modifiers;
	for (auto& mod : m_defense_modifiers) {
		
		if (mod->getType() == modType) {
			modifiers.emplace_back(mod);
		}
	}
	return modifiers;
}