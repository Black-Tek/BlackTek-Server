// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "server.h"

#include "game.h"

#include "iomarket.h"

#include "configmanager.h"
#include "scriptmanager.h"
#include "rsa.h"
#include "protocolold.h"
#include "protocollogin.h"
#include "protocolstatus.h"
#include "databasemanager.h"
#include "scheduler.h"
#include "databasetasks.h"
#include "script.h"
#include <fstream>
#include <fmt/color.h>
#include "augments.h"
#include "zones.h"
#include "console.h"
#include <memory>

#if __has_include("gitmetadata.h")
	#include "gitmetadata.h"
#endif

DatabaseTasks g_databaseTasks;
Dispatcher g_dispatcher;
Dispatcher g_utility_boss;
Scheduler g_scheduler;

Game g_game;
ConfigManager g_config;
Monsters g_monsters;
Vocations g_vocations;
extern Scripts* g_scripts;
RSA g_RSA;

std::mutex g_loaderLock;
std::condition_variable g_loaderSignal;
std::unique_lock<std::mutex> g_loaderUniqueLock(g_loaderLock);

// ============================================================================
// BLACKTEK HOLOGRAPHIC CONSOLE - Color Definitions
// ============================================================================

namespace Console {
	// Primary colors for the holographic theme
	const auto cyan = fmt::color::cyan;
	const auto magenta = fmt::color::magenta;
	const auto white = fmt::color::white;
	const auto gray = fmt::color::gray;
	const auto dark_gray = fmt::color::dim_gray;
	const auto green = fmt::color::lime_green;
	const auto red = fmt::color::red;
	const auto yellow = fmt::color::yellow;
	const auto purple = fmt::color::purple;
	const auto dark_purple = fmt::color::dark_violet;

	// Box drawing characters
	constexpr const char* BOX_TL = "╔";
	constexpr const char* BOX_TR = "╗";
	constexpr const char* BOX_BL = "╚";
	constexpr const char* BOX_BR = "╝";
	constexpr const char* BOX_H = "═";
	constexpr const char* BOX_V = "║";
	constexpr const char* BOX_LT = "╠";
	constexpr const char* BOX_RT = "╣";

	// Status symbols
	constexpr const char* CHECK = "✓";
	constexpr const char* CROSS = "✗";
	constexpr const char* DIAMOND = "◆";
	constexpr const char* ARROW = "▸";
	constexpr const char* DOT = "●";
	constexpr const char* PROGRESS_FULL = "█";
	constexpr const char* PROGRESS_EMPTY = "░";

	// Print the large BLACKTEK ASCII banner with gradient effect
	void printBanner()
	{
		BlackTek::Console::Print("\n");
		BlackTek::Console::StyledPrint(true, fg(dark_purple) | fmt::emphasis::bold,
			"    ██████╗ ██╗      █████╗  ██████╗██╗  ██╗████████╗███████╗██╗  ██╗");
		BlackTek::Console::StyledPrint(true, fg(dark_purple),
			"    ██╔══██╗██║     ██╔══██╗██╔════╝██║ ██╔╝╚══██╔══╝██╔════╝██║ ██╔╝");
		BlackTek::Console::StyledPrint(true, fg(dark_purple),
			"    ██████╔╝██║     ███████║██║     █████╔╝    ██║   █████╗  █████╔╝ ");
		BlackTek::Console::StyledPrint(true, fg(dark_purple),
			"    ██╔══██╗██║     ██╔══██║██║     ██╔═██╗    ██║   ██╔══╝  ██╔═██╗ ");
		BlackTek::Console::StyledPrint(true, fg(dark_purple),
			"    ██████╔╝███████╗██║  ██║╚██████╗██║  ██╗   ██║   ███████╗██║  ██╗");
		BlackTek::Console::StyledPrint(true, fg(dark_purple) | fmt::emphasis::bold,
			"    ╚═════╝ ╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝");
		BlackTek::Console::Print("\n");
	}

