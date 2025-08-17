// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "mailbox.h"
#include "game.h"
#include "iologindata.h"

extern Game g_game;

ReturnValue Mailbox::queryAdd(int32_t, const ThingPtr& thing, uint32_t, uint32_t, CreaturePtr)
{
	if (const auto& item = thing->getItem(); item && Mailbox::canSend(item)) {
		return RETURNVALUE_NOERROR;
	}
	return RETURNVALUE_NOTPOSSIBLE;
}

ReturnValue Mailbox::queryMaxCount(int32_t, const ThingPtr&, const uint32_t count, uint32_t& maxQueryCount, uint32_t)
{
	maxQueryCount = std::max<uint32_t>(1, count);
	return RETURNVALUE_NOERROR;
}

ReturnValue Mailbox::queryRemove(const ThingPtr&, uint32_t, uint32_t, CreaturePtr /*= nullptr */)
{
	return RETURNVALUE_NOTPOSSIBLE;
}

CylinderPtr Mailbox::queryDestination(int32_t&, const ThingPtr&, ItemPtr&, uint32_t&)
{
	return CylinderPtr(this);
}

void Mailbox::addThing(ThingPtr thing)
{
	return addThing(0, thing);
}

void Mailbox::addThing(int32_t, ThingPtr thing)
{
	const auto& item = thing->getItem();
	if (item && Mailbox::canSend(item)) {
		sendItem(item);
	}
}

void Mailbox::updateThing(ThingPtr, uint16_t, uint32_t)
{
	//
}

void Mailbox::replaceThing(uint32_t, ThingPtr)
{
	//
}

void Mailbox::removeThing(ThingPtr, uint32_t)
{
	//
}

void Mailbox::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	getParent()->postAddNotification(thing, oldParent, index, LINK_PARENT);
}

void Mailbox::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	getParent()->postRemoveNotification(thing, newParent, index, LINK_PARENT);
}

bool Mailbox::sendItem(const ItemPtr& item) const
{
	std::string receiver;
	if (!getReceiver(item, receiver)) {
		return false;
	}

	/**No need to continue if its still empty**/
	if (receiver.empty()) {
		return false;
	}

	if (const auto player = g_game.getPlayerByName(receiver)) {
		CylinderPtr newParent = CylinderPtr(item->getParent());
		CylinderPtr inbox = CylinderPtr(player->getInbox());
		if (g_game.internalMoveItem(newParent, inbox, INDEX_WHEREEVER,
		                            item, item->getItemCount(), std::nullopt, FLAG_NOLIMIT) == RETURNVALUE_NOERROR) {
			g_game.transformItem(item, item->getID() + 1);
			player->onReceiveMail();
			return true;
		}
	} else {
		PlayerPtr tmpPlayer = std::make_shared<Player>(nullptr);
		if (!IOLoginData::loadPlayerByName(tmpPlayer, receiver)) {
			return false;
		}
		CylinderPtr newParent = CylinderPtr(item->getParent());
		CylinderPtr inbox = CylinderPtr(tmpPlayer->getInbox());
		if (g_game.internalMoveItem(newParent, inbox, INDEX_WHEREEVER,
		                            item, item->getItemCount(), std::nullopt, FLAG_NOLIMIT) == RETURNVALUE_NOERROR) {
			g_game.transformItem(item, item->getID() + 1);
			IOLoginData::savePlayer(tmpPlayer);
			return true;
		}
	}
	return false;
}

bool Mailbox::getReceiver(const ItemPtr& item, std::string& name) const
{
	if (const auto& container = item->getContainer()) {
		for (const auto& containerItem : container->getItemList()) {
			if (containerItem->getID() == ITEM_LABEL && getReceiver(containerItem, name)) {
				return true;
			}
		}
		return false;
	}

	const std::string& text = item->getText();
	if (text.empty()) {
		return false;
	}

	name = getFirstLine(text);
	trimString(name);
	return true;
}

bool Mailbox::canSend(const ItemConstPtr& item)
{
	return item->getID() == ITEM_PARCEL || item->getID() == ITEM_LETTER;
}
