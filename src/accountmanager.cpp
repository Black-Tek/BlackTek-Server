#include "accountmanager.h"
#include <toml++/toml.hpp>
#include "otpch.h"
#include "game.h"

void AccountManager::initialize()
{
	const auto file = std::filesystem::current_path().generic_string() + "/config/account_manager.toml";
	try 
	{
		const auto tbl = toml::parse_file(file);
		int32_t option_id = 0;
		for (const auto& [index, entry] : tbl) 
		{

			if (!entry.is_table()) 
			{
				std::cerr << "Invalid entry in accountmanager.toml! \n";
				continue;
			}

			CharacterOption option;
			toml::table option_data = *entry.as_table();

			option.vocation = option_data["vocation"].value_or(0);
			option.premium = option_data["premium"].value_or(false);

			if (const auto& towns = option_data["towns"].as_array()) 
			{
				for (const auto& town : *towns) 
				{
					if (town.is_integer()) 
					{
						option.town_list.push_back(static_cast<int32_t>(town.as_integer()->get()));
						option.needsPosition = false;
					}
					else 
					{
						std::cerr << "Invalid town entry (not an integer) in accountmanager.toml!\n";
					}
				}
			}
			else if (const auto& spawn = option_data["spawn"].as_table()) 
			{
				if (option.needsPosition) 
				{
					const auto& position = *spawn;
					auto x = static_cast<uint16_t>(position["x"].value_or(0));
					auto y = static_cast<uint16_t>(position["y"].value_or(0));
					auto z = static_cast<uint8_t>(position["z"].value_or(0));
					option.startingPos = { x, y, z };
					option.needsPosition = false;
				}
			} // todo else { // log warning / error}

			if (const auto& outfit_data = option_data["outfit"].as_table()) 
			{
				const auto& outfit = *outfit_data;
				option.outfit[0] = outfit["id"].value_or(0);
				option.outfit[1] = outfit["ex"].value_or(0);
				option.outfit[2] = outfit["mount"].value_or(0);
				option.outfit[3] = outfit["head"].value_or(0);
				option.outfit[4] = outfit["body"].value_or(0);
				option.outfit[5] = outfit["legs"].value_or(0);
				option.outfit[6] = outfit["feet"].value_or(0);
				option.outfit[7] = outfit["addons"].value_or(0);
			} // todo else { // log warning / error}

			if (const auto& skill_data = option_data["skills"].as_table()) 
			{
				const auto& skill = *skill_data;
				option.skills[0] = skill["fist"].value_or(1);
				option.skills[1] = skill["club"].value_or(1);
				option.skills[2] = skill["sword"].value_or(1);
				option.skills[3] = skill["axe"].value_or(1);
				option.skills[4] = skill["distance"].value_or(1);
				option.skills[5] = skill["shield"].value_or(1);
				option.skills[6] = skill["fishing"].value_or(1);
				option.magiclevel = skill["magiclevel"].value_or(1);
			} // todo else { // log warning / error}

			option.name = std::string(index.str());
			option.id = option_id;
			g_game.addCharacterOption(option);
			++option_id;
		}
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Failed to load account_manager.toml - " << e.what() << std::endl;
	}
}