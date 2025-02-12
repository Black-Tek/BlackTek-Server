// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "container.h"
#include "iomap.h"
#include "game.h"

extern Game g_game;

Container::Container(uint16_t type) :
	Container(type, items[type].maxItems) {}

Container::Container(uint16_t type, uint16_t size, bool unlocked /*= true*/, bool pagination /*= false*/) :
	Item(type),
	maxSize(size),
	unlocked(unlocked),
	pagination(pagination)
{}

Container::Container(const TilePtr& tile) : Container(ITEM_BROWSEFIELD, 30, false, true) { setParent(tile); }

void Container::setParentToTileItems(const TilePtr& tile)
{
	auto itemVector = tile->getItemList();
	if (itemVector) {
		auto self = getContainer();
		for (const ItemPtr& item : *itemVector) {
			if ((item->getContainer() || item->hasProperty(CONST_PROP_MOVEABLE)) &&
				!item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)) {
				itemlist.push_front(item);
				item->setParent(self);
			}
		}
	}
}


Container::~Container()
{
	if (getID() == ITEM_BROWSEFIELD) {
		g_game.browseFields.erase(getTile());

		for (const auto item : itemlist) {
			item->setParent(parent);
		}
	} else {
		for (const auto item : itemlist) {
			item->clearParent();
		}
	}
}

ItemPtr Container::clone() const
{
	const auto& clone = std::dynamic_pointer_cast<Container>(Item::clone());
	for (const auto& item : itemlist) {
		clone->addItem(item->clone());
	}
	clone->totalWeight = totalWeight;
	return clone;
}

ContainerPtr Container::getParentContainer() {
	const auto& thing = getParent();
	if (!thing) {
		return nullptr;
	}
	return thing->getContainer();
}

std::string Container::getName(bool addArticle /* = false*/) const {
	const ItemType& it = items[id];
	return getNameDescription(it, std::dynamic_pointer_cast<const Container>(shared_from_this()), -1, addArticle);
}

bool Container::hasParent()
{
	return getID() != ITEM_BROWSEFIELD && std::dynamic_pointer_cast<Player>(getParent()) == nullptr;
}

void Container::addItem(const ItemPtr& item)
{
	itemlist.push_back(item);
	item->setParent(getContainer());
}

Attr_ReadValue Container::readAttr(const AttrTypes_t attr, PropStream& propStream)
{
	if (attr == ATTR_CONTAINER_ITEMS) {
		if (!propStream.read<uint32_t>(serializationCount)) {
			return ATTR_READ_ERROR;
		}
		return ATTR_READ_END;
	}
	return Item::readAttr(attr, propStream);
}

bool Container::unserializeItemNode(OTB::Loader& loader, const OTB::Node& node, PropStream& propStream)
{
	bool ret = Item::unserializeItemNode(loader, node, propStream);
	if (!ret) {
		return false;
	}

	for (auto& itemNode : node.children) {
		//load container items
		if (itemNode.type != OTBM_ITEM) {
			// unknown type
			return false;
		}

		PropStream itemPropStream;
		if (!loader.getProps(itemNode, itemPropStream)) {
			return false;
		}

		auto item = Item::CreateItem(itemPropStream);
		if (!item) {
			return false;
		}

		if (!item->unserializeItemNode(loader, itemNode, itemPropStream)) {
			return false;
		}

		addItem(item);
		updateItemWeight(item->getWeight());
	}
	return true;
}

void Container::updateItemWeight(int32_t diff)
{
	totalWeight += diff;
	auto parentContainer = getContainer();
	while ((parentContainer = parentContainer->getParentContainer()) != nullptr) {
		parentContainer->totalWeight += diff;
	}
}

uint32_t Container::getWeight() const
{
	return Item::getWeight() + totalWeight;
}

std::string Container::getContentDescription() const
{
	std::ostringstream os;
	return getContentDescription(os).str();
}

std::ostringstream& Container::getContentDescription(std::ostringstream& os) const
{
	bool firstitem = true;
	for (ContainerIterator it = iterator(); it.hasNext(); it.advance()) {
		const auto item = *it;

		const auto container = item->getContainer();
		if (container && !container->empty()) {
			continue;
		}

		if (firstitem) {
			firstitem = false;
		} else {
			os << ", ";
		}

		os << item->getNameDescription();
	}

	if (firstitem) {
		os << "nothing";
	}
	return os;
}

