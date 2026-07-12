// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "declarations.h"

struct StackposResolution
{
    CreaturePtr creature;
    ItemPtr item;

    [[nodiscard]] explicit operator bool() const noexcept { return creature or item; }
};
