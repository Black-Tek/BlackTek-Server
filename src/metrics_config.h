// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <cstddef>

namespace BlackTek::Metrics
{
    // Setting this false removes all metrics code at compile time.
    inline constexpr bool ENABLED = false;

    // Not populated for batched/AoE strikes (block_batch path).
    inline constexpr bool CAPTURE_PIPELINE = true;

    inline constexpr bool CAPTURE_FORMULAS = true;

    inline constexpr bool CAPTURE_MODIFIERS = true;

    inline constexpr bool CAPTURE_HEALING = true;

    inline constexpr bool CAPTURE_CONDITIONS = true;

    // No heap allocation on the capture path.
    inline constexpr std::size_t STRIKE_BUFFER_SIZE    = 16'384;
    inline constexpr std::size_t MODIFIER_BUFFER_SIZE   = 32'768;
    inline constexpr std::size_t HEAL_BUFFER_SIZE       =  8'192;
    inline constexpr std::size_t CONDITION_BUFFER_SIZE  = 32'768;

    // Sentinel for bind_key when the formula bind cannot be determined.
    inline constexpr unsigned char NO_BIND_KEY = 0xFF;
}
