// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <cstdint>

namespace BlackTek
{
    template <typename Tag>
    struct Handle
    {
        static constexpr uint32_t INVALID_INDEX = 0xFFFFFFFFu;

        uint32_t slot_index = INVALID_INDEX;
        uint32_t generation = 0;

        [[nodiscard]] constexpr bool IsValid() const noexcept
        {
            return slot_index != INVALID_INDEX;
        }

        friend constexpr bool operator==(const Handle&, const Handle&) = default;
    };
}
