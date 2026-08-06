// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "creatureevent.h"
#include "player.h"

CreatureEvents::CreatureEvents() :
	scriptInterface("CreatureScript Interface")
{
	scriptInterface.initState();
}

void CreatureEvents::clear(bool fromLua)
{
	for (auto it = creatureEvents.begin(); it != creatureEvents.end(); ++it) {
		if (fromLua == it->second.fromLua) {
			it->second.clearEvent();
		}
	}

	reInitState(fromLua);
}

bool CreatureEvents::reload()
{
    loaded = false;
    clear(false);
    removeInvalidEvents();
    loaded = true;
    return true;
}

std::vector<CreatureEvent*> CreatureEvents::getInvalidEvents()
{
    std::vector<CreatureEvent*> invalidEvents;
    for (auto& [name, event] : creatureEvents)
    {
        if (event.getScriptId() == 0)
        {
            invalidEvents.push_back(&event);
        }
    }
    return invalidEvents;
}

void CreatureEvents::removeInvalidEvents()
{
	for (auto it = creatureEvents.begin(); it != creatureEvents.end();) {
		if (it->second.getScriptId() == 0) {
			it = creatureEvents.erase(it);
		} else {
			++it;
		}
	}
}

LuaScriptInterface& CreatureEvents::getScriptInterface()
{
	return scriptInterface;
}

bool CreatureEvents::registerLuaEvent(CreatureEvent* event)
{
	CreatureEvent_ptr creatureEvent{ event };
	if (creatureEvent->getEventType() == CREATURE_EVENT_NONE) {
		std::cout << "Error: [CreatureEvents::registerLuaEvent] Trying to register event without type!" << std::endl;
		return false;
	}

	CreatureEvent* oldEvent = getEventByName(creatureEvent->getName(), false);
	if (oldEvent) {
		//if there was an event with the same that is not loaded
		//(happens when reloading), it is reused
		if (!oldEvent->isLoaded() && oldEvent->getEventType() == creatureEvent->getEventType()) {
			oldEvent->copyEvent(creatureEvent);
		}

		return false;
	} else {
		//if not, register it normally
		creatureEvents.emplace(creatureEvent->getName(), std::move(*creatureEvent));
		return true;
	}
}

CreatureEvent* CreatureEvents::getEventByName(const std::string& name, bool forceLoaded /*= true*/)
{
	auto it = creatureEvents.find(name);
	if (it != creatureEvents.end()) {
		if (!forceLoaded || it->second.isLoaded()) {
			return &it->second;
		}
	}
	return nullptr;
}

bool CreatureEvents::playerLogin(const PlayerPtr& player) const
{
    //fire global event if is registered
    for (const auto& it : creatureEvents)
    {
        if (it.second.isLoaded() and it.second.getEventType() == CREATURE_EVENT_LOGIN)
        {
            if (not it.second.executeOnLogin(player))
            {
                return false;
            }
        }
    }
    return true;
}

bool CreatureEvents::playerLogout(const PlayerPtr& player) const
{
    //fire global event if is registered
    for (const auto& val : creatureEvents | std::views::values)
    {
        if (val.isLoaded() and val.getEventType() == CREATURE_EVENT_LOGOUT)
        {
            if (not val.executeOnLogout(player))
            {
                return false;
            }
        }
    }
    return true;
}

bool CreatureEvents::playerAdvance(const PlayerPtr& player, skills_t skill, uint32_t oldLevel,
                                       uint32_t newLevel)
{
    for (auto& val : creatureEvents | std::views::values)
    {
        if (val.isLoaded() and val.getEventType() == CREATURE_EVENT_ADVANCE)
        {
            if (not val.executeAdvance(player, skill, oldLevel, newLevel))
            {
                return false;
            }
        }
    }
    return true;
}

/////////////////////////////////////

CreatureEvent::CreatureEvent(LuaScriptInterface* interface) :
	Event(interface), type(CREATURE_EVENT_NONE), loaded(false) {}

std::string_view CreatureEvent::getScriptEventName() const
{
	//Depending on the type script event name is different
	switch (type) {
		case CREATURE_EVENT_LOGIN:
			return "onLogin";

		case CREATURE_EVENT_LOGOUT:
			return "onLogout";

		case CREATURE_EVENT_THINK:
			return "onThink";

		case CREATURE_EVENT_PREPAREDEATH:
			return "onPrepareDeath";

		case CREATURE_EVENT_DEATH:
			return "onDeath";

		case CREATURE_EVENT_KILL:
			return "onKill";

		case CREATURE_EVENT_ADVANCE:
			return "onAdvance";

		case CREATURE_EVENT_MODALWINDOW:
			return "onModalWindow";

		case CREATURE_EVENT_TEXTEDIT:
			return "onTextEdit";

		case CREATURE_EVENT_EXTENDED_OPCODE:
			return "onExtendedOpcode";

		case CREATURE_EVENT_NONE:
		default:
			return "";
	}
}

