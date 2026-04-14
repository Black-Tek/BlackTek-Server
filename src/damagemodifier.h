// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licensed MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.

#ifndef FS_DAMAGEMODIFIER_H
#define FS_DAMAGEMODIFIER_H

#include "otpch.h"
#include "tools.h"
#include "fileloader.h"
#include <span>
#include <string_view>
#include <memory>
#include <type_traits>
#include <gtl/phmap.hpp>
#include <atomic>

namespace BlackTek
{
	struct [[nodiscard]] DamageModifier
	{
		inline static uint64_t generateGUID()
		{
			static std::atomic<uint64_t> s_next_guid{ 1 };
			return s_next_guid.fetch_add(1, std::memory_order_relaxed);
		}

		DamageModifier() : guid(generateGUID()) {}
		~DamageModifier() = default;

		// guid is preserved on copy so the identity tracks the original
		DamageModifier(const DamageModifier&) = default;
		DamageModifier& operator=(const DamageModifier&) = default;

		enum Flag : uint16_t
		{
			InvalidFlag = 0,
			Attack = 1 << 0,
			Defense = 1 << 1,
			Damage = 1 << 2,
			Origin = 1 << 3,
			Creature = 1 << 4,
			Race = 1 << 5,
			Named = 1 << 6,
			Reformed = 1 << 7,
			Converted = 1 << 8,
		};

		enum class AttackType : uint8_t
		{
			None,				// default
			Lifesteal,			// damage is converted to health
			Manasteal,			// damage is converted to mana
			Staminasteal,		// damage is converted stamina
			Soulsteal,			// damage is converted soul
			Critical,			// damage can critcally hit
			Piercing,			// damage ignores defenses
			Conversion,			// damage is converted to different type
			// attack_modifier_cripple,			// new modifier for paralyzing target

			First = Lifesteal,
			Last = Conversion,
		};

		enum class DefenseType : uint8_t
		{
			None,				// default
			Absorb,				// damage is converted to health
			Restore,			// damage is converted to mana 
			Replenish,			// damage is converted to stamina
			Revive,				// damage is converted to soul
			Reflect,			// damage is reduced on defender and returns to attacker
			Deflect,			// damage is negated on defender but hits all nearby enemies
			Ricochet,			// damage is negated on defender but hits one random enemy
			Resist,				// damage reduction
			Reform,				// convert damage to another type
			Weakness,			// increases damage taken

			First = None,
			Last = Weakness
		};

		enum class Factor : uint8_t
		{
			Percent,
			Flat
		};

		enum class Stance : uint8_t
		{
			Attack,
			Defense
		};

		DamageModifier(uint8_t stance, uint8_t modType, uint16_t amount, uint8_t factorType, uint8_t chance, uint16_t combatType = COMBAT_NONE, uint8_t source = 0, uint8_t creatureType = CREATURETYPE_ATTACKABLE, uint8_t race = RACE_NONE, const std::string& creatureName = "none") :
			guid(generateGUID()),
			damage_type(combatType),			// if none, defaults to all damage types
			to_damage_type(COMBAT_NONE),
			value(amount),						// value to modify; default = percent
			mod_stance(stance),					// attack / defense
			mod_type(modType),					// the enum specific type
			factor(factorType),					// flat or percent based? defaults to percent.
			chance(chance),						// chance; if chance is 0, chance is not used.
			origin_type(source),				// if none, is used on all origin types
			creature_type(creatureType),		// defaults to all creatures if not set
			race_type(race)						// if none, all races.
		{
			auto attack_mod = (mod_stance == std::to_underlying(Stance::Attack));

			if (attack_mod)
			{
				auto conversion = (modType == std::to_underlying(AttackType::Conversion));

				if (conversion)
					filter_index |= Flag::Converted;

				filter_index |= Flag::Attack;
			}
			else
			{
				auto reformed = (modType == std::to_underlying(DefenseType::Reform));

				if (reformed)
					filter_index |= Flag::Reformed;

				filter_index |= Flag::Defense;
			}

			if (combatType != 0)
				filter_index |= Flag::Damage;

			if (source != 0)
				filter_index |= Flag::Origin;

			if (race != 0)
				filter_index |= Flag::Race;

			if (creatureType != CREATURETYPE_ATTACKABLE)
				filter_index |= Flag::Creature;

			if (creatureName != "none")
				filter_index |= Flag::Named;
		}

