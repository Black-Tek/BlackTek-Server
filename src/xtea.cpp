// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "xtea.h"
#include "simd_dispatch.h"

#include <array>
#include <assert.h>

#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
#include <immintrin.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define BT_XTEA_TARGET_AVX2 __attribute__((target("avx2")))
#  define BT_XTEA_TARGET_SSE2 __attribute__((target("sse2")))
#else
#  define BT_XTEA_TARGET_AVX2
#  define BT_XTEA_TARGET_SSE2
#endif

namespace xtea {

round_keys expand_key(const key& k)
{
	constexpr uint32_t delta = 0x9E3779B9;
	round_keys expanded;

	for (uint32_t i = 0, sum = 0, next_sum = sum + delta; i < expanded.size(); i += 2, sum = next_sum, next_sum += delta) {
		expanded[i] = sum + k[sum & 3];
		expanded[i + 1] = next_sum + k[(next_sum >> 11) & 3];
	}

	return expanded;
}

namespace
{

	inline void scalar_encrypt_block(uint32_t& left, uint32_t& right, const round_keys& k)
	{
		for (int32_t i = 0; i < static_cast<int32_t>(k.size()); i += 2) {
			left += ((right << 4 ^ right >> 5) + right) ^ k[i];
			right += ((left << 4 ^ left >> 5) + left) ^ k[i + 1];
		}
	}

	inline void scalar_decrypt_block(uint32_t& left, uint32_t& right, const round_keys& k)
	{
		for (int32_t i = static_cast<int32_t>(k.size()) - 1; i > 0; i -= 2) {
			right -= ((left << 4 ^ left >> 5) + left) ^ k[i];
			left -= ((right << 4 ^ right >> 5) + right) ^ k[i - 1];
		}
	}

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)

