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
