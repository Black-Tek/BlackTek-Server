// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_MAILBOX_H
#define FS_MAILBOX_H

#include "item.h"
#include "cylinder.h"

class Mailbox final : public Item, public Cylinder
{
	public:
		explicit Mailbox(const uint16_t itemId) : Item(itemId) {}

		MailboxPtr getMailbox() override {
			return static_shared_this<Mailbox>();
		}
	
		MailboxConstPtr getMailbox() const override {
			return static_shared_this<Mailbox>();
		}

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;
		ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count,
				uint32_t& maxQueryCount, uint32_t flags) override;
		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;
		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem,
				uint32_t& flags) override; // another optional ref wrapper

		void addThing(ThingPtr thing) override;
		void addThing(int32_t index, ThingPtr thing) override;

		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) override;
		void replaceThing(uint32_t index, ThingPtr thing) override;

		void removeThing(ThingPtr thing, uint32_t count) override;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

	private:
		bool getReceiver(const ItemPtr& item, std::string& name) const;
		bool sendItem(const ItemPtr& item) const;

		static bool canSend(const ItemConstPtr& item);
};

#endif
