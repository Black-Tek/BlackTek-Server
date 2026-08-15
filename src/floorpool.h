// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "declarations.h"
#include "handle.h"

#include <cstdint>
#include <deque>

namespace BlackTek::World
{

    static constexpr int32_t MaxLayers = 16;
    struct FloorTag {};
    using FloorHandle = BlackTek::Handle<FloorTag>;

    struct Floor
    {    
        static constexpr int32_t Bits = 3;
        static constexpr int32_t Size = (1 << Bits);
        static constexpr int32_t Mask = (Size - 1);


        TilePtr tiles[Size][Size] = {};
    };

    class FloorPool
    {
        public:
            FloorHandle Allocate();

            [[nodiscard]] Floor* Get(FloorHandle handle);
            [[nodiscard]] const Floor* Get(FloorHandle handle) const;

        private:
            struct Slot
            {
                Floor floor;
                uint32_t generation = 0;
                bool occupied = false;
            };

            std::deque<Slot> slots;
    };
}
