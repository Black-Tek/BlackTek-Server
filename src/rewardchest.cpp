// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "rewardchest.h"

RewardChest::RewardChest(uint16_t type, bool paginated /*= true*/) :
    Container{ type, items[type].maxItems, true, paginated } {
}

ReturnValue RewardChest::queryAdd(int32_t, const Thing&, uint32_t, uint32_t, Creature* actor/* = nullptr*/) const
{
	if (actor) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	return RETURNVALUE_NOERROR;
}

void RewardChest::postAddNotification(Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t)
{
	if (parent != nullptr) {
		parent->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void RewardChest::postRemoveNotification(Thing* thing, const Cylinder* newParent, int32_t index, cylinderlink_t)
{
	if (parent != nullptr) {
		parent->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}
