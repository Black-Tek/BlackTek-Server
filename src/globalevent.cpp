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

#include "otpch.h"

#include "configmanager.h"
#include "globalevent.h"
#include "scheduler.h"
#include "tools.h"

extern ConfigManager g_config;

GlobalEvents::GlobalEvents() :
	scriptInterface("GlobalEvent Interface")
{
	scriptInterface.initState();
}

GlobalEvents::~GlobalEvents()
{
	clear(false);
}

void GlobalEvents::clearMap(GlobalEventMap& map, bool fromLua)
{
	for (auto it = map.begin(); it != map.end(); ) {
		if (fromLua == it->second.fromLua) {
			it = map.erase(it);
		} else {
			++it;
		}
	}
}

void GlobalEvents::clear(bool fromLua)
{
	g_scheduler.stopEvent(thinkEventId);
	thinkEventId = 0;
	g_scheduler.stopEvent(timerEventId);
	timerEventId = 0;

	clearMap(thinkMap, fromLua);
	clearMap(serverMap, fromLua);
	clearMap(timerMap, fromLua);

	reInitState(fromLua);
}

bool GlobalEvents::reload()
{
	loaded = false;
	clear(false);
	loaded = true;
	return true;
}

bool GlobalEvents::registerLuaEvent(GlobalEvent* event)
{
	GlobalEvent_ptr globalEvent{ event };
	if (globalEvent->getEventType() == GLOBALEVENT_TIMER) {
		auto result = timerMap.emplace(globalEvent->getName(), std::move(*globalEvent));
		if (result.second) {
			if (timerEventId == 0) {
				timerEventId = g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, [this]() { timer(); }));
			}
			return true;
		}
	} else if (globalEvent->getEventType() != GLOBALEVENT_NONE) {
		auto result = serverMap.emplace(globalEvent->getName(), std::move(*globalEvent));
		if (result.second) {
			return true;
		}
	} else { // think event
		auto result = thinkMap.emplace(globalEvent->getName(), std::move(*globalEvent));
		if (result.second) {
			if (thinkEventId == 0) {
				thinkEventId = g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, [this]() { think(); }));
			}
			return true;
		}
	}

	std::cout << "[Warning - GlobalEvents::configureEvent] Duplicate registered globalevent with name: " << globalEvent->getName() << std::endl;
	return false;
}

void GlobalEvents::startup() const
{
	execute(GLOBALEVENT_STARTUP);
}

void GlobalEvents::shutdown() const 
{ 
	execute(GLOBALEVENT_SHUTDOWN); 
}

void GlobalEvents::save() const 
{ 
	execute(GLOBALEVENT_SAVE); 
}

void GlobalEvents::timer()
{
	time_t now = time(nullptr);

	int64_t nextScheduledTime = std::numeric_limits<int64_t>::max();

	auto it = timerMap.begin();
	while (it != timerMap.end()) {
		GlobalEvent& globalEvent = it->second;

		int64_t nextExecutionTime = globalEvent.getNextExecution() - now;
		if (nextExecutionTime > 0) {
			if (nextExecutionTime < nextScheduledTime) {
				nextScheduledTime = nextExecutionTime;
			}

			++it;
			continue;
		}

		if (!globalEvent.executeEvent()) {
			it = timerMap.erase(it);
			continue;
		}

		nextExecutionTime = 86400;
		if (nextExecutionTime < nextScheduledTime) {
			nextScheduledTime = nextExecutionTime;
		}

		globalEvent.setNextExecution(globalEvent.getNextExecution() + nextExecutionTime);

		++it;
	}

	if (nextScheduledTime != std::numeric_limits<int64_t>::max()) {
		timerEventId = g_scheduler.addEvent(createSchedulerTask(std::max<int64_t>(1000, nextScheduledTime * 1000), [this]() { timer(); }));
	}
}

void GlobalEvents::think()
{
	int64_t now = OTSYS_TIME();

	int64_t nextScheduledTime = std::numeric_limits<int64_t>::max();
	for (auto& it : thinkMap) {
		GlobalEvent& globalEvent = it.second;

		int64_t nextExecutionTime = globalEvent.getNextExecution() - now;
		if (nextExecutionTime > 0) {
			if (nextExecutionTime < nextScheduledTime) {
				nextScheduledTime = nextExecutionTime;
			}
			continue;
		}

		if (!globalEvent.executeEvent()) {
			std::cout << "[Error - GlobalEvents::think] Failed to execute event: " << globalEvent.getName() << std::endl;
		}

		nextExecutionTime = globalEvent.getInterval();
		if (nextExecutionTime < nextScheduledTime) {
			nextScheduledTime = nextExecutionTime;
		}

		globalEvent.setNextExecution(globalEvent.getNextExecution() + nextExecutionTime);
	}

	if (nextScheduledTime != std::numeric_limits<int64_t>::max()) {
		thinkEventId = g_scheduler.addEvent(createSchedulerTask(nextScheduledTime, [this]() { think(); }));
	}
}

void GlobalEvents::execute(GlobalEvent_t type) const
{
	for (const auto& it : serverMap) {
		const GlobalEvent& globalEvent = it.second;
		if (globalEvent.getEventType() == type) {
			globalEvent.executeEvent();
		}
	}
}

GlobalEventMap GlobalEvents::getEventMap(GlobalEvent_t type)
{
	// TODO: This should be better implemented. Maybe have a map for every type.
	switch (type) {
		case GLOBALEVENT_NONE: return thinkMap;
		case GLOBALEVENT_TIMER: return timerMap;
		case GLOBALEVENT_STARTUP:
		case GLOBALEVENT_SHUTDOWN:
		case GLOBALEVENT_RECORD:
		case GLOBALEVENT_SAVE: {
			GlobalEventMap retMap;
			for (const auto& it : serverMap) {
				if (it.second.getEventType() == type) {
					retMap.emplace(it.first, it.second);
				}
			}
			return retMap;
		}
		default: return GlobalEventMap();
	}
}

GlobalEvent::GlobalEvent(LuaScriptInterface* interface) : Event(interface) {}

std::string_view GlobalEvent::getScriptEventName() const
{
	switch (eventType) {
		case GLOBALEVENT_STARTUP: return "onStartup";
		case GLOBALEVENT_SHUTDOWN: return "onShutdown";
		case GLOBALEVENT_RECORD: return "onRecord";
		case GLOBALEVENT_TIMER: return "onTime";
		case GLOBALEVENT_SAVE: return "onSave";
		default: return "onThink";
	}
}

bool GlobalEvent::executeRecord(uint32_t current, uint32_t old) const
{
	//onRecord(current, old)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - GlobalEvent::executeRecord] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	lua_pushinteger(L, current);
	lua_pushinteger(L, old);
	return scriptInterface->callFunction(2);
}

bool GlobalEvent::executeEvent() const
{
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - GlobalEvent::executeEvent] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);
	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	int32_t params = 0;
	if (eventType == GLOBALEVENT_NONE || eventType == GLOBALEVENT_TIMER) {
		lua_pushinteger(L, interval);
		params = 1;
	}

	return scriptInterface->callFunction(params);
}
