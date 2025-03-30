// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_DEPOTLOCKER_H
#define FS_DEPOTLOCKER_H

#include "container.h"
#include "inbox.h"

class DepotLocker final : public Container
{
	public:
		explicit DepotLocker(uint16_t type);

		DepotLockerPtr getDepotLocker() override {
			return static_shared_this<DepotLocker>();
		}
	
		DepotLockerConstPtr getDepotLocker() const override {
			return static_shared_this<const DepotLocker>();
		}

		void removeInbox(const InboxPtr& inbox);

		//serialization
		Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream) override;

		uint16_t getDepotId() const {
			return depotId;
		}
	
		void setDepotId(uint16_t depotId) {
			this->depotId = depotId;
		}

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

		bool canRemove() const override {
			return false;
		}

	private:
		uint16_t depotId;
};

#endif

