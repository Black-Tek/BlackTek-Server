// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <immintrin.h>
#include "simd_dispatch.h"

namespace BlackTek
{
	struct ResistanceFactors;
	struct ResolutionFactors;
}

namespace BlackTek::SIMD
{

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
	using Vec128Int   = __m128i;    // SSE2 128-bit packed integer register (8×i16 or 4×i32)
	using Vec128Float = __m128;     // SSE2 128-bit packed float register   (4×f32)
	using Vec256Int   = __m256i;    // AVX2 256-bit packed integer register (16×i16 or 8×i32)
	using Vec256Float = __m256;     // AVX2 256-bit packed float register   (8×f32)
#endif

namespace detail
{
	inline MapCoord scalar_hmin_u16(const MapCoord* values, size_t count) noexcept
	{
		MapCoord runningMin = 0xFFFFu;
		for (size_t index = 0; index < count; ++index) runningMin = std::min(runningMin, values[index]);
		return runningMin;
	}
	inline MapCoord scalar_hmax_u16(const MapCoord* values, size_t count) noexcept
	{
		MapCoord runningMax = 0u;
		for (size_t index = 0; index < count; ++index) runningMax = std::max(runningMax, values[index]);
		return runningMax;
	}

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)

		inline CoordOffset sse2_hmin_epi16(Vec128Int packedValues) noexcept
		{
			packedValues = _mm_min_epi16(packedValues, _mm_shuffle_epi32(packedValues, _MM_SHUFFLE(1, 0, 3, 2)));
			packedValues = _mm_min_epi16(packedValues, _mm_shufflelo_epi16(packedValues, _MM_SHUFFLE(1, 0, 3, 2)));
			packedValues = _mm_min_epi16(packedValues, _mm_shufflelo_epi16(packedValues, _MM_SHUFFLE(0, 1, 0, 1)));
			return static_cast<CoordOffset>(_mm_extract_epi16(packedValues, 0));
		}

		inline CoordOffset sse2_hmax_epi16(Vec128Int packedValues) noexcept
		{
			packedValues = _mm_max_epi16(packedValues, _mm_shuffle_epi32(packedValues, _MM_SHUFFLE(1, 0, 3, 2)));
			packedValues = _mm_max_epi16(packedValues, _mm_shufflelo_epi16(packedValues, _MM_SHUFFLE(1, 0, 3, 2)));
			packedValues = _mm_max_epi16(packedValues, _mm_shufflelo_epi16(packedValues, _MM_SHUFFLE(0, 1, 0, 1)));
			return static_cast<CoordOffset>(_mm_extract_epi16(packedValues, 0));
		}

		inline MapCoord sse41_hmin_epu16(Vec128Int packedValues) noexcept
		{
			return static_cast<MapCoord>(_mm_extract_epi16(_mm_minpos_epu16(packedValues), 0));
		}

		inline MapCoord sse41_hmax_epu16(Vec128Int packedValues) noexcept
		{
			Vec128Int invertedValues = _mm_xor_si128(packedValues, _mm_set1_epi16(-1));
			return static_cast<MapCoord>(~_mm_extract_epi16(_mm_minpos_epu16(invertedValues), 0) & 0xFFFFu);
		}

	#endif // MSVC / GCC / Clang

} // namespace detail


namespace detail
{

	inline void scalar_gen_positions(const CoordOffset* spreads, const CoordOffset* forwards, size_t count,	MapCoord base_x, MapCoord base_y, MapCoord* out_xs, MapCoord* out_ys) noexcept
	{
		for (size_t index = 0; index < count; ++index)
		{
			out_xs[index] = static_cast<MapCoord>(base_x + spreads[index]);
			out_ys[index] = static_cast<MapCoord>(base_y + forwards[index]);
		}
	}

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)

		inline void sse2_gen_positions(const CoordOffset* spreads, const CoordOffset* forwards, size_t count, MapCoord base_x, MapCoord base_y,	MapCoord* out_xs, MapCoord* out_ys) noexcept
		{
			const Vec128Int broadcastBaseX = _mm_set1_epi16(static_cast<CoordOffset>(base_x));
			const Vec128Int broadcastBaseY = _mm_set1_epi16(static_cast<CoordOffset>(base_y));
			size_t index = 0;

			for (; index + 8 <= count; index += 8)
			{
				Vec128Int spreadChunk  = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(spreads  + index));
				Vec128Int forwardChunk = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(forwards + index));
				_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out_xs + index), _mm_add_epi16(broadcastBaseX, spreadChunk));
				_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out_ys + index), _mm_add_epi16(broadcastBaseY, forwardChunk));
			}

			scalar_gen_positions(spreads + index, forwards + index, count - index, base_x, base_y, out_xs + index, out_ys + index);
		}

		inline void avx2_gen_positions(const CoordOffset* spreads, const CoordOffset* forwards, size_t count, MapCoord base_x, MapCoord base_y, MapCoord* out_xs, MapCoord* out_ys) noexcept
		{
			const Vec256Int broadcastBaseX = _mm256_set1_epi16(static_cast<CoordOffset>(base_x));
			const Vec256Int broadcastBaseY = _mm256_set1_epi16(static_cast<CoordOffset>(base_y));
			size_t index = 0;

			for (; index + 16 <= count; index += 16)
			{
				Vec256Int spreadChunk  = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(spreads  + index));
				Vec256Int forwardChunk = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(forwards + index));
				_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out_xs + index), _mm256_add_epi16(broadcastBaseX, spreadChunk));
				_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out_ys + index), _mm256_add_epi16(broadcastBaseY, forwardChunk));
			}

			sse2_gen_positions(spreads + index, forwards + index, count - index, base_x, base_y, out_xs + index, out_ys + index);
		}
	#endif

} // namespace detail

