// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

// Targeted tests for the SIMD-accelerated combat math in src/simd_dispatch.h
// and src/simd_kernels.h. Combat::block_batch (src/combat.cpp) is the one
// production caller of these kernels today — it's the vectorized, multi-target
// counterpart to the per-target Combat::calculate_resistance/calculate_resolution
// formulas used everywhere else in combat (e.g. Combat::block()).
//
// These tests exist specifically to catch divergence between:
//   (a) the scalar/SSE2/SSE4.1/AVX2 kernels and each other, and
//   (b) the batched kernels and the single-target formulas they're supposed
//       to reproduce.
// Any such divergence means an AoE/multi-target hit can resolve to a
// different damage/resistance than a single-target hit would, for the same
// inputs — which is the class of bug this file is written to surface.

#include "otpch.h"

#include "combat.h"
#include "simd_kernels.h"

#include <gtest/gtest.h>
#include <vector>
#include <cstring>

using BlackTek::Combat;
using namespace BlackTek::SIMD;

namespace
{
	bool g_hasSse41 = false;
	bool g_hasAvx2 = false;

	void DetectHostFeaturesOnce()
	{
		static const bool done = []
		{
#if defined(__GNUC__) || defined(__clang__)
			__builtin_cpu_init();
			g_hasSse41 = __builtin_cpu_supports("sse4.1");
			g_hasAvx2  = __builtin_cpu_supports("avx2");
#elif defined(_MSC_VER)
			int info[4];
			__cpuid(info, 1);
			const bool hasSse2    = (info[3] >> 26) & 1;
			const bool hasSse4_1  = (info[2] >> 19) & 1;
			const bool hasOsxsave = (info[2] >> 27) & 1;
			g_hasSse41 = hasSse2 and hasSse4_1;
			if (hasSse4_1 and hasOsxsave)
			{
				int info7[4];
				__cpuidex(info7, 7, 0);
				const bool hasAvx2 = (info7[1] >> 5) & 1;
				const uint64_t xcr0 = _xgetbv(0);
				g_hasAvx2 = hasAvx2 and ((xcr0 & 0x6u) == 0x6u);
			}
#endif
			return true;
		}();
		(void)done;
	}

	// Forces BlackTek::SIMD::g_level for the lifetime of the scope, then restores it.
	// Used to drive the public dispatch functions down a specific kernel without
	// depending on detect().
	struct ScopedSimdLevel
	{
		Level previous;
		explicit ScopedSimdLevel(Level level) : previous(g_level) { g_level = level; }
		~ScopedSimdLevel() { g_level = previous; }
	};

	struct SimdKernelsTest : ::testing::Test
	{
		void SetUp() override { DetectHostFeaturesOnce(); }
	};

	constexpr std::array<size_t, 8> kBoundaryCounts { 0, 1, 7, 8, 15, 16, 17, 37 };
}

// ---------------------------------------------------------------------------
// SIMD dispatch / CPU feature detection (src/simd_dispatch.h)
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, DetectSelectsAvx2OnAvx2CapableHardware)
{
	if (not g_hasAvx2)
		GTEST_SKIP() << "host CPU doesn't support AVX2; nothing meaningful to assert here";

	const Level previousLevel = g_level;
	detect();
	const Level detectedLevel = g_level;
	g_level = previousLevel;

	EXPECT_EQ(detectedLevel, Level::AVX2);
}

// ---------------------------------------------------------------------------
// gen_positions (src/simd_kernels.h)
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, GenPositionsAgreesAcrossScalarSse2Avx2)
{
	for (const size_t count : kBoundaryCounts)
	{
		std::vector<CoordOffset> spreads(count), forwards(count);
		for (size_t i = 0; i < count; ++i)
		{
			spreads[i]  = static_cast<CoordOffset>(static_cast<int>(i) * 7 - 50);
			forwards[i] = static_cast<CoordOffset>(static_cast<int>(i) * 11 - 80);
		}
		const MapCoord base_x = 30000, base_y = 128;

		std::vector<MapCoord> scalarXs(count), scalarYs(count);
		{
			ScopedSimdLevel scope(Level::Scalar);
			gen_positions(spreads.data(), forwards.data(), count, base_x, base_y, scalarXs.data(), scalarYs.data());
		}

		std::vector<MapCoord> sse2Xs(count), sse2Ys(count);
		{
			ScopedSimdLevel scope(Level::SSE2);
			gen_positions(spreads.data(), forwards.data(), count, base_x, base_y, sse2Xs.data(), sse2Ys.data());
		}
		EXPECT_EQ(scalarXs, sse2Xs) << "SSE2 diverged from scalar at count=" << count;
		EXPECT_EQ(scalarYs, sse2Ys) << "SSE2 diverged from scalar at count=" << count;

		if (g_hasAvx2)
		{
			std::vector<MapCoord> avx2Xs(count), avx2Ys(count);
			ScopedSimdLevel scope(Level::AVX2);
			gen_positions(spreads.data(), forwards.data(), count, base_x, base_y, avx2Xs.data(), avx2Ys.data());
			EXPECT_EQ(scalarXs, avx2Xs) << "AVX2 diverged from scalar at count=" << count;
			EXPECT_EQ(scalarYs, avx2Ys) << "AVX2 diverged from scalar at count=" << count;
		}
	}
}

