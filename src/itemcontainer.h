// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <queue>

#include "cylinder.h"
#include "item.h"

class ContainerIterator
{
	public:
		bool hasNext() const {	return not over.empty(); }

		void advance();
		ItemPtr operator*() const;

	private:
		std::list<ContainerConstPtr> over;
		ItemDeque::const_iterator cur;

		friend class ItemContainer;
};

class ItemContainer final
{
	public:
		explicit ItemContainer(uint16_t size, bool unlocked = true, bool pagination = false, ContainerSubType subtype = ContainerSubType::None);

		// non-copyable
		ItemContainer(const ItemContainer&) = delete;
		ItemContainer& operator=(const ItemContainer&) = delete;

		ContainerIterator iterator() const;
		ItemPtr getItemByIndex(size_t index) const;
		CylinderPtr getParent() const;

		[[nodiscard]] ItemPtr getOwner()						const { return owner.lock(); }
		[[nodiscard]] CreaturePtr getHoldingCreature()			const { return holdingCreature.lock(); }
		[[nodiscard]] ContainerSubType getContainerSubType()	const { return container_subtype; }

		const ItemDeque& getItemList()							const { return itemlist; }
		ItemDeque::const_reverse_iterator getReversedItems()	const { return itemlist.rbegin(); }
		ItemDeque::const_reverse_iterator getReversedEnd()		const { return itemlist.rend(); }

		void addItem(const ItemPtr& item);
		void cloneInto(ItemContainer& target) const;
		void setParentToTileItems(const TilePtr& tile);
		void setHoldingCreature(const CreaturePtr& creature);
		void removeInbox(const ItemPtr& inbox);
		void addThing(ThingPtr thing);
		void addThing(int32_t index, ThingPtr thing);
		void addItemBack(ItemPtr& item);
		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count);
		void replaceThing(uint32_t index, ThingPtr thing);
		void removeThing(ThingPtr thing, uint32_t count);
		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER);
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER);
		void internalAddThing(ThingPtr thing);
		void internalAddThing(uint32_t index, ThingPtr thing);
		void startDecaying();

		void setDepotId(uint16_t newDepotId)		{ depotId = newDepotId; }
		void setMaxDepotItems(uint32_t maxItems)	{ maxDepotItems = maxItems; }

		[[nodiscard]] bool unserializeItemNode(OTB::Loader& loader, const OTB::Node& node, PropStream& propStream);
		[[nodiscard]] bool isHoldingItem(const ItemConstPtr& item) const noexcept;
		[[nodiscard]] bool isRewardCorpse() const noexcept;
		[[nodiscard]] bool hasParent() const noexcept;

		[[nodiscard]] bool isDepotLocker()	const noexcept { return container_subtype == ContainerSubType::DepotLocker; }
		[[nodiscard]] bool isStoreInbox()	const noexcept { return container_subtype == ContainerSubType::StoreInbox; }
		[[nodiscard]] bool isRewardChest()	const noexcept { return container_subtype == ContainerSubType::RewardChest;	}
		[[nodiscard]] bool isDepotChest()	const noexcept { return container_subtype == ContainerSubType::DepotChest; }
		[[nodiscard]] bool isInbox()		const noexcept { return container_subtype == ContainerSubType::Inbox; }
		[[nodiscard]] bool canRemove()		const noexcept { return config.Has(BlackTek::Containers::ContainerProperty::Removable); }
		[[nodiscard]] bool empty()			const noexcept { return itemlist.empty(); }
		[[nodiscard]] bool isUnlocked()		const noexcept { return unlocked; }
		[[nodiscard]] bool hasPagination()	const noexcept { return pagination; }

		[[nodiscard]] int32_t getThingIndex(ThingPtr thing);
		[[nodiscard]] uint32_t getItemHoldingCount() const;
		[[nodiscard]] uint32_t getItemTypeCount(uint16_t itemId, int32_t subType = -1) const;

		[[nodiscard]] uint16_t getDepotId()		const noexcept { return depotId; }
		[[nodiscard]] uint32_t capacity()		const noexcept { return maxSize; }
		[[nodiscard]] uint32_t getAmmoCount()	const noexcept { return ammoCount; }
		[[nodiscard]] uint32_t getChildWeight()	const noexcept { return totalWeight; }

		[[nodiscard]] Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream);
		
		[[nodiscard]] std::string getContentDescription() const;
		[[nodiscard]] std::string getName(bool addArticle = false) const;

		[[nodiscard]] size_t getFirstIndex()	const;
		[[nodiscard]] size_t getLastIndex()		const;
		[[nodiscard]] size_t size()				const { return itemlist.size(); }

		[[nodiscard]] ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr);
		[[nodiscard]] ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t& maxQueryCount, uint32_t flags);
		[[nodiscard]] ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr);
		[[nodiscard]] ThingPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem, uint32_t& flags);
		[[nodiscard]] ThingPtr getThing(size_t index);
		
		[[nodiscard]] gtl::btree_map<uint32_t, uint32_t>& getAllItemTypeCount(gtl::btree_map<uint32_t, uint32_t>& countMap) const;

	private:
		std::ostringstream& getContentDescription(std::ostringstream& os) const;

		ItemDeque itemlist;
		ContainerSubType container_subtype = ContainerSubType::None;
		BlackTek::Containers::ContainerConfig config;

		ItemWeakPtr owner;
		CreatureWeakPtr holdingCreature;

		uint32_t maxSize;
		uint32_t totalWeight = 0;
		uint32_t serializationCount = 0;
		uint32_t ammoCount = 0;
		uint32_t maxDepotItems = 0;
		uint16_t depotId = 0;

		bool unlocked;
		bool pagination;

		void onAddContainerItem(const ItemPtr& ownerItem, ItemPtr& item);
		void onUpdateContainerItem(const ItemPtr& ownerItem, uint32_t index, const ItemPtr& oldItem, const ItemPtr& newItem);
		void onRemoveContainerItem(const ItemPtr& ownerItem, uint32_t index, const ItemPtr& item);

		void updateItemWeight(const ItemPtr& ownerItem, int32_t diff);
		bool isGenuinelyPlaced(const ItemPtr& ownerItem) const;
		void propagateHoldingCreatureTo(const ItemPtr& item) const;

		ReturnValue queryAddGeneric(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor);
		ReturnValue queryAddDepotChest(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor);
		ReturnValue queryAddInbox(const ThingPtr& thing, uint32_t flags) const;
		ReturnValue queryAddStoreInbox(const ThingPtr& thing, uint32_t flags) const;
		ReturnValue queryAddValidateItem(const ThingPtr& thing, ItemPtr& outItem) const;

		void postAddNotificationDefault(ThingPtr thing, CylinderPtr oldParent, int32_t index);
		void postRemoveNotificationDefault(ThingPtr thing, CylinderPtr newParent, int32_t index);

		friend class ContainerIterator;
		friend class IOMapSerialize;
		friend class Item;
};
