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

#include "metrics_types.h"

#include <array>
#include <cstddef>
#include <optional>
#include <vector>
#include <gtl/phmap.hpp>

namespace BlackTek::Metrics
{
    // Fixed-capacity ring buffer; no heap allocation on the capture path.
    // All Push() callers run on the dispatcher thread, so no locks are needed.
    template <typename T, std::size_t N>
    class RingBuffer
    {
    public:
        void Push(T&& item) noexcept
        {
            buffer_[write_index_ % N] = std::move(item);
            ++write_index_;
            if (count_ < N)
                ++count_;
        }

        void Push(const T& item) noexcept
        {
            buffer_[write_index_ % N] = item;
            ++write_index_;
            if (count_ < N)
                ++count_;
        }

        // Returns up to `count_` elements, oldest first.
        [[nodiscard]] std::vector<T> Snapshot() const
        {
            std::vector<T> elements;
            elements.reserve(count_);

            const std::size_t start = (count_ < N) ? 0 : (write_index_ % N);

            for (std::size_t elementIndex = 0; elementIndex < count_; ++elementIndex)
                elements.push_back(buffer_[(start + elementIndex) % N]);

            return elements;
        }

        void Reset() noexcept
        {
            write_index_ = 0;
            count_ = 0;
        }

        [[nodiscard]] std::size_t Size() const noexcept { return count_; }

    private:
        std::array<T, N> buffer_{};
        std::size_t write_index_ = 0;
        std::size_t count_ = 0;
    };

    enum class ResetScope : uint8_t
    {
        Session,
        Window,
        Formulas,
        All
    };

    // Owns the ring buffers and running-aggregate maps. Dispatcher-thread-only.
    class AggregateStore
    {
    public:
        void RecordStrike(const StrikeRecord& record, uint32_t modifier_proc_count) noexcept;
        void RecordModifier(const ModifierRecord& record) noexcept;
        void RecordHeal(const HealRecord& record) noexcept;
        void RecordCondition(const ConditionRecord& record) noexcept;

        bool UpdateFormulaUsage(int64_t combat_id, const FormulaSnapshot& snapshot, uint64_t timestamp_ms) noexcept;

        [[nodiscard]] std::optional<FormulaSignature> GetFormulaSignature(int64_t combat_id) const;

        [[nodiscard]] std::vector<AggregateData> SnapshotStrikeAggregates() const;
        [[nodiscard]] std::vector<HealAggregateData> SnapshotHealAggregates() const;
        [[nodiscard]] std::vector<ConditionAggregateData> SnapshotConditionAggregates() const;
        [[nodiscard]] std::vector<ModifierAggregateData> SnapshotModifierAggregates() const;
        [[nodiscard]] std::vector<FormulaSignature> SnapshotFormulaUsage() const;

        [[nodiscard]] std::vector<StrikeRecord> SnapshotStrikes() const { return strike_ring_.Snapshot(); }
        [[nodiscard]] std::vector<ModifierRecord> SnapshotModifiers() const { return modifier_ring_.Snapshot(); }
        [[nodiscard]] std::vector<HealRecord> SnapshotHeals() const { return heal_ring_.Snapshot(); }
        [[nodiscard]] std::vector<ConditionRecord> SnapshotConditions() const { return condition_ring_.Snapshot(); }

        void Reset(ResetScope scope) noexcept;

    private:
        RingBuffer<StrikeRecord, STRIKE_BUFFER_SIZE> strike_ring_;
        RingBuffer<ModifierRecord, MODIFIER_BUFFER_SIZE> modifier_ring_;
        RingBuffer<HealRecord, HEAL_BUFFER_SIZE> heal_ring_;
        RingBuffer<ConditionRecord, CONDITION_BUFFER_SIZE> condition_ring_;

        gtl::flat_hash_map<uint64_t, AggregateData> strike_aggregates_;
        gtl::flat_hash_map<uint64_t, HealAggregateData> heal_aggregates_;
        gtl::flat_hash_map<uint64_t, ConditionAggregateData> condition_aggregates_;
        gtl::flat_hash_map<uint16_t, ModifierAggregateData> modifier_aggregates_;
        gtl::flat_hash_map<int64_t, FormulaSignature> formula_usage_map_;
    };

    extern AggregateStore g_metrics_store;
}