void CreatureEvent::copyEvent(const CreatureEvent_ptr& creatureEvent)
{
	scriptId = creatureEvent->scriptId;
	scriptInterface = creatureEvent->scriptInterface;
	scripted = creatureEvent->scripted;
	loaded = creatureEvent->loaded;
}

void CreatureEvent::clearEvent()
{
	scriptId = 0;
	scriptInterface = nullptr;
	scripted = false;
	loaded = false;
}

bool CreatureEvent::executeOnLogin(const PlayerPtr& player) const
{
	//onLogin(player)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeOnLogin] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");
	return scriptInterface->callFunction(1);
}

bool CreatureEvent::executeOnLogout(const PlayerPtr& player) const
{
	//onLogout(player)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeOnLogout] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");
	return scriptInterface->callFunction(1);
}

bool CreatureEvent::executeOnThink(const CreaturePtr& creature, uint32_t interval) const
{
	//onThink(creature, interval)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeOnThink] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	lua_pushinteger(L, interval);

	return scriptInterface->callFunction(2);
}

bool CreatureEvent::executeOnPrepareDeath(const CreaturePtr& creature, const CreaturePtr& killer) const
{
	//onPrepareDeath(creature, killer)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeOnPrepareDeath] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);

	if (killer) {
		LuaScriptInterface::pushSharedPtr(L, killer);
		LuaScriptInterface::setCreatureMetatable(L, -1, killer);
	} else {
		lua_pushnil(L);
	}

	return scriptInterface->callFunction(2);
}

bool CreatureEvent::executeOnDeath(const CreaturePtr& creature, const ItemPtr& corpse, const CreaturePtr& killer, const CreaturePtr& mostDamageKiller, bool lastHitUnjustified, bool mostDamageUnjustified) const
{
	//onDeath(creature, corpse, killer, mostDamageKiller, lastHitUnjustified, mostDamageUnjustified)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeOnDeath] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);

	LuaScriptInterface::pushItem(L, corpse);

	if (killer) {
		LuaScriptInterface::pushSharedPtr(L, killer);
		LuaScriptInterface::setCreatureMetatable(L, -1, killer);
	} else {
		lua_pushnil(L);
	}

	if (mostDamageKiller) {
		LuaScriptInterface::pushSharedPtr(L, mostDamageKiller);
		LuaScriptInterface::setCreatureMetatable(L, -1, mostDamageKiller);
	} else {
		lua_pushnil(L);
	}

	LuaScriptInterface::pushBoolean(L, lastHitUnjustified);
	LuaScriptInterface::pushBoolean(L, mostDamageUnjustified);

	return scriptInterface->callFunction(6);
}

bool CreatureEvent::executeAdvance(const PlayerPtr& player, skills_t skill, uint32_t oldLevel,
                                       uint32_t newLevel) const
{
	//onAdvance(player, skill, oldLevel, newLevel)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeAdvance] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");
	lua_pushinteger(L, static_cast<uint32_t>(skill));
	lua_pushinteger(L, oldLevel);
	lua_pushinteger(L, newLevel);

	return scriptInterface->callFunction(4);
}

void CreatureEvent::executeOnKill(const CreaturePtr& creature, const CreaturePtr& target) const
{
	//onKill(creature, target)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeOnKill] Call stack overflow" << std::endl;
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);
	LuaScriptInterface::pushSharedPtr(L, creature);
	LuaScriptInterface::setCreatureMetatable(L, -1, creature);
	LuaScriptInterface::pushSharedPtr(L, target);
	LuaScriptInterface::setCreatureMetatable(L, -1, target);
	scriptInterface->callVoidFunction(2);
}

void CreatureEvent::executeModalWindow(const PlayerPtr& player, uint32_t modalWindowId, uint8_t buttonId, uint8_t choiceId) const
{
	//onModalWindow(player, modalWindowId, buttonId, choiceId)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeModalWindow] Call stack overflow" << std::endl;
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	lua_pushinteger(L, modalWindowId);
	lua_pushinteger(L, buttonId);
	lua_pushinteger(L, choiceId);

	scriptInterface->callVoidFunction(4);
}

bool CreatureEvent::executeTextEdit(const PlayerPtr& player, const ItemPtr& item, const std::string& text) const
{
	//onTextEdit(player, item, text)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeTextEdit] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	LuaScriptInterface::pushItem(L, item);
	LuaScriptInterface::pushString(L, text);

	return scriptInterface->callFunction(3);
}

void CreatureEvent::executeExtendedOpcode(const PlayerPtr& player, uint8_t opcode, const std::string& buffer) const
{
	//onExtendedOpcode(player, opcode, buffer)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CreatureEvent::executeExtendedOpcode] Call stack overflow" << std::endl;
		return;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(scriptId);

	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	lua_pushinteger(L, opcode);
	LuaScriptInterface::pushString(L, buffer);

	scriptInterface->callVoidFunction(3);
}
