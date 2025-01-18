// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "depotchest.h"
#include "tools.h"

DepotChest::DepotChest(uint16_t type, bool paginated /*= true*/) :
	Container{ type, items[type].maxItems, true, paginated } {}

ReturnValue DepotChest::queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
                                 uint32_t flags, CreaturePtr actor/* = nullptr*/)
{
	auto item = thing->getItem();
	if (item == nullptr) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	bool skipLimit = hasBitSet(FLAG_NOLIMIT, flags);
	if (!skipLimit) {
		int32_t addCount = 0;

		if ((item->isStackable() && item->getItemCount() != count)) {
			addCount = 1;
		}

		if (item->getTopParent().get() != this) {
			if (const auto container = item->getContainer()) {
				addCount = container->getItemHoldingCount() + 1;
			} else {
				addCount = 1;
			}
		}

		if (getItemHoldingCount() + addCount > maxDepotItems) {
			return RETURNVALUE_DEPOTISFULL;
		}
	}

	return Container::queryAdd(index, thing, count, flags, actor);
}

void DepotChest::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (getParent()) {
		getParent()->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void DepotChest::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (getParent()) {
		getParent()->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}

CylinderPtr DepotChest::getParent()
{
	if (parent.lock()) {
		return parent.lock()->getParent();
	}
	return nullptr;
}