	// Print version info bar
	void printVersionBar()
	{
		BlackTek::Console::StyledPrint(false, fg(dark_gray), "    ─────────────────────────────────────────────────────────────────\n");
		BlackTek::Console::StyledPrint(false, fg(gray),      "    VERSION ");
		BlackTek::Console::StyledPrint(false, fg(white) | fmt::emphasis::bold, "{} ", STATUS_SERVER_VERSION);
		BlackTek::Console::StyledPrint(false, fg(dark_gray), " · ");
		BlackTek::Console::StyledPrint(false, fg(gray),      " CLIENT ");
		BlackTek::Console::StyledPrint(false, fg(white) | fmt::emphasis::bold, "{} ", CLIENT_VERSION_STR);
		BlackTek::Console::StyledPrint(false, fg(dark_gray), " · ");
		BlackTek::Console::StyledPrint(false, fg(gray),      " BUILD ");
		#if defined(GIT_RETRIEVED_STATE) && GIT_RETRIEVED_STATE
			#if GIT_IS_DIRTY
				BlackTek::Console::StyledPrint(false, fg(yellow) | fmt::emphasis::bold, "DIRTY\n");
			#else
				BlackTek::Console::StyledPrint(false, fg(green)  | fmt::emphasis::bold, "CLEAN\n");
			#endif
		#else
				BlackTek::Console::StyledPrint(false, fg(green)  | fmt::emphasis::bold, "OFFICIAL\n");
		#endif
		BlackTek::Console::StyledPrint(false, fg(dark_gray), "    ─────────────────────────────────────────────────────────────────\n\n");
	}

	// Print a section header
	void printSection(const std::string& name)
	{
		std::string out;
		out += fmt::format(fg(cyan) | fmt::emphasis::bold, "\n    {} {}\n", DIAMOND, name);
		out += fmt::format(fg(dark_gray), "    ────────────────────────────────────────");
		BlackTek::Console::LogAndPrint(std::move(out));
	}

	void printProgress(const std::string& name, bool success = true, const std::string& detail = "")
	{
		std::string out;
		out += fmt::format(fg(gray),  "    ");
		out += fmt::format(fg(white), "{:<24}", name);

		if (success)
			out += fmt::format(fg(green) | fmt::emphasis::bold, "{}", CHECK);
		else
			out += fmt::format(fg(red)   | fmt::emphasis::bold, "{}", CROSS);

		if (not detail.empty())
			out += fmt::format(fg(dark_gray), "  {:>12}", detail);

		BlackTek::Console::LogAndPrint(std::move(out));
	}

	// Print a simple status line
	void printStatus(const std::string& message, bool success = true)
	{
		std::string out;
		out += fmt::format(fg(gray),  "    {} ", ARROW);
		out += fmt::format(fg(white), "{}", message);
		if (success)
			out += fmt::format(fg(green) | fmt::emphasis::bold, " {}", CHECK);
		else
			out += fmt::format(fg(red)   | fmt::emphasis::bold, " {}", CROSS);
		BlackTek::Console::LogAndPrint(std::move(out));
	}

	// Print info line
	void printInfo(const std::string& label, const std::string& value)
	{
		std::string out;
		out += fmt::format(fg(dark_gray), ":    {:<16}", label);
		out += fmt::format(fg(white),     "{}", value);
		BlackTek::Console::LogAndPrint(std::move(out));
	}

	// Print the final online status box
	void printOnline(const std::string& serverName)
	{
		std::string onlineText = "◆ " + serverName + " SERVER ONLINE ◆";
		int totalWidth = 68;
		auto textLen = static_cast<int>(onlineText.length());
		int padding = (totalWidth - textLen) / 2;

		std::string out;
		out += "\n";
		out += fmt::format(fg(dark_gray), "    ╔══════════════════════════════════════════════════════════════════╗\n");
		out += fmt::format(fg(dark_gray), "    ║");
		out += fmt::format(fg(cyan),                              "{:>{}}", "", padding);
		out += fmt::format(fg(cyan)  | fmt::emphasis::bold,       "{}", DIAMOND);
		out += fmt::format(fg(white) | fmt::emphasis::bold,       " {} ", serverName);
		out += fmt::format(fg(green) | fmt::emphasis::bold,       "| ONLINE | ");
		out += fmt::format(fg(cyan)  | fmt::emphasis::bold,       "{}", DIAMOND);
		out += fmt::format(fg(cyan),                              "{:<{}}", "", padding - 2);
		out += fmt::format(fg(dark_gray), "       ║\n");
		out += fmt::format(fg(dark_gray), "    ╚══════════════════════════════════════════════════════════════════╝\n\n");
		BlackTek::Console::LogAndPrint(std::move(out));
	}

