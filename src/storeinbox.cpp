// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "storeinbox.h"

StoreInbox::StoreInbox(uint16_t type) : Container(type, 20, true, true) {}

ReturnValue StoreInbox::queryAdd(int32_t, const ThingPtr& thing, uint32_t, uint32_t flags, CreaturePtr)
{
	const auto& item = thing->getItem();
	if (!item) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (item == this->getItem()) {
		return RETURNVALUE_THISISIMPOSSIBLE;
	}

	if (!item->isPickupable()) {
		return RETURNVALUE_CANNOTPICKUP;
	}

	if (!hasBitSet(FLAG_NOLIMIT, flags)) {
		if (!item->isStoreItem()) {
			return RETURNVALUE_CANNOTMOVEITEMISNOTSTOREITEM;
		}

		if (const auto& container = item->getContainer(); container && !container->empty()) {
			return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
		}
	}

	return RETURNVALUE_NOERROR;
}

void StoreInbox::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (parent.lock()) {
		parent.lock()->postAddNotification(thing, oldParent, index, LINK_TOPPARENT);
	}
}

void StoreInbox::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (parent.lock()) {
		parent.lock()->postRemoveNotification(thing, newParent, index, LINK_TOPPARENT);
	}
}

