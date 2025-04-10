// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

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
