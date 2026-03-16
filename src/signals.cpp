// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include <csignal>

#include "signals.h"
#include "console.h"
#include "tasks.h"
#include "game.h"
#include "actions.h"
#include "configmanager.h"
#include "spells.h"
#include "talkaction.h"
#include "movement.h"
#include "weapons.h"
#include "raids.h"
#include "quests.h"
#include "mounts.h"
#include "globalevent.h"
#include "monster.h"
#include "events.h"
#include "scheduler.h"
#include "databasetasks.h"

extern Scheduler g_scheduler;
extern DatabaseTasks g_databaseTasks;
extern Dispatcher g_dispatcher;

extern ConfigManager g_config;
extern Actions* g_actions;
extern Monsters g_monsters;
extern TalkActions* g_talkActions;
extern MoveEvents* g_moveEvents;
extern Spells* g_spells;
extern Weapons* g_weapons;
extern Game g_game;
extern CreatureEvents* g_creatureEvents;
extern GlobalEvents* g_globalEvents;
extern Events* g_events;
extern Chat* g_chat;
extern LuaEnvironment g_luaEnvironment;

namespace
{
	#ifndef _WIN32
	void sigusr1Handler()
	{
		BlackTek::Console::Info("SIGUSR1 received, saving the game state...");
		g_globalEvents->save();
		g_game.saveGameState();
	}

	void sighupHandler()
	{
		BlackTek::Console::Info("SIGHUP received, reloading config files...");

		g_actions->reload();
		BlackTek::Console::Info("Reloaded actions.");

		g_config.reload();
		BlackTek::Console::Info("Reloaded config.");

		g_creatureEvents->reload();
		BlackTek::Console::Info("Reloaded creature scripts.");

		g_moveEvents->reload();
		BlackTek::Console::Info("Reloaded movements.");

		Npcs::reload();
		BlackTek::Console::Info("Reloaded npcs.");

		g_game.raids.reload();
		g_game.raids.startup();
		BlackTek::Console::Info("Reloaded raids.");

		g_monsters.reload();
		BlackTek::Console::Info("Reloaded monsters.");

		g_spells->reload();
		BlackTek::Console::Info("Reloaded spells.");

		g_talkActions->reload();
		BlackTek::Console::Info("Reloaded talk actions.");

		Item::items.reload();
		BlackTek::Console::Info("Reloaded items.");

		g_weapons->reload();
		g_weapons->loadDefaults();
		BlackTek::Console::Info("Reloaded weapons.");

		g_game.quests.reload();
		BlackTek::Console::Info("Reloaded quests.");

		g_game.mounts.reload();
		BlackTek::Console::Info("Reloaded mounts.");

		g_globalEvents->reload();
		BlackTek::Console::Info("Reloaded globalevents.");

		g_events->load();
		BlackTek::Console::Info("Reloaded events.");

		g_chat->load();
		BlackTek::Console::Info("Reloaded chatchannels.");

		g_luaEnvironment.loadFile("data/global.lua");
		BlackTek::Console::Info("Reloaded global.lua.");

		lua_gc(g_luaEnvironment.getLuaState(), LUA_GCCOLLECT, 0);
	}
	#else
	void sigbreakHandler()
	{
		BlackTek::Console::Error("SIGBREAK received, shutting game server down...");
		g_game.setGameState(GAME_STATE_SHUTDOWN);
	}
	#endif

	void sigtermHandler()
	{
		BlackTek::Console::Error("SIGTERM received, shutting game server down...");
		g_game.setGameState(GAME_STATE_SHUTDOWN);
	}

	void sigintHandler()
	{
		BlackTek::Console::Error("SIGINT received, shutting game server down...");
		g_game.setGameState(GAME_STATE_SHUTDOWN);
	}

	void dispatchSignalHandler(int signal)
	{
		switch(signal) {
			case SIGINT:
				g_dispatcher.addTask(createTask(sigintHandler));
				break;
			case SIGTERM:
				g_dispatcher.addTask(createTask(sigtermHandler));
				break;
	#ifndef _WIN32
			case SIGHUP:
				g_dispatcher.addTask(createTask(sighupHandler));
				break;
			case SIGUSR1:
				g_dispatcher.addTask(createTask(sigusr1Handler));
				break;
	#endif
			default:
				break;
		}
	}

}

Signals::Signals(boost::asio::io_context& context): set(context)
{
	set.add(SIGINT);
	set.add(SIGTERM);
#ifndef _WIN32
	set.add(SIGUSR1);
	set.add(SIGHUP);
#else
	// Here we register a Windows console control handler for Ctrl+Break, window close, logoff,
	// and shutdown events. Windows invokes this in a dedicated thread and terminates the
	// process when the handler returns (or after 5 seconds), so we block until all
	// threads have joined. CTRL_C_EVENT is intentionally left to the boost::asio
	// signal_set above, which translates it to SIGINT.
	SetConsoleCtrlHandler([](DWORD ctrlType) -> BOOL {
		switch (ctrlType) {
			case CTRL_BREAK_EVENT:
			case CTRL_CLOSE_EVENT:
			case CTRL_LOGOFF_EVENT:
			case CTRL_SHUTDOWN_EVENT:
				g_dispatcher.addTask(createTask(sigbreakHandler));
				g_scheduler.join();
				g_databaseTasks.join();
				g_dispatcher.join();
				return TRUE;
			default:
				return FALSE;
		}
	}, TRUE);
#endif

	asyncWait();
}

void Signals::asyncWait()
{
	set.async_wait([this](const boost::system::error_code& err, int signal) {
		if (err) {
			std::cerr << "Signal handling error: "  << err.message() << std::endl;
			return;
		}
		dispatchSignalHandler(signal);
		asyncWait();
	});
}
