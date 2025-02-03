// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_TELEPORT_H
#define FS_TELEPORT_H

#include "tile.h"

class Teleport final : public Item, public Cylinder
{
	public:
		explicit Teleport(uint16_t type) : Item(type) {};

		TeleportPtr getTeleport() override {
			return {shared_from_this(), this};
		}
	
		TeleportConstPtr getTeleport() const override {
			return {shared_from_this(), this};
		}

		//serialization
		Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream) override;
		void serializeAttr(PropWriteStream& propWriteStream) const override;

		const Position& getDestPos() const {
			return destPos;
		}
	
		void setDestPos(const Position& pos) {
			destPos = pos;
		}

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;
		ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count,
				uint32_t& maxQueryCount, uint32_t flags) override;
		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;
		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem,
				uint32_t& flags) override; // again optional ref wrapper

		void addThing(ThingPtr thing) override;
		void addThing(int32_t index, ThingPtr thing) override;

		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) override;
		void replaceThing(uint32_t index, ThingPtr thing) override;

		void removeThing(ThingPtr thing, uint32_t count) override;

		void postAddNotification(ThingPtr thing,  CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing,  CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

	private:
		Position destPos;
};

#endif
