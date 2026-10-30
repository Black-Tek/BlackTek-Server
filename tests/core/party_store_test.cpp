#include "otpch.h"
#include "party.h"
#include "game.h"
#include "events.h"
#include "player.h"
#include "storewindow.h"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

extern Game g_game;
extern Events* g_events;

TEST(PartyLifetime, DisbandReleasesRegistryAndPlayers)
{
    Events events;
    struct RestoreEvents
    {
        Events* previous;
        ~RestoreEvents() { g_events = previous; }
    } restore{g_events};
    g_events = &events;
    auto leader = g_game.MakePlayer(nullptr);
    auto party = Party::make(leader);
    const auto id = party->getId();
    leader->setParty(id);
    std::weak_ptr<Player> weakLeader = leader;
    party->disband();
    EXPECT_EQ(Party::get(id), nullptr);
    EXPECT_EQ(leader->getPartyId(), 0);
    EXPECT_EQ(party->getLeader(), nullptr);
    EXPECT_NO_THROW(party->disband());
    leader.reset();
    EXPECT_TRUE(weakLeader.expired());
}

TEST(StoreValidation, CallbackErrorsAndInvalidReturnTypesFailClosed)
{
    LuaScriptInterface interface("Store regression");
    ASSERT_TRUE(interface.initState());
    auto* state = interface.getLuaState();
    ASSERT_EQ(luaL_dostring(state,
        "function broken(p, id) error('validation failed') end\n"
        "function missing(p, id) return nil end\n"
        "function text(p, id) return 'yes' end\n"
        "function denied(p, id) return false, 'not allowed' end\n"
        "function allowed(p, id) return true end"), LUA_OK);
    BlackTek::StoreCategory category;
    category.scriptInterface = &interface;
    const auto player = g_game.MakePlayer(nullptr);
    for (const auto name : {"broken", "missing", "text", "denied"})
    {
        category.canPurchaseScriptId = interface.getEvent(name);
        ASSERT_NE(category.canPurchaseScriptId, -1);
        std::string reason;
        EXPECT_FALSE(category.executeCanPurchase(player, 1, reason)) << name;
    }
    category.canPurchaseScriptId = interface.getEvent("allowed");
    std::string reason;
    EXPECT_TRUE(category.executeCanPurchase(player, 1, reason));
}

TEST(PartyLifetime, DiscardRejectedInitialInvitationReleasesRegistry)
{
    auto leader = g_game.MakePlayer(nullptr);
    auto party = Party::make(leader);
    const auto id = party->getId();
    leader->setParty(id);
    EXPECT_TRUE(party->discardEmpty());
    EXPECT_EQ(Party::get(id), nullptr);
    EXPECT_EQ(leader->getPartyId(), 0U);
    EXPECT_EQ(party->getLeader(), nullptr);
}

TEST(PartyLifetime, RegistryReleasesPooledPlayersBeforeGameDestruction)
{
    ASSERT_EXIT(
        {
            {
                auto leader = g_game.MakePlayer(nullptr);
                auto party = Party::make(leader);
                leader->setParty(party->getId());
            }
            std::exit(0);
        },
        testing::ExitedWithCode(0), "");
}

namespace
{
    class PartyReentrancy : public testing::Test
    {
    protected:
        Events events;
        Events* previous = nullptr;

        void SetUp() override
        {
            previous = g_events;
            g_events = &events;
        }

        void TearDown() override { g_events = previous; }

