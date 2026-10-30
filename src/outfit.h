/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Upstream Copyright:
 *   Copyright (C) The Forgotten Server Authors
 *   Governed by the GNU General Public License v2.0 (GPL-2.0)
 *
 * Modifications & BlackTek Core Additions:
 *   Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * ============================================================================
 * DUAL LICENSING & TRANSITION NOTICE
 * ============================================================================
 *
 * This file contains original code from The Forgotten Server alongside substantial
 * modifications and new architecture by BlackTek.
 *
 * To facilitate the project's long-term goal of transitioning the codebase to a
 * fully permissive MIT License:
 *
 * 1. All original TFS source lines remain governed by GPL-2.0.
 * 2. All contributions, modifications, and new additions authored by BlackTek
 *    are dual-licensed under both the MIT License and GPL-2.0.
 * 3. Downstream users distributing this combined codebase must comply with
 *    the GPL-2.0 requirements as long as original upstream TFS code remains.
 *
 * Once an individual module or file has been completely refactored or rewritten
 * such that no original TFS code remains, the file will be relicensed solely
 * under the MIT License.
 *
 * See licensing/original codebase/GNU-GPL (GPL-2.0) and licensing/License (MIT)
 * in the project root for the full texts.
 */

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
			if (static_cast<size_t>(sex) > PLAYERSEX_LAST)
			{
				static const std::vector<Outfit> empty;
				return empty;
			}
			return outfits[sex];
		}

	private:
		std::vector<Outfit> outfits[PLAYERSEX_LAST + 1];
};

#endif
