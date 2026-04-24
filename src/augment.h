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
		Augment(const std::string_view name, const std::string_view description);
		Augment(const Augment& original);

		bool operator==(const Augment& other) const
		{
			return getName() == other.getName();
		}

		~Augment() = default;

		Augment& operator=(const Augment&) = default;

		const std::string getName() const;
		const std::string getDescription() const;

		static std::shared_ptr<Augment> MakeAugment(std::string_view augmentName, std::string_view description);
		static std::shared_ptr<Augment> MakeAugment(const Augment& original);

		void addModifier(DamageModifier&& modifier);
		void addAttackModifier(DamageModifier&& modifier);
		void addDefenseModifier(DamageModifier&& modifier);
		void removeModifier(uint64_t guid);

		[[nodiscard]] uint32_t attack_mod_count() const noexcept;
		[[nodiscard]] uint32_t defense_mod_count() const noexcept;
		[[nodiscard]] uint32_t damage_triggers() const noexcept;
		[[nodiscard]] uint32_t origin_triggers() const noexcept;
		[[nodiscard]] uint32_t creature_triggers() const noexcept;
		[[nodiscard]] uint32_t race_triggers() const noexcept;
		[[nodiscard]] uint32_t conversion_count() const noexcept;
		[[nodiscard]] uint32_t reform_count() const noexcept;
		[[nodiscard]] uint32_t name_count() const noexcept;

		[[nodiscard]] std::vector<DamageModifier> getAttackModifiers(uint8_t modType) const noexcept;
		[[nodiscard]] std::vector<DamageModifier> getDefenseModifiers(uint8_t modType) const noexcept;
		[[nodiscard]] std::span<const DamageModifier> getModifiers() const noexcept;

		void serialize(PropWriteStream& propWriteStream) const;
		static std::optional<std::shared_ptr<Augment>> deserialize(PropStream& propReadStream);

	private:

		[[nodiscard]] std::span<DamageModifier> getAttackModifiers() noexcept;
		[[nodiscard]] std::span<DamageModifier> getDefenseModifiers() noexcept;

		[[nodiscard]] std::span<const DamageModifier> getAttackModifiers() const noexcept;
		[[nodiscard]] std::span<const DamageModifier> getDefenseModifiers() const noexcept;

		void rebuild_triggers() noexcept;

		[[nodiscard]] uint64_t getGuid() const noexcept { return m_guid; }

		// m_modifiers is a partitioned vector: [0, m_attack_count) = attack, [m_attack_count, size()) = defense
		uint64_t m_guid = generateGUID();
		std::vector<DamageModifier> m_modifiers;
		uint32_t m_attack_count = 0;
		uint16_t trigger_index = DamageModifier::Flag::InvalidFlag;

		uint8_t damage_count = 0;
		uint8_t origin_count = 0;
		uint8_t creature_count = 0;
		uint8_t race_count = 0;
		uint8_t reformed_count = 0;
		uint8_t converted_count = 0;
		uint8_t named_count = 0;
	};

	inline std::shared_ptr<Augment> Augment::MakeAugment(std::string_view augmentName, std::string_view description)
	{
		return std::make_shared<Augment>(augmentName, description);
	}

	inline std::shared_ptr<Augment> Augment::MakeAugment(const Augment& original)
	{
		return std::make_shared<Augment>(original);
	}

	extern gtl::flat_hash_map<uint64_t, Augment> loaded_augments;
	extern gtl::flat_hash_map<uint64_t, std::string> loaded_augment_names;
	extern gtl::flat_hash_map<uint64_t, std::string> loaded_agment_descriptions;
}