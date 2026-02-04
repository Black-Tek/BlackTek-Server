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
		std::cout << "\n";
		
		// Gradient ASCII art - using cyan and magenta
		fmt::print(fg(dark_purple) | fmt::emphasis::bold,
			"    ██████╗ ██╗      █████╗  ██████╗██╗  ██╗████████╗███████╗██╗  ██╗\n");
		fmt::print(fg(dark_purple),
			"    ██╔══██╗██║     ██╔══██╗██╔════╝██║ ██╔╝╚══██╔══╝██╔════╝██║ ██╔╝\n");
		fmt::print(fg(dark_purple),
			"    ██████╔╝██║     ███████║██║     █████╔╝    ██║   █████╗  █████╔╝ \n");
		fmt::print(fg(dark_purple),
			"    ██╔══██╗██║     ██╔══██║██║     ██╔═██╗    ██║   ██╔══╝  ██╔═██╗ \n");
		fmt::print(fg(dark_purple),
			"    ██████╔╝███████╗██║  ██║╚██████╗██║  ██╗   ██║   ███████╗██║  ██╗\n");
		fmt::print(fg(dark_purple) | fmt::emphasis::bold,
			"    ╚═════╝ ╚══════╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝\n");
		
		std::cout << "\n";
	}

	// Print version info bar
	void printVersionBar()
	{
		fmt::print(fg(dark_gray), "    ─────────────────────────────────────────────────────────────────\n");
		fmt::print(fg(gray), "    VERSION ");
		fmt::print(fg(white) | fmt::emphasis::bold, "{} ", STATUS_SERVER_VERSION);
		fmt::print(fg(dark_gray), " · ");
		fmt::print(fg(gray), " CLIENT ");
		fmt::print(fg(white) | fmt::emphasis::bold, "{} ", CLIENT_VERSION_STR);
		fmt::print(fg(dark_gray), " · ");
		fmt::print(fg(gray), " BUILD ");
		#if defined(GIT_RETRIEVED_STATE) && GIT_RETRIEVED_STATE
			#if GIT_IS_DIRTY
				fmt::print(fg(yellow) | fmt::emphasis::bold, "DIRTY\n");
			#else
				fmt::print(fg(green) | fmt::emphasis::bold, "CLEAN\n");
			#endif
		#else
				fmt::print(fg(green) | fmt::emphasis::bold, "OFFICIAL\n");
		#endif
		fmt::print(fg(dark_gray), "    ─────────────────────────────────────────────────────────────────\n\n");
	}

	// Print a section header
	void printSection(const std::string& name)
	{
		fmt::print(fg(cyan) | fmt::emphasis::bold, "\n    {} {}\n", DIAMOND, name);
		fmt::print(fg(dark_gray), "    ────────────────────────────────────────\n");
	}

	void printProgress(const std::string& name, bool success = true, const std::string& detail = "")
	{
		fmt::print(fg(gray), "    ");
		fmt::print(fg(white), "{:<24}", name);
				
		if (success)
		{
			fmt::print(fg(green) | fmt::emphasis::bold, "{}", CHECK);
		}
		else
		{
			fmt::print(fg(red) | fmt::emphasis::bold, "{}", CROSS);
		}
		
		// Optional detail
		if (not detail.empty())
		{
			fmt::print(fg(dark_gray), "  {:>12}", detail);
		}
		
		std::cout << "\n";
	}

	// Print a simple status line
	void printStatus(const std::string& message, bool success = true)
	{
		fmt::print(fg(gray), "    {} ", ARROW);
		fmt::print(fg(white), "{}", message);
		if (success)
		{
			fmt::print(fg(green) | fmt::emphasis::bold, " {}\n", CHECK);
		} 
		else
		{
			fmt::print(fg(red) | fmt::emphasis::bold, " {}\n", CROSS);
		}
	}

	// Print info line
	void printInfo(const std::string& label, const std::string& value)
	{
		fmt::print(fg(dark_gray), ":    {:<16}", label);
		fmt::print(fg(white), "{}\n", value);
	}

	// Print the final online status box
	void printOnline(const std::string& serverName)
	{
		std::cout << "\n";
		fmt::print(fg(dark_gray), "    ╔══════════════════════════════════════════════════════════════════╗\n");
		fmt::print(fg(dark_gray), "    ║");
		
		// Calculate padding for centered text
		std::string onlineText = "◆ " + serverName + " SERVER ONLINE ◆";
		int totalWidth = 68;
		int textLen = onlineText.length();
		int padding = (totalWidth - textLen) / 2;
		
		fmt::print(fg(cyan), "{:>{}}", "", padding);
		fmt::print(fg(cyan) | fmt::emphasis::bold, "{}", DIAMOND);
		fmt::print(fg(white) | fmt::emphasis::bold, " {} ", serverName);
		fmt::print(fg(green) | fmt::emphasis::bold, "| ONLINE | ");
		fmt::print(fg(cyan) | fmt::emphasis::bold, "{}", DIAMOND);
		fmt::print(fg(cyan), "{:<{}}", "", padding - 2);
		
		fmt::print(fg(dark_gray), "       ║\n");
		fmt::print(fg(dark_gray), "    ╚══════════════════════════════════════════════════════════════════╝\n\n");
	}

	// Print error message
	void printError(const std::string& message)
	{
		fmt::print(fg(red) | fmt::emphasis::bold, "\n    {} ERROR: {}\n\n", CROSS, message);
	}

	// Print warning message
	void printWarning(const std::string& message)
	{
		fmt::print(fg(yellow) | fmt::emphasis::bold, "    ⚠ WARNING: {}\n", message);
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

	ServiceManager serviceManager;

	g_dispatcher.start();
	g_scheduler.start();
	g_utility_boss.start();

	g_dispatcher.addTask(createTask([=, services = &serviceManager]() { mainLoader(argc, argv, services); }));

	g_loaderSignal.wait(g_loaderUniqueLock);

	if (serviceManager.is_running())
	{
		Console::printOnline(g_config.getString(ConfigManager::SERVER_NAME));
		serviceManager.run();
	}
	else
	{
		Console::printError("No services running. The server is NOT online.");
		g_scheduler.shutdown();
		g_databaseTasks.shutdown();
		g_dispatcher.shutdown();
		g_utility_boss.shutdown();
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

		// fixes a problem with escape characters not being processed in Windows consoles
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode = 0;
		GetConsoleMode(hOut, &dwMode);
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hOut, dwMode);
	#endif

	// banner and version info
	g_utility_boss.addTask(createTask([]() { printServerVersion(); }));
	g_utility_boss.addTask(createTask([]() { std::cout <<"\n"; }));
	g_utility_boss.addTask(createTask([]() { fmt::print(fg(Console::dark_gray), "    ────────────────────────────────────────\n"); }));

	// check if config.lua or config.lua.dist exist
	const std::string& configFile = g_config.getString(ConfigManager::CONFIG_FILE);
	std::ifstream c_test("./" + configFile);

	if (not c_test.is_open())
	{
		// todo: instead of copying from dist here, we should have a default built in
		std::ifstream config_lua_dist("./config.lua.dist");
		if (config_lua_dist.is_open())
		{
			Console::printStatus("Copying config.lua.dist to " + configFile);
			std::ofstream config_lua(configFile);
			config_lua << config_lua_dist.rdbuf();
			config_lua.close();
			config_lua_dist.close();
		}
	}
	else
	{
		c_test.close();
	}

	// Load config
	if (not g_config.load())
	{
		startupErrorMessage("Unable to load " + configFile + "!");
		return;
	}

	#ifdef _WIN32
		const std::string& defaultPriority = g_config.getString(ConfigManager::DEFAULT_PRIORITY);
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

	g_utility_boss.addTask(createTask([]() { Console::printInfo("Compiler", BOOST_COMPILER); }));
	g_utility_boss.addTask(createTask([]() { Console::printInfo("Compiled", std::string(__DATE__) + " " + __TIME__); }));
	g_utility_boss.addTask(createTask([]() { Console::printInfo("Lua Version", LUA_VERSION); }));
	g_utility_boss.addTask(createTask([]() { Console::printInfo("Database", std::string("MariaDB ") + Database::getClientVersion()); }));

	// Run database manager
	if (not DatabaseManager::isDatabaseSetup())
	{
		startupErrorMessage("The database you have specified in config.lua is empty, please import the schema.sql to your database.");
		return;
	}
	g_databaseTasks.start();
	DatabaseManager::updateDatabase();

	if (g_config.getBoolean(ConfigManager::OPTIMIZE_DATABASE) && !DatabaseManager::optimizeTables())
	{
		g_utility_boss.addTask(createTask([]() { Console::printWarning("No tables were optimized."); }));
	}

	// ========================================================================
	// SERVER CONFIGURATION
	// ========================================================================
	g_utility_boss.addTask(createTask([]() { Console::printSection("SERVER CONFIG"); }));

	// Check world type
	std::string worldType = asLowerCaseString(g_config.getString(ConfigManager::WORLD_TYPE));
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
		startupErrorMessage(fmt::format("Unknown world type: {:s}, valid world types are: pvp, no-pvp and pvp-enforced.", g_config.getString(ConfigManager::WORLD_TYPE)));
		return;
	}

	g_utility_boss.addTask(createTask([worldType]()
	{ 
		Console::printProgress("World Type", true, asUpperCaseString(worldType)); 
	}));

	g_utility_boss.addTask(createTask([]()
	{ 
		Console::printProgress("World Map", true, g_config.getString(ConfigManager::MAP_NAME)); 
	}));


	// Account Manager
	if (g_config.getBoolean(ConfigManager::ENABLE_ACCOUNT_MANAGER))
	{
		AccountManager::initialize();
		g_utility_boss.addTask(createTask([]() { Console::printProgress("Account Manager", true, "enabled"); }));
	}

	g_utility_boss.addTask(createTask([]()
	{
		Console::printProgress("Game Port", true, std::to_string(g_config.getNumber(ConfigManager::GAME_PORT)));
		Console::printProgress("Login Port", true, std::to_string(g_config.getNumber(ConfigManager::LOGIN_PORT)));
		Console::printProgress("Status Port", true, std::to_string(g_config.getNumber(ConfigManager::STATUS_PORT)));
	}));

	// ========================================================================
	// GAME DATA
	// ========================================================================
	g_utility_boss.addTask(createTask([]() { Console::printSection("GAME DATA"); }));

	// Load items
	if (not Item::items.loadFromDat(g_config.getString(ConfigManager::ASSETS_DAT_PATH)))
	{
		startupErrorMessage("Unable to load items (DAT)!");
		return;
	}
	if (not Item::items.loadFromToml())
	{
		startupErrorMessage("Unable to load items (TOML)!");
		return;
	}
	g_utility_boss.addTask(createTask([]()
	{ 
		Console::printProgress("Items", true, std::to_string(Item::items.size())); 
	}));

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


	// Load vocations
	if (not g_vocations.loadFromToml())
	{
		startupErrorMessage("Unable to load vocations!");
		return;
	}
	g_utility_boss.addTask(createTask([]() { Console::printProgress("Vocations", true, std::to_string(g_vocations.getVocations().size())); }));

	// Load outfits
	if (not Outfits::getInstance().load())
	{
		startupErrorMessage("Unable to load outfits!");
		return;
	}

	// todo: split this to show both counts individually
	g_utility_boss.addTask(createTask([]() { Console::printProgress("Outfits", true, std::to_string(Outfits::getInstance().getOutfits(PLAYERSEX_FEMALE).size() +  Outfits::getInstance().getOutfits(PLAYERSEX_MALE).size())); }));

	// Load guilds
	IOGuild::loadGuilds();
	g_utility_boss.addTask(createTask([]()
	{ 
		Console::printProgress("Guilds", true, std::to_string(g_game.getGuilds().size())); 
	}));

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
	g_utility_boss.addTask(createTask([]() { Console::printProgress("Monsters", true, std::to_string(g_monsters.count())); }));

	// Load zones
	Zones::load();
	g_utility_boss.addTask(createTask([]() { Console::printProgress("Zones", true, std::to_string(Zones::count())); }));

	// Load augments
	Augments::loadAll();
	g_utility_boss.addTask(createTask([]() { Console::printProgress("Augments", true, std::to_string(Augments::count())); }));


	// Load map
	if (not g_game.loadMainMap(g_config.getString(ConfigManager::MAP_NAME)))
	{
		startupErrorMessage("Failed to load map");
		return;
	}

	// Initialize game state
	g_game.setGameState(GAME_STATE_INIT);

	// Game client protocols
	services->add<ProtocolGame>(static_cast<uint16_t>(g_config.getNumber(ConfigManager::GAME_PORT)));
	services->add<ProtocolLogin>(static_cast<uint16_t>(g_config.getNumber(ConfigManager::LOGIN_PORT)));

	// OT protocols
	services->add<ProtocolStatus>(static_cast<uint16_t>(g_config.getNumber(ConfigManager::STATUS_PORT)));

	// Legacy login protocol
	services->add<ProtocolOld>(static_cast<uint16_t>(g_config.getNumber(ConfigManager::LOGIN_PORT)));

	// House rent
	RentPeriod_t rentPeriod;
	std::string strRentPeriod = asLowerCaseString(g_config.getString(ConfigManager::HOUSE_RENT_PERIOD));


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
			"\t--config=$1\t\tAlternate configuration file path.\n"
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

		if (tmp[0] == "--config")
			g_config.setString(ConfigManager::CONFIG_FILE, tmp[1]);
		else if (tmp[0] == "--ip")
			g_config.setString(ConfigManager::IP, tmp[1]);
		else if (tmp[0] == "--login-port")
			g_config.setNumber(ConfigManager::LOGIN_PORT, std::stoi(tmp[1].data()));
		else if (tmp[0] == "--game-port")
			g_config.setNumber(ConfigManager::GAME_PORT, std::stoi(tmp[1].data()));
	}

	return true;
}