ItemPtr Container::getItemByIndex(size_t index) const
{
	if (index >= size()) {
		return nullptr;
	}
	return itemlist[index];
}

uint32_t Container::getItemHoldingCount() const
{
	uint32_t counter = 0;
	for (ContainerIterator it = iterator(); it.hasNext(); it.advance()) {
		++counter;
	}
	return counter;
}

bool Container::isHoldingItem(const ItemConstPtr& item) const
{
	for (ContainerIterator it = iterator(); it.hasNext(); it.advance()) {
        if ((*it)->equals(item)) {
            return true;
        }
	}
	return false;
}

bool Container::isRewardCorpse() const {
	for (const auto subItem : getItemList()) {
		if (subItem->getID() == ITEM_REWARD_CONTAINER) {
			return true;
		}
	}
	return false;
}

void Container::onAddContainerItem(ItemPtr& item)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), false, true, 1, 1, 1, 1);
	
	for (auto spectator : spectators) {
		if (const auto c_player = spectator->getPlayer())
		{
			const auto t_container = std::dynamic_pointer_cast<Container>(shared_from_this());
			c_player->sendAddContainerItem(t_container, item);
		}
	}

	for (auto spectator : spectators) {
		if (const auto c_player = spectator->getPlayer())
		{
			const auto t_container = std::dynamic_pointer_cast<Container>(shared_from_this());
			c_player->onAddContainerItem(item);
		}
	}
}

void Container::onUpdateContainerItem(uint32_t index, const ItemPtr& oldItem, const ItemPtr& newItem)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), false, true, 1, 1, 1, 1);
	
	for (auto spectator : spectators) {
		if (const auto c_player = spectator->getPlayer())
		{
			auto t_container = std::dynamic_pointer_cast<Container>(shared_from_this());
			c_player->sendUpdateContainerItem(t_container, index, newItem);
		}
	}

	for (auto spectator : spectators) {
		if (const auto c_player = spectator->getPlayer())
		{
			auto t_container = std::dynamic_pointer_cast<Container>(shared_from_this());
			c_player->onUpdateContainerItem(t_container, oldItem, newItem);
		}
	}
}

void Container::onRemoveContainerItem(uint32_t index, const ItemPtr& item)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), false, true, 1, 1, 1, 1);
	
	for (auto spectator : spectators) {
		if (const auto c_player = spectator->getPlayer())
		{
			auto t_container = std::dynamic_pointer_cast<Container>(shared_from_this());
			c_player->sendRemoveContainerItem(t_container, index);
		}
	}

	for (auto spectator : spectators) {
		if (const auto c_player = spectator->getPlayer())
		{
			auto t_container = std::dynamic_pointer_cast<Container>(shared_from_this());
			c_player->onRemoveContainerItem(t_container, item);
		}
	}
}

