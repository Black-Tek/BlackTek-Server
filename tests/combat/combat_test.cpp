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

// ---------------------------------------------------------------------------
// transformDamage
// ---------------------------------------------------------------------------

TEST(CombatTransformDamage, ProducesAHandleCarryingTheRequestedTypeAndAmount)
{
	Combat combat;

	auto handle = combat.transformDamage(static_cast<uint16_t>(Combat::DamageType::Fire), 42u);

	EXPECT_EQ(handle->GetDamageType(), static_cast<uint16_t>(Combat::DamageType::Fire));
	EXPECT_EQ(handle->GetDamage(), 42u);
	EXPECT_TRUE(handle->GetConfig(Combat::Config::AttackModified));
	EXPECT_FALSE(handle->GetConfig(Combat::Config::HealthTarget));
}

TEST(CombatTransformDamage, HealingTypeAlsoSetsHealthTarget)
{
	Combat combat;

	auto handle = combat.transformDamage(static_cast<uint16_t>(Combat::DamageType::Healing), 10u);

	EXPECT_TRUE(handle->GetConfig(Combat::Config::HealthTarget));
}

// ---------------------------------------------------------------------------
// immunity_block_effect
// ---------------------------------------------------------------------------

TEST(CombatImmunityBlockEffect, MapsDamageTypeToExpectedMagicEffect)
{
	Combat physical;
	physical.SetDamageType(Combat::DamageType::Physical);
	EXPECT_EQ(physical.immunity_block_effect(), CONST_ME_BLOCKHIT);

	Combat fire;
	fire.SetDamageType(Combat::DamageType::Fire);
	EXPECT_EQ(fire.immunity_block_effect(), CONST_ME_BLOCKHIT);

	Combat earth;
	earth.SetDamageType(Combat::DamageType::Earth);
	EXPECT_EQ(earth.immunity_block_effect(), CONST_ME_GREEN_RINGS);

	Combat holy;
	holy.SetDamageType(Combat::DamageType::Holy);
	EXPECT_EQ(holy.immunity_block_effect(), CONST_ME_HOLYDAMAGE);

	Combat undefined;
	undefined.SetDamageType(Combat::DamageType::Undefined);
	EXPECT_EQ(undefined.immunity_block_effect(), CONST_ME_NONE);

	Combat healing;
	healing.SetDamageType(Combat::DamageType::Healing);
	EXPECT_EQ(healing.immunity_block_effect(), CONST_ME_NONE);
}

// ---------------------------------------------------------------------------
// collect_heal_notice_data
// ---------------------------------------------------------------------------

TEST(CombatHealNoticeData, NoHealTargetFlagYieldsZero)
{
	Combat combat;
	EXPECT_EQ(combat.collect_heal_notice_data(), 0u);
}

TEST(CombatHealNoticeData, EachHealTargetFlagYieldsADistinctNonZeroValue)
{
	Combat health;
	health.SetConfig(Combat::Config::HealthTarget);
	Combat mana;
	mana.SetConfig(Combat::Config::ManaTarget);
	Combat stamina;
	stamina.SetConfig(Combat::Config::StaminaTarget);
	Combat soul;
	soul.SetConfig(Combat::Config::SoulTarget);

	EXPECT_NE(health.collect_heal_notice_data(), 0u);
	EXPECT_NE(mana.collect_heal_notice_data(), 0u);
	EXPECT_NE(stamina.collect_heal_notice_data(), 0u);
	EXPECT_NE(soul.collect_heal_notice_data(), 0u);

	EXPECT_NE(health.collect_heal_notice_data(), mana.collect_heal_notice_data());
	EXPECT_NE(health.collect_heal_notice_data(), stamina.collect_heal_notice_data());
	EXPECT_NE(health.collect_heal_notice_data(), soul.collect_heal_notice_data());
	EXPECT_NE(mana.collect_heal_notice_data(), stamina.collect_heal_notice_data());
	EXPECT_NE(mana.collect_heal_notice_data(), soul.collect_heal_notice_data());
	EXPECT_NE(stamina.collect_heal_notice_data(), soul.collect_heal_notice_data());
}

// ---------------------------------------------------------------------------
// collect_origin_notice
// ---------------------------------------------------------------------------

TEST(CombatOriginNotice, MapsOriginToExpectedEffectAndVerb)
{
	auto melee = Combat::collect_origin_notice(Combat::Origin::Melee);
	EXPECT_EQ(melee.effect, CONST_ME_NONE);
	EXPECT_EQ(melee.verb, "a melee attack");

	auto reflect = Combat::collect_origin_notice(Combat::Origin::Reflect);
	EXPECT_EQ(reflect.effect, CONST_ME_MAGIC_RED);
	EXPECT_EQ(reflect.verb, "a reflected attack");

	auto deflect = Combat::collect_origin_notice(Combat::Origin::Deflect);
	EXPECT_EQ(deflect.effect, CONST_ME_MAGIC_BLUE);
	EXPECT_EQ(deflect.verb, "a deflected attack");

	auto ricochet = Combat::collect_origin_notice(Combat::Origin::Ricochet);
	EXPECT_EQ(ricochet.effect, CONST_ME_MAGIC_BLUE);
	EXPECT_EQ(ricochet.verb, "a ricocheting attack");
}

// ---------------------------------------------------------------------------
// Config flags: every set*/is* wrapper pair round-trips through the
// underlying bitset exactly like GetConfig/SetConfig do directly.
// ---------------------------------------------------------------------------

