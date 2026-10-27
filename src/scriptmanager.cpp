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