// ---------------------------------------------------------------------------
// compute_bbox (src/simd_kernels.h) -- also exercises the SSE4.1-only path,
// which gen_positions/batch_resistance/batch_resolve don't have.
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, ComputeBboxAgreesAcrossScalarSse2Sse41Avx2)
{
	for (const size_t count : kBoundaryCounts)
	{
		if (count == 0) continue; // compute_bbox() is a no-op on an empty range

		std::vector<MapCoord> xs(count), ys(count);
		for (size_t i = 0; i < count; ++i)
		{
			// alternate near-zero and near-65535 values to stress the uint16
			// sign-flip trick the SSE2 path uses for min/max
			xs[i] = static_cast<MapCoord>((i % 2 == 0) ? (i * 37) : (65535 - i * 13));
			ys[i] = static_cast<MapCoord>((i % 3 == 0) ? (i * 19) : (65535 - i * 29));
		}

		MapCoord sMinX, sMaxX, sMinY, sMaxY;
		{
			ScopedSimdLevel scope(Level::Scalar);
			compute_bbox(xs.data(), ys.data(), count, sMinX, sMaxX, sMinY, sMaxY);
		}

		MapCoord minX, maxX, minY, maxY;
		{
			ScopedSimdLevel scope(Level::SSE2);
			compute_bbox(xs.data(), ys.data(), count, minX, maxX, minY, maxY);
		}
		EXPECT_EQ(minX, sMinX) << "SSE2 minX diverged at count=" << count;
		EXPECT_EQ(maxX, sMaxX) << "SSE2 maxX diverged at count=" << count;
		EXPECT_EQ(minY, sMinY) << "SSE2 minY diverged at count=" << count;
		EXPECT_EQ(maxY, sMaxY) << "SSE2 maxY diverged at count=" << count;

		if (g_hasSse41)
		{
			ScopedSimdLevel scope(Level::SSE4_1);
			compute_bbox(xs.data(), ys.data(), count, minX, maxX, minY, maxY);
			EXPECT_EQ(minX, sMinX) << "SSE4.1 minX diverged at count=" << count;
			EXPECT_EQ(maxX, sMaxX) << "SSE4.1 maxX diverged at count=" << count;
			EXPECT_EQ(minY, sMinY) << "SSE4.1 minY diverged at count=" << count;
			EXPECT_EQ(maxY, sMaxY) << "SSE4.1 maxY diverged at count=" << count;
		}

		if (g_hasAvx2)
		{
			ScopedSimdLevel scope(Level::AVX2);
			compute_bbox(xs.data(), ys.data(), count, minX, maxX, minY, maxY);
			EXPECT_EQ(minX, sMinX) << "AVX2 minX diverged at count=" << count;
			EXPECT_EQ(maxX, sMaxX) << "AVX2 maxX diverged at count=" << count;
			EXPECT_EQ(minY, sMinY) << "AVX2 minY diverged at count=" << count;
			EXPECT_EQ(maxY, sMaxY) << "AVX2 maxY diverged at count=" << count;
		}
	}
}

