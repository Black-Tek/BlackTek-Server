// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "thing.h"
#include "tile.h"

const Position& Thing::getPosition() const
{
	const auto tile = getTile();
	if (!tile) {
		static Position null_position(0xFFFF, 0xFFFF, 0xFF);
		return null_position;
	}
	return tile->getPosition();
}

std::shared_ptr<Tile> Thing::getTile() {
	return nullptr;
}

std::shared_ptr<const Tile> Thing::getTile() const {
	return nullptr;
}
