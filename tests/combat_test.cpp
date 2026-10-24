// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

// Unit tests for the pure combat math and damage-modifier helpers in
// src/combat.h / src/combat.cpp. These exercise Combat's static formula
// evaluators and a handful of stateless-ish member helpers directly,
// without needing a running game world, map, or connected players.

#include "otpch.h"

#include "combat.h"

#include <gtest/gtest.h>

using BlackTek::Combat;

namespace
{
	// normal_random()/uniform_random() return the bound itself when
	// min == max (see tools.cpp), which lets the "random" formulas be
	// asserted exactly instead of only range-checked.
	constexpr int32_t kRepeat = 200;
}

// ---------------------------------------------------------------------------
// calculate_output
// ---------------------------------------------------------------------------

TEST(CombatOutputFormula, FlatReturnsWithinFixedRange)
{
	Combat::OutputFactors factors{};
	factors.formula_type = Combat::OutputFormula::Flat;
	factors.min_value = 10.0f;
	factors.max_value = 20.0f;

	for (int32_t i = 0; i < kRepeat; ++i)
	{
		const int32_t result = Combat::calculate_output(factors, /*stat=*/0);
		EXPECT_GE(result, 10);
		EXPECT_LE(result, 20);
	}
}

TEST(CombatOutputFormula, LinearScalesStatAndAddsBase)
{
	Combat::OutputFactors factors{};
	factors.formula_type = Combat::OutputFormula::Linear;
	factors.scaling = 2.0f;
	factors.base = 5.0f;

	EXPECT_EQ(Combat::calculate_output(factors, 10), 25); // 10*2 + 5
	EXPECT_EQ(Combat::calculate_output(factors, 0), 5);
}

TEST(CombatOutputFormula, LinearClampsNegativeResultToZero)
{
	Combat::OutputFactors factors{};
	factors.formula_type = Combat::OutputFormula::Linear;
	factors.scaling = 1.0f;
	factors.base = -100.0f;

	EXPECT_EQ(Combat::calculate_output(factors, 10), 0);
}

TEST(CombatOutputFormula, LinearRangeIsDeterministicWhenBoundsMatch)
{
	Combat::OutputFactors factors{};
	factors.formula_type = Combat::OutputFormula::LinearRange;
	factors.min_scale = 1.0f;
	factors.max_scale = 1.0f;
	factors.min_base = 3.0f;
	factors.max_base = 3.0f;

	EXPECT_EQ(Combat::calculate_output(factors, 7), 10); // 7*1+3 for both bounds
}

TEST(CombatOutputFormula, LinearRangeStaysWithinComputedBounds)
{
	Combat::OutputFactors factors{};
	factors.formula_type = Combat::OutputFormula::LinearRange;
	factors.min_scale = 0.0f;
	factors.max_scale = 1.0f;

	for (int32_t i = 0; i < kRepeat; ++i)
	{
		const int32_t result = Combat::calculate_output(factors, 100);
		EXPECT_GE(result, 0);
		EXPECT_LE(result, 100);
	}
}

TEST(CombatOutputFormula, PowerAppliesExponentBeforeScalingAndBase)
{
	Combat::OutputFactors factors{};
	factors.formula_type = Combat::OutputFormula::Power;
	factors.exponent = 2.0f;
	factors.scaling = 1.0f;
	factors.base = 0.0f;

	EXPECT_EQ(Combat::calculate_output(factors, 4), 16); // 4^2
}

// ---------------------------------------------------------------------------
// calculate_resistance
// ---------------------------------------------------------------------------

TEST(CombatResistanceFormula, NonPositiveStatAlwaysResolvesToZero)
{
	Combat::ResistanceFactors factors{};
	factors.formula_type = Combat::ResistanceFormula::Identity;

	EXPECT_EQ(Combat::calculate_resistance(factors, 0), 0);
	EXPECT_EQ(Combat::calculate_resistance(factors, -5), 0);
}

