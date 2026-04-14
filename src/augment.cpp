// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT.
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#include "otpch.h"
#include "augment.h"

namespace BlackTek
{

	Augment::Augment(const std::string& name, const std::string& description) : m_name(name), m_description(description) {}

	Augment::Augment(std::shared_ptr<Augment>& original)
		: m_name(original->m_name),
		m_description(original->m_description),
		m_modifiers(original->m_modifiers),
		m_attack_count(original->m_attack_count) {
	}


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

	//inline const std::string Augment::getName() const
	//{
	//	return m_name;
	//}
	//
	//inline const std::string Augment::getDescription() const
	//{
	//	return m_description;
	//}
	//
	//inline void Augment::setName(const std::string& name)
	//{
	//	m_name = name;
	//}
	//
	//inline void Augment::setDescription(const std::string& description)
	//{
	//	m_description = description;
	//}

	inline void Augment::addModifier(const DamageModifier& mod)
	{
		if (mod.getStance() == std::to_underlying(DamageModifier::Stance::Attack))
		{
			addAttackModifier(mod);
		}
		else if (mod.getStance() == std::to_underlying(DamageModifier::Stance::Defense))
		{
			addDefenseModifier(mod);
		}
	}

	inline void Augment::addAttackModifier(const DamageModifier& modifier)
	{
		m_modifiers.insert(m_modifiers.begin() + m_attack_count, modifier);
		++m_attack_count;
	}

	inline void Augment::addDefenseModifier(const DamageModifier& modifier)
	{
		m_modifiers.push_back(modifier);
	}

	inline void Augment::removeModifier(uint64_t guid)
	{
		auto attackEnd = m_modifiers.begin() + m_attack_count;

		auto it = std::find_if(m_modifiers.begin(), attackEnd, [guid](const DamageModifier& m) { return m.getGUID() == guid;});

		if (it != attackEnd)
		{
			m_modifiers.erase(it);
			--m_attack_count;
			return;
		}

		auto dit = std::find_if(attackEnd, m_modifiers.end(), [guid](const DamageModifier& m) {	return m.getGUID() == guid;	});

		if (dit != m_modifiers.end())
		{
			m_modifiers.erase(dit);
		}
	}

	inline uint32_t Augment::attack_mod_count() const noexcept
	{
		return m_attack_count;
	}

	inline uint32_t Augment::defense_mod_count() const noexcept
	{
		return static_cast<uint32_t>(m_modifiers.size()) - m_attack_count;
	}

	//inline uint32_t Augment::reform_mod_count() const noexcept
	//{
	//	return reform_count;
	//}
	//
	//inline uint32_t Augment::conversion_mod_count() const noexcept
	//{
	//	return conversion_count;
	//}

	inline std::span<DamageModifier> Augment::getAttackModifiers() noexcept
	{
		return std::span<DamageModifier>(m_modifiers.data(), m_attack_count);
	}

	inline std::span<DamageModifier> Augment::getDefenseModifiers() noexcept
	{
		return std::span<DamageModifier>(m_modifiers.data() + m_attack_count, m_modifiers.size() - m_attack_count);
	}

	inline std::span<const DamageModifier> Augment::getAttackModifiers() const noexcept
	{
		return std::span<const DamageModifier>(m_modifiers.data(), m_attack_count);
	}

	inline std::span<const DamageModifier> Augment::getDefenseModifiers() const noexcept
	{
		return std::span<const DamageModifier>(m_modifiers.data() + m_attack_count, m_modifiers.size() - m_attack_count);
	}

}