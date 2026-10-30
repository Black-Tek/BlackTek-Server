#include "otpch.h"
#include "combat.h"
#include "events.h"
#include "player.h"
#include "scheduler.h"

#include <gtest/gtest.h>

extern Events* g_events;

namespace BlackTek::World::Tests
{
    class PlayerAccess
    {
    public:
        static uint8_t Percent(uint64_t count, uint64_t required) { return Player::getPercentLevel(count, required); }
        static void SetVocation(Player& player, Vocation& vocation) { player.vocation = &vocation; }
        static void SetSkillTries(Player& player, uint64_t tries) { player.skills[SKILL_FIST].tries = tries; }
        static uint64_t SkillTries(const Player& player) { return player.skills[SKILL_FIST].tries; }
        static void SetManaSpent(Player& player, uint64_t spent) { player.manaSpent = spent; }
        static void SetExperience(Player& player, uint64_t experience) { player.experience = experience; }
        static void RemoveExperience(Player& player, uint64_t experience) { player.removeExperience(experience); }
        static void SetWalkTask(Player& player, SchedulerTask* task) { player.setNextWalkActionTask(task); }
        static void CacheModifier(Player& player, const DamageModifier& modifier) { player.cacheModifier(modifier); }
        static void UncacheModifier(Player& player, const DamageModifier& modifier) { player.uncacheModifier(modifier); }
        static void SetInventoryItem(Player& player, slots_t slot, ItemPtr item) { player.inventory[slot] = std::move(item); }
    };

    TEST(PlayerMoney, OverflowingBalanceIsRejected)
    {
        auto& type = Item::items.getItemType(0);
        struct RestoreType
        {
            ItemType& type;
            uint64_t worth = type.worth;
            bool stackable = type.stackable;
            ~RestoreType() { type.worth = worth; type.stackable = stackable; }
        } restore{type};
        type.worth = UINT64_MAX;
        type.stackable = true;
        auto player = std::make_shared<Player>(nullptr);
        PlayerAccess::SetInventoryItem(*player, CONST_SLOT_LEFT, std::make_shared<Item>(0, 1));
        EXPECT_EQ(player->getMoney(), UINT64_MAX);
        PlayerAccess::SetInventoryItem(*player, CONST_SLOT_RIGHT, std::make_shared<Item>(0, 1));
        EXPECT_EQ(player->getMoney(), 0u);
    }

    TEST(PlayerProgress, PercentageHandlesFullUint64Range)
    {
        const auto maximum = std::numeric_limits<uint64_t>::max();
        EXPECT_EQ(PlayerAccess::Percent(maximum, maximum), 100);
        EXPECT_EQ(PlayerAccess::Percent(maximum - 1, maximum), 99);
        EXPECT_EQ(PlayerAccess::Percent(maximum / 2, maximum), 49);
        EXPECT_EQ(PlayerAccess::Percent(3, 4), 75);
        EXPECT_EQ(PlayerAccess::Percent(1, 3), 33);
        EXPECT_EQ(PlayerAccess::Percent(3, 0), 0);
        EXPECT_EQ(PlayerAccess::Percent(256, 100), 0);
    }

    TEST(PlayerProgress, RemovingSkillTriesPreservesWideRemainder)
    {
        Vocation vocation(0);
        auto player = std::make_shared<Player>(nullptr);
        PlayerAccess::SetVocation(*player, vocation);
        const uint64_t tries = uint64_t{1} << 32;
        PlayerAccess::SetSkillTries(*player, tries);
        player->removeSkillTries(SKILL_FIST, 1);
        EXPECT_EQ(PlayerAccess::SkillTries(*player), tries - 1);
    }

    TEST(PlayerProgress, RemovingManaSpentStopsAtZero)
    {
        Vocation vocation(0);
        auto player = std::make_shared<Player>(nullptr);
        PlayerAccess::SetVocation(*player, vocation);
        PlayerAccess::SetManaSpent(*player, 10);
        player->removeManaSpent(20);
        EXPECT_EQ(player->getSpentMana(), 0u);
        EXPECT_EQ(player->getBaseMagicLevel(), 0u);
    }

    TEST(PlayerProgress, RemovingHugeExperienceStopsAtZero)
    {
        Events events;
        struct RestoreEvents
        {
            Events* previous;
            ~RestoreEvents() { g_events = previous; }
        } restore{g_events};
        g_events = &events;
        auto player = std::make_shared<Player>(nullptr);
        PlayerAccess::SetExperience(*player, 100);
        PlayerAccess::RemoveExperience(*player, std::numeric_limits<uint64_t>::max());
        EXPECT_EQ(player->getExperience(), 0u);
    }

    TEST(PlayerContainers, MissingContainerIndexIsZero)
    {
        const auto player = std::make_shared<Player>(nullptr);
        EXPECT_EQ(player->getContainerIndex(0), 0u);
    }

    TEST(PlayerTasks, PendingWalkTaskReleasesCaptureOnDestruction)
    {
        std::weak_ptr<int> weakToken;
        {
            auto player = std::make_shared<Player>(nullptr);
            auto token = std::make_shared<int>(1);
            weakToken = token;
            PlayerAccess::SetWalkTask(*player, createSchedulerTask(50, [token] {}));
        }
        EXPECT_TRUE(weakToken.expired());
    }

    TEST(PlayerModifiers, InvalidTypeDoesNotIndexCacheArrays)
    {
        auto player = std::make_shared<Player>(nullptr);
        for (const auto stance : {DamageModifier::Stance::Attack, DamageModifier::Stance::Defense})
        {
            DamageModifier modifier(std::to_underlying(stance), 255, 10,
                std::to_underlying(DamageModifier::Factor::Flat), 0);
            PlayerAccess::CacheModifier(*player, modifier);
            PlayerAccess::UncacheModifier(*player, modifier);
        }
        EXPECT_FALSE(player->hasNamedAttackMods());
        EXPECT_FALSE(player->hasNamedDefenseMods());
    }

    TEST(PlayerDeflection, NullAttackerAndOutOfRangeCountsAreSafe)
    {
        const auto player = std::make_shared<Player>(nullptr);
        EXPECT_NE(player->generateDeflectArea(CreaturePtr{}, 1000), nullptr);
        player->setDirection(DIRECTION_NORTHEAST);
        EXPECT_NE(player->generateDeflectArea(std::nullopt, -1), nullptr);
    }
}
