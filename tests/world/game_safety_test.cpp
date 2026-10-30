#include "otpch.h"
#include "game.h"
#include "groups.h"
#include "item.h"
#include "itemcontainer.h"
#include "player.h"
#include "protocolgame.h"

#include <gtest/gtest.h>

extern Game g_game;

namespace BlackTek::World::Tests
{
    TEST(GameItems, ContainerSearchHonorsDirectAndRecursiveDepth)
    {
        auto outer = std::make_shared<Item>(0);
        outer->attachContainer(10, true, false);
        auto inner = std::make_shared<Item>(0);
        inner->attachContainer(10, true, false);
        auto target = std::make_shared<Item>(1);
        inner->getContainer()->addItem(target);
        outer->getContainer()->addItem(inner);
        EXPECT_EQ(g_game.findItemOfType({.containerItem = outer}, 0, false), inner);
        EXPECT_EQ(g_game.findItemOfType({.containerItem = outer}, 1, false), nullptr);
        EXPECT_EQ(g_game.findItemOfType({.containerItem = outer}, 1, true), target);
    }

    TEST(GameItems, InvalidMoveAndAddLocationsFailSafely)
    {
        auto item = std::make_shared<Item>(0);
        auto nonContainer = std::make_shared<Item>(0);
        auto player = std::make_shared<Player>(nullptr);
        const ItemLocation invalid{.containerItem = nonContainer};
        EXPECT_EQ(g_game.internalMoveItem({}, {.player = player}, INDEX_ANYWHERE, item, 1, std::nullopt), RETURNVALUE_NOTPOSSIBLE);
        EXPECT_EQ(g_game.internalMoveItem({.player = player}, {}, INDEX_ANYWHERE, item, 1, std::nullopt), RETURNVALUE_NOTPOSSIBLE);
        EXPECT_EQ(g_game.internalMoveItem({.player = player}, invalid, INDEX_ANYWHERE, item, 1, std::nullopt), RETURNVALUE_NOTPOSSIBLE);
        EXPECT_EQ(g_game.internalAddItem(invalid, item), RETURNVALUE_NOTPOSSIBLE);
        EXPECT_EQ(g_game.internalRemoveItem(nullptr), RETURNVALUE_NOTPOSSIBLE);
    }

    TEST(GameItems, MoneyCanBeRemovedFromContainerLocation)
    {
        auto& type = Item::items.getItemType(0);
        struct RestoreType
        {
            ItemType& type;
            uint64_t worth = type.worth;
            bool stackable = type.stackable;
            ~RestoreType() { type.worth = worth; type.stackable = stackable; }
        } restore{type};
        type.worth = 1;
        type.stackable = true;
        auto bag = std::make_shared<Item>(0);
        bag->attachContainer(10, true, false);
        auto money = std::make_shared<Item>(0, 10);
        bag->getContainer()->addItem(money);
        EXPECT_TRUE(g_game.removeMoney({.containerItem = bag}, 10));
        EXPECT_TRUE(bag->getContainer()->empty());
    }

    TEST(GameDecay, ExpirationPreservesTimeBeyondUint32Boundary)
    {
        const uint64_t now = uint64_t{UINT32_MAX} - 100;
        Expirable entry(nullptr, now + 1000, now);
        EXPECT_EQ(entry.getExpiration(), now + 1000);
        EXPECT_GT(entry.getExpiration(), now);
    }

    TEST(GameCurrency, WideWorthRoundTripsWithoutTruncation)
    {
        auto& type = Item::items.getItemType(0);
        struct RestoreCurrency
        {
            ItemType& type;
            uint16_t id = type.getID();
            uint64_t worth = type.worth;
            bool stackable = type.stackable;
            bool pickupable = type.pickupable;
            Items::CurrencyMap currencies = Item::items.currencyItems;
            ~RestoreCurrency()
            {
                type.setID(id); type.worth = worth; type.stackable = stackable; type.pickupable = pickupable;
                Item::items.currencyItems = std::move(currencies);
            }
        } restore{type};
        const uint64_t worth = uint64_t{UINT32_MAX} + 17;
        type.setID(1); type.worth = worth; type.stackable = true; type.pickupable = true;
        Item::items.currencyItems = {{worth, 0}};
        auto bag = std::make_shared<Item>(0);
        bag->attachContainer(10, true, false);
        const ItemLocation location{.containerItem = bag};
        g_game.addMoney(location, 3 * worth);
        ASSERT_EQ(bag->getContainer()->size(), 1u);
        EXPECT_EQ(bag->getContainer()->getItemByIndex(0)->getWorth(), 3 * worth);
        EXPECT_TRUE(g_game.removeMoney(location, 2 * worth));
        ASSERT_EQ(bag->getContainer()->size(), 1u);
        EXPECT_EQ(bag->getContainer()->getItemByIndex(0)->getWorth(), worth);
        EXPECT_TRUE(g_game.removeMoney(location, worth));
        EXPECT_TRUE(bag->getContainer()->empty());

        type.worth = UINT64_MAX;
        auto overflowingStack = std::make_shared<Item>(0, 2);
        EXPECT_EQ(overflowingStack->getWorth(), 0u);
        bag->getContainer()->addItem(overflowingStack);
        EXPECT_FALSE(g_game.removeMoney(location, 1));
        EXPECT_EQ(overflowingStack->getItemCount(), 2u);

    }

