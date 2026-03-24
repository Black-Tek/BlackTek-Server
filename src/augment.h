// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#ifndef FS_AUGMENT_H
#define FS_AUGMENT_H

#include "damagemodifier.h"

class Augment : public std::enable_shared_from_this<Augment>
{

public:
	Augment() = default;
	Augment(const std::string& name, const std::string& description = "");
	Augment(std::shared_ptr<Augment>& original);
	
	 bool operator==(const Augment& other) const 
    {
        return getName() == other.getName();
    }

	~Augment() = default;

	// allow copying
	explicit Augment(const Augment&) = default;
	Augment& operator=(const Augment&) = default;

	const std::string getName() const;
	const std::string getDescription() const;

	void setName(const std::string& name);
	void setDescription(const std::string& description);

	static std::shared_ptr<Augment> MakeAugment(std::string augmentName, std::string description = "");
	static std::shared_ptr<Augment> MakeAugment(std::shared_ptr<Augment>& originalPointer);

	void addModifier(std::shared_ptr<DamageModifier>& modifier);
	void addAttackModifier(std::shared_ptr<DamageModifier>& modifier);
	void addDefenseModifier(std::shared_ptr<DamageModifier>& modifier);
	void removeModifier(std::shared_ptr<DamageModifier>& modifier);

	uint32_t attack_mod_count() const noexcept;
	uint32_t defense_mod_count() const noexcept;
	uint32_t reform_mod_count() const noexcept;
	uint32_t conversion_mod_count() const noexcept;

	std::vector<std::shared_ptr<DamageModifier>>& getAttackModifiers();
	std::vector<std::shared_ptr<DamageModifier>>& getDefenseModifiers();

	std::vector<std::shared_ptr<DamageModifier>> getAttackModifiers(uint8_t modType);
	std::vector<std::shared_ptr<DamageModifier>> getDefenseModifiers(uint8_t modType);

