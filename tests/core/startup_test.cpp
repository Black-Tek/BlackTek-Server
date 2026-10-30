#include "otpch.h"
#include <gtest/gtest.h>
#include "configmanager.h"
#include "tools.h"

extern ConfigManager g_config;
bool argumentsHandler(const StringVector& args);
namespace Console { void printOnline(const std::string& serverName); }

TEST(StartupArguments, RejectsMissingMalformedAndOutOfRangePorts)
{
	for (const auto* argument : {"--login-port", "--game-port=", "--game-port=-1", "--login-port=65536", "--game-port=7172junk", "--game-port=0", "--game-port=99999999999999999999", "--ip="})
	{
		EXPECT_FALSE(argumentsHandler({"server", argument})) << argument;
	}
}

TEST(StartupArguments, AppliesCompletePortValuesAndAddress)
{
	const auto oldIp = g_config.GetString(ConfigManager::IP);
	const auto oldLogin = g_config.GetNumber(ConfigManager::LOGIN_PORT);
	const auto oldGame = g_config.GetNumber(ConfigManager::GAME_PORT);
	EXPECT_TRUE(argumentsHandler({"server", "--ip=127.0.0.1", "--login-port=7171", "--game-port=65535"}));
	EXPECT_EQ(g_config.GetString(ConfigManager::IP), "127.0.0.1");
	EXPECT_EQ(g_config.GetNumber(ConfigManager::LOGIN_PORT), 7171);
	EXPECT_EQ(g_config.GetNumber(ConfigManager::GAME_PORT), 65535);
	g_config.SetString(ConfigManager::IP, oldIp);
	g_config.SetNumber(ConfigManager::LOGIN_PORT, oldLogin);
	g_config.SetNumber(ConfigManager::GAME_PORT, oldGame);
}

TEST(StartupConsole, LongServerNameDoesNotProduceNegativeFormatWidth)
{
	EXPECT_NO_THROW(Console::printOnline(std::string(100, 'x')));
}