	// Print error message
	void printError(const std::string& message)
	{
		std::string out = fmt::format(fg(red) | fmt::emphasis::bold, "\n    {} ERROR: {}\n\n", CROSS, message);
		BlackTek::Console::LogAndPrint(std::move(out));
	}

	// Print warning message
	void printWarning(const std::string& message)
	{
		std::string out = fmt::format(fg(yellow) | fmt::emphasis::bold, "    ⚠ WARNING: {}\n", message);
		BlackTek::Console::LogAndPrint(std::move(out));
	}
}

// ============================================================================
// Server Functions
// ============================================================================

void startupErrorMessage(const std::string& errorStr)
{
	Console::printError(errorStr);
	g_loaderSignal.notify_all();
}

void mainLoader(int argc, char* argv[], ServiceManager* services);
bool argumentsHandler(const StringVector& args);

[[noreturn]] 
void badAllocationHandler()
{
	// Use functions that only use stack allocation
	puts("Allocation failed, server out of memory.\nDecrease the size of your map or compile in 64 bits mode.\n");
	getchar();
	exit(-1);
}

int main(int argc, char* argv[])
{
	StringVector args = StringVector(argv, argv + argc);
	if(argc > 1 && !argumentsHandler(args))
	{
		return 0;
	}

	// Setup bad allocation handler
	std::set_new_handler(badAllocationHandler);
	BlackTek::Console::Initialize();
	ServiceManager serviceManager;

	g_dispatcher.start();
	g_scheduler.start();
	g_utility_boss.start();

	g_dispatcher.addTask(createTask([=, services = &serviceManager]() { mainLoader(argc, argv, services); }));

	g_loaderSignal.wait(g_loaderUniqueLock);

	if (serviceManager.is_running())
	{
		Console::printOnline(g_config.GetString(ConfigManager::SERVER_NAME));
		serviceManager.run();
	}
	else
	{
		Console::printError("No services running. The server is NOT online.");

		g_scheduler.shutdown();
		g_databaseTasks.shutdown();
		g_dispatcher.shutdown();
		g_utility_boss.shutdown();

		BlackTek::Console::Shutdown();
	}

	g_scheduler.join();
	g_databaseTasks.join();
	g_dispatcher.join();
	g_utility_boss.join();

	return 0;
}

#include "accountmanager.h"

void printServerVersion()
{
	Console::printBanner();
	Console::printVersionBar();
	
	#if defined(GIT_RETRIEVED_STATE) && GIT_RETRIEVED_STATE
		Console::printInfo("Git SHA1", std::string(GIT_SHORT_SHA1) + " (" + GIT_COMMIT_DATE_ISO8601 + ")");
	#endif

	Console::printInfo("Developed By", STATUS_SERVER_DEVELOPERS);
	Console::printInfo("Maintainer", STATUS_SERVER_MAINTAINER);
	Console::printInfo("Community", STATUS_SERVER_COMMUNITY_LINK);
	Console::printInfo("Website", "black-tek.github.io/blacktek/welcome/");
}

