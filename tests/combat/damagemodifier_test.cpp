#include "otpch.h"
#include "damagemodifier.h"

#include <gtest/gtest.h>

namespace BlackTek::CombatTests
{
    TEST(DamageModifier, NameSetterActivatesFilterAndPreservesLongNames)
    {
        DamageModifier modifier;
        const std::string name = "a creature name longer than the serialized name buffer";
        modifier.setCreatureName(name);
        EXPECT_NE(modifier.getFilterIndex() & DamageModifier::Flag::Named, 0);
        EXPECT_TRUE(modifier.appliesByName(name));
        EXPECT_FALSE(modifier.appliesByName("other creature"));
        modifier.setCreatureName(modifier.getMonsterName());
        EXPECT_TRUE(modifier.appliesByName(name));
        const auto copy = modifier;
        EXPECT_TRUE(copy.appliesByName(name));
    }

    TEST(DamageModifier, DeserializesUnalignedInputAndBoundsWireName)
    {
        DamageModifier modifier;
        modifier.setCreatureName("dragon");
        modifier.value = 25;
        std::array<std::byte, sizeof(DamageModifier) + 1> buffer;
        std::memcpy(buffer.data() + 1, modifier.serialize().data(), sizeof(DamageModifier));
        auto restored = DamageModifier::deserialize({buffer.data() + 1, sizeof(DamageModifier)});
        EXPECT_EQ(restored.getValue(), 25u);
        EXPECT_TRUE(restored.appliesByName("dragon"));

        std::fill_n(buffer.data() + 1 + offsetof(DamageModifier, name_buf), sizeof(modifier.name_buf), std::byte{'x'});
        restored = DamageModifier::deserialize({buffer.data() + 1, sizeof(DamageModifier)});
        EXPECT_EQ(restored.getMonsterName(), std::string(31, 'x'));
    }

    TEST(DamageModifier, TruncatedInputReturnsEmptyModifier)
    {
        const std::array<std::byte, 1> buffer{};
        const auto modifier = DamageModifier::deserialize(buffer);
        EXPECT_EQ(modifier.getValue(), 0u);
        EXPECT_EQ(modifier.getFilterIndex(), DamageModifier::Flag::InvalidFlag);
    }
}
