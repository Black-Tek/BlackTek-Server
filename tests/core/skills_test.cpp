#include "otpch.h"
#include "skills.h"
#include "vocation.h"

#include <gtest/gtest.h>

using Components::Skills::CustomSkill;

TEST(CustomSkillBounds, AddingLevelsSaturatesWithoutWrapping)
{
    CustomSkill skill(65530);
    skill.addLevels(10);
    EXPECT_EQ(skill.level(false), 65535);
}

TEST(CustomSkillBounds, RemovingTooManyLevelsStopsAtOne)
{
    CustomSkill skill(10);
    skill.removeLevels(20);
    EXPECT_EQ(skill.level(false), 1);
}

TEST(CustomSkillBounds, RemovingPartialPreviousLevelDecreasesLevel)
{
    CustomSkill skill(3, FormulaType::LINEAR, 0, 1, 0, 10);
    skill.removePoints(5);
    EXPECT_EQ(skill.level(false), 2);
    EXPECT_EQ(skill.points(), 25);
}

TEST(CustomSkillBounds, PointGainStopsAtRepresentableLevel)
{
    CustomSkill skill(65535, FormulaType::LINEAR, 0, 1, 0, 1);
    skill.addPoints(65536);
    EXPECT_EQ(skill.level(false), 65535);
    EXPECT_EQ(skill.points(), 0);
}

TEST(CustomSkillBounds, ProgressUsesNextLevelRequirement)
{
    CustomSkill skill(2, FormulaType::LINEAR, 0, 1, 0, 10);
    skill.addPoints(15);
    EXPECT_EQ(skill.percent<unsigned>(), 50);
    skill.addLevels(1, true);
    EXPECT_EQ(skill.points(), 20);
    EXPECT_EQ(skill.percent<unsigned>(), 50);
    skill.removeLevels(1, true);
    EXPECT_EQ(skill.points(), 15);
}

TEST(CustomSkillBounds, NonfiniteFormulaDoesNotConvertNanToInteger)
{
    CustomSkill skill(1, FormulaType::LINEAR, 0, std::numeric_limits<float>::quiet_NaN(), 1, 1);
    EXPECT_EQ(skill.needed(), std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(CustomSkill::make_skill(std::shared_ptr<CustomSkill>{}), nullptr);
}

TEST(VocationRequirements, NormalValuesAndOverflowAreWellDefined)
{
    Vocation vocation(1);
    EXPECT_EQ(vocation.getReqMana(1), 1600);
    EXPECT_EQ(vocation.getReqMana(2), 6400);
    EXPECT_EQ(vocation.getReqMana(std::numeric_limits<uint32_t>::max()), std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(vocation.getReqSkillTries(SKILL_SWORD, 11), 50);
    EXPECT_EQ(vocation.getReqSkillTries(SKILL_SWORD, 65535), std::numeric_limits<uint64_t>::max());
}
