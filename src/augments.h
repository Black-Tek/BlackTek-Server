// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#pragma once

#include "augment.h"
#include <gtl/phmap.hpp>

namespace BlackTek
{

	class Augments 
	{

	public:
		// No Constructors! Purely static class.
		Augments() = delete;
		~Augments() = delete;
		Augments(const Augments&) = delete;
		Augments& operator=(const Augments&) = delete;
		Augments(Augments&&) = delete;
		Augments& operator=(Augments&&) = delete;

		static constexpr auto path = "data/augments/";
		static const uint8_t ParseStance(std::string_view stanceName) noexcept;
		static const uint8_t ParseFactor(std::string_view factor) noexcept;
		static const uint8_t ParseAttackModifier(std::string_view modName) noexcept;
		static const uint8_t ParseDefenseModifier(std::string_view modName) noexcept;
		static const uint16_t ParseDamage(std::string_view damageName) noexcept;
		static const uint8_t ParseOrigin(std::string_view originName) noexcept;
		static const uint8_t ParseRaceType(std::string_view raceType) noexcept;
		static const uint8_t ParseCreatureType(std::string_view creatureType) noexcept;

		static std::shared_ptr<Augment> MakeAugment(std::string_view augmentName);

		static void loadAll();
		static void clearAll();
		static void reload();
		static size_t count();
		static void AddAugment(std::shared_ptr<Augment> augment);
		static void RemoveAugment(const std::shared_ptr<Augment>& augment);
		static void RemoveAugment(std::string_view augName);
		static void RemoveAugment(const std::string& augName);
		static std::shared_ptr<Augment> GetAugment(std::string_view augName);
	};
}