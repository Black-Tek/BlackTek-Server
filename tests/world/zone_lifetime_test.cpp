#include "otpch.h"
#include "zones.h"

#include <gtest/gtest.h>

namespace BlackTek::World::Tests
{
    static Zones::SpawnCoroTask HoldZoneFrame(Zones::Zone& zone, std::shared_ptr<int> token)
    {
        co_await std::suspend_always{};
    }

    TEST(ZonePool, ClearInvalidatesHandlesAcrossSlotReuse)
    {
        Zones::ZonePool pool;
        const auto oldHandle = pool.Emplace();
        pool.Clear();
        const auto newHandle = pool.Emplace();
        EXPECT_FALSE(pool.Alive(oldHandle));
        EXPECT_TRUE(pool.Alive(newHandle));
        EXPECT_NE(oldHandle, newHandle);
        pool.Clear();
    }

    TEST(ZonePool, ClearDestroysSuspendedFrames)
    {
        Zones::ZonePool pool;
        const auto handle = pool.Emplace();
        auto token = std::make_shared<int>(1);
        std::weak_ptr<int> weakToken = token;
        HoldZoneFrame(*pool.TryGetMutable(handle), token);
        token.reset();
        ASSERT_FALSE(weakToken.expired());
        pool.Clear();
        EXPECT_TRUE(weakToken.expired());
    }

    TEST(ZonePool, ClearDefersFramesUntilResumeScopeEnds)
    {
        Zones::ZonePool pool;
        const auto handle = pool.Emplace();
        auto zone = pool.TryGetMutable(handle);
        auto token = std::make_shared<int>(1);
        std::weak_ptr<int> weakToken = token;
        HoldZoneFrame(*zone, token);
        token.reset();
        pool.BeginResume(zone->liveFrames.front());
        pool.Clear();
        ASSERT_TRUE(pool.Alive(handle));
        EXPECT_FALSE(weakToken.expired());
        EXPECT_FALSE(zone->Active());
        pool.EndResume();
        pool.DrainGraveyard();
        EXPECT_FALSE(pool.Alive(handle));
        EXPECT_TRUE(weakToken.expired());
    }

    static Zones::SpawnCoroTask ClearDuringInitialRun(Zones::Zone& zone, Zones::ZonePool& pool,
        std::shared_ptr<int> token, bool& reachedSuspension)
    {
        pool.Clear();
        reachedSuspension = true;
        co_await std::suspend_always{};
    }

    TEST(ZonePool, ClearDefersCurrentlyRunningInitialCoroutine)
    {
        Zones::ZonePool pool;
        const auto handle = pool.Emplace();
        auto token = std::make_shared<int>(1);
        std::weak_ptr<int> weakToken = token;
        bool reachedSuspension = false;
        ClearDuringInitialRun(*pool.TryGetMutable(handle), pool, token, reachedSuspension);
        token.reset();
        EXPECT_TRUE(reachedSuspension);
        EXPECT_TRUE(pool.Alive(handle));
        EXPECT_FALSE(weakToken.expired());
        pool.DrainGraveyard();
        EXPECT_FALSE(pool.Alive(handle));
        EXPECT_TRUE(weakToken.expired());
    }

    TEST(ZoneManager, RejectsNegativeIds)
    {
        EXPECT_FALSE(Zones::ZoneManager::RegisterZone(Zones::Zone(-1)));
        EXPECT_THROW(Zones::ZoneManager::CreateZone(-1, {}), std::invalid_argument);
    }

    TEST(ZoneManager, GeneratedIdsFollowExplicitIds)
    {
        Zones::ZoneManager::Clear();
        const auto explicitHandle = Zones::ZoneManager::RegisterZone(Zones::Zone(17));
        const auto generatedHandle = Zones::ZoneManager::RegisterZone(Zones::Zone());
        ASSERT_TRUE(explicitHandle);
        ASSERT_TRUE(generatedHandle);
        EXPECT_GT(Zones::ZoneManager::TryGet(*generatedHandle)->id, 17);
        Zones::ZoneManager::Clear();
    }
}
