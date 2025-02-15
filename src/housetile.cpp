// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "housetile.h"
#include "house.h"
#include "game.h"
#include "configmanager.h"

extern Game g_game;
extern ConfigManager g_config;

HouseTile::HouseTile(const int32_t x, int32_t y, const int32_t z, House* house) :
	DynamicTile(x, y, z), house(house) {}

void HouseTile::addThing(int32_t index, ThingPtr thing)
{
	Tile::addThing(index, thing);

	if (!thing->getParent()) {
		return;
	}

	if (const auto item = thing->getItem()) {
		updateHouse(item);
	}
}

void HouseTile::internalAddThing(uint32_t index, ThingPtr thing)
{
	Tile::internalAddThing(index, thing);

	if (!thing->getParent()) {
		return;
	}

	if (const auto item = thing->getItem()) {
		updateHouse(item);
	}
}

void HouseTile::updateHouse(const ItemPtr& item)
{
	if (item->getParent() != getParent()) {
		return;
	}

	if (auto door = item->getDoor()) {
		if (door->getDoorId() != 0) {
			house->addDoor(door);
		}
	} else {
		if (const auto bed = item->getBed()) {
			house->addBed(bed);
		}
	}
}

ReturnValue HouseTile::queryAdd(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor/* = nullptr*/)
{
	if (CreaturePtr creature = std::dynamic_pointer_cast<Creature>(thing)) {
		if (const auto player = creature->getPlayer()) {
			if (!house->isInvited(player)) {
				return RETURNVALUE_PLAYERISNOTINVITED;
			}
		} else {
			return RETURNVALUE_NOTPOSSIBLE;
		}
		return Tile::queryAdd(creature, flags);
	}

	if (ItemPtr item = std::dynamic_pointer_cast<Item>(thing)) {
		if (item->isStoreItem() && !item->hasAttribute(ITEM_ATTRIBUTE_WRAPID)) {
			return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
		}

		if (actor && g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
			if (!house->isInvited(actor->getPlayer())) {
				return RETURNVALUE_PLAYERISNOTINVITED;
			}
		}
		return Tile::queryAdd(item, flags);
	}
	return RETURNVALUE_NOERROR;
}

CylinderPtr HouseTile::queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem, uint32_t& flags)
{
	if (const auto creature = thing->getCreature()) {
		if (const auto player = creature->getPlayer()) {
			if (!house->isInvited(player)) {
				const Position& entryPos = house->getEntryPosition();
				auto destTile = g_game.map.getTile(entryPos);
				if (!destTile) {
					destTile = g_game.map.getTile(player->getTemplePosition());
					if (!destTile) {
						destTile = std::make_shared<StaticTile>(0xFFFF, 0xFFFF, 0xFF);
					}
				}

				index = -1;
				destItem.reset();
				return destTile;
			}
		}
	}

	return Tile::queryDestination(index, thing, destItem, flags);
}

ReturnValue HouseTile::queryRemove(const ThingPtr& thing, const uint32_t count, const uint32_t flags, CreaturePtr actor /*= nullptr*/)
{
	const auto item = thing->getItem();
	if (!item) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (actor && g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
		if (!house->isInvited(actor->getPlayer())) {
			return RETURNVALUE_PLAYERISNOTINVITED;
		}
	}
	return Tile::queryRemove(thing, count, flags);
}