ReturnValue Container::queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
                                uint32_t flags, CreaturePtr actor/* = std::nullopt*/)
{
	bool childIsOwner = hasBitSet(FLAG_CHILDISOWNER, flags);
	if (childIsOwner) {
		//a child container is querying, since we are the top container (not carried by a player)
		//just return with no error.
		return RETURNVALUE_NOERROR;
	}

	if (!unlocked) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	const auto item = thing->getItem();
	if (item == nullptr) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (!item->isPickupable()) {
		return RETURNVALUE_CANNOTPICKUP;
	}

    if (item.get() == this) {
		return RETURNVALUE_THISISIMPOSSIBLE;
	}

	// quiver: allow ammo only
	if (getWeaponType() == WEAPON_QUIVER && item->getWeaponType() != WEAPON_AMMO) {
		return RETURNVALUE_QUIVERAMMOONLY;
	}

	// store items can be only moved into depot chest or store inbox
	if (item->isStoreItem() && !dynamic_cast<const DepotChest*>(this)) {
		return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
	}

	auto cylinder = getParent();

	// don't allow moving items into container that is store item and is in store inbox
	if (isStoreItem() && std::dynamic_pointer_cast<StoreInbox>(cylinder)) {
		ReturnValue ret = RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
		if (!item->isStoreItem()) {
			ret = RETURNVALUE_CANNOTMOVEITEMISNOTSTOREITEM;
		}
		return ret;
	}

	if (!hasBitSet(FLAG_NOLIMIT, flags)) {
		while (cylinder) {
			if (cylinder == thing) {
				return RETURNVALUE_THISISIMPOSSIBLE;
			}

			if (std::dynamic_pointer_cast<const Inbox>(cylinder)) {
				return RETURNVALUE_CONTAINERNOTENOUGHROOM;
			}

			cylinder = cylinder->getParent();
		}

		if (index == INDEX_WHEREEVER && size() >= capacity() && !hasPagination()) {
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
		}
	} else {
		while (cylinder) {
			if (cylinder == thing) {
				return RETURNVALUE_THISISIMPOSSIBLE;
			}

			cylinder = cylinder->getParent();
		}
	}

	const auto topParent = getTopParent();
	if (actor && g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
		if (const auto& houseTile = std::dynamic_pointer_cast<const HouseTile>(topParent->getTile())) {
			if (!topParent->getCreature() && !houseTile->getHouse()->isInvited(actor->getPlayer())) {
				return RETURNVALUE_PLAYERISNOTINVITED;
			}
		}
	}

	if (topParent.get() != this) {
		return topParent->queryAdd(INDEX_WHEREEVER, item, count, flags | FLAG_CHILDISOWNER, actor);
	} else {
		return RETURNVALUE_NOERROR;
	}
}

ReturnValue Container::queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count,
		uint32_t& maxQueryCount, uint32_t flags)
{
	auto item = thing->getItem();
	if (item == nullptr) {
		maxQueryCount = 0;
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (hasBitSet(FLAG_NOLIMIT, flags) || hasPagination()) {
		maxQueryCount = std::max<uint32_t>(1, count);
		return RETURNVALUE_NOERROR;
	}

	int32_t freeSlots = std::max<int32_t>(capacity() - size(), 0);

	if (item->isStackable()) {
		uint32_t n = 0;

		if (index == INDEX_WHEREEVER) {
			//Iterate through every item and check how much free stackable slots there is.
			uint32_t slotIndex = 0;
			for (auto containerItem : itemlist) {
				if (containerItem != item && containerItem->equals(item) && containerItem->getItemCount() < 100) {
					if (queryAdd(slotIndex++, item, count, flags) == RETURNVALUE_NOERROR) {
						n += 100 - containerItem->getItemCount();
					}
				}
			}
		} else {
			const auto destItem = getItemByIndex(index);
			if (item->equals(destItem) && destItem->getItemCount() < 100) {
				if (queryAdd(index, item, count, flags) == RETURNVALUE_NOERROR) {
					n = 100 - destItem->getItemCount();
				}
			}
		}

		maxQueryCount = freeSlots * 100 + n;
		if (maxQueryCount < count) {
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
		}
	} else {
		maxQueryCount = freeSlots;
		if (maxQueryCount == 0) {
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
		}
	}
	return RETURNVALUE_NOERROR;
}

ReturnValue Container::queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor /*= nullptr */)
{
	int32_t index = getThingIndex(thing);
	if (index == -1) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	const auto item = thing->getItem();
	if (item == nullptr) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (count == 0 || (item->isStackable() && count > item->getItemCount())) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (!item->isMoveable() && !hasBitSet(FLAG_IGNORENOTMOVEABLE, flags)) {
		return RETURNVALUE_NOTMOVEABLE;
	}

	if (actor && g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
		const auto topParent = getTopParent();
		if (const auto& houseTile = std::dynamic_pointer_cast<const HouseTile>(topParent->getTile())) {
			if (!topParent->getCreature() && !houseTile->getHouse()->isInvited(actor->getPlayer())) {
				return RETURNVALUE_PLAYERISNOTINVITED;
			}
		}
	}

	return RETURNVALUE_NOERROR;
}

