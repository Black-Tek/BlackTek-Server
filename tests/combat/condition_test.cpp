#include "otpch.h"

#include "condition.h"
#include "../world/test_creature.h"

#include <gtest/gtest.h>

TEST(ConditionAttributes, RejectsExcessSerializedArrayEntries)
{
    const std::array entries =
    {
        std::pair{CONDITIONATTR_SKILLS, SKILL_LAST + 1},
        std::pair{CONDITIONATTR_SPECIALSKILLS, SPECIALSKILL_LAST + 1},
        std::pair{CONDITIONATTR_STATS, STAT_LAST + 1}
    };
    for (const auto& [attribute, count] : entries)
    {
        ConditionAttributes condition(CONDITIONID_DEFAULT, CONDITION_ATTRIBUTES, 1000);
        const int32_t value = 42;
        for (int32_t index = 0; index < count; ++index)
        {
            PropStream stream;
            stream.init(reinterpret_cast<const char*>(&value), sizeof(value));
            ASSERT_TRUE(condition.unserializeProp(attribute, stream));
        }
        PropStream stream;
        stream.init(reinterpret_cast<const char*>(&value), sizeof(value));
        EXPECT_FALSE(condition.unserializeProp(attribute, stream));
        EXPECT_EQ(stream.size(), sizeof(value));
    }
}

TEST(ConditionAttributes, NormalizesSerializedBoolean)
{
    ConditionAttributes condition(CONDITIONID_DEFAULT, CONDITION_ATTRIBUTES, 1000);
    const char value = 2;
    PropStream stream;
    stream.init(&value, sizeof(value));
    ASSERT_TRUE(condition.unserializeProp(CONDITIONATTR_DISABLEDEFENSE, stream));
    EXPECT_EQ(condition.getParam(CONDITION_PARAM_DISABLE_DEFENSE), 1);
}

TEST(ConditionLight, SupportsZeroLevelOnStartAndRefresh)
{
    auto creature = std::make_shared<BlackTek::World::Tests::TestCreature>();
    auto condition = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_LIGHT, 1000);
    ASSERT_TRUE(condition->startCondition(creature));
    EXPECT_EQ(creature->getCreatureLight().level, 0);

    auto replacement = Condition::createCondition(CONDITIONID_DEFAULT, CONDITION_LIGHT, 2000);
    condition->addCondition(creature, replacement.get());
    EXPECT_EQ(creature->getCreatureLight().level, 0);
    condition->endCondition(creature);
}