TEST(CombatResistanceFormula, IdentityReturnsStatUnchanged)
{
	Combat::ResistanceFactors factors{};
	factors.formula_type = Combat::ResistanceFormula::Identity;

	EXPECT_EQ(Combat::calculate_resistance(factors, 42), 42);
}

TEST(CombatResistanceFormula, ParityAboveThresholdStaysBelowStat)
{
	Combat::ResistanceFactors factors{};
	factors.formula_type = Combat::ResistanceFormula::Parity;
	factors.threshold = 3.0f;
	factors.min_scale = 0.5f;
	factors.parity_offset = 1;

	for (int32_t i = 0; i < kRepeat; ++i)
	{
		const int32_t result = Combat::calculate_resistance(factors, 20);
		EXPECT_GE(result, 10);
		EXPECT_LE(result, 19); // stat - (stat%2 + parity_offset) == 20 - (0+1) == 19
	}
}

TEST(CombatResistanceFormula, ParityAtOrBelowThresholdReturnsFlatAmount)
{
	Combat::ResistanceFactors factors{};
	factors.formula_type = Combat::ResistanceFormula::Parity;
	factors.threshold = 10.0f;
	factors.flat_amount = 2.0f;

	EXPECT_EQ(Combat::calculate_resistance(factors, 5), 2);
	EXPECT_EQ(Combat::calculate_resistance(factors, 10), 2); // stat must exceed, not equal
}

TEST(CombatResistanceFormula, PercentMatchesClassicArmorFormula)
{
	Combat::ResistanceFactors factors{};
	factors.formula_type = Combat::ResistanceFormula::Percent;
	factors.constant = 100.0f;

	// (stat*100)/(stat+constant), truncated towards zero via integer division
	EXPECT_EQ(Combat::calculate_resistance(factors, 100), 50);
	EXPECT_EQ(Combat::calculate_resistance(factors, 1), 0);
}

// ---------------------------------------------------------------------------
// calculate_resolution
// ---------------------------------------------------------------------------

TEST(CombatResolutionFormula, SubtractiveFloorsAtConfiguredMinimum)
{
	Combat::ResolutionFactors factors{};
	factors.formula_type = Combat::ResolutionFormula::Subtractive;
	factors.floor = 0.0f;

	EXPECT_EQ(Combat::calculate_resolution(factors, 100, 40), 60);
	EXPECT_EQ(Combat::calculate_resolution(factors, 100, 500), 0); // would go negative, clamped to floor
}

TEST(CombatResolutionFormula, RatioMitigationHalvesDamageWhenResistanceEqualsConstant)
{
	Combat::ResolutionFactors factors{};
	factors.formula_type = Combat::ResolutionFormula::RatioMitigation;
	factors.constant = 100.0f;

	EXPECT_EQ(Combat::calculate_resolution(factors, 200, 100), 100);
}

TEST(CombatResolutionFormula, ScaledDivisionFallsBackToMultiplierWhenResistanceNonPositive)
{
	Combat::ResolutionFactors factors{};
	factors.formula_type = Combat::ResolutionFormula::ScaledDivision;
	factors.multiplier = 2.0f;
	factors.addend = 3.0f;

	EXPECT_EQ(Combat::calculate_resolution(factors, 10, 0), 23);  // 10*2 + 3
	EXPECT_EQ(Combat::calculate_resolution(factors, 10, -5), 23); // negative resistance treated the same
}

TEST(CombatResolutionFormula, ScaledDivisionDividesByPositiveResistance)
{
	Combat::ResolutionFactors factors{};
	factors.formula_type = Combat::ResolutionFormula::ScaledDivision;
	factors.multiplier = 1.0f;
	factors.addend = 0.0f;

	EXPECT_EQ(Combat::calculate_resolution(factors, 100, 2), 50);
}