TEST(CombatConfigFlags, EveryNamedWrapperRoundTripsItsOwnBit)
{
	Combat combat;

	EXPECT_FALSE(combat.isTrueDamage());
	combat.setTrueDamage(true);
	EXPECT_TRUE(combat.isTrueDamage());
	combat.setTrueDamage(false);
	EXPECT_FALSE(combat.isTrueDamage());

	EXPECT_FALSE(combat.isBlockedByArmor());
	combat.setBlockedByArmor(true);
	EXPECT_TRUE(combat.isBlockedByArmor());

	EXPECT_FALSE(combat.isBlockedByShield());
	combat.setBlockedByShield(true);
	EXPECT_TRUE(combat.isBlockedByShield());

	EXPECT_FALSE(combat.isTopTargetOnly());
	combat.setTopTargetOnly(true);
	EXPECT_TRUE(combat.isTopTargetOnly());

	EXPECT_FALSE(combat.isSelfOnly());
	combat.setSelfOnly(true);
	EXPECT_TRUE(combat.isSelfOnly());

	EXPECT_FALSE(combat.isFriendlyParty());
	combat.setFriendlyParty(true);
	EXPECT_TRUE(combat.isFriendlyParty());

	EXPECT_FALSE(combat.isEnemyParty());
	combat.setEnemyParty(true);
	EXPECT_TRUE(combat.isEnemyParty());

	EXPECT_FALSE(combat.isFraggedOnly());
	combat.setFraggedOnly(true);
	EXPECT_TRUE(combat.isFraggedOnly());

	EXPECT_FALSE(combat.isAggressive());
	combat.setAggressive(true);
	EXPECT_TRUE(combat.isAggressive());

	EXPECT_FALSE(combat.isUtility());
	combat.setIsUtility(true);
	EXPECT_TRUE(combat.isUtility());

	EXPECT_FALSE(combat.isIgnoreBarriers());
	combat.setIgnoreBarriers(true);
	EXPECT_TRUE(combat.isIgnoreBarriers());

	EXPECT_FALSE(combat.usesCharges());
	combat.setUseCharges(true);
	EXPECT_TRUE(combat.usesCharges());

	// None of the wrappers above should have touched an unrelated flag.
	EXPECT_FALSE(combat.GetConfig(Combat::Config::Critical));
}

// ---------------------------------------------------------------------------
// Trivial scalar getter/setter round trips
// ---------------------------------------------------------------------------

TEST(CombatScalarAccessors, EachSetterRoundTripsItsGetter)
{
	Combat combat;

	combat.SetDamageType(Combat::DamageType::Ice);
	EXPECT_EQ(combat.GetDamageType(), static_cast<uint16_t>(Combat::DamageType::Ice));

	combat.SetDamage(1337u);
	EXPECT_EQ(combat.GetDamage(), 1337u);

	combat.SetImpactEffect(CONST_ME_EXPLOSIONAREA);
	EXPECT_EQ(combat.GetImpactEffect(), CONST_ME_EXPLOSIONAREA);

	combat.SetDistanceEffect(CONST_ANI_FIRE);
	EXPECT_EQ(combat.GetDistanceEffect(), CONST_ANI_FIRE);

	combat.SetBlockType(Combat::BlockType::Armor);
	EXPECT_EQ(combat.GetBlockType(), static_cast<uint8_t>(Combat::BlockType::Armor));

	combat.SetItemId(3031u);
	EXPECT_EQ(combat.GetItemId(), 3031u);

	combat.set_id(777);
	EXPECT_EQ(combat.id(), 777);
}

// ---------------------------------------------------------------------------
// AreaCombat
// ---------------------------------------------------------------------------

TEST(AreaCombatTest, DefaultConstructedAreaHasNoShapeUntilSetup)
{
	BlackTek::AreaCombat area;
	auto combatArea = area.GetCombatArea();

	for (size_t dir = 0; dir < 4; ++dir)
		EXPECT_EQ(combatArea.count(dir), 0u);
	EXPECT_EQ(combatArea.max_extent, 0);
}

TEST(AreaCombatTest, RadiusOneProducesANonEmptySymmetricShape)
{
	BlackTek::AreaCombat area;
	area.setupArea(/*radius=*/1);
	auto combatArea = area.GetCombatArea();

	// A radius-1 area always covers at least the center tile in every
	// direction's position list, and every direction is generated from the
	// same base shape, so they should all report the same tile count.
	const size_t northCount = combatArea.count(DIRECTION_NORTH);
	EXPECT_GT(northCount, 0u);
	EXPECT_EQ(combatArea.count(DIRECTION_EAST), northCount);
	EXPECT_EQ(combatArea.count(DIRECTION_SOUTH), northCount);
	EXPECT_EQ(combatArea.count(DIRECTION_WEST), northCount);
	EXPECT_GT(combatArea.max_extent, 0);
}

TEST(CombatArea, HasAreaIsFalseUntilAnAreaIsSet)
{
	auto handle = BlackTek::g_combat_registry.Create();
	EXPECT_FALSE(handle->hasArea());

	auto area = std::make_unique<BlackTek::AreaCombat>();
	area->setupArea(/*radius=*/1);
	handle->setArea(std::move(area));

	EXPECT_TRUE(handle->hasArea());
	EXPECT_TRUE(handle->GetConfig(Combat::Config::HasArea));
}
