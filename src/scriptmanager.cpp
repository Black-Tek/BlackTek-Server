// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "scriptmanager.h"

#include "chat.h"
#include "talkaction.h"
#include "spells.h"
#include "globalevent.h"
#include "events.h"
#include "script.h"
#include "itemevents.h"

CreatureEvents* g_creatureEvents = nullptr;
Chat* g_chat = nullptr;
Events* g_events = nullptr;
GlobalEvents* g_globalEvents = nullptr;
Spells* g_spells = nullptr;
TalkActions* g_talkActions = nullptr;
Scripts* g_scripts = nullptr;
ItemEvents* g_itemEvents = nullptr;

extern LuaEnvironment g_luaEnvironment;

ScriptingManager::~ScriptingManager()
{
	delete g_itemEvents;
	delete g_events;
	delete g_spells;
	delete g_talkActions;
	delete g_chat;
	delete g_creatureEvents;
	delete g_globalEvents;
	delete g_scripts;
}

bool ScriptingManager::loadScriptSystems()
{
	if (g_luaEnvironment.loadFile("data/global.lua") == -1)
	{
		std::cout << "[Warning - ScriptingManager::loadScriptSystems] Can not load data/global.lua" << std::endl;
	}

	// It's ok for us to go ahead and create all of these in the expectation that they will succeed
	// because if any fail we will abort starting server anyways, so the only time this could possibly be an
	// extremely minor performance hit, is in the case in which we quit anyways.
	g_scripts = new Scripts();
	g_chat = new Chat();
	g_spells = new Spells();
	g_talkActions = new TalkActions();
	g_creatureEvents = new CreatureEvents();
	g_globalEvents = new GlobalEvents();
	g_events = new Events();

	g_itemEvents = new ItemEvents();

	[[unlikely]]
	if (not g_scripts->loadScripts("scripts/lib", true, false))
	{
		std::cout << "> ERROR: Unable to load lua libs!" << std::endl;
		return false;
	}

	[[unlikely]]
	if (not g_events->load())
	{
		std::cout << "> ERROR: Unable to load events!" << std::endl;
		return false;
	}

	return true;
}