	void serialize(PropWriteStream& propWriteStream) const
	{
		propWriteStream.writeString(m_name);
		propWriteStream.writeString(m_description);
		propWriteStream.write<uint32_t>(m_attack_modifiers.size());

		for (const auto& modifier : m_attack_modifiers)
		{
			modifier->serialize(propWriteStream);
		}

		propWriteStream.write<uint32_t>(m_defense_modifiers.size());

		for (const auto& modifier : m_defense_modifiers)
		{
			modifier->serialize(propWriteStream);
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

		m_name = std::string(name);
		auto [description, successDesc] = propReadStream.readString();

		if (not successDesc)
		{
			std::cout << "WARNING: Failed to deserialize augment description" << std::endl;
			return false;
		}

		m_description = std::string(description);
		uint32_t attackModifierCount;

		if (not propReadStream.read<uint32_t>(attackModifierCount))
		{
			std::cout << "WARNING: Failed to deserialize attack modifier count" << std::endl;
			return false;
		}

		m_attack_modifiers.clear();

		for (uint32_t i = 0; i < attackModifierCount; ++i)
		{
			auto modifier = std::make_shared<DamageModifier>();

			if (not modifier->unserialize(propReadStream))
			{
				std::cout << "WARNING: Failed to deserialize attack modifier " << i << std::endl;
				return false;
			}
			addAttackModifier(modifier);
		}

		uint32_t defenseModifierCount;

		if (not propReadStream.read<uint32_t>(defenseModifierCount))
		{
			std::cout << "WARNING: Failed to deserialize defense modifier count" << std::endl;
			return false;
		}

		m_defense_modifiers.clear();

		for (uint32_t i = 0; i < defenseModifierCount; ++i)
		{
			auto modifier = std::make_shared<DamageModifier>();

			if (not modifier->unserialize(propReadStream))
			{
				std::cout << "WARNING: Failed to deserialize defense modifier " << i << std::endl;
				return false;
			}
			addDefenseModifier(modifier);
		}

		return true;
	}


private:
	// todo : rearrange these for better memory layout
	std::vector<std::shared_ptr<DamageModifier>> m_attack_modifiers;
	std::vector<std::shared_ptr<DamageModifier>> m_defense_modifiers;
	std::string m_name;
	std::string m_description;
	uint32_t conversion_count = 0;
	uint32_t reform_count = 0;
};


inline std::shared_ptr<Augment> Augment::MakeAugment(std::string augmentName, std::string description)
{
	auto augment = std::make_shared<Augment>(augmentName, description);
	return augment;
}

inline std::shared_ptr<Augment> Augment::MakeAugment(std::shared_ptr<Augment>& originalRef)
{			 
	return std::make_shared<Augment>(originalRef);
}

inline const std::string Augment::getName() const
{
	return m_name;
}

inline const std::string Augment::getDescription() const
{
	return m_description;
}

inline void Augment::setName(const std::string& name)
{
	m_name = name;
}

inline void Augment::setDescription(const std::string& description)
{
	m_description = description;
}

// we have code dupe by having this function, so lets not forget to replace it's usages everywhere in the source code and eliminate it
inline void Augment::addModifier(std::shared_ptr<DamageModifier>& mod) 
{
	if (mod->getStance() == ATTACK_MOD)
	{
		m_attack_modifiers.push_back(mod);
	}
	else if (mod->getStance() == DEFENSE_MOD)
	{
		m_defense_modifiers.push_back(mod);
	}

	if (mod->getType() == ATTACK_MODIFIER_CONVERSION)
	{
		conversion_count += 1;
		return;
	}
		
	if (mod->getType() == DEFENSE_MODIFIER_REFORM)
	{
		reform_count += 1;
		return;
	}
}

inline void Augment::addAttackModifier(std::shared_ptr<DamageModifier>& modifier)
{
	if (modifier->getType() == ATTACK_MODIFIER_CONVERSION)
		conversion_count += 1;

	m_attack_modifiers.push_back(modifier);
}

inline void Augment::addDefenseModifier(std::shared_ptr<DamageModifier>& modifier)
{
	if (modifier->getType() == DEFENSE_MODIFIER_REFORM)
		reform_count += 1;

	m_defense_modifiers.push_back(modifier);
}

// todo: also split this and remove it
inline void Augment::removeModifier(std::shared_ptr<DamageModifier>& mod)
{
	if (mod->getType() == ATTACK_MODIFIER_CONVERSION)
	{
		conversion_count -= 1;
		m_attack_modifiers.erase(std::remove(m_attack_modifiers.begin(), m_attack_modifiers.end(), mod), m_attack_modifiers.end());
		return;
	}
	else if (mod->getType() == DEFENSE_MODIFIER_REFORM)
	{
		reform_count -= 1;
		m_defense_modifiers.erase(std::remove(m_defense_modifiers.begin(), m_defense_modifiers.end(), mod), m_defense_modifiers.end());
		return;
	}

	if (mod->getStance() == ATTACK_MOD)
	{
		m_attack_modifiers.erase(std::remove(m_attack_modifiers.begin(), m_attack_modifiers.end(), mod), m_attack_modifiers.end());
		return;
	}
	else if (mod->getStance() == DEFENSE_MOD)
	{
		m_defense_modifiers.erase(std::remove(m_defense_modifiers.begin(), m_defense_modifiers.end(), mod),	m_defense_modifiers.end());
		return;
	}
}

inline uint32_t Augment::attack_mod_count() const noexcept
{
	return m_attack_modifiers.size();
}

inline uint32_t Augment::defense_mod_count() const noexcept
{
	return m_defense_modifiers.size();
}

inline uint32_t Augment::reform_mod_count() const noexcept
{
	return reform_count;
}

inline uint32_t Augment::conversion_mod_count() const noexcept
{
	return conversion_count;
}

inline std::vector<std::shared_ptr<DamageModifier>>& Augment::getAttackModifiers()
{
	return m_attack_modifiers;
}

inline std::vector<std::shared_ptr<DamageModifier>>& Augment::getDefenseModifiers()
{
	return m_defense_modifiers;
}
#endif