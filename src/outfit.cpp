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

#include "otpch.h"

#include "outfit.h"

#include "pugicast.h"
#include "tools.h"
#include "console.h"

#include <toml++/toml.hpp>

bool Outfits::load()
{

	const auto file = std::filesystem::current_path().generic_string() + "/config/outfits.toml";
	try 
	{
		const auto tbl = toml::parse_file(file);

		for (const auto& [index, entry] : tbl) 
		{

			if (!entry.is_table()) 
			{
				std::cerr << "Invalid entry in outfits.toml! \n";
				continue;
			}

			toml::table outfit_data = *entry.as_table();
			int32_t sex = 0;
			// Todo : Break the sexes down into two separate toml files.
			if (auto sexType = outfit_data["sex"].as_string())
			{
				const auto& noun = sexType->get();
				if (noun == "Man" or noun == "man" or noun == "male" or noun == "Male")
				{
					sex = 1;
				}
				// rather than throw exceptions, we default to female if you don't specify male
			}
			else if (auto sexType = outfit_data["sex"].as_integer()) 
			{
				sex = sexType->get();
			}
			else
			{
				BlackTek::Console::Warn("Outfits::load: outfit entry '{}' has no valid 'sex' field, defaulting to female", index.str());
			}

			int32_t id = outfit_data["id"].value_or(0);
			const auto& name = outfit_data["name"].value_or("none");
			bool premium = outfit_data["premium"].value_or(false);
			bool locked = outfit_data["locked"].value_or(false);

			// todo, encapsulate entry parsing inside the below branch
			// because if it's not enabled we are wasting resources
			if (outfit_data["enabled"].value_or(true))
			{
				outfits[sex].emplace_back(name, id, premium, locked);
			}
		}
		// todo - trade out exceptions for parse errors to remove overheard of try-catch.
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Failed to load outfits.toml - " << e.what() << std::endl;
	}
	return true;
}


const Outfit* Outfits::getOutfitByLookType(PlayerSex_t sex, uint16_t lookType) const
{
	for (const Outfit& outfit : outfits[sex]) {
		if (outfit.lookType == lookType) {
			return &outfit;
		}
	}
	return nullptr;
}

const Outfit* Outfits::getOutfitByLookType(uint16_t lookType) const
{
	for (uint8_t sex = PLAYERSEX_FEMALE; sex <= PLAYERSEX_LAST; sex++) {
		for (const Outfit& outfit : outfits[sex]) {
			if (outfit.lookType == lookType) {
				return &outfit;
			}
		}
	}
	return nullptr;
}
