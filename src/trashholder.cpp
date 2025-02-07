// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "trashholder.h"
#include "game.h"

extern Game g_game;

ReturnValue TrashHolder::queryAdd(int32_t, const ThingPtr&, uint32_t, uint32_t, CreaturePtr)
{
	return RETURNVALUE_NOERROR;
}

ReturnValue TrashHolder::queryMaxCount(int32_t, const ThingPtr&, uint32_t count, uint32_t& maxQueryCount, uint32_t)
{
	maxQueryCount = std::max<uint32_t>(1, count);
	return RETURNVALUE_NOERROR;
}

ReturnValue TrashHolder::queryRemove(const ThingPtr&, uint32_t, uint32_t, CreaturePtr /*= nullptr*/)
{
	return RETURNVALUE_NOTPOSSIBLE;
}

CylinderPtr TrashHolder::queryDestination(int32_t&, const ThingPtr&, ItemPtr&, uint32_t&)
{
	return CylinderPtr(this);
}

void TrashHolder::addThing(ThingPtr thing)
{
	return addThing(0, thing);
}

void TrashHolder::addThing(int32_t, ThingPtr thing)
{
	const auto& item = thing->getItem();
	if (!item) {
		return;
	}

	if (item == this->getItem() || !item->hasProperty(CONST_PROP_MOVEABLE)) {
		return;
	}

	const ItemType& it = Item::items[id];
	if (item->isHangable() && it.isGroundTile()) {
		const auto& tile = std::dynamic_pointer_cast<Tile>(getParent());
		if (tile && tile->hasFlag(TILESTATE_SUPPORTS_HANGABLE)) {
			return;
		}
	}

	g_game.internalRemoveItem(item);

	if (it.magicEffect != CONST_ME_NONE) {
		g_game.addMagicEffect(getPosition(), it.magicEffect);
	}
}

void TrashHolder::updateThing(ThingPtr, uint16_t, uint32_t)
{
	//
}

void TrashHolder::replaceThing(uint32_t, ThingPtr)
{
	//
}

void TrashHolder::removeThing(ThingPtr, uint32_t)
{
	//
}

void TrashHolder::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (parent.lock()) {
		parent.lock()->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void TrashHolder::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (parent.lock()) {
		parent.lock()->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}
