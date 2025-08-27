// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_ITEMLOADER_H
#define FS_ITEMLOADER_H

#include <cstdint>

enum itemgroup_t {
	ITEM_GROUP_NONE,

	ITEM_GROUP_GROUND,
	ITEM_GROUP_CONTAINER,
	ITEM_GROUP_WEAPON, //deprecated
	ITEM_GROUP_AMMUNITION, //deprecated
	ITEM_GROUP_ARMOR, //deprecated
	ITEM_GROUP_CHARGES,
	ITEM_GROUP_TELEPORT, //deprecated
	ITEM_GROUP_MAGICFIELD, //deprecated
	ITEM_GROUP_WRITEABLE, //deprecated
	ITEM_GROUP_KEY, //deprecated
	ITEM_GROUP_SPLASH,
	ITEM_GROUP_FLUID,
	ITEM_GROUP_DOOR, //deprecated
	ITEM_GROUP_DEPRECATED,
	ITEM_GROUP_PODIUM,

	ITEM_GROUP_LAST
};

enum ItemDatFlag : uint8_t {
	Ground            = 0,
	GroundBorder      = 1,
	OnBottom          = 2,
	OnTop             = 3,
	Container         = 4,
	Stackable         = 5,
	ForceUse          = 6,
	MultiUse          = 7,
	Writable          = 8,
	WritableOnce      = 9,
	FluidContainer    = 10,
	Fluid             = 11,
	IsUnpassable      = 12,
	IsUnmoveable      = 13,
	BlockMissiles     = 14,
	BlockPathfinder   = 15,
	NoMoveAnimation   = 16,
	Pickupable        = 17,
	Hangable          = 18,
	IsHorizontal      = 19,
	IsVertical        = 20,
	Rotatable         = 21,
	HasLight          = 22,
	DontHide          = 23,
	Translucent       = 24,
	HasOffset         = 25,
	HasElevation      = 26,
	Lying             = 27,
	AnimateAlways     = 28,
	Minimap           = 29,
	LensHelp          = 30,
	FullGround        = 31,
	IgnoreLook        = 32,
	Cloth             = 33,
	Market            = 34,
	DefaultAction     = 35,
	Wrappable         = 36,
	Unwrappable       = 37,
	TopEffect         = 38,
	Usable            = 254,
	LastFlag          = 255
};

#endif
