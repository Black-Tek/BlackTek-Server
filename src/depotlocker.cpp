// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "depotlocker.h"

DepotLocker::DepotLocker(uint16_t type) :
	Container(type), depotId(0) {}

Attr_ReadValue DepotLocker::readAttr(AttrTypes_t attr, PropStream& propStream)
{
	if (attr == ATTR_DEPOT_ID) {
		if (!propStream.read<uint16_t>(depotId)) {
			return ATTR_READ_ERROR;
		}
		return ATTR_READ_CONTINUE;
	}
	return Item::readAttr(attr, propStream);
}

ReturnValue DepotLocker::queryAdd(int32_t, const ThingPtr&, uint32_t, uint32_t, CreaturePtr)
{
	return RETURNVALUE_NOTENOUGHROOM;
}

void DepotLocker::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (getParent()) {
		getParent()->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void DepotLocker::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (getParent()) {
		getParent()->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}

void DepotLocker::removeInbox(const InboxPtr& inbox)
{
	auto cit = std::ranges::find(itemlist, inbox);
	if (cit == itemlist.end()) {
		return;
	}
	itemlist.erase(cit);
}
