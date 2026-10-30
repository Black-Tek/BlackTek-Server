#include "otpch.h"
#include "matrixarea.h"

#include <gtest/gtest.h>

namespace BlackTek::CombatTests
{
    TEST(MatrixArea, RejectsIncompleteRows)
    {
        const auto area = CreateArea({3, 1, 1, 1, 1}, 2);
        EXPECT_EQ(area.GetRows(), 0u);
        EXPECT_EQ(area.GetCols(), 0u);
    }

    TEST(MatrixArea, RectangularAreaRetainsCellsAndCenter)
    {
        const auto area = CreateArea({0, 3, 1, 1, 0, 1}, 2);
        EXPECT_EQ(area.GetRows(), 2u);
        EXPECT_EQ(area.GetCols(), 3u);
        EXPECT_TRUE(area(0, 1));
        EXPECT_TRUE(area(1, 2));
        EXPECT_FALSE(area(1, 1));
        EXPECT_EQ(area.GetCenter(), (std::pair<uint32_t, uint32_t>{1, 0}));
    }
}
