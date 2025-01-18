// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_TRASHHOLDER_H
#define FS_TRASHHOLDER_H

#include "item.h"
#include "cylinder.h"
#include "const.h"

class TrashHolder final : public Item, public Cylinder
{
	public:
		explicit TrashHolder(uint16_t itemId) : Item(itemId) {}

		TrashHolderPtr getTrashHolder() override {
			return dynamic_shared_this<TrashHolder>();
		}
	
		TrashHolderConstPtr getTrashHolder() const override {
			return dynamic_shared_this<const TrashHolder>();
		}

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;
		ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t& maxQueryCount, uint32_t flags) override;
		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;
		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr* destItem, uint32_t& flags) override; // another place to put a ref wrapper or optional or both

		void addThing(ThingPtr thing) override;
		void addThing(int32_t index, ThingPtr thing) override;

		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) override;
		void replaceThing(uint32_t index, ThingPtr thing) override;

		void removeThing(ThingPtr thing, uint32_t count) override;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
};

#endif
