// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "spectators.h"
#include "creature.h"

// partitionByType is defined here rather than inline in spectators.h because
// spectators.h is pulled in via tile.h which is pulled in via creature.h,
// forming a cycle that prevents creature.h from being included in spectators.h.
// By the time this translation unit is compiled, Creature is fully defined.
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
