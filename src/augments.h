// Credits: BlackTek Server Creator Codinablack@github.com.
// This project is based of otland's The Forgottenserver.
// Any and all code taken from otland's The Forgottenserver is licensed under GPL 2.0
// Any code Authored by: Codinablack or BlackTek contributers, that is not already licensed, is hereby licesned MIT. 
// The GPL 2.0 License that can be found in the LICENSE file.
// All code found in this file is licensed under MIT and can be found in the LICENSE file.


#ifndef FS_AUGMENTS_H
#define FS_AUGMENTS_H

#include "augment.h"

class Augments {

public:
	// No Constructors! Purely static class.
	Augments() = delete;
	~Augments() = delete;
	Augments(const Augments&) = delete;
	Augments& operator=(const Augments&) = delete;
	Augments(Augments&&) = delete;
	Augments& operator=(Augments&&) = delete;

	static constexpr auto path = "data/augments/";
	static const ModifierStance ParseStance(std::string_view stanceName) noexcept;
	static const ModFactor ParseFactor(std::string_view factor) noexcept;
	static const ModifierAttackType ParseAttackModifier(std::string_view modName) noexcept;
	static const ModifierDefenseType ParseDefenseModifier(std::string_view modName) noexcept;
	static const CombatType_t ParseDamage(std::string_view damageName) noexcept;
	static const CombatOrigin ParseOrigin(std::string_view originName) noexcept;
	static const RaceType_t ParseRaceType(std::string_view raceType) noexcept;
	static const CreatureType_t ParseCreatureType(std::string_view creatureType) noexcept;

	static std::shared_ptr<Augment> MakeAugment(std::string_view augmentName);

	static void loadAll();
	static void clearAll();
	static void reload();
	static void AddAugment(std::shared_ptr<Augment> augment);
	static void RemoveAugment(std::shared_ptr<Augment> augment);
	static void RemoveAugment(std::string_view augName);
	static void RemoveAugment(std::string augName);
	static std::shared_ptr<Augment> GetAugment(std::string_view augName);
};


#endif