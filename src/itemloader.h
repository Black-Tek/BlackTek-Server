/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Upstream Copyright:
 *   Copyright (C) The Forgotten Server Authors
 *   Governed by the GNU General Public License v2.0 (GPL-2.0)
 *
 * Modifications & BlackTek Core Additions:
 *   Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * ============================================================================
 * DUAL LICENSING & TRANSITION NOTICE
 * ============================================================================
 *
 * This file contains original code from The Forgotten Server alongside substantial
 * modifications and new architecture by BlackTek.
 *
 * To facilitate the project's long-term goal of transitioning the codebase to a
 * fully permissive MIT License:
 *
 * 1. All original TFS source lines remain governed by GPL-2.0.
 * 2. All contributions, modifications, and new additions authored by BlackTek
 *    are dual-licensed under both the MIT License and GPL-2.0.
 * 3. Downstream users distributing this combined codebase must comply with
 *    the GPL-2.0 requirements as long as original upstream TFS code remains.
 *
 * Once an individual module or file has been completely refactored or rewritten
 * such that no original TFS code remains, the file will be relicensed solely
 * under the MIT License.
 *
 * See licensing/original codebase/GNU-GPL (GPL-2.0) and licensing/License (MIT)
 * in the project root for the full texts.
 */

#pragma once

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

enum class ItemDatFlag : uint8_t {
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