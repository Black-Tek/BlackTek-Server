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

#ifndef FS_IOLOGINDATA_H
#define FS_IOLOGINDATA_H

#include "account.h"
#include "player.h"
#include "database.h"

using ItemBlockList = std::list<std::pair<int32_t, ItemPtr>>;

class IOLoginData
{
	public:
		static Account loadAccount(uint32_t accno);

		static bool loginserverAuthentication(const std::string& name, const std::string& password, Account& account);
		static std::pair<uint32_t, uint32_t> gameworldAuthentication(std::string_view accountName, std::string_view password, std::string_view characterName, std::string_view token, uint32_t tokenTime);
		static uint32_t getAccountIdByPlayerName(const std::string& playerName);
		static uint32_t getAccountIdByPlayerId(uint32_t playerId);

		static AccountType_t getAccountType(uint32_t accountId);
		static void setAccountType(uint32_t accountId, AccountType_t accountType);
		static std::pair<uint32_t, uint32_t> getAccountIdByAccountName(std::string_view accountName, std::string_view password, std::string_view characterName);
		static void updateOnlineStatus(uint32_t guid, bool login);
		static bool preloadPlayer(const PlayerPtr& player);

		static bool loadPlayerById(const PlayerPtr& player, uint32_t id, std::vector<ConditionHandle>* outConditions = nullptr);
		static bool loadPlayerByName(const PlayerPtr& player, const std::string& name);
		static bool loadPlayer(const PlayerPtr& player, DBResult_ptr result, std::vector<ConditionHandle>* outConditions = nullptr);
		static bool savePlayer(const PlayerPtr& player);
		static uint32_t getGuidByName(const std::string& name);
		static bool getGuidByNameEx(uint32_t& guid, bool& specialVip, std::string& name);
		static std::string getNameByGuid(uint32_t guid);
		static bool formatPlayerName(std::string& name);
		static void increaseBankBalance(uint32_t guid, uint64_t bankBalance);
		static bool hasBiddedOnHouse(uint32_t guid);

		static std::forward_list<VIPEntry> getVIPEntries(uint32_t accountId);
		static void addVIPEntry(uint32_t accountId, uint32_t guid, const std::string& description, uint32_t icon, bool notify);
		static void editVIPEntry(uint32_t accountId, uint32_t guid, const std::string& description, uint32_t icon, bool notify);
		static void removeVIPEntry(uint32_t accountId, uint32_t guid);

		static void updatePremiumTime(uint32_t accountId, time_t endTime);

		static bool addRewardItems(uint32_t playerId, const ItemBlockList& itemList, DBInsert& query_insert, PropWriteStream& propWriteStream);

		static bool accountExists(const std::string& accountName);

	private:
		using ItemMap = std::map<uint32_t, std::pair<ItemPtr, uint32_t>>;

		static void loadItems(ItemMap& itemMap, const DBResult_ptr& result);
		static bool saveItems(const PlayerConstPtr& player, const ItemBlockList& itemList, DBInsert& query_insert, PropWriteStream& propWriteStream);
		static bool saveAugments(const PlayerConstPtr& player, DBInsert& query_insert, PropWriteStream& augmentStream);
		static void loadPlayerAugments(std::vector<std::shared_ptr<BlackTek::Augment>>& augmentList, const DBResult_ptr& result);
		static void serializeCustomSkills(const PlayerConstPtr player, DBInsert query, PropWriteStream& binary_stream);
		static void serializeCustomStats(PlayerPtr player, DBInsert query, PropWriteStream& binary_stream);
		static void serializeCustomStats(ItemPtr item, DBInsert query, PropWriteStream& binary_stream);
		static void serializeCustomSkills(const ItemConstPtr item, DBInsert query, PropWriteStream& binary_stream);
		static SkillRegistry deserializeCustomSkills(PropStream binary_stream);
		static StatRegistry deserializeCustomStats(PropStream binary_stream);
		static bool savePlayerCustomSkills(const PlayerConstPtr& player, DBInsert& query_insert, PropWriteStream& binary_stream);
		static bool savePlayerCustomStats(const PlayerPtr& player, DBInsert& query_insert, PropWriteStream& binary_stream);
};

#endif
