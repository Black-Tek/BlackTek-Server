// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once
#include <cstdint>

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#	include <intrin.h>
#	include <immintrin.h>
#endif

namespace BlackTek::SIMD
{
	enum class Level : uint8_t { Scalar = 0, SSE2 = 1, SSE4_1 = 2, AVX2 = 3 };

	using MapCoord    = uint16_t;   // unsigned map coordinate component (x or y axis value)
	using CoordOffset = int16_t;    // signed position delta applied during spread/forward generation
	using CombatStat  = int32_t;    // signed combat stat, damage output, or resistance value
	using RandomRoll  = uint32_t;   // unsigned random number consumed by resistance formulas

	inline Level g_level = Level::Scalar;

	inline void detect() noexcept
	{
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
		int info[4];

		__cpuid(info, 1);
		const bool has_sse2    = (info[3] >> 26) & 1;
		const bool has_sse4_1  = (info[2] >> 19) & 1;
		const bool has_osxsave = (info[2] >> 27) & 1;

		if (not has_sse2)
		{
			g_level = Level::Scalar;
			return;
		}

		if (not has_sse4_1)
		{
			g_level = Level::SSE2;
			return;
		}

		__cpuidex(info, 7, 0);
		const bool has_avx2 = (info[1] >> 5) & 1;

		if (has_avx2 and has_osxsave)
		{
			const uint64_t xcr0 = _xgetbv(0);
			if ((xcr0 & 0x6u) == 0x6u)
			{
				g_level = Level::AVX2;
				return;
			}
		}

		g_level = Level::SSE4_1;
#else
		g_level = Level::Scalar;
#endif
	}

} // namespace BlackTek::SIMD
