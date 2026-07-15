// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <optional>

#include "declarations.h"
#include "enums.h"

class CreatureContainer final
{
    public:
        CreatureContainer() = default;

        // non-copyable
        CreatureContainer(const CreatureContainer&) = delete;
        CreatureContainer& operator=(const CreatureContainer&) = delete;

        [[nodiscard]] TilePtr getOwner() const { return owner.lock(); }

        [[nodiscard]] const CreatureVector& getList() const { return creatureList; }
        [[nodiscard]] size_t size() const noexcept { return creatureList.size(); }
        [[nodiscard]] bool empty() const noexcept { return creatureList.empty(); }

        [[nodiscard]] bool hasCreature(const CreatureConstPtr& creature) const;
        [[nodiscard]] int32_t getCreatureIndex(const CreatureConstPtr& creature) const;

        [[nodiscard]] CreaturePtr getTopCreature() const;
        [[nodiscard]] CreatureConstPtr getBottomCreature() const;
        [[nodiscard]] CreaturePtr getTopVisibleCreature(const CreaturePtr& creature) const;
        [[nodiscard]] CreatureConstPtr getBottomVisibleCreature(const CreatureConstPtr& creature) const;

        [[nodiscard]] ReturnValue canEnter(const PlayerPtr& player, uint32_t flags) const;
        [[nodiscard]] ReturnValue canEnter(const MonsterPtr& monster, uint32_t flags) const;
        [[nodiscard]] ReturnValue canEnter(const NpcPtr& npc, uint32_t flags) const;

        void addCreature(const CreaturePtr& creature);
        void addCreatureSilently(const CreaturePtr& creature);
        void removeCreature(const CreaturePtr& creature);

    private:
        std::optional<ReturnValue> checkEntryRestrictions(uint32_t flags) const;

        CreatureVector creatureList;
        TileWeakPtr owner;

        friend class Tile;
};
