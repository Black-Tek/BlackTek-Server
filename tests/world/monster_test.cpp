#include "otpch.h"
#include "monster.h"

#include <gtest/gtest.h>

namespace BlackTek::World::Tests
{
    TEST(MonsterType, MonsterOwnedSummonAcceptsMonsterCaller)
    {
        MonsterType type;
        const auto owner = std::make_shared<Monster>(&type);
        const auto caller = std::make_shared<Monster>(&type);
        const auto summon = std::make_shared<Monster>(&type);
        ASSERT_TRUE(summon->setMaster(owner));
        EXPECT_EQ(summon->getType(owner), CREATURETYPE_SUMMON_OWN);
        EXPECT_EQ(summon->getType(caller), CREATURETYPE_SUMMON_MONSTER_FRIEND);
        summon->setMaster(nullptr);
    }
}
