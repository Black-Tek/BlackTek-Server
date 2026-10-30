#include "otpch.h"
#include "pointbasedstat.h"

#include <gtest/gtest.h>

namespace BlackTek::World::Tests
{
    using Components::Stats::PointStat;
    using Components::Stats::StatModifierType;

    TEST(PointStat, AddsOnlyRequestedPoints)
    {
        PointStat<uint32_t> stat(40, 100);
        ASSERT_TRUE(stat.add(10));
        EXPECT_EQ(stat.current(), 50u);
        ASSERT_TRUE(stat.add(0));
        EXPECT_EQ(stat.current(), 50u);
        ASSERT_TRUE(stat.add(60));
        EXPECT_EQ(stat.current(), 100u);
    }

    TEST(PointStat, MaximumDecreaseCannotRefillOrReachZero)
    {
        PointStat<uint32_t> stat(10, 100);
        ASSERT_TRUE(stat.decreaseMax(50));
        EXPECT_EQ(stat.current(), 0u);
        EXPECT_EQ(stat.max(), 50u);
        EXPECT_FALSE(stat.decreaseMax(50));
        EXPECT_EQ(stat.max(), 50u);
    }

    TEST(PointStat, ModifierScalingUsesOriginalFillRatio)
    {
        PointStat<uint32_t> stat(80, 100);
        stat.addModifier(StatModifierType::Divide, 2);
        EXPECT_EQ(stat.max(), 50u);
        EXPECT_EQ(stat.current(), 40u);
        ASSERT_TRUE(stat.removeModifier(StatModifierType::Divide, 2));
        EXPECT_EQ(stat.max(), 100u);
        EXPECT_EQ(stat.current(), 80u);
    }
}
