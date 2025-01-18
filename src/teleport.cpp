// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "teleport.h"
#include "game.h"

extern Game g_game;

Attr_ReadValue Teleport::readAttr(AttrTypes_t attr, PropStream& propStream)
{
	if (attr == ATTR_TELE_DEST) {
		if (!propStream.read<uint16_t>(destPos.x) || !propStream.read<uint16_t>(destPos.y) || !propStream.read<uint8_t>(destPos.z)) {
			return ATTR_READ_ERROR;
		}
		return ATTR_READ_CONTINUE;
	}
	return Item::readAttr(attr, propStream);
}

void Teleport::serializeAttr(PropWriteStream& propWriteStream) const
{
	Item::serializeAttr(propWriteStream);

	propWriteStream.write<uint8_t>(ATTR_TELE_DEST);
	propWriteStream.write<uint16_t>(destPos.x);
	propWriteStream.write<uint16_t>(destPos.y);
	propWriteStream.write<uint8_t>(destPos.z);
}

ReturnValue Teleport::queryAdd(int32_t, const ThingPtr&, uint32_t, uint32_t, CreaturePtr)
{
	return RETURNVALUE_NOTPOSSIBLE;
}

ReturnValue Teleport::queryMaxCount(int32_t, const ThingPtr&, uint32_t, uint32_t&, uint32_t)
{
	return RETURNVALUE_NOTPOSSIBLE;
}

ReturnValue Teleport::queryRemove(const ThingPtr&, uint32_t, uint32_t, CreaturePtr /*= nullptr */)
{
	return RETURNVALUE_NOERROR;
}

CylinderPtr Teleport::queryDestination(int32_t&, const ThingPtr&, ItemPtr*, uint32_t&)
{
	return this->getTile();
}

void Teleport::addThing(ThingPtr thing)
{
	return addThing(0, thing);
}

void Teleport::addThing(int32_t, ThingPtr thing)
{
	const auto destTile = g_game.map.getTile(destPos);
	if (!destTile) {
		return;
	}

	// Prevent infinite loop
	if (auto destTeleport = destTile->getTeleportItem()) {
		std::vector<Position> lastPositions = { getPosition() };

		while (true) {
			const Position& nextPos = destTeleport->getDestPos();
			if (std::ranges::find(lastPositions, nextPos) != lastPositions.end()) {
				std::cout << "Warning: possible infinite loop teleport. " << nextPos << std::endl;
				return;
			}

			const auto& tile = g_game.map.getTile(nextPos);
			if (!tile) {
				break;
			}

			destTeleport = tile->getTeleportItem();
			if (!destTeleport) {
				break;
			}

			lastPositions.push_back(nextPos);
		}
	}

	const MagicEffectClasses effect = Item::items[id].magicEffect;

	if (auto creature = thing->getCreature()) {
		Position origPos = creature->getPosition();
		g_game.internalCreatureTurn(creature, origPos.x > destPos.x ? DIRECTION_WEST : DIRECTION_EAST);
		g_game.map.moveCreature(creature, destTile);
		if (effect != CONST_ME_NONE) {
			g_game.addMagicEffect(origPos, effect);
			g_game.addMagicEffect(destTile->getPosition(), effect);
		}
	} else if (const auto& item = thing->getItem()) {
		if (effect != CONST_ME_NONE) {
			g_game.addMagicEffect(destTile->getPosition(), effect);
			g_game.addMagicEffect(item->getPosition(), effect);
		}
		CylinderPtr f_cylinder = getTile();
		CylinderPtr t_cylinder = destTile;
		g_game.internalMoveItem(f_cylinder, t_cylinder, INDEX_WHEREEVER, item, item->getItemCount(), nullptr, FLAG_NOLIMIT);
	}
}

void Teleport::updateThing(ThingPtr, uint16_t, uint32_t)
{
	//
}

void Teleport::replaceThing(uint32_t, ThingPtr)
{
	//
}

void Teleport::removeThing(ThingPtr, uint32_t)
{
	//
}

void Teleport::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (parent.lock()) {
		parent.lock()->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void Teleport::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (parent.lock()) {
		parent.lock()->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}
