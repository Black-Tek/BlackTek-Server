// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_TOOLS_H
#define FS_TOOLS_H

#include <random>
#include <string_view>

#include "position.h"
#include "const.h"
#include "enums.h"

static constexpr MagicEffectClasses CombatTypeToAreaEffect(CombatType_t combatType) {
	switch (combatType) {
		case COMBAT_PHYSICALDAMAGE:
			return CONST_ME_BLOCKHIT;
		case COMBAT_ENERGYDAMAGE:
			return CONST_ME_ENERGYHIT;
		case COMBAT_EARTHDAMAGE:
			return CONST_ME_HITBYPOISON;
		case COMBAT_FIREDAMAGE:
			return CONST_ME_HITBYFIRE;
		case COMBAT_UNDEFINEDDAMAGE:
			return CONST_ME_PURPLEENERGY;
		case COMBAT_LIFEDRAIN:
			return CONST_ME_REDSMOKE;
		case COMBAT_MANADRAIN:
			return CONST_ME_LOSEENERGY;
		case COMBAT_HEALING:
			return CONST_ME_MAGIC_RED;
		case COMBAT_DROWNDAMAGE:
			return CONST_ME_WATERSPLASH;
		case COMBAT_ICEDAMAGE:
			return CONST_ME_ICEATTACK;
		case COMBAT_HOLYDAMAGE:
			return CONST_ME_HOLYDAMAGE;
		case COMBAT_DEATHDAMAGE:
			return CONST_ME_MORTAREA;

		default:
			return CONST_ME_BLOCKHIT;
	}
}

void printXMLError(const std::string& where, const std::string& fileName, const pugi::xml_parse_result& result);

const std::vector<Direction>& getShuffleDirections();

std::string transformToSHA1(std::string_view input);
std::string generateToken(const std::string& key, uint32_t ticks);

void replaceString(std::string& str, const std::string& sought, const std::string& replacement);
void trim_right(std::string& source, char t);
void trim_left(std::string& source, char t);
void toLowerCaseString(std::string& source);
std::string asLowerCaseString(std::string source);
std::string asUpperCaseString(std::string source);

// checks that str1 is equivalent to str2 ignoring letter case
bool caseInsensitiveEqual(std::string_view str1, std::string_view str2);
// checks that str1 starts with str2 ignoring letter case
bool caseInsensitiveStartsWith(std::string_view str, std::string_view prefix);

using StringVector = std::vector<std::string>;
using IntegerVector = std::vector<int32_t>;

std::vector<std::string_view> explodeString(std::string_view inString, const std::string& separator, int32_t limit = -1);
IntegerVector vectorAtoi(const std::vector<std::string_view>& stringVector);
constexpr bool hasBitSet(uint32_t flag, uint32_t flags) {
	return (flags & flag) != 0;
}

std::mt19937& getRandomGenerator();
int32_t uniform_random(int32_t minNumber, int32_t maxNumber);
int32_t normal_random(int32_t minNumber, int32_t maxNumber);
bool boolean_random(double probability = 0.5);

Direction getDirection(const std::string& string);
Position getNextPosition(Direction direction, Position pos);
Direction getDirectionTo(const Position& from, const Position& to);

static Direction getOppositeDirection(Direction currentDirection) {
	switch (currentDirection) {
	case DIRECTION_NORTH:
		return DIRECTION_SOUTH;
	case DIRECTION_SOUTH:
		return DIRECTION_NORTH;
	case DIRECTION_WEST:
		return DIRECTION_EAST;
	case DIRECTION_EAST:
		return DIRECTION_WEST;
	default:
		return DIRECTION_SOUTH;
	}
	return DIRECTION_SOUTH;
}

std::string getFirstLine(const std::string& str);

std::string formatDate(time_t time);
std::string formatDateShort(time_t time);
std::string convertIPToString(uint32_t ip);

void trimString(std::string& str);

uint16_t getDepotBoxId(uint16_t index);
MagicEffectClasses getMagicEffect(const std::string& strValue);
ShootType_t getShootType(const std::string& strValue);
Ammo_t getAmmoType(const std::string& strValue);
WeaponAction_t getWeaponAction(const std::string& strValue);
Skulls_t getSkullType(const std::string& strValue);
std::string getCombatName(CombatType_t combatType);

std::string getSpecialSkillName(uint8_t skillid);
std::string getSkillName(uint8_t skillid);
std::string getStatName(uint8_t id);

uint32_t adlerChecksum(const uint8_t* data, size_t length);

std::string ucfirst(std::string str);
std::string ucwords(std::string str);
bool booleanString(std::string_view str);

std::string getWeaponName(WeaponType_t weaponType);

size_t combatTypeToIndex(CombatType_t combatType);
CombatType_t indexToCombatType(size_t v);

uint8_t serverFluidToClient(uint8_t serverFluid);
uint8_t clientFluidToServer(uint8_t clientFluid);

itemAttrTypes stringToItemAttribute(const std::string& str);

const char* getReturnMessage(ReturnValue value);

int64_t OTSYS_TIME();

SpellGroup_t stringToSpellGroup(const std::string& value);

namespace Titan {

#if __has_cpp_attribute(__cpp_lib_to_underlying)

	inline constexpr auto to_underlying(auto e) noexcept { return std::to_underlying(e); }

#else

	inline constexpr auto to_underlying(auto e) noexcept { return static_cast<std::underlying_type_t<decltype(e)>>(e); }

#endif

} // namespace Titan

#endif
