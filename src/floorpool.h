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
