#include "accountmanager.h"
#include <toml++/toml.hpp>
#include <filesystem>
#include <iostream>

// static std::vector<CharacterOption> character_options;

void AccountManager::initialize()
{
	const auto file = std::filesystem::current_path().generic_string() + toml_path;
	try {
		const auto tbl = toml::parse_file(file);

		for (const auto& [index, entry] : tbl) {

			if (!entry.is_table()) {
				std::cerr << "Invalid entry in accountmanager.toml! \n";
				continue;
			}

			CharacterOption option;

			toml::table option_data = *entry.as_table();

			option.vocation = option_data["vocation"].value_or(0);
			option.premium = option_data["premium"].value_or(false);

			if (const auto towns = option_data["towns"].as_array()) {
				for (const auto& town : *towns) {

					if (town.is_integer()) {
						option.town_list.push_back(static_cast<int32_t>(town.as_integer()->get()));
						option.needsPosition = false;
					}
					else {
						std::cerr << "Invalid town entry (not an integer) in accountmanager.toml!\n";
					}
				}
			}
			else if (const auto& spawn = option_data["spawn"].as_array()) {
				for (const auto& entry : *spawn) {
					if (entry.is_table() and option.needsPosition) {

						auto& position = *entry.as_table();
						auto x = static_cast<uint16_t>(position["x"].value_or(0));
						auto y = static_cast<uint16_t>(position["y"].value_or(0));
						auto z = static_cast<uint8_t>(position["z"].value_or(0));
						option.startingPos = { x, y, z };
						option.needsPosition = false;
					}
				}
			}

			if (const auto& outfit_data = option_data["outfit"].as_array()) {
				for (const auto& entry_data : *outfit_data) {

					if (not entry_data.is_table()) {
						std::cerr << "Invalid outfit entry (not a table) in accountmanager.toml \n";
						continue;
					}

					const auto& outfit = *entry_data.as_table();
					option.outfit[0] = outfit["lookType"].value_or(0);
					option.outfit[1] = outfit["lookTypeEx"].value_or(0);
					option.outfit[2] = outfit["lookMount"].value_or(0);
					option.outfit[3] = outfit["lookHead"].value_or(0);
					option.outfit[4] = outfit["lookBody"].value_or(0);
					option.outfit[5] = outfit["lookLegs"].value_or(0);
					option.outfit[6] = outfit["lookFeet"].value_or(0);
					option.outfit[7] = outfit["lookAddons"].value_or(0);
				}
			}

			if (const auto& skill_data = option_data["skills"].as_array()) {
				for (const auto& skill_table : *skill_data) {

					if (not skill_table.is_table()) {
						std::cerr << "Invalid skill entry (not a table) in accountmanager.toml \n";
						continue;
					}

					const auto& skill = *skill_table.as_table();

					option.skills[0] = skill["fist"].value_or(1);
					option.skills[1] = skill["club"].value_or(1);
					option.skills[2] = skill["sword"].value_or(1);
					option.skills[3] = skill["axe"].value_or(1);
					option.skills[4] = skill["distance"].value_or(1);
					option.skills[5] = skill["shield"].value_or(1);
					option.skills[6] = skill["fishing"].value_or(1);
					option.magiclevel = skill["magiclevel"].value_or(1);

				}
			}
		}
	}
	catch (const std::exception& e) {

	}
}