TEST(CombatResolutionFormula, LayeredAppliesPercentMitigationThenFlatReduction)
{
	Combat::ResolutionFactors factors{};
	factors.formula_type = Combat::ResolutionFormula::Layered;
	factors.flat_reduction = 5.0f;
	factors.floor = 0.0f;

	// output*(1 - resistance/100) - flat_reduction == 100*(1-0.5) - 5 == 45
	EXPECT_EQ(Combat::calculate_resolution(factors, 100, 50), 45);
}

// ---------------------------------------------------------------------------
// DamageToConditionType
// ---------------------------------------------------------------------------

TEST(CombatDamageToConditionType, MapsEachKnownDamageTypeToItsCondition)
{
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_FIREDAMAGE), CONDITION_FIRE);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_ENERGYDAMAGE), CONDITION_ENERGY);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_EARTHDAMAGE), CONDITION_POISON);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_DROWNDAMAGE), CONDITION_DROWN);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_ICEDAMAGE), CONDITION_FREEZING);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_HOLYDAMAGE), CONDITION_DAZZLED);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_DEATHDAMAGE), CONDITION_CURSED);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_PHYSICALDAMAGE), CONDITION_BLEEDING);
}

TEST(CombatDamageToConditionType, UnmappedDamageTypeReturnsNone)
{
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_HEALING), CONDITION_NONE);
	EXPECT_EQ(Combat::DamageToConditionType(COMBAT_NONE), CONDITION_NONE);
}

// ---------------------------------------------------------------------------
// HasLeechEffect
// ---------------------------------------------------------------------------

TEST(CombatLeechEffect, AllZeroedFieldsHaveNoLeechEffect)
{
	EXPECT_FALSE(Combat::HasLeechEffect(BlackTek::LeechData{}));
}

TEST(CombatLeechEffect, AnyNonZeroFieldCountsAsLeechEffect)
{
	BlackTek::LeechData percentHealth{};
	percentHealth.percent_health = 10;
	EXPECT_TRUE(Combat::HasLeechEffect(percentHealth));

	BlackTek::LeechData flatSoul{};
	flatSoul.flat_soul = 1;
	EXPECT_TRUE(Combat::HasLeechEffect(flatSoul));
}

// ---------------------------------------------------------------------------
// applyCrit / penetrateDamage (member helpers operating on a bare Combat)
// ---------------------------------------------------------------------------

TEST(CombatDamageModifiers, ApplyCritAddsPercentThenFlatBonus)
{
	Combat combat;

	const uint32_t result = combat.applyCrit(/*currentDamage=*/100, /*percent=*/50, /*flat=*/10);

	EXPECT_EQ(result, 160u); // 100 + 100*50/100 + 10
	EXPECT_TRUE(combat.GetConfig(Combat::Config::Critical));
}

TEST(CombatDamageModifiers, ApplyCritWithNoBonusLeavesDamageUnchangedAndFlagUnset)
{
	Combat combat;

	const uint32_t result = combat.applyCrit(/*currentDamage=*/100, /*percent=*/0, /*flat=*/0);

	EXPECT_EQ(result, 100u);
	EXPECT_FALSE(combat.GetConfig(Combat::Config::Critical));
}

TEST(CombatDamageModifiers, PenetrateDamageSplitsOffTrueDamagePortion)
{
	Combat combat;

	auto [pierceHandle, remaining] = combat.penetrateDamage(/*currentDamage=*/100, /*percent=*/20, /*flat=*/5);

	EXPECT_EQ(remaining, 75u);              // 100 - (20% of 100 + 5) == 100 - 25
	EXPECT_EQ(pierceHandle->GetDamage(), 25u);
	EXPECT_TRUE(pierceHandle->isTrueDamage());
}

TEST(CombatDamageModifiers, PenetrateDamageNeverExceedsCurrentDamage)
{
	Combat combat;

	auto [pierceHandle, remaining] = combat.penetrateDamage(/*currentDamage=*/10, /*percent=*/200, /*flat=*/0);

	EXPECT_EQ(remaining, 0u);
	EXPECT_EQ(pierceHandle->GetDamage(), 10u); // clamped to currentDamage, never goes negative
}
