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