// ---------------------------------------------------------------------------
// compute_spectator_mask / reject_mask (src/simd_kernels.h)
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, ComputeSpectatorMaskAgreesAcrossScalarSse2Avx2)
{
	for (const size_t count : kBoundaryCounts)
	{
		std::vector<MapCoord> xs(count), ys(count);
		for (size_t i = 0; i < count; ++i)
		{
			xs[i] = static_cast<MapCoord>((i % 2 == 0) ? (100 + i) : (65535 - i));
			ys[i] = static_cast<MapCoord>((i % 3 == 0) ? (100 + i * 2) : (65535 - i * 2));
		}
		const MapCoord xMin = 50, xMax = 200, yMin = 50, yMax = 300;

		std::vector<uint8_t> scalarMask(count);
		{
			ScopedSimdLevel scope(Level::Scalar);
			compute_spectator_mask(xs.data(), ys.data(), count, xMin, xMax, yMin, yMax, scalarMask.data());
		}

		std::vector<uint8_t> sse2Mask(count);
		{
			ScopedSimdLevel scope(Level::SSE2);
			compute_spectator_mask(xs.data(), ys.data(), count, xMin, xMax, yMin, yMax, sse2Mask.data());
		}
		EXPECT_EQ(scalarMask, sse2Mask) << "SSE2 reject mask diverged at count=" << count;

		if (g_hasAvx2)
		{
			std::vector<uint8_t> avx2Mask(count);
			ScopedSimdLevel scope(Level::AVX2);
			compute_spectator_mask(xs.data(), ys.data(), count, xMin, xMax, yMin, yMax, avx2Mask.data());
			EXPECT_EQ(scalarMask, avx2Mask) << "AVX2 reject mask diverged at count=" << count;
		}
	}
}

// ---------------------------------------------------------------------------
// batch_resistance vs Combat::calculate_resistance (src/combat.h)
//
// Identity and Percent are fully deterministic (no RNG involved anywhere),
// so the batched kernel must match the single-target formula exactly for
// every input -- any mismatch here is an unambiguous, reproducible bug.
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, BatchResistanceIdentityAndPercentMatchCalculateResistance)
{
	Combat::ResistanceFactors identity{};
	identity.formula_type = Combat::ResistanceFormula::Identity;

	Combat::ResistanceFactors percent{};
	percent.formula_type = Combat::ResistanceFormula::Percent;
	percent.constant = 100.0f;

	const std::vector<int32_t> statsToTest { -50, -1, 0, 1, 2, 3, 7, 25, 50, 99, 100, 101, 250, 999, 1000, 5000, 32767 };

	for (const auto* factors : { &identity, &percent })
	{
		std::vector<CombatStat> stats(statsToTest.begin(), statsToTest.end());
		const size_t n = stats.size();
		std::vector<CombatStat> out(n);

		for (const Level level : { Level::Scalar, Level::SSE2, Level::AVX2 })
		{
			if (level == Level::AVX2 and not g_hasAvx2) continue;

			ScopedSimdLevel scope(level);
			batch_resistance(stats.data(), n, *factors, nullptr, out.data());

			for (size_t i = 0; i < n; ++i)
			{
				EXPECT_EQ(out[i], Combat::calculate_resistance(*factors, stats[i]))
					<< "formula=" << static_cast<int>(factors->formula_type)
					<< " level=" << static_cast<int>(level) << " stat=" << stats[i];
			}
		}
	}
}

// ---------------------------------------------------------------------------
// batch_resistance LinearRandom/Parity: roll-to-range mapping bounds check.
//
// A roll of 0 must map to the formula's lower bound, and the maximum roll
// (INT32_MAX, which uniform_random(0, INT32_MAX) can and does legitimately
// produce) must map to no more than the formula's upper bound -- the single-
// target Combat::calculate_resistance path (std::uniform_int_distribution)
// can never exceed its upper bound by construction, so the batch path
// shouldn't be able to either.
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, BatchResistanceLinearRandomStaysWithinItsOwnBounds)
{
	Combat::ResistanceFactors factors{};
	factors.formula_type = Combat::ResistanceFormula::LinearRandom;
	factors.min_scale = 0.0f;
	factors.max_scale = 1.0f;

	const std::vector<int32_t> statsToTest { 1, 2, 3, 7, 25, 50, 99, 100, 101, 250, 999, 1000, 5000 };

	for (const int32_t stat : statsToTest)
	{
		const int32_t lowerBound = static_cast<int32_t>(static_cast<float>(stat) * factors.min_scale + factors.min_base);
		const int32_t upperBound = static_cast<int32_t>(static_cast<float>(stat) * factors.max_scale + factors.max_base);

		const std::vector<CombatStat> stats { stat };

		for (const int32_t roll : { 0, 0x7FFFFFFE, 0x7FFFFFFF })
		{
			const std::vector<CombatStat> rng { roll };
			std::vector<CombatStat> out(1);

			for (const Level level : { Level::Scalar, Level::SSE2, Level::AVX2 })
			{
				if (level == Level::AVX2 and not g_hasAvx2) continue;

				ScopedSimdLevel scope(level);
				batch_resistance(stats.data(), 1, factors, rng.data(), out.data());

				if (roll == 0)
				{
					EXPECT_EQ(out[0], lowerBound)
						<< "level=" << static_cast<int>(level) << " stat=" << stat << ": roll=0 should map to the lower bound exactly";
				}

				EXPECT_LE(out[0], upperBound)
					<< "level=" << static_cast<int>(level) << " stat=" << stat << " roll=" << roll;
			}
		}
	}
}

