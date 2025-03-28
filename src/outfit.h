// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_OUTFIT_H
#define FS_OUTFIT_H

#include "enums.h"

struct Outfit {
	Outfit(std::string name, uint16_t lookType, bool premium, bool locked) :
		name(std::move(name)), lookType(lookType), premium(premium), locked(locked) {}

	bool operator==(const Outfit& otherOutfit) const
	{
		return name == otherOutfit.name && lookType == otherOutfit.lookType && premium == otherOutfit.premium && locked == otherOutfit.locked;
	}

	std::string name;
	uint16_t lookType;
	bool premium;
	bool locked;
};

struct ProtocolOutfit {
	ProtocolOutfit(std::string_view name, uint16_t lookType, uint8_t addons) :
		name{ name }, lookType{ lookType }, addons{ addons } {}

	std::string name;
	uint16_t lookType;
	uint8_t addons;
};

class Outfits
{
	public:
		static Outfits& getInstance() {
			static Outfits instance;
			return instance;
		}

		bool load();
		const Outfit* getOutfitByLookType(PlayerSex_t sex, uint16_t lookType) const;
		const Outfit* getOutfitByLookType(uint16_t lookType) const;
		const std::vector<Outfit>& getOutfits(PlayerSex_t sex) const {
			return outfits[sex];
		}

	private:
		std::vector<Outfit> outfits[PLAYERSEX_LAST + 1];
};

#endif
