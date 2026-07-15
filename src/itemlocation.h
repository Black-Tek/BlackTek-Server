// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "declarations.h"

namespace BlackTek
{
    struct ItemLocation
    {
        TilePtr tile;
        PlayerPtr player;
        ItemPtr containerItem;

        [[nodiscard]] explicit operator bool() const noexcept { return tile or player or containerItem; }

        friend bool operator==(const ItemLocation&, const ItemLocation&) = default;
    };
}