        void LoadCallback(const std::string& method, const std::string& body)
        {
            const auto directory = std::filesystem::temp_directory_path() / ("blacktek-party-callback-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
            struct RestoreDirectory
            {
                std::filesystem::path previous;
                std::filesystem::path temporary;
                ~RestoreDirectory()
                {
                    std::filesystem::current_path(previous);
                    if (std::filesystem::weakly_canonical(temporary).parent_path() == std::filesystem::weakly_canonical(std::filesystem::temp_directory_path())
                        and temporary.filename().string().starts_with("blacktek-party-callback-"))
                        std::filesystem::remove_all(temporary);
                }
            } restore{std::filesystem::current_path(), directory};
            std::filesystem::create_directories(directory / "data/events/scripts");
            {
                std::ofstream file(directory / "data/events/events.xml");
                file << "<events><event class=\"Party\" method=\"" << method << "\" enabled=\"1\"/></events>";
            }
            {
                std::ofstream file(directory / "data/events/scripts/party.lua");
                file << "function Party:" << method << "(value) " << body << " end";
            }
            std::filesystem::current_path(directory);
            ASSERT_TRUE(events.load());
        }

        PartyPtr MakeParty(const PlayerPtr& leader, const PlayerPtr& member)
        {
            leader->setName("Leader");
            member->setName("Member");
            auto party = Party::make(leader);
            leader->setParty(party->getId());
            member->setParty(party->getId());
            party->getMembers().push_back(member);
            return party;
        }
    };
}

TEST_F(PartyReentrancy, LeaveCallbackMayDisband)
{
    LoadCallback("onLeave", "self:disband(); return true");
    auto leader = g_game.MakePlayer(nullptr);
    auto member = g_game.MakePlayer(nullptr);
    auto party = MakeParty(leader, member);
    EXPECT_FALSE(party->leaveParty(member));
    EXPECT_EQ(party->getLeader(), nullptr);
    EXPECT_EQ(member->getPartyId(), 0U);
}

TEST_F(PartyReentrancy, LeadershipCallbackMayDisband)
{
    LoadCallback("onPassLeadership", "self:disband(); return true");
    auto leader = g_game.MakePlayer(nullptr);
    auto member = g_game.MakePlayer(nullptr);
    auto party = MakeParty(leader, member);
    EXPECT_FALSE(party->passPartyLeadership(member));
    EXPECT_EQ(party->getLeader(), nullptr);
    EXPECT_TRUE(party->getMembers().empty());
}

TEST_F(PartyReentrancy, SharedExperienceCallbackMayDisband)
{
    LoadCallback("onShareExperience", "self:disband(); return value");
    auto leader = g_game.MakePlayer(nullptr);
    auto member = g_game.MakePlayer(nullptr);
    auto party = MakeParty(leader, member);
    EXPECT_NO_THROW(party->shareExperience(100));
    EXPECT_EQ(party->getLeader(), nullptr);
}

TEST_F(PartyReentrancy, DisbandVetoKeepsLuaHandleAndMembership)
{
    LoadCallback("onDisband", "return false");
    auto leader = g_game.MakePlayer(nullptr);
    auto member = g_game.MakePlayer(nullptr);
    auto party = MakeParty(leader, member);
    LuaScriptInterface interface("Party veto regression");
    ASSERT_TRUE(interface.initState());
    auto* state = interface.getLuaState();
    LuaScriptInterface::pushSharedPtr(state, party);
    LuaScriptInterface::setMetatable(state, -1, "Party");
    lua_setglobal(state, "vetoedParty");
    ASSERT_EQ(luaL_dostring(state, "return vetoedParty:disband(), vetoedParty:getLeader() ~= nil"), LUA_OK);
    EXPECT_FALSE(lua_toboolean(state, -2));
    EXPECT_TRUE(lua_toboolean(state, -1));
    lua_pop(state, 2);
    EXPECT_EQ(leader->getPartyId(), party->getId());
    EXPECT_EQ(Party::get(party->getId()), party);
    LoadCallback("onDisband", "return true");
    party->disband();
    ASSERT_EQ(luaL_dostring(state, "vetoedParty = nil"), LUA_OK);
}

TEST(StoreLifetime, LookupAndRepeatedRegistrationRetainSingleOwner)
{
    auto window = std::make_shared<BlackTek::StoreWindow>("test");
    std::weak_ptr<BlackTek::StoreWindow> weak = window;
    ASSERT_TRUE(g_storeManager.registerWindow(window));
    window->setAccountType(ACCOUNT_TYPE_GOD);
    ASSERT_TRUE(g_storeManager.registerWindow(window));
    auto active = g_storeManager.getWindowForAccountType(ACCOUNT_TYPE_NORMAL);
    window.reset();
    g_storeManager.clear();
    ASSERT_FALSE(weak.expired());
    EXPECT_EQ(active->getTitle(), "test");
    active.reset();
    EXPECT_TRUE(weak.expired());
}

TEST(StoreLifetime, LuaCategoryRetainsWindowAfterRegistryClear)
{
    LuaScriptInterface interface("Store ownership regression");
    ASSERT_TRUE(interface.initState());
    auto* state = interface.getLuaState();
    ASSERT_EQ(luaL_dostring(state,
        "local window = StoreWindow('ownership'); retainedCategory = window:category('items', 'icon'); window:register()"), LUA_OK);
    std::weak_ptr<BlackTek::StoreWindow> weak = g_storeManager.getWindowForAccountType(ACCOUNT_TYPE_NORMAL);
    g_storeManager.clear();
    lua_gc(state, LUA_GCCOLLECT, 0);
    ASSERT_FALSE(weak.expired());
    ASSERT_EQ(luaL_dostring(state, "retainedCategory:product(1, 'item', 1, 'icon'); retainedCategory = nil"), LUA_OK);
    lua_gc(state, LUA_GCCOLLECT, 0);
    lua_gc(state, LUA_GCCOLLECT, 0);
    EXPECT_TRUE(weak.expired());
}

TEST(StoreValidation, ConfiguredValidatorRequiresInterface)
{
    BlackTek::StoreCategory category;
    category.canPurchaseScriptId = 123;
    std::string reason;
    EXPECT_FALSE(category.executeCanPurchase(g_game.MakePlayer(nullptr), 1, reason));
}
