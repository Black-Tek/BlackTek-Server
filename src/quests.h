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

#ifndef FS_QUESTS_H
#define FS_QUESTS_H

#include "player.h"
#include "networkmessage.h"

class Mission;
class Quest;

using MissionsList = std::list<Mission>;
using QuestsList = std::list<Quest>;

class Mission
{
	public:
		Mission(std::string name, int32_t storageID, int32_t startValue, int32_t endValue, bool ignoreEndValue) :
			name(std::move(name)), storageID(storageID), startValue(startValue), endValue(endValue), ignoreEndValue(ignoreEndValue) {}

		bool isCompleted(const PlayerPtr& player) const;
		bool isStarted(const PlayerPtr& player) const;
		std::string getName(const PlayerPtr& player) const;
		std::string getDescription(const PlayerPtr& player) const;

		uint32_t getStorageId() const {
			return storageID;
		}
	
		int32_t getStartStorageValue() const {
			return startValue;
		}
	
		int32_t getEndStorageValue() const {
			return endValue;
		}

		std::map<int32_t, std::string> descriptions;
		std::string mainDescription;

	private:
		std::string name;
		uint32_t storageID;
		int32_t startValue, endValue;
		bool ignoreEndValue;
};

class Quest
{
	public:
		Quest(std::string name, uint16_t id, int32_t startStorageID, int32_t startStorageValue) :
			name(std::move(name)), startStorageID(startStorageID), startStorageValue(startStorageValue), id(id) {}

		bool isCompleted(const PlayerPtr& player) const;
		bool isStarted(const PlayerPtr& player) const;
	
		uint16_t getID() const {
			return id;
		}
	
		std::string getName() const {
			return name;
		}
	
		uint16_t getMissionsCount(const PlayerPtr& player) const;

		uint32_t getStartStorageId() const {
			return startStorageID;
		}
	
		int32_t getStartStorageValue() const {
			return startStorageValue;
		}

		const MissionsList& getMissions() const {
			return missions;
		}

	private:
		std::string name;

		uint32_t startStorageID;
		int32_t startStorageValue;
		uint16_t id;

		MissionsList missions;

	friend class Quests;
};

class Quests
{
	public:
		const QuestsList& getQuests() const {
			return quests;
		}

		bool loadFromToml();
		Quest* getQuestByID(uint16_t id);
		bool isQuestStorage(const uint32_t key, const int32_t value, const int32_t oldValue) const;
		uint16_t getQuestsCount(const PlayerPtr& player) const;
		bool reload();

	private:
		QuestsList quests;
};

#endif
