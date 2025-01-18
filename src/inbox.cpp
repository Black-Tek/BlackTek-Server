// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "inbox.h"
#include "tools.h"

Inbox::Inbox(uint16_t type) : Container(type, 30, false, true) {}

ReturnValue Inbox::queryAdd(int32_t, const ThingPtr& thing, uint32_t,
                            uint32_t flags, CreaturePtr)
{
	if (!hasBitSet(FLAG_NOLIMIT, flags)) {
		return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}

	auto item = thing->getItem();
	if (!item) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (item.get() == this) {
		return RETURNVALUE_THISISIMPOSSIBLE;
	}

	if (!item->isPickupable()) {
		return RETURNVALUE_CANNOTPICKUP;
	}

	return RETURNVALUE_NOERROR;
}

void Inbox::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (const auto parent = getParent(); parent != nullptr) {
		parent->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void Inbox::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (const auto parent = getParent(); parent != nullptr) {
		parent->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}

CylinderPtr Inbox::getParent()
{
	if (parent.lock()) {
		return parent.lock()->getParent();
	}
	return nullptr;
}