		std::span<const std::byte> serialize() const
		{
			return std::span{ reinterpret_cast<const std::byte*>(this), sizeof(*this) };
		}

		template<typename T = DamageModifier>
		static T deserialize(std::span<const std::byte, sizeof(T)> data) 
		{
			const T* obj = std::start_lifetime_as<const T>(data.data());
			return *obj;
		}

		[[nodiscard]] const uint64_t getGUID() const noexcept;
		[[nodiscard]] const uint8_t getStance() const noexcept;
		[[nodiscard]] const uint8_t getType() const noexcept;
		[[nodiscard]] const uint16_t getValue() const noexcept;
		[[nodiscard]] const uint8_t getChance() const noexcept;

		[[nodiscard]] const uint16_t getDamageType() const noexcept;
		[[nodiscard]] const uint8_t getOriginType() const noexcept;
		[[nodiscard]] const uint8_t getCreatureType() const noexcept;
		[[nodiscard]] const uint8_t getRaceType() const noexcept;

		[[nodiscard]] const bool isPercent() const noexcept;
		[[nodiscard]] const bool isFlatValue() const noexcept;
		[[nodiscard]] const bool appliesToDamage(const uint16_t damageType) const noexcept;
		[[nodiscard]] const bool appliesToOrigin(const uint8_t origin) const noexcept;
		[[nodiscard]] const bool appliesToTarget(const uint8_t creatureType, const uint8_t race, const std::string_view creatureName) const noexcept;
		[[nodiscard]] const bool applies(uint16_t damageType, uint8_t creatureType, uint8_t origin, uint8_t race, const std::string_view creatureName) const noexcept;
		[[nodiscard]] const bool isAttackStance() const noexcept;
		[[nodiscard]] const bool isDefenseStance() const noexcept;
		[[nodiscard]] const bool hasCreatureName() const noexcept;
		[[nodiscard]] const std::string& getMonsterName() const noexcept;
		[[nodiscard]] const uint16_t getConversionType() const noexcept;
		[[nodiscard]] const uint16_t getFilterIndex()  const noexcept;

		// todo: see which of these voids can use noexcept and apply it.
		void setType(uint8_t modType);
		// void setStance(uint8_t stance); // I think we need to prevent users from changing the stance on a modifier so for now, we comment this one out..
		void setValue(uint16_t amount);
		void setChance(uint8_t chance);
		void setFactor(uint8_t factor);
		void setCombatType(uint16_t combatType);
		void setOriginType(uint8_t origin);
		void setRaceType(uint8_t race);
		void setCreatureType(uint8_t c_type);
		void increaseValue(uint16_t amount);
		void decreaseValue(uint16_t amount);
		void setTransformDamageType(uint16_t damageType);
		void setCreatureName(std::string_view creatureName) const noexcept;

		bool operator==(const DamageModifier& other) const { return guid == other.guid; }

		uint64_t guid = 0;
		uint16_t damage_type = COMBAT_NONE;
		uint16_t to_damage_type = COMBAT_NONE;
		uint16_t value = 0;
		uint16_t filter_index = Flag::InvalidFlag;
		uint8_t mod_stance = 0;						// 0 = none, 1 = attack, 2 = defense;
		uint8_t mod_type = 0;
		uint8_t factor = 0;
		uint8_t chance = 0;
		uint8_t origin_type = 0;
		uint8_t creature_type = CREATURETYPE_ATTACKABLE;
		uint8_t race_type = RACE_NONE;
	};
	extern gtl::flat_hash_map<uint64_t, std::string> modifier_monster_names;
}

#endif