void mainLoader(int, char*[], ServiceManager* services)
{
	// dispatcher thread
	g_game.setGameState(GAME_STATE_STARTUP);

	srand(static_cast<unsigned int>(OTSYS_TIME()));

	#ifdef _WIN32
		SetConsoleTitle(STATUS_SERVER_NAME);

		// Enable ANSI/VT100 escape code processing
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);

		// We disable quick edit mode because it causes the server to freeze whenever a user clicks the console window
		HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
		DWORD dwInMode = 0;
		GetConsoleMode(hIn, &dwInMode);
		dwInMode &= ~ENABLE_QUICK_EDIT_MODE;
		SetConsoleMode(hIn, dwInMode);
	#endif

	// banner and version info
	printServerVersion();
	BlackTek::Console::Print("\n");
	BlackTek::Console::LogAndPrint(fmt::format(fg(Console::dark_gray), "    ────────────────────────────────────────"));

	// Load configuration from config/ directory (TOML files)
	if (not g_config.Load())
	{
		startupErrorMessage("Unable to load configuration files from the config/ directory!");
		return;
	}

	#ifdef _WIN32
		const std::string& defaultPriority = g_config.GetString(ConfigManager::DEFAULT_PRIORITY);
		if (caseInsensitiveEqual(defaultPriority, "high"))
		{
			SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
		}
		else if (caseInsensitiveEqual(defaultPriority, "above-normal"))
		{
			SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
		}
	#endif

	try
	{
		g_RSA.loadPEM("key.pem");
	}
	catch(const std::exception& e)
	{
		startupErrorMessage(e.what());
		return;
	}

	// Connect to database
	if (not Database::getInstance().connect())
	{
		startupErrorMessage("Failed to connect to database.");
		return;
	}

	Console::printInfo("Compiler", BOOST_COMPILER);
	Console::printInfo("Compiled", std::string(__DATE__) + " " + __TIME__);
	Console::printInfo("Lua Version", LUA_VERSION);
	Console::printInfo("Database", std::string("MariaDB ") + Database::getClientVersion());

	// Run database manager
	if (not DatabaseManager::isDatabaseSetup())
	{
		startupErrorMessage("The database you have specified in config.lua is empty, please import the schema.sql to your database.");
		return;
	}
	g_databaseTasks.start();
	DatabaseManager::updateDatabase();

	if (g_config.GetBoolean(ConfigManager::OPTIMIZE_DATABASE) and not DatabaseManager::optimizeTables())
	{
		Console::printWarning("No tables were optimized.");
	}

	// ========================================================================
	// SERVER CONFIGURATION
	// ========================================================================
	Console::printSection("SERVER CONFIG");

	// Check world type
	std::string worldType = asLowerCaseString(g_config.GetString(ConfigManager::WORLD_TYPE));
	if (worldType == "pvp")
	{
		g_game.setWorldType(WORLD_TYPE_PVP);
	}
	else if (worldType == "no-pvp")
	{
		g_game.setWorldType(WORLD_TYPE_NO_PVP);
	}
	else if (worldType == "pvp-enforced")
	{
		g_game.setWorldType(WORLD_TYPE_PVP_ENFORCED);
	}
	else
	{
		startupErrorMessage(fmt::format("Unknown world type: {:s}, valid world types are: pvp, no-pvp and pvp-enforced.", g_config.GetString(ConfigManager::WORLD_TYPE)));
		return;
	}

	Console::printProgress("World Type", true, asUpperCaseString(worldType));
	Console::printProgress("World Map",  true, g_config.GetString(ConfigManager::MAP_NAME));

	// Account Manager
	if (g_config.GetBoolean(ConfigManager::ENABLE_ACCOUNT_MANAGER))
	{
		AccountManager::initialize();
		Console::printProgress("Account Manager", true, "enabled");
	}

	Console::printProgress("Game Port",   true, std::to_string(g_config.GetNumber(ConfigManager::GAME_PORT)));
	Console::printProgress("Login Port",  true, std::to_string(g_config.GetNumber(ConfigManager::LOGIN_PORT)));
	Console::printProgress("Status Port", true, std::to_string(g_config.GetNumber(ConfigManager::STATUS_PORT)));

	// ========================================================================
	// GAME DATA
	// ========================================================================
	Console::printSection("GAME DATA");

	// Load vocations
	if (not g_vocations.loadFromToml())
	{
		startupErrorMessage("Unable to load vocations!");
		return;
	}
	Console::printProgress("Vocations", true, std::to_string(g_vocations.getVocations().size()));

	// Load items
	if (not Item::items.loadFromDat(g_config.GetString(ConfigManager::ASSETS_DAT_PATH)))
	{
		startupErrorMessage("Unable to load items (DAT)!");
		return;
	}
	if (not Item::items.loadFromToml())
	{
		startupErrorMessage("Unable to load items (TOML)!");
		return;
	}
	Console::printProgress("Items", true, std::to_string(Item::items.size()));

	// Load script systems
	if (not ScriptingManager::getInstance().loadScriptSystems())
	{
		startupErrorMessage("Failed to load script systems");
		return;
	}

	// Load lua scripts
	if (not g_scripts->loadScripts("scripts", false, false))
	{
		startupErrorMessage("Failed to load lua scripts");
		return;
	}

	// Load outfits
	if (not Outfits::getInstance().load())
	{
		startupErrorMessage("Unable to load outfits!");
		return;
	}

	// todo: split this to show both counts individually
	Console::printProgress("Outfits", true, std::to_string(Outfits::getInstance().getOutfits(PLAYERSEX_FEMALE).size() + Outfits::getInstance().getOutfits(PLAYERSEX_MALE).size()));

	// Load guilds
	IOGuild::loadGuilds();
	Console::printProgress("Guilds", true, std::to_string(g_game.getGuilds().size()));

	// Load monsters
	if (not g_monsters.loadFromXml())
	{
		startupErrorMessage("Unable to load monsters!");
		return;
	}

	// Load lua monsters
	if (not g_scripts->loadScripts("monster", false, false))
	{
		startupErrorMessage("Failed to load lua monsters");
		return;
	}
	Console::printProgress("Monsters", true, std::to_string(g_monsters.count()));

	// Load zones
	Zones::load();
	Console::printProgress("Zones", true, std::to_string(Zones::count()));

	// Load augments
	Augments::loadAll();
	Console::printProgress("Augments", true, std::to_string(Augments::count()));


	// Load map
	if (not g_game.loadMainMap(g_config.GetString(ConfigManager::MAP_NAME)))
	{
		startupErrorMessage("Failed to load map");
		return;
	}

	// Initialize game state
	g_game.setGameState(GAME_STATE_INIT);

	// Game client protocols
	services->add<ProtocolGame>(static_cast<uint16_t>(g_config.GetNumber(ConfigManager::GAME_PORT)));
	services->add<ProtocolLogin>(static_cast<uint16_t>(g_config.GetNumber(ConfigManager::LOGIN_PORT)));

	// OT protocols
	services->add<ProtocolStatus>(static_cast<uint16_t>(g_config.GetNumber(ConfigManager::STATUS_PORT)));

	// Legacy login protocol
	services->add<ProtocolOld>(static_cast<uint16_t>(g_config.GetNumber(ConfigManager::LOGIN_PORT)));

	// House rent
	RentPeriod_t rentPeriod;
	std::string strRentPeriod = asLowerCaseString(g_config.GetString(ConfigManager::HOUSE_RENT_PERIOD));


	// TODO: I want to add the load times for things, and we are still not displaying the old information about the map size
	// the method was changed to std::expected and given error codes for handling each type of failure for loading
	// this needs to be repeated for other loader methods, having them return statistical data we might want to show 
	// on the console, and probably have a dedicated section for this purpose, or, at the very least, alter the entries
	// which already show the "counts" of things loaded, to display "loaded x in x.xx seconds". 



	if (strRentPeriod == "yearly")
	{
		rentPeriod = RENTPERIOD_YEARLY;
	}
	else if (strRentPeriod == "weekly")
	{
		rentPeriod = RENTPERIOD_WEEKLY;
	}
	else if (strRentPeriod == "monthly")
	{
		rentPeriod = RENTPERIOD_MONTHLY;
	}
	else if (strRentPeriod == "daily")
	{
		rentPeriod = RENTPERIOD_DAILY;
	}
	else
	{
		rentPeriod = RENTPERIOD_NEVER;
	}

	g_game.map.houses.payHouses(rentPeriod);

	IOMarket::checkExpiredOffers();
	IOMarket::getInstance().updateStatistics();

