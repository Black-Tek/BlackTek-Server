// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include <toml++/toml.hpp>
#include "otpch.h"

#include "vocation.h"

#include "player.h"
#include "pugicast.h"
#include "tools.h"

static gtl::flat_hash_map<uint16_t, SkillRegistry> vocation_skills;

Vocation* Vocations::getVocation(uint16_t id)
{
	auto it = vocationsMap.find(id);
	if (it == vocationsMap.end()) {
		std::cout << "[Warning - Vocations::getVocation] Vocation " << id << " not found." << std::endl;
		return nullptr;
	}
	return &it->second;
}

bool Vocations::addVocationSkill(uint32_t vocationId, std::string_view name, const std::shared_ptr<CustomSkill>& skill)
{
	auto& skillMap = vocation_skills[vocationId];
	return skillMap.try_emplace(name, skill).second;
}

std::optional<std::shared_ptr<CustomSkill>> Vocations::getVocationSkill(std::string_view name, uint16_t vocation_id)
{
	if (auto it = vocation_skills.find(vocation_id); it != vocation_skills.end()) 
	{
		const auto& skills = it->second;
		if (auto skillIt = skills.find(name); skillIt != skills.end()) 
		{
			return skillIt->second;
		}
	}
	return std::nullopt;
}

// std::expected candidate <container, bool>
// it would save on the overhead of creating the empty container
// and assigning it later down the line of execution perhaps?
SkillRegistry Vocations::getRegisteredSkills(uint16_t vocation_id)
{
	if (auto it = vocation_skills.find(vocation_id); it != vocation_skills.end())
	{
		return it->second;
	}
	return SkillRegistry();
}

