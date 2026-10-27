/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Upstream Copyright:
 *   Copyright (C) The Forgotten Server Authors
 *   Governed by the GNU General Public License v2.0 (GPL-2.0)
 *
 * Modifications & BlackTek Core Additions:
 *   Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * ============================================================================
 * DUAL LICENSING & TRANSITION NOTICE
 * ============================================================================
 *
 * This file contains original code from The Forgotten Server alongside substantial
 * modifications and new architecture by BlackTek.
 *
 * To facilitate the project's long-term goal of transitioning the codebase to a
 * fully permissive MIT License:
 *
 * 1. All original TFS source lines remain governed by GPL-2.0.
 * 2. All contributions, modifications, and new additions authored by BlackTek
 *    are dual-licensed under both the MIT License and GPL-2.0.
 * 3. Downstream users distributing this combined codebase must comply with
 *    the GPL-2.0 requirements as long as original upstream TFS code remains.
 *
 * Once an individual module or file has been completely refactored or rewritten
 * such that no original TFS code remains, the file will be relicensed solely
 * under the MIT License.
 *
 * See licensing/original codebase/GNU-GPL (GPL-2.0) and licensing/License (MIT)
 * in the project root for the full texts.
 */

#pragma once

#include <vector>
#include <span>
#include <ranges>

#include "declarations.h"

class SpectatorVec
{
    using Vec           = std::vector<CreaturePtr>;
    using Iterator      = Vec::iterator;
    using ConstIterator = Vec::const_iterator;

public:
    SpectatorVec() { vec_.reserve(32); }

    std::span<const CreaturePtr> players() const noexcept
    {
        return { vec_.data(), player_end_ };
    }

    std::span<const CreaturePtr> monsters() const noexcept
    {
        return { vec_.data() + player_end_, static_cast<size_t>(monster_end_ - player_end_) };
    }

    std::span<const CreaturePtr> npcs() const noexcept
    {
        return { vec_.data() + monster_end_, vec_.size() - monster_end_ };
    }

    Iterator begin() noexcept { return vec_.begin(); }
    ConstIterator begin() const noexcept { return vec_.begin(); }
    Iterator end() noexcept { return vec_.end(); }
    ConstIterator end() const noexcept { return vec_.end(); }
    size_t size() const noexcept { return vec_.size(); }
    bool empty() const noexcept { return vec_.empty(); }
    CreaturePtr operator[](uint8_t index) const { return vec_[index]; }
    void emplace_back(const CreaturePtr& c) { vec_.emplace_back(c); }

    void clear() noexcept
    {
        vec_.clear();
        player_end_  = 0;
        monster_end_ = 0;
    }

    void erase(const CreaturePtr& spectator)
    {
        const auto it = std::ranges::find(vec_, spectator);

        if (it == vec_.end())
        {
            return;
        }

        const auto idx = static_cast<uint16_t>(it - vec_.begin());
        vec_.erase(it);

        if (idx < player_end_)
        {
            --player_end_;
            --monster_end_;
        }
        else if (idx < monster_end_)
        {
            --monster_end_;
        }
    }

    void addSpectators(const SpectatorVec& other);

    void partitionByType();
    void setPlayersOnlyMode() noexcept
    {
        player_end_  = static_cast<uint16_t>(vec_.size());
        monster_end_ = player_end_;
    }

private:
    Vec      vec_;
    uint16_t player_end_  = 0;
    uint16_t monster_end_ = 0;
};
