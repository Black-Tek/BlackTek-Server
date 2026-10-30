#include "otpch.h"
#include "luascript.h"
#include "zones.h"

#include <gtest/gtest.h>

namespace BlackTek::Net::Tests
{
    class LuaZoneLifetime : public testing::Test
    {
    protected:
        void SetUp() override { Zones::ZoneManager::Clear(); }
        void TearDown() override
        {
            Zones::ZoneManager::Clear();
            Zones::ZoneManager::DrainGraveyard();
        }
    };

    TEST_F(LuaZoneLifetime, ActivationRegistersBeforeStartingFrames)
    {
        LuaScriptInterface interface("Zone lifetime regression");
        ASSERT_TRUE(interface.initState());
        auto* state = interface.getLuaState();
        ASSERT_EQ(luaL_dostring(state,
            "zone = Zone({name = 'AuditPendingActivation', spawnType = 'monster', policy = 'fixed'})\n"
            "assert(zone:getId() == 0)\n"
            "assert(zone:activate())\n"
            "assert(zone:getId() > 0)\n"
            "assert(zone:register(false))\n"
            "assert(zone:isActive())"), LUA_OK);

        const auto handle = Zones::ZoneManager::GetZoneByName("AuditPendingActivation");
        ASSERT_TRUE(handle);
        const auto* zone = Zones::ZoneManager::TryGet(*handle);
        ASSERT_NE(zone, nullptr);
        ASSERT_FALSE(zone->liveFrames.empty());
        for (const auto frame : zone->liveFrames)
        {
            const auto ownedFrame = std::coroutine_handle<Zones::SpawnCoroTask::promise_type>::from_address(frame.address());
            EXPECT_EQ(ownedFrame.promise().owner, zone);
        }

        ASSERT_EQ(luaL_dostring(state, "zone = nil; collectgarbage('collect')"), LUA_OK);
        EXPECT_TRUE(Zones::ZoneManager::Alive(*handle));
        Zones::ZoneManager::Clear();
        EXPECT_FALSE(Zones::ZoneManager::Alive(*handle));
    }

    TEST_F(LuaZoneLifetime, FailedRegistrationDoesNotActivatePendingZone)
    {
        LuaScriptInterface interface("Zone registration regression");
        ASSERT_TRUE(interface.initState());
        auto* state = interface.getLuaState();
        EXPECT_EQ(luaL_dostring(state,
            "local first = Zone({name = 'AuditDuplicateActivation', spawnType = 'monster', policy = 'fixed'})\n"
            "local second = Zone({name = 'AuditDuplicateActivation', spawnType = 'monster', policy = 'fixed'})\n"
            "assert(first:register(false))\n"
            "assert(second:activate() == false)\n"
            "assert(second:getId() == 0)\n"
            "assert(not second:isActive())"), LUA_OK);
    }
}
