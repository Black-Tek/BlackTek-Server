// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_REWARDCHEST_H
#define FS_REWARDCHEST_H

#include "container.h"

class RewardChest final : public Container
{
public:
	explicit RewardChest(uint16_t type, bool paginated = true);

	RewardChestPtr getRewardChest() override {
		return {shared_from_this(), this};
	}
	
	RewardChestConstPtr getRewardChest() const override {
		return dynamic_shared_this<RewardChest>();
	}

	//cylinder implementations
	ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
	                     uint32_t flags, CreaturePtr actor = nullptr) override;

	void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
	void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

};

#endif