    TEST(GameDecay, ExpiredItemsInsideBagsAreRemoved)
    {
        Group group{};
        auto player = std::make_shared<Player>(nullptr);
        player->setGroup(&group);
        auto bag = std::make_shared<Item>(0);
        bag->attachContainer(10, true, false);
        bag->setInventoryOwner(player);
        auto item = std::make_shared<Item>(0);
        item->setIntAttr(ITEM_ATTRIBUTE_DECAYTO, 0);
        item->setDuration(1);
        bag->getContainer()->addItem(item);
        g_game.addEquippedItemDecay(Expirable(item, 0));
        item->setDecaying(DECAYING_TRUE);
        g_game.equipment_decay_cycle();
        g_timer_queue.clear();
        EXPECT_TRUE(bag->getContainer()->empty());
    }

    TEST(GameDecay, ReplacedEquipmentTimerDoesNotExpireNewSchedule)
    {
        auto player = std::make_shared<Player>(nullptr);
        auto item = std::make_shared<Item>(0);
        item->setInventoryOwner(player);
        item->setDecaying(DECAYING_TRUE);
        g_game.addEquippedItemDecay(Expirable(item, 0, 1));
        const auto future = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()) + 60000;
        g_game.addEquippedItemDecay(Expirable(item, future, 2));
        g_game.equipment_decay_cycle();
        g_timer_queue.clear();
        const auto found = g_game.decaying_eq.find(Expirable(item, 0));
        ASSERT_NE(found, g_game.decaying_eq.end());
        EXPECT_EQ(found->expiration, future);
        g_game.decaying_eq.erase(found);
    }

    TEST(GameTrade, AcceptWithoutCounterOfferDoesNotAdvanceState)
    {
        Game game;
        auto first = std::make_shared<Player>(nullptr);
        auto second = std::make_shared<Player>(nullptr);
        first->setName("First Trader"); second->setName("Second Trader");
        first->setID(); second->setID();
        game.addPlayer(first); game.addPlayer(second);
        auto offer = std::make_shared<Item>(0);
        ASSERT_TRUE(game.internalStartTrade(first, second, offer));
        game.playerAcceptTrade(first->getID());
        game.playerAcceptTrade(second->getID());
        EXPECT_EQ(first->getTradeState(), TRADE_INITIATED);
        EXPECT_EQ(second->getTradeState(), TRADE_ACKNOWLEDGE);
        game.internalCloseTrade(first, false);
    }

    TEST(AccountManagerInput, InvalidOptionAndMissingTownAreNotDereferenced)
    {
        Game game;
        EXPECT_EQ(game.CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_TOWN, UINT32_MAX).id,
            AccountManager::PRIVATE_CHARACTER_VOCATION);
        CharacterOption option{};
        option.town_list = {123456};
        game.addCharacterOption(option);
        EXPECT_TRUE(game.CreatePrivateAccountManagerWindow(AccountManager::PRIVATE_CHARACTER_TOWN, 0).choices.empty());
    }

    TEST(AccountManagerInput, ForgedTownSelectionPreservesState)
    {
        Game game;
        CharacterOption option{};
        option.town_list = {1};
        game.addCharacterOption(option);
        auto client = std::make_shared<ProtocolGame>(nullptr);
        auto player = std::make_shared<Player>(client);
        player->setTempCharacterChoice(0);
        player->setTempTownId(7);
        game.onPrivateAccountManagerInput(player, AccountManager::PRIVATE_CHARACTER_TOWN, ButtonID::PRIMARY, 2);
        EXPECT_EQ(player->getTempTownId(), 7u);
        game.onPrivateAccountManagerInput(player, AccountManager::PRIVATE_CHARACTER_TOWN, ButtonID::PRIMARY, 1);
        EXPECT_EQ(player->getTempTownId(), 7u);
    }

    TEST(AccountManagerInput, EmptyConfirmationAndCharacterNamesAreRejected)
    {
        Game game;
        auto client = std::make_shared<ProtocolGame>(nullptr);
        auto player = std::make_shared<Player>(client);
        player->setName("Account Input Test"); player->setID();
        game.addPlayer(player);
        game.onAccountManagerRecieveText(player->getID(), AccountManager::CONFIRMATION_TEXT_BOX, "");
        EXPECT_TRUE(player->hasModalWindowOpen(AccountManager::COMMON_CONFIRMATION_FAILED));
        game.onPrivateAccountManagerRecieveText(player->getID(), AccountManager::CONFIRMATION_TEXT_BOX, "");
        EXPECT_TRUE(player->hasModalWindowOpen(AccountManager::PRIVATE_PASSWORD_MISMATCH));
        game.onPrivateAccountManagerRecieveText(player->getID(), AccountManager::PASSWORD_RESET_CONFIRM, "");
        EXPECT_TRUE(player->hasModalWindowOpen(AccountManager::PRIVATE_PASSWORD_RESET_MISMATCH));
        game.onPrivateAccountManagerRecieveText(player->getID(), AccountManager::CHARACTER_NAME_TEXT_BOX, "");
        EXPECT_TRUE(player->hasModalWindowOpen(AccountManager::PRIVATE_CHARACTER_FAILED));
    }
}