#ifndef _WIN32
	if (getuid() == 0 || geteuid() == 0) {
		Console::printWarning(std::string(STATUS_SERVER_NAME) + " has been executed as root user, please consider running it as a normal user.");
	}
#endif

	g_game.start(services);
	g_game.setGameState(GAME_STATE_NORMAL);
	g_loaderSignal.notify_all();
}

bool argumentsHandler(const StringVector& args)
{
	for (const auto& arg : args) {
		if (arg == "--help") {
			std::clog << "Usage:\n"
			"\n"
			"\t--ip=$1\t\t\tIP address of the server.\n"
			"\t\t\t\tShould be equal to the global IP.\n"
			"\t--login-port=$1\tPort for login server to listen on.\n"
			"\t--game-port=$1\tPort for game server to listen on.\n";
			return false;
		} else if (arg == "--version") {
			printServerVersion();
			return false;
		}

		auto tmp = explodeString(arg, "=");

		if (tmp[0] == "--ip")
			g_config.SetString(ConfigManager::IP, tmp[1]);
		else if (tmp[0] == "--login-port")
			g_config.SetNumber(ConfigManager::LOGIN_PORT, std::stoi(tmp[1].data()));
		else if (tmp[0] == "--game-port")
			g_config.SetNumber(ConfigManager::GAME_PORT, std::stoi(tmp[1].data()));
	}

	return true;
}
