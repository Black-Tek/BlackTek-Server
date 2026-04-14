// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT.
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#pragma once

#include "damagemodifier.h"
#include <span>

namespace BlackTek
{

	class [[nodiscard]] Augment
	{
		inline static uint64_t generateGUID()
		{
			static std::atomic<uint64_t> s_next_guid{ 1 };
			return s_next_guid.fetch_add(1, std::memory_order_relaxed);
		}

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

		[[nodiscard]] uint32_t attack_mod_count() const noexcept;
		[[nodiscard]] uint32_t defense_mod_count() const noexcept;

		[[nodiscard]] std::vector<DamageModifier> getAttackModifiers(uint8_t modType) const noexcept;
		[[nodiscard]] std::vector<DamageModifier> getDefenseModifiers(uint8_t modType) const noexcept;

		void serialize(PropWriteStream& propWriteStream) const
		{

		}

		bool unserialize(PropStream& propReadStream)
		{
			return true;
		}


	private:

		[[nodiscard]] std::span<DamageModifier> getAttackModifiers() noexcept;
		[[nodiscard]] std::span<DamageModifier> getDefenseModifiers() noexcept;

		[[nodiscard]] std::span<const DamageModifier> getAttackModifiers() const noexcept;
		[[nodiscard]] std::span<const DamageModifier> getDefenseModifiers() const noexcept;

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

	extern gtl::flat_hash_map<uint64_t, Augment> loaded_augments;
	extern gtl::flat_hash_map<uint64_t, std::string> loaded_augment_names;
	extern gtl::flat_hash_map<uint64_t, std::string> loaded_agment_descriptions;
}