// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <cstddef>
#include <cstdint>

namespace BlackTek::Net
{
    struct KnownCreatureResult
    {
        bool known = false;
        uint32_t evictedId = 0;
    };

    template <std::size_t Capacity, std::size_t TableSize>
    class KnownCreatureCache
    {
        static_assert((TableSize & (TableSize - 1)) == 0, "TableSize must be a power of two");
        static_assert(TableSize > Capacity, "TableSize must exceed Capacity to keep probe sequences short! Recommended max load is 70 percent!");

        static constexpr int IndexBits = std::bit_width(TableSize - 1);
        static_assert(IndexBits <= 32, "TableSize must fit within 32-bit hash output");

        public:
            KnownCreatureCache()
            {
                slot_ids.fill(0);
            }

            KnownCreatureResult Insert(uint32_t id)
            {
                std::size_t slot;
                if (Find(id, slot))
                {
                    referenced.set(slot);
                    return { true, 0 };
                }

                if (used_slots < Capacity)
                {
                    slot = FirstFreeSlotFrom(HomeSlot(id));
                    PlaceInSlot(slot, id);
                    ++used_slots;
                    return { false, 0 };
                }

                const auto victimSlot = Evict();
                const auto evictedId = slot_ids[victimSlot];
                RemoveFromSlot(victimSlot);

                slot = FirstFreeSlotFrom(HomeSlot(id));
                PlaceInSlot(slot, id);
                ++used_slots;
                return { false, evictedId };
            }

            void Clear()
            {
                slot_ids.fill(0);
                referenced.reset();
                used_slots = 0;
                clock_hand = 0;
            }

        private:
            static std::size_t HomeSlot(uint32_t id)
            {
                constexpr uint32_t fibonacciMultiplier = 0x9E3779B1u;
                return static_cast<std::size_t>((id * fibonacciMultiplier) >> (32 - IndexBits));
            }

            bool Find(uint32_t id, std::size_t& outSlot) const
            {
                auto slot = HomeSlot(id);

                for (std::size_t probes = 0; probes < TableSize; ++probes)
                {
                    if (slot_ids[slot] == 0)
                    {
                        return false;
                    }

                    if (slot_ids[slot] == id)
                    {
                        outSlot = slot;
                        return true;
                    }

                    slot = (slot + 1) & (TableSize - 1);
                }

                return false;
            }

            std::size_t FirstFreeSlotFrom(std::size_t slot) const
            {
                while (slot_ids[slot] != 0)
                {
                    slot = (slot + 1) & (TableSize - 1);
                }

                return slot;
            }

            void PlaceInSlot(std::size_t slot, uint32_t id)
            {
                slot_ids[slot] = id;
                referenced.set(slot);
            }

            // after vacating deletedSlot, we pull forward any later entry
            // whose own probe sequence would otherwise be interrupted
            // by the new gap, so, no surviving entry ends up unreachable
            void RemoveFromSlot(std::size_t deletedSlot)
            {
                slot_ids[deletedSlot] = 0;
                referenced.reset(deletedSlot);
                --used_slots;

                auto gap = deletedSlot;
                auto scan = (deletedSlot + 1) & (TableSize - 1);

                while (slot_ids[scan] != 0)
                {
                    const auto home = HomeSlot(slot_ids[scan]);
                    const auto gapDistance = (gap + TableSize - home) & (TableSize - 1);
                    const auto scanDistance = (scan + TableSize - home) & (TableSize - 1);

                    if (gapDistance <= scanDistance)
                    {
                        slot_ids[gap] = slot_ids[scan];
                        referenced.set(gap, referenced.test(scan));

                        slot_ids[scan] = 0;
                        referenced.reset(scan);
                        gap = scan;
                    }

                    scan = (scan + 1) & (TableSize - 1);
                }
            }

            // Sweeps forward from clock_hand, giving every referenced slot a
            // second chance (clearing the bit) before evicting the first
            // occupied, unreferenced slot it finds. Empty slots are skipped,
            // they exist only for the extra probe sequence headroom and are never
            // eviction candidates.
            std::size_t Evict()
            {
                while (true)
                {
                    if (slot_ids[clock_hand] != 0)
                    {
                        if (not referenced.test(clock_hand))
                        {
                            const auto victim = clock_hand;
                            clock_hand = (clock_hand + 1) & (TableSize - 1);
                            return victim;
                        }

                        referenced.reset(clock_hand);
                    }

                    clock_hand = (clock_hand + 1) & (TableSize - 1);
                }
            }

            std::array<uint32_t, TableSize> slot_ids{};
            std::bitset<TableSize> referenced;
            std::size_t used_slots = 0;
            std::size_t clock_hand = 0;
    };
}
