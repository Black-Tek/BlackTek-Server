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

#ifndef FS_TOWN_H
#define FS_TOWN_H

#include "position.h"

class Town
{
	public:
		explicit Town(uint32_t id) : id(id) {}

		const Position& getTemplePosition() const {
			return templePosition;
		}
		const std::string& getName() const {
			return name;
		}

		void setTemplePos(Position pos) {
			templePosition = pos;
		}
		void setName(std::string_view name) {
			this->name = name; 
		}
		uint32_t getID() const {
			return id;
		}

	private:
		uint32_t id;
		std::string name;
		Position templePosition;
};

using TownMap = std::map<uint32_t, Town*>;

class Towns
{
	public:
		Towns() = default;
		~Towns() {
			for (const auto& it : townMap) {
				delete it.second;
			}
		}

		// non-copyable
		Towns(const Towns&) = delete;
		Towns& operator=(const Towns&) = delete;

		bool addTown(uint32_t townId, Town* town) {
			return townMap.emplace(townId, town).second;
		}

		Town* getTown(const std::string& townName) const {
			for (const auto& it : townMap) {
				if (caseInsensitiveEqual(townName, it.second->getName())) {
					return it.second;
				}
			}
			return nullptr;
		}

		Town* getTown(uint32_t townId) const {
			auto it = townMap.find(townId);
			if (it == townMap.end()) {
				return nullptr;
			}
			return it->second;
		}

		const TownMap& getTowns() const {
			return townMap;
		}

	private:
		TownMap townMap;
};

#endif
