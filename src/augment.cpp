// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT.
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#include "otpch.h"
#include "augment.h"

Augment::Augment(const std::string& name, const std::string& description) : m_name(name), m_description(description) {}

Augment::Augment(std::shared_ptr<Augment>& original)
    : m_name(original->m_name),
      m_description(original->m_description),
      m_modifiers(original->m_modifiers),
      m_attack_count(original->m_attack_count) {}


std::vector<DamageModifier> Augment::getAttackModifiers(uint8_t modType) const
{
	std::vector<DamageModifier> result;
	for (const auto& mod : getAttackModifiers())
	{
		if (mod.getType() == modType)
		{
			result.push_back(mod);
		}
	}
	return result;
}

std::vector<DamageModifier> Augment::getDefenseModifiers(uint8_t modType) const
{
	std::vector<DamageModifier> result;
	for (const auto& mod : getDefenseModifiers())
	{
		if (mod.getType() == modType)
		{
			result.push_back(mod);
		}
	}
	return result;
}