	BT_XTEA_TARGET_SSE2 inline void sse2_encrypt4(uint8_t* data, const round_keys& k)
	{
		__m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data));
		__m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + 16));

		__m128i v0s = _mm_shuffle_epi32(v0, _MM_SHUFFLE(3, 1, 2, 0));
		__m128i v1s = _mm_shuffle_epi32(v1, _MM_SHUFFLE(3, 1, 2, 0));

		__m128i lefts  = _mm_unpacklo_epi64(v0s, v1s);
		__m128i rights = _mm_unpackhi_epi64(v0s, v1s);

		for (int32_t i = 0; i < static_cast<int32_t>(k.size()); i += 2)
		{
			__m128i keyA = _mm_set1_epi32(static_cast<int32_t>(k[i]));
			__m128i keyB = _mm_set1_epi32(static_cast<int32_t>(k[i + 1]));

			__m128i newLefts = _mm_add_epi32(lefts,
				_mm_xor_si128(
					_mm_add_epi32(_mm_xor_si128(_mm_slli_epi32(rights, 4), _mm_srli_epi32(rights, 5)), rights),
					keyA));

			__m128i newRights = _mm_add_epi32(rights,
				_mm_xor_si128(
					_mm_add_epi32(_mm_xor_si128(_mm_slli_epi32(newLefts, 4), _mm_srli_epi32(newLefts, 5)), newLefts),
					keyB));

			lefts = newLefts;
			rights = newRights;
		}

		__m128i outV0 = _mm_unpacklo_epi32(lefts, rights);
		__m128i outV1 = _mm_unpackhi_epi32(lefts, rights);

		_mm_storeu_si128(reinterpret_cast<__m128i*>(data), outV0);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(data + 16), outV1);
	}

	BT_XTEA_TARGET_SSE2 inline void sse2_decrypt4(uint8_t* data, const round_keys& k)
	{
		__m128i v0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data));
		__m128i v1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + 16));

		__m128i v0s = _mm_shuffle_epi32(v0, _MM_SHUFFLE(3, 1, 2, 0));
		__m128i v1s = _mm_shuffle_epi32(v1, _MM_SHUFFLE(3, 1, 2, 0));

		__m128i lefts  = _mm_unpacklo_epi64(v0s, v1s);
		__m128i rights = _mm_unpackhi_epi64(v0s, v1s);

		for (int32_t i = static_cast<int32_t>(k.size()) - 1; i > 0; i -= 2)
		{
			__m128i keyA = _mm_set1_epi32(static_cast<int32_t>(k[i]));
			__m128i keyB = _mm_set1_epi32(static_cast<int32_t>(k[i - 1]));

			__m128i newRights = _mm_sub_epi32(rights,
				_mm_xor_si128(
					_mm_add_epi32(_mm_xor_si128(_mm_slli_epi32(lefts, 4), _mm_srli_epi32(lefts, 5)), lefts),
					keyA));

			__m128i newLefts = _mm_sub_epi32(lefts,
				_mm_xor_si128(
					_mm_add_epi32(_mm_xor_si128(_mm_slli_epi32(newRights, 4), _mm_srli_epi32(newRights, 5)), newRights),
					keyB));

			lefts = newLefts;
			rights = newRights;
		}

		__m128i outV0 = _mm_unpacklo_epi32(lefts, rights);
		__m128i outV1 = _mm_unpackhi_epi32(lefts, rights);

		_mm_storeu_si128(reinterpret_cast<__m128i*>(data), outV0);
		_mm_storeu_si128(reinterpret_cast<__m128i*>(data + 16), outV1);
	}

	BT_XTEA_TARGET_AVX2 inline void avx2_encrypt8(uint8_t* data, const round_keys& k)
	{
		__m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
		__m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + 32));

		__m256i v0s = _mm256_shuffle_epi32(v0, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i v1s = _mm256_shuffle_epi32(v1, _MM_SHUFFLE(3, 1, 2, 0));

		__m256i v0p = _mm256_permute4x64_epi64(v0s, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i v1p = _mm256_permute4x64_epi64(v1s, _MM_SHUFFLE(3, 1, 2, 0));

		__m256i lefts  = _mm256_permute2x128_si256(v0p, v1p, 0x20);
		__m256i rights = _mm256_permute2x128_si256(v0p, v1p, 0x31);

		for (int32_t i = 0; i < static_cast<int32_t>(k.size()); i += 2)
		{
			__m256i keyA = _mm256_set1_epi32(static_cast<int32_t>(k[i]));
			__m256i keyB = _mm256_set1_epi32(static_cast<int32_t>(k[i + 1]));

			__m256i newLefts = _mm256_add_epi32(lefts,
				_mm256_xor_si256(
					_mm256_add_epi32(_mm256_xor_si256(_mm256_slli_epi32(rights, 4), _mm256_srli_epi32(rights, 5)), rights),
					keyA));

			__m256i newRights = _mm256_add_epi32(rights,
				_mm256_xor_si256(
					_mm256_add_epi32(_mm256_xor_si256(_mm256_slli_epi32(newLefts, 4), _mm256_srli_epi32(newLefts, 5)), newLefts),
					keyB));

			lefts = newLefts;
			rights = newRights;
		}

		__m256i outV0p = _mm256_permute2x128_si256(lefts, rights, 0x20);
		__m256i outV1p = _mm256_permute2x128_si256(lefts, rights, 0x31);

		__m256i outV0s = _mm256_permute4x64_epi64(outV0p, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i outV1s = _mm256_permute4x64_epi64(outV1p, _MM_SHUFFLE(3, 1, 2, 0));

		__m256i outV0 = _mm256_shuffle_epi32(outV0s, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i outV1 = _mm256_shuffle_epi32(outV1s, _MM_SHUFFLE(3, 1, 2, 0));

		_mm256_storeu_si256(reinterpret_cast<__m256i*>(data), outV0);
		_mm256_storeu_si256(reinterpret_cast<__m256i*>(data + 32), outV1);
	}

	BT_XTEA_TARGET_AVX2 inline void avx2_decrypt8(uint8_t* data, const round_keys& k)
	{
		__m256i v0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
		__m256i v1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + 32));

		__m256i v0s = _mm256_shuffle_epi32(v0, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i v1s = _mm256_shuffle_epi32(v1, _MM_SHUFFLE(3, 1, 2, 0));

		__m256i v0p = _mm256_permute4x64_epi64(v0s, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i v1p = _mm256_permute4x64_epi64(v1s, _MM_SHUFFLE(3, 1, 2, 0));

		__m256i lefts  = _mm256_permute2x128_si256(v0p, v1p, 0x20);
		__m256i rights = _mm256_permute2x128_si256(v0p, v1p, 0x31);

		for (int32_t i = static_cast<int32_t>(k.size()) - 1; i > 0; i -= 2)
		{
			__m256i keyA = _mm256_set1_epi32(static_cast<int32_t>(k[i]));
			__m256i keyB = _mm256_set1_epi32(static_cast<int32_t>(k[i - 1]));

			__m256i newRights = _mm256_sub_epi32(rights,
				_mm256_xor_si256(
					_mm256_add_epi32(_mm256_xor_si256(_mm256_slli_epi32(lefts, 4), _mm256_srli_epi32(lefts, 5)), lefts),
					keyA));

			__m256i newLefts = _mm256_sub_epi32(lefts,
				_mm256_xor_si256(
					_mm256_add_epi32(_mm256_xor_si256(_mm256_slli_epi32(newRights, 4), _mm256_srli_epi32(newRights, 5)), newRights),
					keyB));

			lefts = newLefts;
			rights = newRights;
		}

		__m256i outV0p = _mm256_permute2x128_si256(lefts, rights, 0x20);
		__m256i outV1p = _mm256_permute2x128_si256(lefts, rights, 0x31);

		__m256i outV0s = _mm256_permute4x64_epi64(outV0p, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i outV1s = _mm256_permute4x64_epi64(outV1p, _MM_SHUFFLE(3, 1, 2, 0));

		__m256i outV0 = _mm256_shuffle_epi32(outV0s, _MM_SHUFFLE(3, 1, 2, 0));
		__m256i outV1 = _mm256_shuffle_epi32(outV1s, _MM_SHUFFLE(3, 1, 2, 0));

		_mm256_storeu_si256(reinterpret_cast<__m256i*>(data), outV0);
		_mm256_storeu_si256(reinterpret_cast<__m256i*>(data + 32), outV1);
	}

	#endif

	} // namespace

	void encrypt(uint8_t* data, size_t length, const round_keys& k)
	{
		size_t blocksRemaining = length / 8;
		uint8_t* cursor = data;

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
		if (BlackTek::SIMD::g_level == BlackTek::SIMD::Level::AVX2)
		{
			while (blocksRemaining >= 8)
			{
				avx2_encrypt8(cursor, k);
				cursor += 64;
				blocksRemaining -= 8;
			}
		}

		if (BlackTek::SIMD::g_level != BlackTek::SIMD::Level::Scalar)
		{
			while (blocksRemaining >= 4)
			{
				sse2_encrypt4(cursor, k);
				cursor += 32;
				blocksRemaining -= 4;
			}
		}
	#endif

		while (blocksRemaining > 0)
		{
			uint32_t left, right;
			std::memcpy(&left, cursor, 4);
			std::memcpy(&right, cursor + 4, 4);

			scalar_encrypt_block(left, right, k);

			std::memcpy(cursor, &left, 4);
			std::memcpy(cursor + 4, &right, 4);

			cursor += 8;
			--blocksRemaining;
		}
	}

	void decrypt(uint8_t* data, size_t length, const round_keys& k)
	{
		size_t blocksRemaining = length / 8;
		uint8_t* cursor = data;

	#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__)
		if (BlackTek::SIMD::g_level == BlackTek::SIMD::Level::AVX2)
		{
			while (blocksRemaining >= 8)
			{
				avx2_decrypt8(cursor, k);
				cursor += 64;
				blocksRemaining -= 8;
			}
		}

		if (BlackTek::SIMD::g_level != BlackTek::SIMD::Level::Scalar)
		{
			while (blocksRemaining >= 4)
			{
				sse2_decrypt4(cursor, k);
				cursor += 32;
				blocksRemaining -= 4;
			}
		}
	#endif

		while (blocksRemaining > 0)
		{
			uint32_t left, right;
			std::memcpy(&left, cursor, 4);
			std::memcpy(&right, cursor + 4, 4);

			scalar_decrypt_block(left, right, k);

			std::memcpy(cursor, &left, 4);
			std::memcpy(cursor + 4, &right, 4);

			cursor += 8;
			--blocksRemaining;
		}
	}
} // namespace xtea
