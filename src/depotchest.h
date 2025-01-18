// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_DEPOTCHEST_H
#define FS_DEPOTCHEST_H

#include "container.h"

class DepotChest final : public Container
{
	public:
		explicit DepotChest(uint16_t type, bool paginated = true);

		//serialization
		void setMaxDepotItems(uint32_t maxitems) {
			maxDepotItems = maxitems;
		}

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

		//overrides
		bool canRemove() const override {
			return false;
		}

		CylinderPtr getParent() override;
	
		CylinderPtr getRealParent() override {
			return parent.lock();
		}

	private:
		uint32_t maxDepotItems = 0;
};

using DepotChestPtr = std::shared_ptr<DepotChest>;
using DepotChestConstPtr = std::shared_ptr<const DepotChest>;

#endif

