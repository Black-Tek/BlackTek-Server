// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include <toml++/toml.hpp>
#include "otpch.h"
#include "groups.h"
#include "pugicast.h"
#include "tools.h"

const gtl::flat_hash_map<std::string, PlayerFlags> ParsePlayerFlagMap = {
	{"cannotusecombat", PlayerFlag_CannotUseCombat},
	{"cannotattackplayer", PlayerFlag_CannotAttackPlayer},
	{"cannotattackmonster", PlayerFlag_CannotAttackMonster},
	{"cannotbeattacked", PlayerFlag_CannotBeAttacked},
	{"canconvinceall", PlayerFlag_CanConvinceAll},
	{"cansummonall", PlayerFlag_CanSummonAll},
	{"canillusionall", PlayerFlag_CanIllusionAll},
	{"cansenseinvisibility", PlayerFlag_CanSenseInvisibility},
	{"ignoredbymonsters", PlayerFlag_IgnoredByMonsters},
	{"notgaininfight", PlayerFlag_NotGainInFight},
	{"hasinfinitemana", PlayerFlag_HasInfiniteMana},
	{"hasinfinitesoul", PlayerFlag_HasInfiniteSoul},
	{"hasnoexhaustion", PlayerFlag_HasNoExhaustion},
	{"cannotusespells", PlayerFlag_CannotUseSpells},
	{"cannotpickupitem", PlayerFlag_CannotPickupItem},
	{"canalwayslogin", PlayerFlag_CanAlwaysLogin},
	{"canbroadcast", PlayerFlag_CanBroadcast},
	{"canedithouses", PlayerFlag_CanEditHouses},
	{"cannotbebanned", PlayerFlag_CannotBeBanned},
	{"cannotbepushed", PlayerFlag_CannotBePushed},
	{"hasinfinitecapacity", PlayerFlag_HasInfiniteCapacity},
	{"canpushallcreatures", PlayerFlag_CanPushAllCreatures},
	{"cantalkredprivate", PlayerFlag_CanTalkRedPrivate},
	{"cantalkredchannel", PlayerFlag_CanTalkRedChannel},
	{"talkorangehelpchannel", PlayerFlag_TalkOrangeHelpChannel},
	{"notgainexperience", PlayerFlag_NotGainExperience},
	{"notgainmana", PlayerFlag_NotGainMana},
	{"notgainhealth", PlayerFlag_NotGainHealth},
	{"notgainskill", PlayerFlag_NotGainSkill},
	{"setmaxspeed", PlayerFlag_SetMaxSpeed},
	{"specialvip", PlayerFlag_SpecialVIP},
	{"notgenerateloot", PlayerFlag_NotGenerateLoot},
	{"ignoreprotectionzone", PlayerFlag_IgnoreProtectionZone},
	{"ignorespellcheck", PlayerFlag_IgnoreSpellCheck},
	{"ignoreweaponcheck", PlayerFlag_IgnoreWeaponCheck},
	{"cannotbemuted", PlayerFlag_CannotBeMuted},
	{"isalwayspremium", PlayerFlag_IsAlwaysPremium}
};

bool Groups::load()
{

	const auto file = std::filesystem::current_path().generic_string() + "/config/groups.toml";
	try {
		const auto tbl = toml::parse_file(file);

		for (const auto& [index, entry] : tbl) {

			if (!entry.is_table()) {
				std::cerr << "Invalid entry in groups.toml! \n";
				continue;
			}

			Group group;
			toml::table group_data = *entry.as_table();
			group.id = group_data["id"].value_or(1);
			group.name = group_data["name"].value_or("");
			group.access = group_data["access"].value_or(false);
			group.maxDepotItems = group_data["maxdepotitems"].value_or(0);
			group.maxVipEntries = group_data["maxvipentries"].value_or(0);
			group.flags = 0;

			uint64_t t_flags = 0;
			if (const auto flags = group_data["flags"].as_array()) {
				flags->for_each([&t_flags, &group](auto&& prop) {
					if constexpr (toml::is_string<decltype(prop)>) {
						if (auto match_found = ParsePlayerFlagMap.find(prop.get()); match_found != ParsePlayerFlagMap.end()) {
							t_flags |= match_found->second;
						}
					}
				});
				group.flags = t_flags;
			}
			else if (auto flags = group_data["flags"].as_integer()) {
				group.flags = flags->get();
			}

			groups.push_back(group);
		}
		// todo - trade out exceptions for parse errors to remove overheard of try-catch.
	} catch (const std::exception& e) {
		std::cerr << "Failed to load groups.toml - " << e.what() << std::endl;
	}
	return true;
}

Group* Groups::getGroup(uint16_t id)
{
	for (Group& group : groups) {
		if (group.id == id) {
			return &group;
		}
	}
	return nullptr;
}
