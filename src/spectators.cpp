// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

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
