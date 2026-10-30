#include "loginqueue.h"

#include <gtest/gtest.h>

TEST(LoginQueue, PremiumWaitsWhenServerIsFull)
{
	BlackTek::Net::LoginQueue queue;
	EXPECT_EQ(queue.Login(1, true, 100, 100, 0), 1U);
	EXPECT_EQ(queue.Login(1, true, 100, 100, 1), 1U);
}

TEST(LoginQueue, PremiumPriorityPreservesArrivalOrder)
{
	BlackTek::Net::LoginQueue queue;
	EXPECT_EQ(queue.Login(1, false, 100, 100, 0), 1U);
	EXPECT_EQ(queue.Login(2, true, 100, 100, 0), 1U);
	EXPECT_EQ(queue.Login(3, true, 100, 100, 0), 2U);
	EXPECT_EQ(queue.Login(4, false, 100, 100, 0), 4U);
	EXPECT_EQ(queue.Login(1, false, 100, 100, 1), 3U);
	EXPECT_EQ(queue.Login(2, true, 99, 100, 1), 0U);
	EXPECT_EQ(queue.Login(3, true, 100, 100, 2), 1U);
}

TEST(LoginQueue, ExpiryDoesNotLeaveInvalidPriorityBoundary)
{
	BlackTek::Net::LoginQueue queue;
	EXPECT_EQ(queue.Login(1, true, 100, 100, 0), 1U);
	EXPECT_EQ(queue.Login(2, false, 100, 100, 1000), 2U);
	EXPECT_EQ(queue.Login(3, true, 100, 100, 20000), 1U);
	EXPECT_EQ(queue.Login(2, false, 100, 100, 20000), 2U);
}

TEST(LoginQueue, NewArrivalUsesCapacityAfterExpiredEntriesAreRemoved)
{
	BlackTek::Net::LoginQueue queue;
	EXPECT_EQ(queue.Login(1, true, 100, 100, 0), 1U);
	EXPECT_EQ(queue.Login(2, false, 99, 100, 20000), 0U);
}

TEST(LoginQueue, RepeatedGrowthAndErasureKeepsAllSlotsValid)
{
	BlackTek::Net::LoginQueue queue;
	for (uint32_t guid = 1; guid <= 1000; ++guid)
		EXPECT_EQ(queue.Login(guid, false, 100, 100, 0), guid);

	for (uint32_t guid = 1001; guid <= 1100; ++guid)
		EXPECT_EQ(queue.Login(guid, true, 100, 100, 0), guid - 1000);

	for (uint32_t guid = 1001; guid <= 1100; ++guid)
		EXPECT_EQ(queue.Login(guid, true, 99, 100, 1), 0U);

	EXPECT_EQ(queue.Login(1, false, 100, 100, 2), 1U);
}

TEST(LoginQueue, UnlimitedCapacityDoesNotQueue)
{
	BlackTek::Net::LoginQueue queue;
	EXPECT_EQ(queue.Login(1, true, 100, 0, 0), 0U);
}
