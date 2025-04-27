// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "quests.h"

#include "pugicast.h"

std::string Mission::getDescription(const PlayerPtr& player) const
{
	int32_t value;
	player->getStorageValue(storageID, value);

	if (!mainDescription.empty()) {
		std::string desc = mainDescription;
		replaceString(desc, "|STATE|", std::to_string(value));
		replaceString(desc, "\\n", "\n");
		return desc;
	}

	if (ignoreEndValue) {
		for (int32_t current = endValue; current >= startValue; current--) {
			if (value >= current) {
				auto sit = descriptions.find(current);
				if (sit != descriptions.end()) {
					return sit->second;
				}
			}
		}
	} else {
		for (int32_t current = endValue; current >= startValue; current--) {
			if (value == current) {
				auto sit = descriptions.find(current);
				if (sit != descriptions.end()) {
					return sit->second;
				}
			}
		}
	}
	return "An error has occurred, please contact a gamemaster.";
}

bool Mission::isStarted(const PlayerPtr& player) const
{
	if (!player) {
		return false;
	}

	int32_t value;
	if (!player->getStorageValue(storageID, value)) {
		return false;
	}

	if (value < startValue) {
		return false;
	}

	if (!ignoreEndValue && value > endValue) {
		return false;
	}

	return true;
}

bool Mission::isCompleted(const PlayerPtr& player) const
{
	if (!player) {
		return false;
	}

	int32_t value;
	if (!player->getStorageValue(storageID, value)) {
		return false;
	}

	if (ignoreEndValue) {
		return value >= endValue;
	}

	return value == endValue;
}

std::string Mission::getName(const PlayerPtr& player) const
{
	if (isCompleted(player)) {
		return name + " (completed)";
	}
	return name;
}

uint16_t Quest::getMissionsCount(const PlayerPtr& player) const
{
	uint16_t count = 0;
	for (const Mission& mission : missions) {
		if (mission.isStarted(player)) {
			count++;
		}
	}
	return count;
}

bool Quest::isCompleted(const PlayerPtr& player) const
{
	for (const Mission& mission : missions) {
		if (!mission.isCompleted(player)) {
			return false;
		}
	}
	return true;
}

bool Quest::isStarted(const PlayerPtr& player) const
{
	if (!player) {
		return false;
	}

	int32_t value;
	if (!player->getStorageValue(startStorageID, value) || value < startStorageValue) {
		return false;
	}

	return true;
}

bool Quests::reload()
{
	quests.clear();
	return loadFromToml();
}

bool Quests::loadFromToml()
{
	for (const auto& entry : std::filesystem::recursive_directory_iterator("data/quests/"))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".toml")
		{
			try {
				auto tbl = toml::parse_file(entry.path().string());
				uint16_t id = 0;

				// Iterate over each top-level table (each represents a quest)
				for (const auto& [key, value] : tbl) {
					if (value.is_table()) {

						auto quest_table = value.as_table();
						std::string name = (*quest_table)["name"].value<std::string>().value_or("");
						int32_t startstorage = static_cast<int32_t>((*quest_table)["startstorage"].value<int64_t>().value_or(0));
						int32_t startvalue = static_cast<int32_t>((*quest_table)["startvalue"].value<int64_t>().value_or(0));

						quests.emplace_back(name, ++id, startstorage, startvalue);
						Quest& quest = quests.back();

						if (auto missions_node = (*quest_table)["missions"]; missions_node.is_array()) {
							auto missions_array = missions_node.as_array();


							for (const auto& mission_val : *missions_array) {
								if (mission_val.is_table()) {

									auto mission_table = mission_val.as_table();
									std::string mission_name = (*mission_table)["name"].value<std::string>().value_or("");
									int32_t storage = 0;

									if (auto storage_node = (*mission_table)["storage"]; storage_node.is_integer()) {
										storage = static_cast<int32_t>(storage_node.value<int64_t>().value_or(0));
									}

									int32_t start = 0;
									if (auto start_node = (*mission_table)["start"]; start_node.is_integer()) {
										start = static_cast<int32_t>(start_node.value<int64_t>().value_or(0));
									}

									int32_t end = 0;
									if (auto end_node = (*mission_table)["end"]; end_node.is_integer()) {
										end = static_cast<int32_t>(end_node.value<int64_t>().value_or(0));
									}

									bool ignoreend = (*mission_table)["ignoreend"].value<bool>().value_or(false);
									quest.missions.emplace_back(mission_name, storage, start, end, ignoreend);
									Mission& mission = quest.missions.back();

									// Handle description or states, why should we not eventually get to have both?
									// need to check into what the client will allow for this...
									if (auto desc_node = (*mission_table)["description"]; desc_node.is_string()) {
										mission.mainDescription = desc_node.value<std::string>().value_or("");
									}
									else if (auto states_node = (*mission_table)["states"]; states_node.is_array()) {
										auto states_array = states_node.as_array();
										for (const auto& state_val : *states_array) {
											if (state_val.is_table()) {
												auto state_table = state_val.as_table();
												int32_t state_id = static_cast<int32_t>((*state_table)["id"].value<int64_t>().value_or(0));
												std::string desc = (*state_table)["description"].value<std::string>().value_or("");
												mission.descriptions.emplace(state_id, desc);
											}
										}
									}
								}
							}
						}
					}
				}
			}
			catch (const toml::parse_error& err) {
				std::cerr << "Failed to parse "<< entry.path().string() << ". Reason : " << err.what() << " \n";
				return false;
			}
		}
	}
	return true;
}

Quest* Quests::getQuestByID(uint16_t id)
{
	for (Quest& quest : quests) {
		if (quest.id == id) {
			return &quest;
		}
	}
	return nullptr;
}

uint16_t Quests::getQuestsCount(const PlayerPtr& player) const
{
	uint16_t count = 0;
	for (const Quest& quest : quests) {
		if (quest.isStarted(player)) {
			count++;
		}
	}
	return count;
}

bool Quests::isQuestStorage(const uint32_t key, const int32_t value, const int32_t oldValue) const
{
	for (const Quest& quest : quests) {
		if (quest.getStartStorageId() == key && quest.getStartStorageValue() == value) {
			return true;
		}

		for (const Mission& mission : quest.getMissions()) {
			if (mission.getStorageId() == key && value >= mission.getStartStorageValue() && value <= mission.getEndStorageValue()) {
				return mission.mainDescription.empty() || oldValue < mission.getStartStorageValue() || oldValue > mission.getEndStorageValue();
			}
		}
	}
	return false;
}
