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

#ifndef FS_GLOBALEVENT_H
#define FS_GLOBALEVENT_H
#include "baseevents.h"

#include "const.h"

enum GlobalEvent_t {
	GLOBALEVENT_NONE,
	GLOBALEVENT_TIMER,

	GLOBALEVENT_STARTUP,
	GLOBALEVENT_SHUTDOWN,
	GLOBALEVENT_RECORD,
	GLOBALEVENT_SAVE
};

class GlobalEvent;
using GlobalEvent_ptr = std::unique_ptr<GlobalEvent>;
using GlobalEventMap = std::map<std::string, GlobalEvent>;

class GlobalEvents final : public BaseEvents
{
	public:
		GlobalEvents();
		~GlobalEvents();

		// non-copyable
		GlobalEvents(const GlobalEvents&) = delete;
		GlobalEvents& operator=(const GlobalEvents&) = delete;

		void startup() const;
		void shutdown() const;
		void save() const;
		void timer();
		void think();
		void execute(GlobalEvent_t type) const;

		GlobalEventMap getEventMap(GlobalEvent_t type);
		static void clearMap(GlobalEventMap& map, bool fromLua);

		bool registerLuaEvent(GlobalEvent* event);
		void clear(bool fromLua) override final;
		bool reload() override;

	private:
		LuaScriptInterface& getScriptInterface() override {
			return scriptInterface;
		}
	
		LuaScriptInterface scriptInterface;

		GlobalEventMap thinkMap, serverMap, timerMap;
		int32_t thinkEventId = 0, timerEventId = 0;
};

class GlobalEvent final : public Event
{
	public:
		explicit GlobalEvent(LuaScriptInterface* interface);

		bool executeRecord(uint32_t current, uint32_t old) const;
		bool executeEvent() const;

		GlobalEvent_t getEventType() const {
			return eventType;
		}
	
		void setEventType(GlobalEvent_t type) {
			eventType = type;
		}

		const std::string& getName() const {
			return name;
		}
	
		void setName(const std::string& eventName) {
			name = eventName;
		}

		uint32_t getInterval() const {
			return interval;
		}
	
		void setInterval(uint32_t eventInterval) {
			interval |= eventInterval;
		}

		int64_t getNextExecution() const {
			return nextExecution;
		}
	
		void setNextExecution(int64_t time) {
			nextExecution = time;
		}

	private:
		GlobalEvent_t eventType = GLOBALEVENT_NONE;

		std::string_view getScriptEventName() const override;

		std::string name;
		int64_t nextExecution = 0;
		uint32_t interval = 0;
};

#endif
