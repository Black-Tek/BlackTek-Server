// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_CONTAINER_H
#define FS_CONTAINER_H

#include <queue>

#include "cylinder.h"
#include "item.h"
#include "tile.h"

class Container;
class DepotChest;
class DepotLocker;
class StoreInbox;
class RewardChest;

using DepotLockerPtr = std::shared_ptr<DepotLocker>;
using DepotLockerConstPtr = std::shared_ptr<const DepotLocker>;

using StoreInboxPtr = std::shared_ptr<StoreInbox>;
using StoreInboxConstPtr = std::shared_ptr<const StoreInbox>;

using RewardChestPtr = std::shared_ptr<RewardChest>;
using RewardChestConstPtr = std::shared_ptr<const RewardChest>;

class ContainerIterator
{
	public:
		bool hasNext() const {
			return !over.empty();
		}

		void advance();
		ItemPtr operator*() const;

	private:
		std::list<ContainerConstPtr> over;
		ItemDeque::const_iterator cur;

		friend class Container;
};

class Container : public Item, public Cylinder
{
	public:
		explicit Container(uint16_t type);
		Container(uint16_t type, uint16_t size, bool unlocked = true, bool pagination = false);
		explicit Container(const TilePtr& tile);
		void setParentToTileItems(const TilePtr& tile);
		~Container() override;

		// non-copyable
		Container(const Container&) = delete;
		Container& operator=(const Container&) = delete;

		ItemPtr clone() const override final;

		ContainerPtr getContainer() override final {
			return dynamic_shared_this<Container>();
		}
	
		ContainerConstPtr getContainer() const override final {
			return dynamic_shared_this<const Container>();
		}

		virtual DepotLockerPtr getDepotLocker() {
			return nullptr;
		}
	
		virtual DepotLockerConstPtr getDepotLocker() const {
			return nullptr;
		}

		virtual StoreInboxPtr getStoreInbox() {
			return nullptr;
		}
	
		virtual StoreInboxConstPtr getStoreInbox() const {
			return nullptr;
		}
		
		virtual RewardChestPtr getRewardChest() {
			return nullptr;
		}
	
		virtual RewardChestConstPtr getRewardChest() const {
			return nullptr;
		}

		Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream) override;
		bool unserializeItemNode(OTB::Loader& loader, const OTB::Node& node, PropStream& propStream) override;
		std::string getContentDescription() const;

		size_t size() const {
			return itemlist.size();
		}
	
		bool empty() const {
			return itemlist.empty();
		}
	
		uint32_t capacity() const {
			return maxSize;
		}
	
		uint32_t getAmmoCount() const { 
			return ammoCount; 
		}

		ContainerIterator iterator() const;

		const ItemDeque& getItemList() const {
			return itemlist;
		}

		ItemDeque::const_reverse_iterator getReversedItems() const {
			return itemlist.rbegin();
		}
	
		ItemDeque::const_reverse_iterator getReversedEnd() const {
			return itemlist.rend();
		}

		std::string getName(bool addArticle = false) const;

		bool hasParent();
		void addItem(const ItemPtr& item);
		ItemPtr getItemByIndex(size_t index) const;
		bool isHoldingItem(const ItemConstPtr& item) const;
		bool isRewardCorpse() const;

		uint32_t getItemHoldingCount() const;
		uint32_t getWeight() const override final;

		bool isUnlocked() const {
			return unlocked;
		}
	
		bool hasPagination() const {
			return pagination;
		}

		//cylinder implementations
		virtual ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                             uint32_t flags, CreaturePtr = nullptr) override;
		ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t& maxQueryCount,
				uint32_t flags)  override final;
		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override final;
		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem,
				uint32_t& flags) override final; // again optional ref wrapper

		void addThing(ThingPtr thing) override final;
		void addThing(int32_t index, ThingPtr thing) override final;
		void addItemBack(ItemPtr& item);

		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) override final;
		void replaceThing(uint32_t index, ThingPtr thing) override final;

		void removeThing(ThingPtr thing, uint32_t count) override final;

		int32_t getThingIndex(ThingPtr thing) override final;
		size_t getFirstIndex() const override final;
		size_t getLastIndex() const override final;
		uint32_t getItemTypeCount(uint16_t itemId, int32_t subType = -1) const override final;
		std::map<uint32_t, uint32_t>& getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const override final;
		ThingPtr getThing(size_t index) override final;

		ItemVector getItems(bool recursive = false) const;

		void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

		void internalAddThing(ThingPtr thing) override final;
		void internalAddThing(uint32_t index, ThingPtr thing) override final;
		void startDecaying() override final;

	protected:
		ItemDeque itemlist;

	private:
		std::ostringstream& getContentDescription(std::ostringstream& os) const;

		uint32_t maxSize;
		uint32_t totalWeight = 0;
		uint32_t serializationCount = 0;
		uint32_t ammoCount = 0;

		bool unlocked;
		bool pagination;

		void onAddContainerItem(ItemPtr& item);
		void onUpdateContainerItem(uint32_t index, const ItemPtr& oldItem, const ItemPtr& newItem);
		void onRemoveContainerItem(uint32_t index, const ItemPtr& item);

		ContainerPtr getParentContainer();
		void updateItemWeight(int32_t diff);

		friend class ContainerIterator;
		friend class IOMapSerialize;
};

#endif
