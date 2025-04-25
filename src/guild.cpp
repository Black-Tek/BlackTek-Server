// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "guild.h"

#include "game.h"

extern Game g_game;

void Guild::addMember(const PlayerPtr& player)
{
	membersOnline.push_back(player);
	for (const auto member : membersOnline) {
		g_game.updatePlayerHelpers(member);
	}
}

void Guild::removeMember(const PlayerPtr& player)
{
	membersOnline.remove(player);
	for (const auto member : membersOnline) {
		g_game.updatePlayerHelpers(member);
	}
	g_game.updatePlayerHelpers(player);
}

GuildRank_ptr Guild::getRankById(const uint32_t rankId) const
{
	for (auto rank : ranks) {
		if (rank->id == rankId) {
			return rank;
		}
	}
	return nullptr;
}

GuildRank_ptr Guild::getRankByName(const std::string& name) const
{
	for (auto rank : ranks) {
		if (rank->name == name) {
			return rank;
		}
	}
	return nullptr;
}

GuildRank_ptr Guild::getRankByLevel(uint8_t level) const
{
	for (auto rank : ranks) {
		if (rank->level == level) {
			return rank;
		}
	}
	return nullptr;
}

void Guild::addRank(uint32_t rankId, std::string_view rankName, uint8_t level)
{
	ranks.emplace_back(std::make_shared<GuildRank>(rankId, rankName, level));
}

bool Guild::addWar(uint32_t guildId, GuildWar& war)
{
	if (isInWar(guildId))
		return false;

	m_mGuildWars.emplace(guildId, std::move(war));
	return true;
}

bool Guild::removeWar(uint32_t guildId)
{
	if (!isInWar(guildId))
		return false;

	m_mGuildWars.erase(guildId);
	return true;
}

bool Guild::isInWar(uint32_t enemyGuildId)
{
	if (m_mGuildWars.find(enemyGuildId) == m_mGuildWars.end())
		return false;

	const auto& war = getWar(enemyGuildId);
	if(!war) {
		return false;
	}

	// Check if the war is inactive
	// Probably unecessary, but just to be sure
	if (war->status != WAR_ACTIVE && war->status != WAR_WITHDRAW_PROPOSED)
		return false;

	return true;
}

bool Guild::isInAnyWar()
{
	if (m_mGuildWars.empty()) {
		return false;
	}

	for (const auto& war : m_mGuildWars) {
		if (war.second.status == WAR_ACTIVE || war.second.status == WAR_WITHDRAW_PROPOSED)
			return true;
	}

	return false;
}

bool IOGuild::loadGuilds()
{
	Database& db = Database::getInstance();
	auto result = db.storeQuery("SELECT `id`, `name`, `balance` FROM `guilds`");
	if (!result) {
		return false;
	}

	do {
		uint32_t guildId = result->getNumber<uint32_t>("id");
		auto name = result->getString("name");
		uint64_t guildBalance = result->getNumber<uint64_t>("balance");

		auto guild = std::make_shared<Guild>(guildId, name);
		guild->setGuildBankBalance(guildBalance);

		// Load Ranks
		auto rankResult = db.storeQuery(fmt::format(
			"SELECT `id`, `name`, `level` FROM `guild_ranks` WHERE `guild_id` = {}", guildId));
		if (rankResult) {
			do {
				guild->addRank(
					rankResult->getNumber<uint32_t>("id"),
					rankResult->getString("name"),
					rankResult->getNumber<uint16_t>("level")
				);
			} while (rankResult->next());
		}

		IOGuild::getWarList(guild);

		g_game.addGuild(guild);
	} while (result->next());

	return true;
}

Guild_ptr IOGuild::loadGuild(uint32_t guildId)
{
	Database& db = Database::getInstance();
	if (DBResult_ptr result = db.storeQuery(fmt::format("SELECT `name`, `balance` FROM `guilds` WHERE `id` = {:d}", guildId))) {
		auto name = result->getString("name");
		uint64_t guildBalance = result->getNumber<uint64_t>("balance");

		const auto& guild = std::make_shared<Guild>(guildId, name);
		guild->setGuildBankBalance(guildBalance);

		if ((result = db.storeQuery(fmt::format("SELECT `id`, `name`, `level` FROM `guild_ranks` WHERE `guild_id` = {:d}", guildId)))) {
			do {
				guild->addRank(result->getNumber<uint32_t>("id"), result->getString("name"), result->getNumber<uint16_t>("level"));
			} while (result->next());
		}

		IOGuild::getWarList(guild);

		return guild;
	}
	return nullptr;
}

uint32_t IOGuild::getGuildIdByName(const std::string& name)
{
	Database& db = Database::getInstance();

	DBResult_ptr result = db.storeQuery(fmt::format("SELECT `id` FROM `guilds` WHERE `name` = {:s}", db.escapeString(name)));
	if (!result) {
		return 0;
	}
	return result->getNumber<uint32_t>("id");
}

void IOGuild::getWarList(const Guild_ptr& guild)
{
	uint32_t guildId = guild->getId();
	Database& db = Database::getInstance();

	// Load only active/ongoing wars
	auto result = db.storeQuery(fmt::format(
		"SELECT gw.*, "
		"COALESCE(SUM(CASE WHEN gk.killerguild = {} THEN 1 ELSE 0 END), 0) AS frags "
		"FROM `guild_wars` AS gw "
		"LEFT JOIN `guildwar_kills` AS gk ON gk.warid = gw.id "
		"WHERE (gw.guild1 = {} OR gw.guild2 = {}) "
		"AND gw.status IN ({}, {}) "
		"GROUP BY gw.id",
		guildId, guildId, guildId, static_cast<int>(WarStatus::WAR_ACTIVE), static_cast<int>(WarStatus::WAR_WITHDRAW_PROPOSED)
	));

	if (!result) {
		return;
	}

	do {
		uint32_t guild1 = result->getNumber<uint32_t>("guild1");
		uint32_t guild2 = result->getNumber<uint32_t>("guild2");

		GuildWar war;
		war.id = result->getNumber<uint32_t>("id");
		war.status = result->getNumber<uint8_t>("status");
		war.frags = result->getNumber<uint32_t>("frags");
		war.started = result->getNumber<uint64_t>("started");
		war.ended = result->getNumber<uint64_t>("ended");

		uint32_t opponentId = (guildId == guild1 ? guild2 : guild1);
		guild->addWar(opponentId, war);
	} while (result->next());
}