inline void gen_positions(const CoordOffset* spreads, const CoordOffset* forwards, size_t count, MapCoord base_x, MapCoord base_y, MapCoord* out_xs, MapCoord* out_ys) noexcept
{
#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
	switch (g_level)
	{
		case Level::AVX2:
			detail::avx2_gen_positions(spreads, forwards, count, base_x, base_y, out_xs, out_ys);
			return;
		case Level::SSE4_1:
		case Level::SSE2:
			detail::sse2_gen_positions(spreads, forwards, count, base_x, base_y, out_xs, out_ys);
			return;
		default: break;
	}
#endif
	detail::scalar_gen_positions(spreads, forwards, count, base_x, base_y, out_xs, out_ys);
}

	namespace detail
	{
		inline void scalar_compute_bbox(
			const MapCoord* xs, const MapCoord* ys, size_t count,
			MapCoord& minX, MapCoord& maxX, MapCoord& minY, MapCoord& maxY) noexcept
		{
			for (size_t index = 0; index < count; ++index)
			{
				minX = std::min(minX, xs[index]);
				maxX = std::max(maxX, xs[index]);
				minY = std::min(minY, ys[index]);
				maxY = std::max(maxY, ys[index]);
			}
		}

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
		inline void sse2_compute_bbox(
			const MapCoord* xs, const MapCoord* ys, size_t count,
			MapCoord& minX, MapCoord& maxX, MapCoord& minY, MapCoord& maxY) noexcept
		{
			// we use the signflip trick, ie, XOR with 0x8000 converts uint16 ordering to int16 ordering.
			const Vec128Int signFlipMask = _mm_set1_epi16(static_cast<CoordOffset>(0x8000));
			Vec128Int packedMinX = _mm_set1_epi16(0x7FFF);   // INT16_MAX == uint16 0xFFFF after flip
			Vec128Int packedMaxX = _mm_set1_epi16(-0x8000);   // INT16_MIN == uint16 0x0000 after flip
			Vec128Int packedMinY = packedMinX;
			Vec128Int packedMaxY = packedMaxX;
			size_t index = 0;

			for (; index + 8 <= count; index += 8)
			{
				Vec128Int xChunkSigned = _mm_xor_si128(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(xs + index)), signFlipMask);
				Vec128Int yChunkSigned = _mm_xor_si128(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(ys + index)), signFlipMask);
				packedMinX = _mm_min_epi16(packedMinX, xChunkSigned);
				packedMaxX = _mm_max_epi16(packedMaxX, xChunkSigned);
				packedMinY = _mm_min_epi16(packedMinY, yChunkSigned);
				packedMaxY = _mm_max_epi16(packedMaxY, yChunkSigned);
			}

			// flip back from signed domain to unsigned
			minX = static_cast<MapCoord>(detail::sse2_hmin_epi16(packedMinX) ^ static_cast<CoordOffset>(0x8000));
			maxX = static_cast<MapCoord>(detail::sse2_hmax_epi16(packedMaxX) ^ static_cast<CoordOffset>(0x8000));
			minY = static_cast<MapCoord>(detail::sse2_hmin_epi16(packedMinY) ^ static_cast<CoordOffset>(0x8000));
			maxY = static_cast<MapCoord>(detail::sse2_hmax_epi16(packedMaxY) ^ static_cast<CoordOffset>(0x8000));

			scalar_compute_bbox(xs + index, ys + index, count - index, minX, maxX, minY, maxY);
		}

		inline void sse41_compute_bbox(
			const MapCoord* xs, const MapCoord* ys, size_t count,
			MapCoord& minX, MapCoord& maxX, MapCoord& minY, MapCoord& maxY) noexcept
		{
			Vec128Int packedMinX = _mm_set1_epi16(-1);   // 0xFFFF
			Vec128Int packedMaxX = _mm_setzero_si128();
			Vec128Int packedMinY = packedMinX;
			Vec128Int packedMaxY = packedMaxX;
			size_t index = 0;

			for (; index + 8 <= count; index += 8)
			{
				Vec128Int xChunk = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(xs + index));
				Vec128Int yChunk = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(ys + index));
				packedMinX = _mm_min_epu16(packedMinX, xChunk);
				packedMaxX = _mm_max_epu16(packedMaxX, xChunk);
				packedMinY = _mm_min_epu16(packedMinY, yChunk);
				packedMaxY = _mm_max_epu16(packedMaxY, yChunk);
			}

			minX = detail::sse41_hmin_epu16(packedMinX);
			maxX = detail::sse41_hmax_epu16(packedMaxX);
			minY = detail::sse41_hmin_epu16(packedMinY);
			maxY = detail::sse41_hmax_epu16(packedMaxY);

			scalar_compute_bbox(xs + index, ys + index, count - index, minX, maxX, minY, maxY);
		}

		inline void avx2_compute_bbox(const MapCoord* xs, const MapCoord* ys, size_t count, MapCoord& minX, MapCoord& maxX, MapCoord& minY, MapCoord& maxY) noexcept
		{
			Vec256Int packedMinX = _mm256_set1_epi16(-1);  // 0xFFFF
			Vec256Int packedMaxX = _mm256_setzero_si256();
			Vec256Int packedMinY = packedMinX;
			Vec256Int packedMaxY = packedMaxX;
			size_t index = 0;

			for (; index + 16 <= count; index += 16)
			{
				Vec256Int xChunk = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(xs + index));
				Vec256Int yChunk = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(ys + index));
				packedMinX = _mm256_min_epu16(packedMinX, xChunk);
				packedMaxX = _mm256_max_epu16(packedMaxX, xChunk);
				packedMinY = _mm256_min_epu16(packedMinY, yChunk);
				packedMaxY = _mm256_max_epu16(packedMaxY, yChunk);
			}

			// we reduce 256-bit to 128-bit then we use the SSE4.1 horizontal reduce
			auto reduce_min = [](const Vec256Int packed) -> MapCoord
			{
				Vec128Int lowerHalf = _mm256_castsi256_si128(packed);
				Vec128Int upperHalf = _mm256_extracti128_si256(packed, 1);
				return detail::sse41_hmin_epu16(_mm_min_epu16(lowerHalf, upperHalf));
			};

			auto reduce_max = [](const Vec256Int packed) -> MapCoord
			{
				Vec128Int lowerHalf = _mm256_castsi256_si128(packed);
				Vec128Int upperHalf = _mm256_extracti128_si256(packed, 1);
				return detail::sse41_hmax_epu16(_mm_max_epu16(lowerHalf, upperHalf));
			};

			minX = reduce_min(packedMinX);
			maxX = reduce_max(packedMaxX);
			minY = reduce_min(packedMinY);
			maxY = reduce_max(packedMaxY);

			sse41_compute_bbox(xs + index, ys + index, count - index, minX, maxX, minY, maxY);
		}
	#endif

} // namespace detail

