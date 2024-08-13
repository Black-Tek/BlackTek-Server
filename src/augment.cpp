// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#include "augment.h"

Augment::Augment(std::string_view name, std::string_view description) : m_name(name), m_description(description), m_mod_list(std::make_shared<ModifierList>()) {
	m_mod_list->initializeSharedPointer();
}

Augment::Augment(std::shared_ptr<Augment>& original) : m_name(original->m_name), m_mod_list(original->m_mod_list) {
	m_mod_list->initializeSharedPointer();
}

std::vector<std::shared_ptr<DamageModifier>>& Augment::getAttackModifiers()
{
	return m_mod_list->getAttackModifiers();
}

std::vector<std::shared_ptr<DamageModifier>>& Augment::getDefenseModifiers()
{
	return m_mod_list->getDefenseModifiers();
}

std::vector<std::shared_ptr<DamageModifier>>& Augment::getAttackModifiers(uint8_t modType)
{
	return m_mod_list->getAttackModifiers(modType);
}

std::vector<std::shared_ptr<DamageModifier>>& Augment::getDefenseModifiers(uint8_t modType)
{
	return m_mod_list->getDefenseModifiers(modType);
}
