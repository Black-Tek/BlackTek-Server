// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT.
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#pragma once

#include "damagemodifier.h"
#include <span>

class Augment
{


public:
	Augment() = default;
	Augment(const std::string& name, const std::string& description = "");
	Augment(const Augment& original);

	bool operator==(const Augment& other) const
	{
		return getName() == other.getName();
	}

	~Augment() = default;

	Augment& operator=(const Augment&) = default;

	const std::string getName() const;
	const std::string getDescription() const;

	//void setName(const std::string& name);
	//void setDescription(const std::string& description);

	static Augment MakeAugment(std::string augmentName, std::string description = "");
	static Augment MakeAugment(const Augment& original);

	void addModifier(const DamageModifier& modifier);
	void addAttackModifier(const DamageModifier& modifier);
	void addDefenseModifier(const DamageModifier& modifier);
	void removeModifier(uint64_t guid);

	uint32_t attack_mod_count() const noexcept;
	uint32_t defense_mod_count() const noexcept;

	std::span<DamageModifier> getAttackModifiers();
	std::span<DamageModifier> getDefenseModifiers();

	std::span<const DamageModifier> getAttackModifiers() const;
	std::span<const DamageModifier> getDefenseModifiers() const;

	std::vector<DamageModifier> getAttackModifiers(uint8_t modType) const;
	std::vector<DamageModifier> getDefenseModifiers(uint8_t modType) const;

	void serialize(PropWriteStream& propWriteStream) const
	{
		propWriteStream.write<uint32_t>(m_attack_count);

		for (const auto& modifier : getAttackModifiers())
		{
			//modifier.serialize(propWriteStream);
		}

		propWriteStream.write<uint32_t>(defense_mod_count());

		for (const auto& modifier : getDefenseModifiers())
		{
			//modifier.serialize(propWriteStream);
		}
	}

	bool unserialize(PropStream& propReadStream)
	{
		auto [name, successName] = propReadStream.readString();

		if (not successName)
		{
			std::cout << "WARNING: Failed to deserialize augment name" << std::endl;
			return false;
		}

		auto [description, successDesc] = propReadStream.readString();

		if (not successDesc)
		{
			std::cout << "WARNING: Failed to deserialize augment description" << std::endl;
			return false;
		}

		uint32_t attackModifierCount;

		if (not propReadStream.read<uint32_t>(attackModifierCount))
		{
			std::cout << "WARNING: Failed to deserialize attack modifier count" << std::endl;
			return false;
		}

		m_modifiers.clear();
		m_attack_count = 0;

		for (uint32_t i = 0; i < attackModifierCount; ++i)
		{
			//DamageModifier modifier;

			//if (not modifier.unserialize(propReadStream))
			//{
			//	std::cout << "WARNING: Failed to deserialize attack modifier " << i << std::endl;
			//	return false;
			//}
			//addAttackModifier(modifier);
		}

		uint32_t defenseModifierCount;

		if (not propReadStream.read<uint32_t>(defenseModifierCount))
		{
			std::cout << "WARNING: Failed to deserialize defense modifier count" << std::endl;
			return false;
		}

		for (uint32_t i = 0; i < defenseModifierCount; ++i)
		{
			//DamageModifier modifier;

			//if (not modifier.unserialize(propReadStream))
			//{
			//	std::cout << "WARNING: Failed to deserialize defense modifier " << i << std::endl;
			//	return false;
			//}
			//addDefenseModifier(modifier);
		}

		return true;
	}


private:

	// m_modifiers is a partitioned vector: [0, m_attack_count) = attack, [m_attack_count, size()) = defense
	std::vector<DamageModifier> m_modifiers;
	uint32_t m_attack_count = 0;
	DamageModifier::Flag trigger_index = DamageModifier::Flag::InvalidFlag;

	uint8_t damage_count = 0;
	uint8_t origin_count = 0;
	uint8_t creature_count = 0;
	uint8_t race_count = 0;
	uint8_t reformed_count = 0;
	uint8_t converted_count = 0;
};

//
//inline Augment::Augment(const std::string& name, const std::string& description)
//	: m_name(name), m_description(description) {
//}
//
//inline Augment::Augment(const Augment& original)
//	: m_name(original.m_name),
//	m_description(original.m_description),
//	m_modifiers(original.m_modifiers),
//	m_attack_count(original.m_attack_count) {
//}

inline Augment Augment::MakeAugment(std::string augmentName, std::string description)
{
	return Augment(augmentName, description);
}

inline Augment Augment::MakeAugment(const Augment& original)
{
	return Augment(original);
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

inline std::span<DamageModifier> Augment::getAttackModifiers()
{
	return std::span<DamageModifier>(m_modifiers.data(), m_attack_count);
}

inline std::span<DamageModifier> Augment::getDefenseModifiers()
{
	return std::span<DamageModifier>(m_modifiers.data() + m_attack_count, m_modifiers.size() - m_attack_count);
}

inline std::span<const DamageModifier> Augment::getAttackModifiers() const
{
	return std::span<const DamageModifier>(m_modifiers.data(), m_attack_count);
}

inline std::span<const DamageModifier> Augment::getDefenseModifiers() const
{
	return std::span<const DamageModifier>(m_modifiers.data() + m_attack_count, m_modifiers.size() - m_attack_count);
}
