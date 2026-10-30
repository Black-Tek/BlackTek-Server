#include "otpch.h"
#include "guild.h"

#include <gtest/gtest.h>

TEST(GuildBalance, LoadedBalanceRetainsAllSixtyFourBits)
{
	Guild guild(1, "Test guild");
	const uint64_t loadedBalance = (uint64_t{1} << 32) + 500;
	guild.setGuildBankBalance(loadedBalance);
	EXPECT_EQ(guild.getGuildBankBalance(), loadedBalance);
	guild.setGuildBankBalance(UINT64_MAX);
	EXPECT_EQ(guild.getGuildBankBalance(), UINT64_MAX);
}