inline void compute_bbox(const MapCoord* xs, const MapCoord* ys, size_t count, MapCoord& minX, MapCoord& maxX, MapCoord& minY, MapCoord& maxY) noexcept
{
	if (count == 0) return;

	minX = maxX = xs[0];
	minY = maxY = ys[0];

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
		switch (g_level)
		{
			case Level::AVX2:
				detail::avx2_compute_bbox(xs, ys, count, minX, maxX, minY, maxY);
				return;
			case Level::SSE4_1:
				detail::sse41_compute_bbox(xs, ys, count, minX, maxX, minY, maxY);
				return;
			case Level::SSE2:
				detail::sse2_compute_bbox(xs, ys, count, minX, maxX, minY, maxY);
				return;
			default: break;
		}
	#endif

	detail::scalar_compute_bbox(xs, ys, count, minX, maxX, minY, maxY);
}

namespace detail
{
	template<class ResistanceFactors>
	inline void scalar_batch_resistance(const CombatStat* stats, size_t count, const ResistanceFactors& f, const CombatStat* rng_vals, CombatStat* out) noexcept
	{
		using RF = decltype(f.formula_type);
		switch (f.formula_type)
		{
			case RF::Identity:
				std::memcpy(out, stats, count * sizeof(CombatStat));
				return;

			case RF::Percent:
				for (size_t index = 0; index < count; ++index)
				{
					const CombatStat statValue = stats[index];
					out[index] = statValue <= 0 ? 0 : (statValue * 100) / (statValue + static_cast<CombatStat>(f.constant));
				}
				return;

			case RF::LinearRandom:
				for (size_t index = 0; index < count; ++index)
				{
					const CombatStat statValue = stats[index];
					if (statValue <= 0) { out[index] = 0; continue; }
					const CombatStat lowerBound = static_cast<CombatStat>(static_cast<float>(statValue) * f.min_scale + f.min_base);
					const CombatStat upperBound = static_cast<CombatStat>(static_cast<float>(statValue) * f.max_scale + f.max_base);
					const CombatStat valueRange = upperBound - lowerBound + 1;
					if (valueRange <= 0) { out[index] = lowerBound; continue; }
					// map pre-generated random to [lowerBound, upperBound]
					const RandomRoll randomNormalized = static_cast<RandomRoll>(rng_vals[index]) & 0x7FFFFFFFu;
					out[index] = lowerBound + static_cast<CombatStat>(static_cast<float>(randomNormalized) * (1.0f / static_cast<float>(0x7FFFFFFFu)) * static_cast<float>(valueRange));
				}
				return;

			case RF::Parity:
				for (size_t index = 0; index < count; ++index)
				{
					const CombatStat statValue = stats[index];
					if (statValue <= 0) { out[index] = 0; continue; }
					if (statValue > static_cast<CombatStat>(f.threshold))
					{
						const CombatStat lowerBound = static_cast<CombatStat>(static_cast<float>(statValue) * f.min_scale);
						const CombatStat upperBound = statValue - (statValue % 2 + static_cast<CombatStat>(f.parity_offset));
						const CombatStat valueRange = upperBound - lowerBound + 1;
						if (valueRange <= 0) { out[index] = lowerBound; continue; }
						const RandomRoll randomNormalized = static_cast<RandomRoll>(rng_vals[index]) & 0x7FFFFFFFu;
						out[index] = lowerBound + static_cast<CombatStat>(static_cast<float>(randomNormalized) * (1.0f / static_cast<float>(0x7FFFFFFFu)) * static_cast<float>(valueRange));
					}
					else
					{
						out[index] = static_cast<CombatStat>(f.flat_amount);
					}
				}
				return;

			default:
				for (size_t index = 0; index < count; ++index) out[index] = 0;
				return;
		}
	}

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
	template<class ResistanceFactors>
	inline void sse2_batch_resistance(const CombatStat* stats, size_t count, const ResistanceFactors& f, const CombatStat* rng_vals, CombatStat* out) noexcept
	{
		using RF = decltype(f.formula_type);
		switch (f.formula_type)
		{
			case RF::Identity:
				std::memcpy(out, stats, count * sizeof(CombatStat));
				return;

			case RF::Percent:
			{
				const Vec128Float vecOneHundred = _mm_set1_ps(100.0f);
				const Vec128Float vecConstant   = _mm_set1_ps(f.constant);
				const Vec128Float vecZero       = _mm_setzero_ps();
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Float floatStats = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(stats + index)));
					// clamp negatives to 0
					floatStats = _mm_max_ps(floatStats, vecZero);
					Vec128Float resultFloat = _mm_div_ps(_mm_mul_ps(floatStats, vecOneHundred), _mm_add_ps(floatStats, vecConstant));
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), _mm_cvttps_epi32(resultFloat));
				}

				scalar_batch_resistance(stats + index, count - index, f, rng_vals ? rng_vals + index : nullptr, out + index);
				return;
			}

			case RF::LinearRandom:
			{
				const Vec128Float vecMinScale = _mm_set1_ps(f.min_scale);
				const Vec128Float vecMaxScale = _mm_set1_ps(f.max_scale);
				const Vec128Float vecMinBase  = _mm_set1_ps(f.min_base);
				const Vec128Float vecMaxBase  = _mm_set1_ps(f.max_base);
				const Vec128Float vecRngInverse = _mm_set1_ps(1.0f / static_cast<float>(0x7FFFFFFFu));
				const Vec128Float vecOne      = _mm_set1_ps(1.0f);
				const Vec128Int   rngMask31Bit = _mm_set1_epi32(0x7FFFFFFF);
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Float  floatStats       = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(stats + index)));
					Vec128Float  floatLowerBound  = _mm_add_ps(_mm_mul_ps(floatStats, vecMinScale), vecMinBase);
					Vec128Float  floatUpperBound  = _mm_add_ps(_mm_mul_ps(floatStats, vecMaxScale), vecMaxBase);
					Vec128Int    intLowerBound    = _mm_cvttps_epi32(floatLowerBound);
					Vec128Int    intUpperBound    = _mm_cvttps_epi32(floatUpperBound);
					Vec128Float  floatRange       = _mm_add_ps(_mm_cvtepi32_ps(_mm_sub_epi32(intUpperBound, intLowerBound)), vecOne);
					floatRange = _mm_max_ps(floatRange, vecOne);
					Vec128Int    maskedRng        = _mm_and_si128(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(rng_vals + index)), rngMask31Bit);
					Vec128Float  floatOffset      = _mm_mul_ps(_mm_mul_ps(_mm_cvtepi32_ps(maskedRng), vecRngInverse), floatRange);
					Vec128Int    outputValues     = _mm_add_epi32(intLowerBound, _mm_cvttps_epi32(floatOffset));
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), outputValues);
				}

				scalar_batch_resistance(stats + index, count - index, f, rng_vals + index, out + index);
				return;
			}

			case RF::Parity:
			{
				const Vec128Int   vecThreshold   = _mm_set1_epi32(static_cast<CombatStat>(f.threshold));
				const Vec128Int   vecFlatAmount  = _mm_set1_epi32(static_cast<CombatStat>(f.flat_amount));
				const Vec128Int   vecParityOffset = _mm_set1_epi32(static_cast<CombatStat>(f.parity_offset));
				const Vec128Float vecMinScale    = _mm_set1_ps(f.min_scale);
				const Vec128Float vecRngInverse  = _mm_set1_ps(1.0f / static_cast<float>(0x7FFFFFFFu));
				const Vec128Float vecOne         = _mm_set1_ps(1.0f);
				const Vec128Int   rngMask31Bit   = _mm_set1_epi32(0x7FFFFFFF);
				const Vec128Int   vecOneInt      = _mm_set1_epi32(1);
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Int   packedStats        = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(stats + index));
					Vec128Int   aboveThresholdMask = _mm_cmpgt_epi32(packedStats, vecThreshold);   // all-ones where stat > threshold
					// above-threshold path: intLowerBound = stat*min_scale, intUpperBound = stat - (stat%2 + parity_off)
					Vec128Float floatStats         = _mm_cvtepi32_ps(packedStats);
					Vec128Int   intLowerBound      = _mm_cvttps_epi32(_mm_mul_ps(floatStats, vecMinScale));
					Vec128Int   statParityBit      = _mm_and_si128(packedStats, vecOneInt);         // stat & 1
					Vec128Int   intUpperBound      = _mm_sub_epi32(packedStats, _mm_add_epi32(statParityBit, vecParityOffset));
					Vec128Float floatRange         = _mm_add_ps(_mm_cvtepi32_ps(_mm_sub_epi32(intUpperBound, intLowerBound)), vecOne);
					floatRange                     = _mm_max_ps(floatRange, vecOne);
					Vec128Int   maskedRng          = _mm_and_si128(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(rng_vals + index)), rngMask31Bit);
					Vec128Int   randomResult       = _mm_add_epi32(intLowerBound, _mm_cvttps_epi32(_mm_mul_ps(_mm_mul_ps(_mm_cvtepi32_ps(maskedRng), vecRngInverse), floatRange)));
					// blend: above-threshold → randomResult, below → vecFlatAmount
					Vec128Int   outputValues       = _mm_or_si128(_mm_and_si128(aboveThresholdMask, randomResult), _mm_andnot_si128(aboveThresholdMask, vecFlatAmount));
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), outputValues);
				}

				scalar_batch_resistance(stats + index, count - index, f, rng_vals + index, out + index);
				return;
			}

			default:
				std::memset(out, 0, count * sizeof(CombatStat));
				return;
		}
	}

	template<class ResistanceFactors>
	inline void avx2_batch_resistance(const CombatStat* stats, size_t count,const ResistanceFactors& f,	const CombatStat* rng_vals,	CombatStat* out) noexcept
	{
		using RF = decltype(f.formula_type);
		switch (f.formula_type)
		{
			case RF::Identity:
				std::memcpy(out, stats, count * sizeof(CombatStat));
				return;

			case RF::Percent:
			{
				const Vec256Float vecOneHundred = _mm256_set1_ps(100.0f);
				const Vec256Float vecConstant   = _mm256_set1_ps(f.constant);
				const Vec256Float vecZero       = _mm256_setzero_ps();
				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Float floatStats = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(stats + index)));
					floatStats             = _mm256_max_ps(floatStats, vecZero);
					Vec256Float resultFloat = _mm256_div_ps(_mm256_mul_ps(floatStats, vecOneHundred), _mm256_add_ps(floatStats, vecConstant));
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), _mm256_cvttps_epi32(resultFloat));
				}

				sse2_batch_resistance(stats + index, count - index, f, rng_vals ? rng_vals + index : nullptr, out + index);
				return;
			}

			case RF::LinearRandom:
			{
				const Vec256Float vecMinScale    = _mm256_set1_ps(f.min_scale);
				const Vec256Float vecMaxScale    = _mm256_set1_ps(f.max_scale);
				const Vec256Float vecMinBase     = _mm256_set1_ps(f.min_base);
				const Vec256Float vecMaxBase     = _mm256_set1_ps(f.max_base);
				const Vec256Float vecRngInverse  = _mm256_set1_ps(1.0f / static_cast<float>(0x7FFFFFFFu));
				const Vec256Float vecOne         = _mm256_set1_ps(1.0f);
				const Vec256Int   rngMask31Bit   = _mm256_set1_epi32(0x7FFFFFFF);
				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Float  floatStats      = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(stats + index)));
					Vec256Float  floatLowerBound = _mm256_add_ps(_mm256_mul_ps(floatStats, vecMinScale), vecMinBase);
					Vec256Float  floatUpperBound = _mm256_add_ps(_mm256_mul_ps(floatStats, vecMaxScale), vecMaxBase);
					Vec256Int    intLowerBound   = _mm256_cvttps_epi32(floatLowerBound);
					Vec256Int    intUpperBound   = _mm256_cvttps_epi32(floatUpperBound);
					Vec256Float  floatRange      = _mm256_add_ps(_mm256_cvtepi32_ps(_mm256_sub_epi32(intUpperBound, intLowerBound)), vecOne);
					floatRange                   = _mm256_max_ps(floatRange, vecOne);
					Vec256Int    maskedRng        = _mm256_and_si256(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(rng_vals + index)), rngMask31Bit);
					Vec256Float  floatOffset     = _mm256_mul_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(maskedRng), vecRngInverse), floatRange);
					Vec256Int    outputValues    = _mm256_add_epi32(intLowerBound, _mm256_cvttps_epi32(floatOffset));
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), outputValues);
				}

				sse2_batch_resistance(stats + index, count - index, f, rng_vals + index, out + index);
				return;
			}

			case RF::Parity:
			{
				const Vec256Int   vecThreshold    = _mm256_set1_epi32(static_cast<CombatStat>(f.threshold));
				const Vec256Int   vecFlatAmount   = _mm256_set1_epi32(static_cast<CombatStat>(f.flat_amount));
				const Vec256Int   vecParityOffset = _mm256_set1_epi32(static_cast<CombatStat>(f.parity_offset));
				const Vec256Float vecMinScale     = _mm256_set1_ps(f.min_scale);
				const Vec256Float vecRngInverse   = _mm256_set1_ps(1.0f / static_cast<float>(0x7FFFFFFFu));
				const Vec256Float vecOne          = _mm256_set1_ps(1.0f);
				const Vec256Int   rngMask31Bit    = _mm256_set1_epi32(0x7FFFFFFF);
				const Vec256Int   vecOneInt       = _mm256_set1_epi32(1);

				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Int   packedStats        = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(stats + index));
					Vec256Int   aboveThresholdMask = _mm256_cmpgt_epi32(packedStats, vecThreshold);
					Vec256Float floatStats         = _mm256_cvtepi32_ps(packedStats);
					Vec256Int   intLowerBound      = _mm256_cvttps_epi32(_mm256_mul_ps(floatStats, vecMinScale));
					Vec256Int   statParityBit      = _mm256_and_si256(packedStats, vecOneInt);
					Vec256Int   intUpperBound      = _mm256_sub_epi32(packedStats, _mm256_add_epi32(statParityBit, vecParityOffset));
					Vec256Float floatRange         = _mm256_add_ps(_mm256_cvtepi32_ps(_mm256_sub_epi32(intUpperBound, intLowerBound)), vecOne);
					floatRange                     = _mm256_max_ps(floatRange, vecOne);
					Vec256Int   maskedRng          = _mm256_and_si256(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(rng_vals + index)), rngMask31Bit);
					Vec256Int   randomResult       = _mm256_add_epi32(intLowerBound, _mm256_cvttps_epi32(_mm256_mul_ps(_mm256_mul_ps(_mm256_cvtepi32_ps(maskedRng), vecRngInverse), floatRange)));
					Vec256Int   outputValues       = _mm256_blendv_epi8(vecFlatAmount, randomResult, aboveThresholdMask);
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), outputValues);
				}

				sse2_batch_resistance(stats + index, count - index, f, rng_vals + index, out + index);
				return;
			}

			default:
				std::memset(out, 0, count * sizeof(CombatStat));
				return;
		}
	}
