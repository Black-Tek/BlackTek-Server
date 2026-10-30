#include "otpch.h"
#include "augment.h"
#include "fileloader.h"

#include <gtest/gtest.h>

namespace BlackTek::CombatTests
{
    TEST(Augment, RejectsModifierCountsThatOverflowOrExceedRemainingData)
    {
        for (const uint32_t defenseCount : {0u, 1u})
        {
            PropWriteStream output;
            output.writeString("test");
            output.writeString("description");
            output.write<uint32_t>(std::numeric_limits<uint32_t>::max());
            output.write<uint32_t>(defenseCount);
            const auto data = output.getStream();
            PropStream input;
            input.init(data.data(), data.size());
            EXPECT_FALSE(Augment::deserialize(input));
        }
    }

    TEST(Augment, EmptyAugmentRoundTrips)
    {
        const auto augment = Augment::MakeAugment("test", "description");
        PropWriteStream output;
        augment->serialize(output);
        const auto data = output.getStream();
        PropStream input;
        input.init(data.data(), data.size());
        const auto restored = Augment::deserialize(input);
        ASSERT_TRUE(restored);
        EXPECT_EQ((*restored)->getName(), "test");
        EXPECT_EQ((*restored)->getDescription(), "description");
    }
}
