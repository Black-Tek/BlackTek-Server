// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <queue>

#include "enums.h"
#include "item.h"
#include "itemlocation.h"
#include "spectators.h"

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
		BlackTek::ItemLocation getParent() const;

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
		void addItemAt(int32_t index, const ItemPtr& item);
		void addItemAt(int32_t index, const ItemPtr& item, std::span<const CreaturePtr> spectators);
		void addItemBack(ItemPtr& item);
		void updateItem(const ItemPtr& item, uint16_t itemId, uint32_t count);
		void replaceItem(uint32_t index, const ItemPtr& item);
		void removeItem(const ItemPtr& item, uint32_t count);
		void notifyItemAdded(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, NotifyLink link = LINK_OWNER);
		void notifyItemAdded(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, std::span<const CreaturePtr> spectators, NotifyLink link = LINK_OWNER);
		void notifyItemRemoved(const ItemPtr& item, const BlackTek::ItemLocation& newLocation, int32_t index, NotifyLink link = LINK_OWNER);
		void addItemSilently(const ItemPtr& item);
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

		[[nodiscard]] int32_t getItemIndex(const ItemConstPtr& item) const;
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

		[[nodiscard]] ReturnValue canAddItem(int32_t index, const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr);
		[[nodiscard]] ReturnValue checkAddCapacity(int32_t index, const ItemPtr& item, uint32_t count, uint32_t& acceptedCount, uint32_t flags);
		[[nodiscard]] ReturnValue canRemoveItem(const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr);
		[[nodiscard]] BlackTek::ItemLocation resolveItemDestination(int32_t& index, const ItemPtr& item, ItemPtr& destItem, uint32_t& flags);

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
		void onAddContainerItem(const ItemPtr& ownerItem, ItemPtr& item, std::span<const CreaturePtr> spectators);
		void onUpdateContainerItem(const ItemPtr& ownerItem, uint32_t index, const ItemPtr& oldItem, const ItemPtr& newItem);
		void onRemoveContainerItem(const ItemPtr& ownerItem, uint32_t index, const ItemPtr& item);

		void updateItemWeight(const ItemPtr& ownerItem, int32_t diff);
		bool isGenuinelyPlaced(const ItemPtr& ownerItem) const;
		void propagateHoldingCreatureTo(const ItemPtr& item) const;

		ReturnValue canAddItemStandard(int32_t index, const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor);
		ReturnValue canAddItemDepotChest(int32_t index, const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor);
		ReturnValue canAddItemInbox(const ItemPtr& item, uint32_t flags) const;
		ReturnValue canAddItemStoreInbox(const ItemPtr& item, uint32_t flags) const;
		ReturnValue validateAddItem(const ItemPtr& item) const;

		void notifyItemAddedDefault(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index);
		void notifyItemAddedDefault(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, std::span<const CreaturePtr> spectators);
		void notifyItemRemovedDefault(const ItemPtr& item, const BlackTek::ItemLocation& newLocation, int32_t index);

		friend class ContainerIterator;
		friend class IOMapSerialize;
		friend class Item;
};
