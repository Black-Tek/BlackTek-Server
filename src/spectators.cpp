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

#include "otpch.h"

#include "spectators.h"
#include "creature.h"
#include <gtl/phmap.hpp>

namespace
{
    constexpr size_t SMALL_MERGE_LIMIT = 64;
}

void SpectatorVec::addSpectators(const SpectatorVec& other)
{
    if (other.empty())
    {
        return;
    }

    if (vec_.empty())
    {
        *this = other;
        return;
    }

    if (vec_.size() <= SMALL_MERGE_LIMIT)
    {
        for (const auto& c : other.vec_)
        {
            if (std::ranges::find(vec_, c) == vec_.end())
            {
                vec_.emplace_back(c);
            }
        }
    }
    else
    {
        gtl::flat_hash_set<CreaturePtr> seen;
        seen.reserve(vec_.size() + other.vec_.size());
        seen.insert(vec_.begin(), vec_.end());
        for (const auto& c : other.vec_)
        {
            if (seen.insert(c).second)
            {
                vec_.emplace_back(c);
            }
        }
    }
    partitionByType();
}

void SpectatorVec::partitionByType()
{
    auto mid1 = std::partition(vec_.begin(), vec_.end(), [](const CreaturePtr& c)
    {
        return c->getCreatureSubType() == CreatureSubType::Player;
    });
    player_end_ = static_cast<uint16_t>(mid1 - vec_.begin());

    auto mid2 = std::partition(mid1, vec_.end(), [](const CreaturePtr& c)
    {
        return c->getCreatureSubType() == CreatureSubType::Monster;
    });
    monster_end_ = static_cast<uint16_t>(mid2 - vec_.begin());
}
