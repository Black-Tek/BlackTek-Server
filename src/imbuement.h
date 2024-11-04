#ifndef BT_IMBUEMENT_H
#define BT_IMBUEMENT_H

#include "otpch.h"
#include "fileloader.h"
#include "scheduler.h"

#include <memory>

enum ImbuementType : uint8_t {
	IMBUEMENT_TYPE_NONE,
	IMBUEMENT_TYPE_FIRE_DAMAGE,
	IMBUEMENT_TYPE_EARTH_DAMAGE,
	IMBUEMENT_TYPE_ICE_DAMAGE,
	IMBUEMENT_TYPE_ENERGY_DAMAGE,
	IMBUEMENT_TYPE_DEATH_DAMAGE,
	IMBUEMENT_TYPE_HOLY_DAMAGE,
	IMBUEMENT_TYPE_LIFE_LEECH,
	IMBUEMENT_TYPE_MANA_LEECH,
	IMBUEMENT_TYPE_CRITICAL_CHANCE,
	IMBUEMENT_TYPE_CRITICAL_AMOUNT,
	IMBUEMENT_TYPE_FIRE_RESIST,
	IMBUEMENT_TYPE_EARTH_RESIST,
	IMBUEMENT_TYPE_ICE_RESIST,
	IMBUEMENT_TYPE_ENERGY_RESIST,
	IMBUEMENT_TYPE_HOLY_RESIST,
	IMBUEMENT_TYPE_DEATH_RESIST,
	IMBUEMENT_TYPE_PARALYSIS_DEFLECTION,
	IMBUEMENT_TYPE_SPEED_BOOST,
	IMBUEMENT_TYPE_CAPACITY_BOOST,
	IMBUEMENT_TYPE_FIST_SKILL,
	IMBUEMENT_TYPE_AXE_SKILL,
	IMBUEMENT_TYPE_SWORD_SKILL,
	IMBUEMENT_TYPE_CLUB_SKILL,
	IMBUEMENT_TYPE_DISTANCE_SKILL,
	IMBUEMENT_TYPE_FISHING_SKILL,
	IMBUEMENT_TYPE_SHIELD_SKILL,
	IMBUEMENT_TYPE_MAGIC_LEVEL,
	IMBUEMENT_TYPE_LAST
};

enum ImbuementDecayType : uint8_t {
	IMBUEMENT_DECAY_NONE,
	IMBUEMENT_DECAY_EQUIPPED,
	IMBUEMENT_DECAY_INFIGHT,
	IMBUEMENT_DECAY_LAST
};

struct Imbuement : std::enable_shared_from_this<Imbuement> {
	Imbuement() = default;
	Imbuement(ImbuementType imbuetype, uint32_t value, uint32_t duration, ImbuementDecayType decayType = ImbuementDecayType::IMBUEMENT_DECAY_NONE) : imbuetype{ imbuetype }, value{ value }, duration{ duration }, decaytype{ decayType } {};

	ImbuementType imbuetype = ImbuementType::IMBUEMENT_TYPE_NONE;
	uint32_t value = 0;
	uint32_t duration = 0;
	ImbuementDecayType decaytype = ImbuementDecayType::IMBUEMENT_DECAY_NONE;
	auto operator<=>(const Imbuement&) const = default;

	bool isSkill() const;
	bool isSpecialSkill() const;
	bool isDamage() const;
	bool isResist() const;
	bool isStat() const;
	bool isEquipDecay() const;
	bool isInfightDecay() const;

	void serialize(PropWriteStream& propWriteStream) const {
		propWriteStream.write<uint8_t>(static_cast<uint8_t>(imbuetype));
		propWriteStream.write<uint32_t>(value);
		propWriteStream.write<uint32_t>(duration);
		propWriteStream.write<uint8_t>(static_cast<uint8_t>(decaytype));
	}

	bool unserialize(PropStream& propReadStream) {
		uint32_t val, dur;
		uint8_t type, decay;
		if (!propReadStream.read<uint8_t>(type) ||
			!propReadStream.read<uint32_t>(val) ||
			!propReadStream.read<uint32_t>(dur) ||
			!propReadStream.read<uint8_t>(decay)) {
			return false;
		}

		imbuetype = static_cast<ImbuementType>(type);
		value = val;
		duration = dur;
		decaytype = static_cast<ImbuementDecayType>(decay);
		return true;
	}
};

#endif // BT_IMBUEMENT_H