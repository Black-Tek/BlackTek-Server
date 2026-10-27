/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * This file is an original BlackTek contribution, created since this project's
 * inception, and contains no code inherited from The Forgotten Server.
 * It is licensed solely under the MIT License (see the licensing/License file
 * in the project root for the full text).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
#elif defined(__GNUC__) || defined(__clang__)
		__builtin_cpu_init();

		if (__builtin_cpu_supports("avx2"))
		{
			g_level = Level::AVX2;
			return;
		}

		if (__builtin_cpu_supports("sse4.1"))
		{
			g_level = Level::SSE4_1;
			return;
		}

		if (__builtin_cpu_supports("sse2"))
		{
			g_level = Level::SSE2;
			return;
		}

		g_level = Level::Scalar;
#else
		g_level = Level::Scalar;
#endif
	}

} // namespace BlackTek::SIMD
