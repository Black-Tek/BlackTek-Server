/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * This file is an original BlackTek contribution, created since this project's
 * inception, and contains no code inherited from The Forgotten Server.
 * It is licensed solely under the MIT License (see the licensing/License file
 * in the project root for the full text).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

		~Augment();

		Augment& operator=(const Augment& other);

		const std::string& getName() const;
		const std::string& getDescription() const;

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
		[[nodiscard]] uint32_t healing_mod_count() const noexcept;

		[[nodiscard]] std::span<const DamageModifier> getAttackModifiers() const noexcept;
		[[nodiscard]] std::span<const DamageModifier> getDefenseModifiers() const noexcept;
		[[nodiscard]] std::span<const DamageModifier> getModifiers() const noexcept;

		void serialize(PropWriteStream& propWriteStream) const;
		static std::optional<std::shared_ptr<Augment>> deserialize(PropStream& propReadStream);

	private:

		[[nodiscard]] std::span<DamageModifier> getAttackModifiers() noexcept;
		[[nodiscard]] std::span<DamageModifier> getDefenseModifiers() noexcept;

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
		uint8_t healing_count = 0;
	};

	inline std::shared_ptr<Augment> Augment::MakeAugment(std::string_view augmentName, std::string_view description)
	{
		return std::make_shared<Augment>(augmentName, description);
	}

	inline std::shared_ptr<Augment> Augment::MakeAugment(const Augment& original)
	{
		return std::make_shared<Augment>(original);
	}

}