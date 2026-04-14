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

	Augment::Augment(const std::string& name, const std::string& description) : m_guid(generateGUID())
	{
		loaded_augment_names[m_guid] = name;
		loaded_agment_descriptions[m_guid] = description;
	}

	Augment::Augment(const Augment& original)
		: m_guid(generateGUID()),
		m_modifiers(original.m_modifiers),
		m_attack_count(original.m_attack_count),
		trigger_index(original.trigger_index),
		damage_count(original.damage_count),
		origin_count(original.origin_count),
		creature_count(original.creature_count),
		race_count(original.race_count),
		reformed_count(original.reformed_count),
		converted_count(original.converted_count)
	{
		loaded_augment_names[m_guid] = original.getName();
		loaded_agment_descriptions[m_guid] = original.getDescription();
	}

	const std::string Augment::getName() const
	{
		auto it = loaded_augment_names.find(m_guid);
		if (it != loaded_augment_names.end())
			return it->second;
		return {};
	}

	const std::string Augment::getDescription() const
	{
		auto it = loaded_agment_descriptions.find(m_guid);
		if (it != loaded_agment_descriptions.end())
			return it->second;
		return {};
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

	inline void Augment::addModifier(const DamageModifier&& mod)
	{
		trigger_index |= mod.getFilterIndex();

		if (mod.getStance() == std::to_underlying(DamageModifier::Stance::Attack))
		{
			addAttackModifier(std::move(mod));
		}
		else if (mod.getStance() == std::to_underlying(DamageModifier::Stance::Defense))
		{
			addDefenseModifier(std::move(mod));
		}
	}

	inline void Augment::addAttackModifier(const DamageModifier&& modifier)
	{
		m_modifiers.insert(m_modifiers.begin() + m_attack_count, modifier);
		++m_attack_count;
	}

	inline void Augment::addDefenseModifier(const DamageModifier&& modifier)
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
		rebuild_triggers();
	}

	inline uint32_t Augment::attack_mod_count() const noexcept
	{
		return m_attack_count;
	}

	inline uint32_t Augment::defense_mod_count() const noexcept
	{
		return static_cast<uint32_t>(m_modifiers.size()) - m_attack_count;
	}

	void Augment::serialize(PropWriteStream& propWriteStream) const
	{
		propWriteStream.writeString(getName());
		propWriteStream.write<uint32_t>(m_attack_count);
		propWriteStream.write<uint32_t>(defense_mod_count());

		for (const auto& mod : m_modifiers)
		{
			propWriteStream.writeBytes(mod.serialize());
		}
	}

	// Todo: in the future change this to std::expected with an error code for our debugger system
	std::optional<Augment> Augment::deserialize(PropStream& propReadStream)
	{
		auto [name, success] = propReadStream.readString();

		if (not success)
			return std::nullopt;

		uint32_t attackCount = 0;

		if (not propReadStream.read<uint32_t>(attackCount))
			return std::nullopt;

		uint32_t defenseCount = 0;

		if (not propReadStream.read<uint32_t>(defenseCount))
			return std::nullopt;

		Augment result;
		result.m_modifiers.reserve(attackCount + defenseCount);

		for (uint32_t i = 0; i < attackCount + defenseCount; ++i)
		{
			// Todo: feels like this should be a thread_local buffer, 
			// investigate if clearing it in a loop as a thread_local buffer,
			// is actually worth doing over creating the buffer fresh each time..
			std::array<std::byte, sizeof(DamageModifier)> buf;

			if (not propReadStream.readBytes(std::span<std::byte>(buf)))
				return std::nullopt;

			result.addModifier(DamageModifier::deserialize(std::span<const std::byte, sizeof(DamageModifier)>(buf)));
		}

		result.m_attack_count = attackCount;
		loaded_augment_names[result.m_guid] = std::string(name);
		return result;
	}

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

	void Augment::rebuild_triggers() noexcept
	{
		trigger_index = 0;

		for (const auto& modifier : m_modifiers)
			trigger_index |= modifier.getFilterIndex();
	}

}