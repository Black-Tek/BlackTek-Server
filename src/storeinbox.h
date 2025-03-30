// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_STOREINBOX_H
#define FS_STOREINBOX_H

#include "container.h"

class StoreInbox final : public Container
{
	public:
		explicit StoreInbox(uint16_t type);

		StoreInboxPtr getStoreInbox() override {
			return static_shared_this<StoreInbox>();
		}
	
		StoreInboxConstPtr getStoreInbox() const override {
			return static_shared_this<StoreInbox>();
		}

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

		bool canRemove() const override {
			return false;
		}
};

#endif
