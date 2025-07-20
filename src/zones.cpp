// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "zones.h"
#include "configmanager.h"

#include <toml++/toml.hpp>

extern ConfigManager g_config;

std::vector<Zone> g_zones = { Zone(0) };
std::multimap<Position, int> p_zones = {};

size_t Zones::count()
{
	return g_zones.size();
}

std::vector<Zone>& Zones::get()
{
	return g_zones;
}

Zone& Zones::getZone(int id)
{
	auto& zones = Zones::get();
	return zones[id];
}

std::vector<int> Zones::getZonesByPosition(const Position& position)
{
	std::vector<int> result;
	auto range = p_zones.equal_range(position);

	for (auto& it = range.first; it != range.second; ++it)
	{
		result.push_back(it->second);
	}

	return result;
}

bool Zones::registerZone(Zone zone)
{
	auto& zones = Zones::get();
	if (zone.id < zones.size() and zones[zone.id].id == zone.id)
	{
		return false;
	}

	if (zone.id >= zones.size())
	{
		zones.resize(static_cast<std::vector<Zone, std::allocator<Zone>>::size_type>(zone.id) + 1);
	}

	for (const auto& position : zone.positions)
	{
		p_zones.insert(std::make_pair(position, zone.id));
	}

	zones[zone.id] = std::move(zone);
	return true;
}

// warning, don't use 0 as the id, it's not properly handled
Zone& Zones::createZone(int id, std::vector<Position> positions)
{
	auto& zones = Zones::get();

	if (id < zones.size() and zones[id].id == id)
	{
		return zones[id];
	}

	auto newZone = Zone(id);
	if (id >= zones.size())
	{
		zones.resize(static_cast<std::vector<Zone, std::allocator<Zone>>::size_type>(id) + 1);
	}

	for (const auto& position : positions)
	{
		p_zones.insert(std::make_pair(position, id));
	}

	newZone.positions = std::move(positions);
	auto& zoneRef = zones[id] = std::move(newZone);
	return zoneRef;
}

void Zones::load()
{
	auto folder = "data/world/" + g_config.getString(ConfigManager::MAP_NAME) + "-zones";
	if (std::filesystem::exists(folder) and std::filesystem::is_directory(folder))
	{
		for (const auto& file : std::filesystem::recursive_directory_iterator(folder))
		{
			if (file.is_regular_file() and file.path().extension() == ".toml")
			{
				try
				{
					toml::table tbl = toml::parse_file(file.path().c_str());
					auto zonesArray = tbl["zone"];
					if (zonesArray and zonesArray.is_array())
					{
						for (const auto& zoneEntry : *zonesArray.as_array())
						{
							if (zoneEntry.is_table())
							{
								const toml::table& zoneTable = *zoneEntry.as_table();
								auto id = zoneTable["id"];
								auto posArray = zoneTable["positions"];
								if (id and id.is_integer() and posArray and posArray.is_array())
								{
									uint16_t zoneId = static_cast<uint16_t>(id.value_or(0));

									if (zoneId == 0)
									{
										// log / error id should not be 0;
										continue;
									}
									std::vector<Position> positions{};
									for (const auto& position : *posArray.as_array())
									{
										if (position.is_table())
										{
											const toml::table& position_data = *position.as_table();
											uint16_t x = static_cast<uint16_t>(position_data["x"].value_or(0));
											uint16_t y = static_cast<uint16_t>(position_data["y"].value_or(0));
											uint8_t z = static_cast<uint8_t>(position_data["z"].value_or(0));
											positions.emplace_back(x, y, z);
										}
									}
									auto zone = Zone(zoneId);
									zone.positions = std::move(positions);
									if (auto registered = Zones::registerZone(std::move(zone)))
									{
										// success!
										continue;
									}
									// failed to register
									// log
								}
							}
						}
					}
					else
					{
						//log ("Invalid zone file: %s", filepath);
					}
				}
				catch (const toml::parse_error& err) {
					// log ("TOML parse error in file %s: %s", filepath, err.what());
				}
			}
		}
	}
	else
	{
		std::error_code ec;
		if (not std::filesystem::create_directories(folder, ec))
		{
			// log 
			std::cout << "Failed to detect and failed to create zones folder... skipping zones! '" << folder << "': " << ec.message() << std::endl;
			return;
		}
	}
}

void Zones::clear()
{
	g_zones.clear();
	p_zones.clear();
}

void Zones::reload()
{
	// todo : can change all these voids to bools,
	// and then check if they are both true for the reload
	// then send appropriate messages / logs for each case.
	clear();
	load();
}