TEST_F(SimdKernelsTest, BatchResistanceParityStaysWithinItsOwnBounds)
{
	Combat::ResistanceFactors factors = Combat::ClassicArmor; // min_scale=0.5, threshold=3, flat_amount=1, parity_offset=1

	const std::vector<int32_t> statsAboveThreshold { 20, 50, 99, 100, 250, 999, 1000, 5000 };

	for (const int32_t stat : statsAboveThreshold)
	{
		const int32_t lowerBound = static_cast<int32_t>(static_cast<float>(stat) * factors.min_scale);
		const int32_t upperBound = stat - (stat % 2 + static_cast<int32_t>(factors.parity_offset));

		const std::vector<CombatStat> stats { stat };

		for (const int32_t roll : { 0, 0x7FFFFFFE, 0x7FFFFFFF })
		{
			const std::vector<CombatStat> rng { roll };
			std::vector<CombatStat> out(1);

			for (const Level level : { Level::Scalar, Level::SSE2, Level::AVX2 })
			{
				if (level == Level::AVX2 and not g_hasAvx2) continue;

				ScopedSimdLevel scope(level);
				batch_resistance(stats.data(), 1, factors, rng.data(), out.data());

				if (roll == 0)
				{
					EXPECT_EQ(out[0], lowerBound)
						<< "level=" << static_cast<int>(level) << " stat=" << stat << ": roll=0 should map to the lower bound exactly";
				}

				EXPECT_LE(out[0], upperBound)
					<< "level=" << static_cast<int>(level) << " stat=" << stat << " roll=" << roll;
			}
		}
	}
}

// ---------------------------------------------------------------------------
// batch_resolve vs Combat::calculate_resolution (src/combat.h)
//
// Resolution never involves randomness, in either the single-target or
// batched path, so every formula type should match exactly across the full
// grid below for every SIMD level.
// ---------------------------------------------------------------------------

TEST_F(SimdKernelsTest, BatchResolveMatchesCalculateResolutionAcrossAllFormulasAndLevels)
{
	struct NamedFactors { const char* name; Combat::ResolutionFactors factors; };
	const std::vector<NamedFactors> variants
	{
		{ "Subtractive",     Combat::ClassicResolution   },
		{ "RatioMitigation", Combat::ScaledResolution    },
		{ "ScaledDivision",  Combat::BalancedResolution  },
		{ "Layered",         Combat::AbsorptionResolution },
	};

	const std::vector<int32_t> outputsToTest     { 0, 1, 5, 10, 25, 50, 99, 100, 101, 250, 500, 1000, 5000 };
	const std::vector<int32_t> resistancesToTest { -50, -1, 0, 1, 2, 10, 25, 50, 99, 100, 101, 250, 999 };

	for (const auto& variant : variants)
	{
		std::vector<CombatStat> outs, resists, expected;
		for (const int32_t o : outputsToTest)
		{
			for (const int32_t r : resistancesToTest)
			{
				outs.push_back(o);
				resists.push_back(r);
				expected.push_back(Combat::calculate_resolution(variant.factors, o, r));
			}
		}
		const size_t n = outs.size();
		std::vector<CombatStat> result(n);

		for (const Level level : { Level::Scalar, Level::SSE2, Level::AVX2 })
		{
			if (level == Level::AVX2 and not g_hasAvx2) continue;

			ScopedSimdLevel scope(level);
			batch_resolve(outs.data(), resists.data(), n, variant.factors, result.data());

			for (size_t i = 0; i < n; ++i)
			{
				EXPECT_EQ(result[i], expected[i])
					<< "formula=" << variant.name << " level=" << static_cast<int>(level)
					<< " output=" << outs[i] << " resistance=" << resists[i];
			}
		}
	}
}
