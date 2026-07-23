// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "floorpool.h"

namespace BlackTek::World
{
    FloorHandle FloorPool::Allocate()
    {
        const uint32_t slotIndex = static_cast<uint32_t>(slots.size());
        slots.emplace_back();
        slots[slotIndex].occupied = true;
        return FloorHandle{ slotIndex, slots[slotIndex].generation };
    }

    Floor* FloorPool::Get(FloorHandle handle)
    {
        if (not handle.IsValid() or handle.slot_index >= slots.size())
            return nullptr;

        Slot& slot = slots[handle.slot_index];

        if (not slot.occupied or slot.generation != handle.generation)
            return nullptr;

        return &slot.floor;
    }

    const Floor* FloorPool::Get(FloorHandle handle) const
    {
        if (not handle.IsValid() or handle.slot_index >= slots.size())
            return nullptr;

        const Slot& slot = slots[handle.slot_index];

        if (not slot.occupied or slot.generation != handle.generation)
            return nullptr;

        return &slot.floor;
    }
}
