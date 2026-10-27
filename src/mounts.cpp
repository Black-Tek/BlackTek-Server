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

#include "mounts.h"

#include "pugicast.h"
#include "tools.h"
#include <toml++/toml.hpp>

bool Mounts::reload()
{
	mounts.clear();
	return loadFromToml();
}

bool Mounts::loadFromToml()
{
	try {
		auto config = toml::parse_file("config/mounts.toml");

		for (const auto& [key, value] : config) {
			if (!value.is_table()) {
				continue;
			}

			const auto& table = *value.as_table();
			uint16_t id = table["id"].value_or(0);
			if (id == 0 || id > std::numeric_limits<uint8_t>::max()) {
				continue;
			}

			if (getMountByID(static_cast<uint8_t>(id))) {
				continue;
			}

			mounts.emplace_back(
				static_cast<uint8_t>(id),
				table["clientid"].value_or(0),
				std::string(key.str()),
				table["speed"].value_or(0),
				table["premium"].value_or(false)
			);
		}
	}
	catch (const toml::parse_error& err) {
		std::cerr << "Error - Mounts::loadFromToml: " << err.description()
			<< "\n  at " << err.source().begin << std::endl;
		return false;
	}

	mounts.shrink_to_fit();
	return true;
}


Mount* Mounts::getMountByID(uint8_t id)
{
	auto it = std::find_if(mounts.begin(), mounts.end(), [id](const Mount& mount) {
		return mount.id == id;
	});

	return it != mounts.end() ? &*it : nullptr;
}

Mount* Mounts::getMountByName(const std::string& name) {
	auto mountName = name.c_str();
	for (auto& it : mounts) {
		if (caseInsensitiveEqual(mountName, it.name)) {
			return &it;
		}
	}

	return nullptr;
}

Mount* Mounts::getMountByClientID(uint16_t clientId)
{
	auto it = std::find_if(mounts.begin(), mounts.end(), [clientId](const Mount& mount) {
		return mount.clientId == clientId;
	});

	return it != mounts.end() ? &*it : nullptr;
}