CylinderPtr Container::queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem,
		uint32_t& flags)
{
	if (!unlocked) {
		destItem = nullptr;
		return getContainer();
	}

	if (index == 254 /*move up*/) {
		index = INDEX_WHEREEVER;
		destItem = nullptr;

		if (auto parentContainer = std::dynamic_pointer_cast<Cylinder>(getParent())) {
			return parentContainer;
		}

		return getContainer();
	}

	if (index == 255 /*add wherever*/) {
		index = INDEX_WHEREEVER;
		destItem = nullptr;
	} else if (index >= static_cast<int32_t>(capacity())) {
		/*
		if you have a container, maximize it to show all 20 slots
		then you open a bag that is inside the container you will have a bag with 8 slots
		and a "grey" area where the other 12 slots where from the container
		if you drop the item on that grey area
		the client calculates the slot position as if the bag has 20 slots
		*/
		index = INDEX_WHEREEVER;
		destItem = nullptr;
	}

	auto item = thing->getItem();
	if (!item) {
		return getContainer();
	}

	if (index != INDEX_WHEREEVER) {
		if (auto itemFromIndex = getItemByIndex(index)) {
			destItem = itemFromIndex;
		}

		if (CylinderPtr subCylinder = std::dynamic_pointer_cast<Cylinder>(destItem)) {
			index = INDEX_WHEREEVER;
			destItem = nullptr;
			return subCylinder;
		}
	}

	bool autoStack = !hasBitSet(FLAG_IGNOREAUTOSTACK, flags);
	if (autoStack && item->isStackable() && item->getParent().get() != this) {
		if (destItem && (destItem)->equals(item) && (destItem)->getItemCount() < 100) {
			return getContainer();
		}

		//try find a suitable item to stack with
		uint32_t n = 0;
		for (auto listItem : itemlist) {
			if (listItem != item && listItem->equals(item) && listItem->getItemCount() < 100) {
				destItem = listItem;
				index = n;
				return getContainer();
			}
			++n;
		}
	}
	return getContainer();
}

void Container::addThing(ThingPtr thing)
{
	return addThing(0, thing);
}

void Container::addThing(int32_t index, ThingPtr thing)
{
	if (index >= static_cast<int32_t>(capacity())) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	ItemPtr item = thing->getItem();
	if (item == nullptr) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	item->setParent(getContainer());
	itemlist.push_front(item);
	updateItemWeight(item->getWeight());
	ammoCount += item->getItemCount();

	//send change to client
	if (getParent() && (getParent() != VirtualCylinder::virtualCylinder)) {
		onAddContainerItem(item);
	}
}

void Container::addItemBack(ItemPtr& item)
{
	addItem(item);
	updateItemWeight(item->getWeight());
	ammoCount += item->getItemCount();

	//send change to client
	if (getParent() && (getParent() != VirtualCylinder::virtualCylinder)) {
		onAddContainerItem(item);
	}
}

void Container::updateThing(ThingPtr thing, uint16_t itemId, uint32_t count)
{
	int32_t index = getThingIndex(thing);
	if (index == -1) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	const auto item = thing->getItem();
	if (item == nullptr) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	ammoCount += count;
	ammoCount -= item->getItemCount();

	const int32_t oldWeight = item->getWeight();
	item->setID(itemId);
	item->setSubType(count);
	updateItemWeight(-oldWeight + item->getWeight());

	//send change to client
	if (getParent()) {
		onUpdateContainerItem(index, item, item);
	}
}

void Container::replaceThing(uint32_t index, ThingPtr thing)
{
	const auto item = thing->getItem();
	if (!item) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	const auto replacedItem = getItemByIndex(index);
	if (!replacedItem) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	ammoCount -= replacedItem->getItemCount();
	itemlist[index] = item;
	item->setParent(getContainer());
	updateItemWeight(-static_cast<int32_t>(replacedItem->getWeight()) + item->getWeight());
	ammoCount += item->getItemCount();

	//send change to client
	if (getParent()) {
		onUpdateContainerItem(index, replacedItem, item);
	}

	replacedItem->clearParent();
}

