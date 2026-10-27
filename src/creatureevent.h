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

#ifndef FS_CREATUREEVENT_H
#define FS_CREATUREEVENT_H

#include "baseevents.h"
#include "enums.h"
#include "luascript.h"

class CreatureEvent;
using CreatureEvent_ptr = std::unique_ptr<CreatureEvent>;

enum CreatureEventType_t {
	CREATURE_EVENT_NONE,
	CREATURE_EVENT_LOGIN,
	CREATURE_EVENT_LOGOUT,
	CREATURE_EVENT_THINK,
	CREATURE_EVENT_PREPAREDEATH,
	CREATURE_EVENT_DEATH,
	CREATURE_EVENT_KILL,
	CREATURE_EVENT_ADVANCE,
	CREATURE_EVENT_MODALWINDOW,
	CREATURE_EVENT_TEXTEDIT,
	CREATURE_EVENT_HEALTHCHANGE,
	CREATURE_EVENT_MANACHANGE,
	CREATURE_EVENT_EXTENDED_OPCODE, // otclient additional network opcodes
};

class CreatureEvent final : public Event
{
	public:
		explicit CreatureEvent(LuaScriptInterface* interface);

		CreatureEventType_t getEventType() const {
			return type;
		}
		void setEventType(CreatureEventType_t eventType) {
			type = eventType;
		}
		const std::string& getName() const {
			return eventName;
		}
		void setName(const std::string& name) {
			eventName = name;
		}
		bool isLoaded() const {
			return loaded;
		}
		void setLoaded(bool b) {
			loaded = b;
		}

		void clearEvent();
		void copyEvent(const CreatureEvent_ptr& creatureEvent);

		//scripting
		bool executeOnLogin(const PlayerPtr& player) const;
		bool executeOnLogout(const PlayerPtr& player) const;
		bool executeOnThink(const CreaturePtr& creature, uint32_t interval) const;
		bool executeOnPrepareDeath(const CreaturePtr& creature, const CreaturePtr& killer) const;
		bool executeOnDeath(const CreaturePtr& creature, const ItemPtr& corpse, const CreaturePtr& killer, const CreaturePtr& mostDamageKiller, bool lastHitUnjustified, bool mostDamageUnjustified) const;
		void executeOnKill(const CreaturePtr& creature, const CreaturePtr& target) const;
		bool executeAdvance(const PlayerPtr& player, skills_t, uint32_t, uint32_t) const;
		void executeModalWindow(const PlayerPtr& player, uint32_t modalWindowId, uint8_t buttonId, uint8_t choiceId) const;
		bool executeTextEdit(const PlayerPtr& player, const ItemPtr& item, const std::string& text) const;
		void executeExtendedOpcode(const PlayerPtr& player, uint8_t opcode, const std::string& buffer) const;
		//

	private:
		std::string_view getScriptEventName() const override;

		std::string eventName;
		CreatureEventType_t type;
		bool loaded;
};

class CreatureEvents final : public BaseEvents
{
	public:
		CreatureEvents();

		// non-copyable
		CreatureEvents(const CreatureEvents&) = delete;
		CreatureEvents& operator=(const CreatureEvents&) = delete;

		// global events
		bool playerLogin(const PlayerPtr& player) const;
		bool playerLogout(const PlayerPtr& player) const;
		bool playerAdvance(const PlayerPtr& player, skills_t, uint32_t, uint32_t);

		CreatureEvent* getEventByName(const std::string& name, bool forceLoaded = true);

		bool registerLuaEvent(CreatureEvent* event);
		void clear(bool fromLua) override final;
		bool reload() override;

		std::vector<CreatureEvent*> getInvalidEvents();
		void removeInvalidEvents();

	private:
		LuaScriptInterface& getScriptInterface() override;

		//creature events
		using CreatureEventMap = std::map<std::string, CreatureEvent>;
		CreatureEventMap creatureEvents;

		LuaScriptInterface scriptInterface;
};

#endif
