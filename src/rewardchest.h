// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_REWARDCHEST_H
#define FS_REWARDCHEST_H

#include "container.h"

using RewardChest_ptr = std::shared_ptr<RewardChest>;

class RewardChest final : public Container
{
public:
	explicit RewardChest(uint16_t type, bool paginated = true);

	RewardChest* getRewardChest() override {
		return this;
	}
	const RewardChest* getRewardChest() const override {
		return this;
	}

	//cylinder implementations
	ReturnValue queryAdd(int32_t index, const Thing& thing, uint32_t count,
		uint32_t flags, Creature* actor = nullptr) const override;

	void postAddNotification(Thing* thing, const Cylinder* oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
	void postRemoveNotification(Thing* thing, const Cylinder* newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

};

#endif