std::string trimString(const std::string& str) {
	const auto strBegin = str.find_first_not_of(" \t");
	if (strBegin == std::string::npos) return "";

	const auto strEnd = str.find_last_not_of(" \t");
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

int32_t Vocations::getVocationId(std::string_view name) const
{
	std::string trimmedName = trimString(std::string(name));

	auto it = std::find_if(vocationsMap.begin(), vocationsMap.end(),
		[&trimmedName](const auto& pair) {
			bool result = caseInsensitiveEqual(trimmedName, pair.second.name);
			return result;
		});

	return it != vocationsMap.end() ? it->first : -1;
}

uint16_t Vocations::getPromotedVocation(uint16_t id) const
{
	auto it = std::find_if(vocationsMap.begin(), vocationsMap.end(), [id](auto it) {
		return it.second.fromVocation == id && it.first != id;
	});
	return it != vocationsMap.end() ? it->first : VOCATION_NONE;
}

static const uint32_t skillBase[SKILL_LAST + 1] = {50, 50, 50, 50, 30, 100, 20};

uint64_t Vocation::getReqSkillTries(uint8_t skill, uint16_t level) const
{
	if (skill > SKILL_LAST) {
		return 0;
	}
	return skillBase[skill] * std::pow(skillMultipliers[skill], static_cast<int32_t>(level - (MINIMUM_SKILL_LEVEL + 1)));
}

uint64_t Vocation::getReqMana(uint32_t magLevel) const
{
	if (magLevel == 0) {
		return 0;
	}
	return 1600 * std::pow(manaMultiplier, static_cast<int32_t>(magLevel - 1));
}

bool Vocations::loadFromToml() {
	bool loaded = false;

	for (const auto& file : std::filesystem::recursive_directory_iterator(folder)) 
	{
		if (file.is_regular_file() and file.path().extension() == ".toml") 
		{
			try 
			{
				auto vocation_file = toml::parse_file(file.path().string());

				for (const auto& [index, entry] : vocation_file) 
				{
					if (not entry.is_table()) 
					{
						std::cerr << "Invalid entry in file: " << file.path() << "\n";
						continue;
					}

					toml::table vocation_data = *entry.as_table();
					const auto vocId = vocation_data["id"].value_or(0);
					// to-do: I hate adding directly to the map here as a practice, it's technically slowing down the IO
					// would be best practice to take non-blocking approach for IO's
					auto res = vocationsMap.emplace(std::piecewise_construct, std::forward_as_tuple(vocId), std::forward_as_tuple(vocId));
					Vocation& vocation = res.first->second;
					vocation.name = vocation_data["name"].value_or("");
					vocation.clientId = vocation_data["cid"].value_or(0);
					vocation.allowPvp = vocation_data["pvp"].value_or(false);
					vocation.description = vocation_data["description"].value_or("");
					vocation.baseSpeed = vocation_data["speed"].value_or(220);
					vocation.soulMax = vocation_data["maxsoul"].value_or(100);
					vocation.attackSpeed = vocation_data["attackspeed"].value_or(2000);
					vocation.fromVocation = vocation_data["promotedfrom"].value_or(0);
					vocation.noPongKickTime = (vocation_data["inactivekicktime"].value_or(60) * 1000);

					if (auto stats = vocation_data["statsperlevel"].as_array()) 
					{
						stats->for_each([&vocation](auto&& prop) 
							{
								if (prop.is_table()) 
								{
									auto& stat = *prop.as_table();
									// client handles uint32 like a float
									// so we must times capacity by 100
									vocation.gainCap = stat["cap"].value_or(10) * 100;
									vocation.gainHP = stat["hp"].value_or(5);
									vocation.gainMana = stat["mana"].value_or(5);
								}
							}
						);
					}

					if (auto regen = vocation_data["regeneration"].as_array()) 
					{
						regen->for_each([&vocation](auto&& prop) 
							{
								if (prop.is_table()) 
								{
									auto& r = *prop.as_table();
									if (auto hp = r["hp"].as_table()) 
									{
										vocation.gainHealthAmount = hp->at("amount").value_or(1);
										vocation.gainHealthTicks = hp->at("interval").value_or(6);
									}
									if (auto mana = r["mana"].as_table()) 
									{
										vocation.gainManaAmount = mana->at("amount").value_or(1);
										vocation.gainManaTicks = mana->at("interval").value_or(6);
									}
									if (auto soul = r["soul"].as_table()) 
									{
										vocation.gainSoulAmount = soul->at("amount").value_or(1);
										vocation.gainSoulTicks = soul->at("interval").value_or(120);
									}
								}
							}
						);
					}

					if (auto skills = vocation_data["skillrates"].as_array()) 
					{
						skills->for_each([&vocation](auto&& prop) 
							{
								if (prop.is_table()) 
								{
									auto& s = *prop.as_table();
									vocation.manaMultiplier = s["magic"].value_or(4.0f);
									vocation.skillMultipliers[SKILL_FIST] = s["fist"].value_or(1.5f);
									vocation.skillMultipliers[SKILL_CLUB] = s["club"].value_or(2.0f);
									vocation.skillMultipliers[SKILL_SWORD] = s["sword"].value_or(2.0f);
									vocation.skillMultipliers[SKILL_AXE] = s["axe"].value_or(2.0f);
									vocation.skillMultipliers[SKILL_DISTANCE] = s["distance"].value_or(2.0f);
									vocation.skillMultipliers[SKILL_SHIELD] = s["shield"].value_or(1.5f);
									vocation.skillMultipliers[SKILL_FISHING] = s["fishing"].value_or(1.1f);
								}
							}
						);
					}



					if (auto extra_skills = vocation_data["extraskills"].as_array()) 
					{
						extra_skills->for_each([vocId, &vocation](auto&& prop) 
							{
								if (prop.is_table()) 
								{
									auto& s = *prop.as_table();
									std::string_view skill_name = s["name"].value_or("none");

									if (skill_name != "none")
									{
										uint16_t max = s["maxlevel"].value_or(0.0f);
										float multiplier = s["mutiplier"].value_or(1.1f);
										float threshold = s["threshold"].value_or(10.0f);
										float difficulty = s["difficulty"].value_or(50.0f);
										FormulaType formula = ParseFormula(s["formula"].value_or("default"));
										auto custom_skill = Components::Skills::CustomSkill::make_skill(formula, max, multiplier, threshold, difficulty);
										Vocations::addVocationSkill(vocId, skill_name, std::move(custom_skill));
									}
								}
							}
						);
					}
				}
				loaded = true;
			}
			catch (const std::exception& e) 
			{
				std::cerr << "Failed to load TOML file: " << file.path() << " - " << e.what() << std::endl;
			}
		}
	}

	return loaded;
}
