// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <vector>
#include <span>
#include <ranges>

class Creature;

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

    void addSpectators(const SpectatorVec& spectators)
    {
        for (const CreaturePtr& c : spectators.vec_)
        {
            if (std::ranges::find(vec_, c) == vec_.end())
                vec_.emplace_back(c);
        }
        partitionByType();
    }

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
