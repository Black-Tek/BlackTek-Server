// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "thing.h"
#include "tile.h"

const Position& Thing::getPosition() const
{
	const auto tile = getTile();
	if (!tile) {
		return std::make_shared<StaticTile>(0xFFFF, 0xFFFF, 0xFF)->getPosition();
	}
	return tile->getPosition();
}

TilePtr Thing::getTile() {
	return nullptr;
}

TileConstPtr Thing::getTile() const {
	return nullptr;
}
