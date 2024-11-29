// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#ifndef FS_AUGMENT_H
#define FS_AUGMENT_H

#include "damagemodifier.h"

class Augment : public std::enable_shared_from_this<Augment> {

public:
	Augment() = default;
	Augment(std::string name, std::string description = "");
	Augment(std::shared_ptr<Augment>& original);

	~Augment() = default;

	// allow copying
	explicit Augment(const Augment&) = default;
	Augment& operator=(const Augment&) = default;

	// comparison operator
	std::strong_ordering operator<=>(const Augment& other) const = default;

	const std::string getName() const;
	const std::string getDescription() const;

	void setName(std::string name);
	void setDescription(std::string description);

	static std::shared_ptr<Augment> MakeAugment(std::string augmentName, std::string description = "");
	static std::shared_ptr<Augment> MakeAugment(std::shared_ptr<Augment>& originalPointer);

	void addModifier(std::shared_ptr<DamageModifier>& modifier);
	void removeModifier(std::shared_ptr<DamageModifier>& modifier);

	std::vector<std::shared_ptr<DamageModifier>>& getAttackModifiers();
	std::vector<std::shared_ptr<DamageModifier>>& getDefenseModifiers();

	std::vector<std::shared_ptr<DamageModifier>> getAttackModifiers(uint8_t modType);
	std::vector<std::shared_ptr<DamageModifier>> getDefenseModifiers(uint8_t modType);

	void serialize(PropWriteStream& propWriteStream) const {
		// Serialize m_name and m_description
		propWriteStream.writeString(m_name);
		propWriteStream.writeString(m_description);

		// Serialize m_attack_modifiers
		propWriteStream.write<uint32_t>(m_attack_modifiers.size());  // Write the number of attack modifiers
		for (const auto& modifier : m_attack_modifiers) {
			modifier->serialize(propWriteStream);  // Call serialize on each DamageModifier
		}

		// Serialize m_defense_modifiers
		propWriteStream.write<uint32_t>(m_defense_modifiers.size());  // Write the number of defense modifiers
		for (const auto& modifier : m_defense_modifiers) {
			modifier->serialize(propWriteStream);  // Call serialize on each DamageModifier
		}
	}

	bool unserialize(PropStream& propReadStream) {
		// Deserialize m_name and m_description
		auto [name, successName] = propReadStream.readString();
		if (!successName) {
			std::cout << "WARNING: Failed to deserialize augment name" << std::endl;
			return false;
		}
		m_name = std::string(name);

		auto [description, successDesc] = propReadStream.readString();
		if (!successDesc) {
			std::cout << "WARNING: Failed to deserialize augment description" << std::endl;
			return false;
		}
		m_description = std::string(description);

		// Deserialize m_attack_modifiers
		uint32_t attackModifierCount;
		if (!propReadStream.read<uint32_t>(attackModifierCount)) {
			std::cout << "WARNING: Failed to deserialize attack modifier count" << std::endl;
			return false;
		}

		m_attack_modifiers.clear();
		for (uint32_t i = 0; i < attackModifierCount; ++i) {
			auto modifier = std::make_shared<DamageModifier>();
			if (!modifier->unserialize(propReadStream)) {
				std::cout << "WARNING: Failed to deserialize attack modifier " << i << std::endl;
				return false;
			}
			m_attack_modifiers.push_back(modifier);
		}

		// Deserialize m_defense_modifiers
		uint32_t defenseModifierCount;
		if (!propReadStream.read<uint32_t>(defenseModifierCount)) {
			std::cout << "WARNING: Failed to deserialize defense modifier count" << std::endl;
			return false;
		}

		m_defense_modifiers.clear();
		for (uint32_t i = 0; i < defenseModifierCount; ++i) {
			auto modifier = std::make_shared<DamageModifier>();
			if (!modifier->unserialize(propReadStream)) {
				std::cout << "WARNING: Failed to deserialize defense modifier " << i << std::endl;
				return false;
			}
			m_defense_modifiers.push_back(modifier);
		}

		return true;
	}


private:

	std::vector<std::shared_ptr<DamageModifier>> m_attack_modifiers;
	std::vector<std::shared_ptr<DamageModifier>> m_defense_modifiers;
	std::string m_name;
	std::string m_description;
};


inline std::shared_ptr<Augment> Augment::MakeAugment(std::string augmentName, std::string description) {
	auto augment = std::make_shared<Augment>(augmentName);
	return augment;
}

inline std::shared_ptr<Augment> Augment::MakeAugment(std::shared_ptr<Augment>& originalRef)
{
	auto augmentClone = std::make_shared<Augment>(originalRef);
	return augmentClone;
}

inline const std::string Augment::getName() const {
	return m_name;
}

inline const std::string Augment::getDescription() const
{
	return m_description;
}

inline void Augment::setName(std::string name) {
	m_name = name;
}

inline void Augment::setDescription(std::string description) {
	m_description = description;
}

inline void Augment::addModifier(std::shared_ptr<DamageModifier>& mod) {
	if (mod->getStance() == ATTACK_MOD) {
		m_attack_modifiers.push_back(mod);
	} else if (mod->getStance() == DEFENSE_MOD) {
		m_defense_modifiers.push_back(mod);
	}
}

inline void Augment::removeModifier(std::shared_ptr<DamageModifier>& mod) {
	if (mod->getStance() == ATTACK_MOD) {
		m_attack_modifiers.erase(std::remove(m_attack_modifiers.begin(), m_attack_modifiers.end(), mod), m_attack_modifiers.end());
	}
	else if (mod->getStance() == DEFENSE_MOD) {
		m_defense_modifiers.erase(std::remove(m_defense_modifiers.begin(), m_defense_modifiers.end(), mod), m_defense_modifiers.end());
	}
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