#endif

} // namespace detail

template<class ResistanceFactors>
inline void batch_resistance(const CombatStat* stats, size_t count, const ResistanceFactors& factors, const CombatStat* rng_vals, CombatStat* out) noexcept
{
	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
		switch (g_level)
		{
			case Level::AVX2:
				detail::avx2_batch_resistance(stats, count, factors, rng_vals, out);
				return;
			case Level::SSE4_1:
			case Level::SSE2:
				detail::sse2_batch_resistance(stats, count, factors, rng_vals, out);
				return;
			default: break;
		}
	#endif

	detail::scalar_batch_resistance(stats, count, factors, rng_vals, out);
}

namespace detail
{
	template<class ResolutionFactors>
	inline void scalar_batch_resolve(const CombatStat* outputs, const CombatStat* resistances, size_t count, const ResolutionFactors& f, CombatStat* out) noexcept
	{
		using RF = decltype(f.formula_type);
		switch (f.formula_type)
		{
			case RF::Subtractive:
				for (size_t index = 0; index < count; ++index)
					out[index] = static_cast<CombatStat>(std::max(f.floor, static_cast<float>(outputs[index] - resistances[index])));
				return;

			case RF::RatioMitigation:
				for (size_t index = 0; index < count; ++index)
				{
					const float resistanceFloat = resistances[index] < 0 ? 0.0f : static_cast<float>(resistances[index]);
					out[index] = static_cast<CombatStat>(std::max(f.floor, static_cast<float>(outputs[index]) * f.constant / (f.constant + resistanceFloat)));
				}
				return;

			case RF::ScaledDivision:
				for (size_t index = 0; index < count; ++index)
				{
					if (resistances[index] <= 0)
						out[index] = static_cast<CombatStat>(std::max(f.floor, static_cast<float>(outputs[index]) * f.multiplier + f.addend));
					else
						out[index] = static_cast<CombatStat>(std::max(f.floor, static_cast<float>(outputs[index]) * f.multiplier / static_cast<float>(resistances[index]) + f.addend));
				}
				return;

			case RF::Layered:
				for (size_t index = 0; index < count; ++index)
				{
					const float mitigationValue = static_cast<float>(outputs[index]) * (1.0f - static_cast<float>(resistances[index]) / 100.0f);
					out[index] = static_cast<CombatStat>(std::max(f.floor, mitigationValue - f.flat_reduction));
				}
				return;

			default:
				std::memset(out, 0, count * sizeof(CombatStat));
				return;
		}
	}

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
	template<class ResolutionFactors>
	inline void sse2_batch_resolve(const CombatStat* outputs, const CombatStat* resistances, size_t count, const ResolutionFactors& f, CombatStat* out) noexcept
	{
		using RF = decltype(f.formula_type);
		const Vec128Float vecFloor = _mm_set1_ps(f.floor);

		switch (f.formula_type)
		{
			case RF::Subtractive:
			{
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Int   packedOutputs    = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(outputs     + index));
					Vec128Int   packedResistances = _mm_loadu_si128(reinterpret_cast<const Vec128Int*>(resistances + index));
					Vec128Float floatDifference  = _mm_cvtepi32_ps(_mm_sub_epi32(packedOutputs, packedResistances));
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), _mm_cvttps_epi32(_mm_max_ps(vecFloor, floatDifference)));
				}

				scalar_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			case RF::RatioMitigation:
			{
				const Vec128Float vecConstant = _mm_set1_ps(f.constant);
				const Vec128Float vecZero     = _mm_setzero_ps();
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Float floatOutputs    = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(outputs     + index)));
					Vec128Float floatResistances = _mm_max_ps(vecZero, _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(resistances + index))));
					Vec128Float resultFloat     = _mm_div_ps(_mm_mul_ps(floatOutputs, vecConstant), _mm_add_ps(vecConstant, floatResistances));
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), _mm_cvttps_epi32(_mm_max_ps(vecFloor, resultFloat)));
				}

				scalar_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			case RF::ScaledDivision:
			{
				const Vec128Float vecMultiplier = _mm_set1_ps(f.multiplier);
				const Vec128Float vecAddend     = _mm_set1_ps(f.addend);
				const Vec128Float vecZero       = _mm_setzero_ps();
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Float floatOutputs            = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(outputs     + index)));
					Vec128Float floatResistances         = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(resistances + index)));
					// mask where resistance > 0
					Vec128Float positiveResistanceMask  = _mm_cmpgt_ps(floatResistances, vecZero);
					Vec128Float divisionResult          = _mm_add_ps(_mm_div_ps(_mm_mul_ps(floatOutputs, vecMultiplier), floatResistances), vecAddend);
					Vec128Float flatResult              = _mm_add_ps(_mm_mul_ps(floatOutputs, vecMultiplier), vecAddend);
					Vec128Float resultFloat             = _mm_or_ps(_mm_and_ps(positiveResistanceMask, divisionResult), _mm_andnot_ps(positiveResistanceMask, flatResult));
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), _mm_cvttps_epi32(_mm_max_ps(vecFloor, resultFloat)));
				}

				scalar_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			case RF::Layered:
			{
				const Vec128Float vecOne          = _mm_set1_ps(1.0f);
				const Vec128Float vecOneHundred   = _mm_set1_ps(100.0f);
				const Vec128Float vecFlatReduction = _mm_set1_ps(f.flat_reduction);
				size_t index = 0;

				for (; index + 4 <= count; index += 4)
				{
					Vec128Float floatOutputs    = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(outputs     + index)));
					Vec128Float floatResistances = _mm_cvtepi32_ps(_mm_loadu_si128(reinterpret_cast<const Vec128Int*>(resistances + index)));
					Vec128Float mitigationValue = _mm_mul_ps(floatOutputs, _mm_sub_ps(vecOne, _mm_div_ps(floatResistances, vecOneHundred)));
					Vec128Float resultFloat     = _mm_sub_ps(mitigationValue, vecFlatReduction);
					_mm_storeu_si128(reinterpret_cast<Vec128Int*>(out + index), _mm_cvttps_epi32(_mm_max_ps(vecFloor, resultFloat)));
				}

				scalar_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			default:
				std::memset(out, 0, count * sizeof(CombatStat));
				return;
		}
	}

	template<class ResolutionFactors>
	inline void avx2_batch_resolve(const CombatStat* outputs, const CombatStat* resistances, size_t count, const ResolutionFactors& f, CombatStat* out) noexcept
	{
		using RF = decltype(f.formula_type);
		const Vec256Float vecFloor = _mm256_set1_ps(f.floor);

		switch (f.formula_type)
		{
			case RF::Subtractive:
			{
				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Int   packedOutputs    = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(outputs     + index));
					Vec256Int   packedResistances = _mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(resistances + index));
					Vec256Float floatDifference  = _mm256_cvtepi32_ps(_mm256_sub_epi32(packedOutputs, packedResistances));
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), _mm256_cvttps_epi32(_mm256_max_ps(vecFloor, floatDifference)));
				}

				sse2_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			case RF::RatioMitigation:
			{
				const Vec256Float vecConstant = _mm256_set1_ps(f.constant);
				const Vec256Float vecZero     = _mm256_setzero_ps();
				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Float floatOutputs     = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(outputs     + index)));
					Vec256Float floatResistances = _mm256_max_ps(vecZero, _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(resistances + index))));
					Vec256Float resultFloat      = _mm256_div_ps(_mm256_mul_ps(floatOutputs, vecConstant), _mm256_add_ps(vecConstant, floatResistances));
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), _mm256_cvttps_epi32(_mm256_max_ps(vecFloor, resultFloat)));
				}

				sse2_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			case RF::ScaledDivision:
			{
				const Vec256Float vecMultiplier = _mm256_set1_ps(f.multiplier);
				const Vec256Float vecAddend     = _mm256_set1_ps(f.addend);
				const Vec256Float vecZero       = _mm256_setzero_ps();
				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Float floatOutputs           = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(outputs     + index)));
					Vec256Float floatResistances        = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(resistances + index)));
					Vec256Float positiveResistanceMask = _mm256_cmp_ps(floatResistances, vecZero, _CMP_GT_OQ);
					Vec256Float divisionResult         = _mm256_add_ps(_mm256_div_ps(_mm256_mul_ps(floatOutputs, vecMultiplier), floatResistances), vecAddend);
					Vec256Float flatResult             = _mm256_add_ps(_mm256_mul_ps(floatOutputs, vecMultiplier), vecAddend);
					Vec256Float resultFloat            = _mm256_blendv_ps(flatResult, divisionResult, positiveResistanceMask);
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), _mm256_cvttps_epi32(_mm256_max_ps(vecFloor, resultFloat)));
				}

				sse2_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			case RF::Layered:
			{
				const Vec256Float vecOne          = _mm256_set1_ps(1.0f);
				const Vec256Float vecOneHundred   = _mm256_set1_ps(100.0f);
				const Vec256Float vecFlatReduction = _mm256_set1_ps(f.flat_reduction);
				size_t index = 0;

				for (; index + 8 <= count; index += 8)
				{
					Vec256Float floatOutputs     = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(outputs     + index)));
					Vec256Float floatResistances = _mm256_cvtepi32_ps(_mm256_loadu_si256(reinterpret_cast<const Vec256Int*>(resistances + index)));
					Vec256Float mitigationValue  = _mm256_mul_ps(floatOutputs, _mm256_sub_ps(vecOne, _mm256_div_ps(floatResistances, vecOneHundred)));
					Vec256Float resultFloat      = _mm256_sub_ps(mitigationValue, vecFlatReduction);
					_mm256_storeu_si256(reinterpret_cast<Vec256Int*>(out + index), _mm256_cvttps_epi32(_mm256_max_ps(vecFloor, resultFloat)));
				}

				sse2_batch_resolve(outputs + index, resistances + index, count - index, f, out + index);
				return;
			}

			default:
				std::memset(out, 0, count * sizeof(CombatStat));
				return;
		}
	}
#endif

} // namespace detail

template<class ResolutionFactors>
inline void batch_resolve(const CombatStat* outputs, const CombatStat* resistances, size_t count, const ResolutionFactors& factors, CombatStat* out) noexcept
{
	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
		switch (g_level)
		{
			case Level::AVX2:
				detail::avx2_batch_resolve(outputs, resistances, count, factors, out);
				return;
			case Level::SSE4_1:
			case Level::SSE2:
				detail::sse2_batch_resolve(outputs, resistances, count, factors, out);
				return;
			default: break;
		}
	#endif

	detail::scalar_batch_resolve(outputs, resistances, count, factors, out);
}

} // namespace BlackTek::SIMD
