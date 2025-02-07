// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_INBOX_H
#define FS_INBOX_H

#include "container.h"

class Inbox final : public Container
{
	public:
		explicit Inbox(uint16_t type);

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
			return getParent();
		}
};

using InboxPtr = std::shared_ptr<Inbox>;
using InboxConstPtr = std::shared_ptr<const Inbox>;

#endif