void Container::removeThing(ThingPtr thing, uint32_t count)
{
	auto item = thing->getItem();
	if (item == nullptr) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	int32_t index = getThingIndex(thing);
	if (index == -1) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	if (item->isStackable() && count != item->getItemCount()) {
		uint8_t newCount = static_cast<uint8_t>(std::max<int32_t>(0, item->getItemCount() - count));
		const int32_t oldWeight = item->getWeight();
		ammoCount -= (item->getItemCount() - newCount);
		item->setItemCount(newCount);
		updateItemWeight(-oldWeight + item->getWeight());

		//send change to client
		if (getParent()) {
			onUpdateContainerItem(index, item, item);
		}
	} else {
		updateItemWeight(-static_cast<int32_t>(item->getWeight()));
		ammoCount -= item->getItemCount();
		//send change to client
		if (getParent()) {
			onRemoveContainerItem(index, item);
		}

		item->clearParent();
		itemlist.erase(itemlist.begin() + index);
	}
}

int32_t Container::getThingIndex(ThingPtr thing)
{
	int32_t index = 0;
	for (const auto item : itemlist) {
		if (item == thing) {
			return index;
		}
		++index;
	}
	return -1;
}

size_t Container::getFirstIndex() const
{
	return 0;
}

size_t Container::getLastIndex() const
{
	return size();
}

uint32_t Container::getItemTypeCount(uint16_t itemId, int32_t subType/* = -1*/) const
{
	uint32_t count = 0;
	for (const auto item : itemlist) {
		if (item->getID() == itemId) {
			count += countByType(item, subType);
		}
	}
	return count;
}

std::map<uint32_t, uint32_t>& Container::getAllItemTypeCount(std::map<uint32_t, uint32_t>& countMap) const
{
	for (const auto item : itemlist) {
		countMap[item->getID()] += item->getItemCount();
	}
	return countMap;
}

ItemVector Container::getItems(bool recursive /*= false*/) const
{
	ItemVector containerItems;
	if (recursive) {
		for (ContainerIterator it = iterator(); it.hasNext(); it.advance()) {
			containerItems.push_back(*it);
		}
	} else {
		for (auto item : itemlist) {
			containerItems.push_back(item);
		}
	}
	return containerItems;
}

ThingPtr Container::getThing(size_t index)
{
	return getItemByIndex(index);
}

void Container::postAddNotification (ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	auto topParent = getTopParent();
	if (topParent->getCreature()) {
		topParent->postAddNotification(thing, oldParent, index, LINK_TOPPARENT);
	} else if (topParent.get() == this) {
		//let the tile class notify surrounding players
		if (topParent->getParent()) {
			topParent->getParent()->postAddNotification(thing, oldParent, index, LINK_NEAR);
		}
	} else {
		topParent->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
}

void Container::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	auto topParent = getTopParent();
	if (topParent->getCreature()) {
		topParent->postRemoveNotification(thing, newParent, index, LINK_TOPPARENT);
	} else if (topParent.get() == this) {
		//let the tile class notify surrounding players
		if (topParent->getParent()) {
			topParent->getParent()->postRemoveNotification(thing, newParent, index, LINK_NEAR);
		}
	} else {
		topParent->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
}

void Container::internalAddThing(ThingPtr thing)
{
	internalAddThing(0, thing);
}

void Container::internalAddThing(uint32_t, ThingPtr thing)
{
	auto item = thing->getItem();
	if (item == nullptr) {
		return;
	}
	item->setParent(getContainer());
	itemlist.push_front(item);
	updateItemWeight(item->getWeight());
	ammoCount += item->getItemCount();
}

void Container::startDecaying()
{
	Item::startDecaying();

	for (const auto item : itemlist) {
		item->startDecaying();
	}
}

ContainerIterator Container::iterator() const
{
	ContainerIterator cit;
	if (!itemlist.empty()) {
		cit.over.push_back(std::dynamic_pointer_cast<const Container>(shared_from_this()));
		cit.cur = itemlist.begin();
	}
	return cit;
}

ItemPtr ContainerIterator::operator*() const
{
	return *cur;
}

void ContainerIterator::advance()
{
	if (const auto i = *cur) {
		if (const auto c = i->getContainer()) {
			if (!c->empty()) {
				over.push_back(c);
			}
		}
	}

	++cur;

	if (cur == over.front()->itemlist.end()) {
		over.pop_front();
		if (!over.empty()) {
			cur = over.front()->itemlist.begin();
		}
	}
}
