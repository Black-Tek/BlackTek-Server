// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_GUILD_H
#define FS_GUILD_H
#include "creature.h"

class Player;

enum WarStatus : uint8_t {
	WAR_PENDING = 0, // There is an invite between guilds
	WAR_ACTIVE = 1,
	WAR_REJECTED = 2, // The invitation to war was rejected
	WAR_CANCELLED = 3, // The invitation to war was cancelled
	WAR_WITHDRAW_PROPOSED = 4, // War is still in progress until it is accepted
	WAR_WITHDRAW_ACCEPTED = 5, // War finished by withdraw
	WAR_FINISHED_KILLS = 6, // War finished by kills
	WAR_FINISHED_TIME = 7, // War finished by time
};

struct GuildWar {
	uint32_t id = 0;
	uint8_t status = 0;
	uint8_t frags = 0;
	uint8_t fragsToEnd = 0;

	time_t started = 0;
	time_t ended = 0;

	GuildWar() = default;
};
using GuildWarMap = std::map<uint32_t, GuildWar>;

struct GuildRank {
	uint32_t id;
	std::string name;
	uint8_t level;

	GuildRank(const uint32_t id, const std::string_view name, const uint8_t level) : id{ id }, name{ name }, level{ level } {}
};

using GuildRank_ptr = std::shared_ptr<GuildRank>;

class Guild
{
	public:
		Guild(const uint32_t id, const std::string_view name) : name{ name }, id{ id } {}

		void addMember(const PlayerPtr& player);
		void removeMember(const PlayerPtr& player);

		uint32_t getId() const {
			return id;
		}
	
		const std::string& getName() const {
			return name;
		}
	
		const std::list<PlayerPtr>& getMembersOnline() const {
			return membersOnline;
		}
	
		uint32_t getMemberCount() const {
			return memberCount;
		}
	
		void setMemberCount(const uint32_t count) {
			memberCount = count;
		}

		const std::vector<GuildRank_ptr>& getRanks() const {
			return ranks;
		}
	
		GuildRank_ptr getRankById(uint32_t rankId) const;
		GuildRank_ptr getRankByName(const std::string& name) const;
		GuildRank_ptr getRankByLevel(uint8_t level) const;
	
		void addRank(uint32_t rankId, std::string_view rankName, uint8_t level);

		const std::string& getMotd() const {
			return motd;
		}
	
		void setMotd(const std::string& motd) {
			this->motd = motd;
		}

		uint32_t getGuildBankBalance() const {
			return guildBankBalance;
		}
		void setGuildBankBalance(uint32_t balance) {
			guildBankBalance = balance;
		}

		bool addWar(uint32_t guildId, GuildWar& war);
		bool removeWar(uint32_t guildId);
		bool isInWar(uint32_t guildId);
		bool isInAnyWar();
		GuildWar* getWar(uint32_t enemyGuildId) {
			auto it = m_mGuildWars.find(enemyGuildId);
			return (it != m_mGuildWars.end()) ? &it->second : nullptr;
		}
		GuildWarMap& getWars() {
			return m_mGuildWars;
		}

	private:
		// Helper function for other methods checking war participation
		bool isWarActive(const GuildWar* war);

		std::list<PlayerPtr> membersOnline;
		std::vector<GuildRank_ptr> ranks;
		std::string name;
		std::string motd;
		uint32_t id;
		uint32_t memberCount = 0;

		uint64_t guildBankBalance = 0;
		GuildWarMap m_mGuildWars;
};

using Guild_ptr = std::shared_ptr<Guild>;

namespace IOGuild
{
	bool loadGuilds();
	Guild_ptr loadGuild(uint32_t guildId);
	uint32_t getGuildIdByName(const std::string& name);

	void getWarList(const Guild_ptr& guild);
};

#endif
