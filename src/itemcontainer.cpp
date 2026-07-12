// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "itemcontainer.h"
#include "iomap.h"
#include "game.h"

extern Game g_game;

using BlackTek::Containers::ContainerConfig;
using BlackTek::Containers::ContainerProperty;

namespace
{
	constexpr ContainerConfig ConfigFor(ContainerSubType subtype)
	{
		switch (subtype)
		{
			case ContainerSubType::DepotChest:
				return ContainerConfig{}
					.Set(ContainerProperty::EnforcesItemCountLimit)
					.Set(ContainerProperty::AcceptsStoreItemsDirectly)
					.Set(ContainerProperty::SkipsOwnNodeInChain)
					.Set(ContainerProperty::NotifiesViaParentWalk);

			case ContainerSubType::DepotLocker:
				return ContainerConfig{}
					.Set(ContainerProperty::NeverAcceptsDirectInsert)
					.Set(ContainerProperty::NotifiesViaParentWalk);

			case ContainerSubType::Inbox:
				return ContainerConfig{}
					.Set(ContainerProperty::RequiresNoLimitFlagToAccept)
					.Set(ContainerProperty::BlocksNestedInsertWithoutNoLimit)
					.Set(ContainerProperty::SkipsOwnNodeInChain)
					.Set(ContainerProperty::NotifiesViaParentWalk);

			case ContainerSubType::StoreInbox:
				return ContainerConfig{}
					.Set(ContainerProperty::RequiresStoreItem)
					.Set(ContainerProperty::BlocksInsertIntoStoreChild)
					.Set(ContainerProperty::NotifiesViaOwnerParent)
					.Set(ContainerProperty::NotifyLinkIsTopParent);

			case ContainerSubType::RewardChest:
				return ContainerConfig{}
					.Set(ContainerProperty::Removable)
					.Set(ContainerProperty::RejectsPlayerInitiatedInserts)
					.Set(ContainerProperty::NotifiesViaOwnerParent);

			default: // None
				return ContainerConfig{}
					.Set(ContainerProperty::Removable);
		}
	}
}

ItemContainer::ItemContainer(uint16_t size, bool unlocked /*= true*/, bool pagination /*= false*/, ContainerSubType subtype /*= ContainerSubType::None*/) :
	container_subtype(subtype),
	config(ConfigFor(subtype)),
	maxSize(size),
	unlocked(unlocked),
	pagination(pagination)
{
}

void ItemContainer::cloneInto(ItemContainer& target) const
{
	for (const auto& item : itemlist)
		target.addItem(item->clone());

	target.totalWeight = totalWeight;
}

void ItemContainer::setParentToTileItems(const TilePtr& tile)
{
	auto itemVector = tile->getItemList();
	if (itemVector)
	{
		auto self = getOwner();
		for (const ItemPtr& item : *itemVector)
		{
			if ((item->getContainer() or item->hasProperty(CONST_PROP_MOVEABLE)) and not item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID))
			{
				itemlist.push_front(item);
				item->setContainerParent(self);
			}
		}
	}
}

void ItemContainer::setHoldingCreature(const CreaturePtr& creature)
{
	holdingCreature = creature;
	for (const auto& item : itemlist)
	{
		if (auto childContainer = item->getContainer())
			childContainer->setHoldingCreature(creature);
	}
}

std::string ItemContainer::getName(bool addArticle /* = false*/) const
{
	auto ownerItem = getOwner();
	if (not ownerItem)
	{
		return {};
	}
	const ItemType& it = Item::items[ownerItem->getID()];
	return Item::getNameDescription(it, ownerItem, -1, addArticle);
}

bool ItemContainer::hasParent() const noexcept
{
	auto ownerItem = getOwner();
	if (not ownerItem)
	{
		return false;
	}

	auto realParent = getParent();
	return ownerItem->getID() != ITEM_BROWSEFIELD and not (realParent and realParent->getCylinderSubType() == CylinderSubType::Player);
}

CylinderPtr ItemContainer::getParent() const
{
	auto current = getOwner();

	if (not current)
		return nullptr;

	while (auto currentContainer = current->getContainer())
	{
		if (not currentContainer->config.Has(ContainerProperty::SkipsOwnNodeInChain))
			break;

		auto next = current->getContainerParent();

		if (not next)
			break;

		current = next;
	}

	return current->getParent();
}

void ItemContainer::addItem(const ItemPtr& item)
{
	itemlist.push_back(item);
	item->setContainerParent(getOwner());
	propagateHoldingCreatureTo(item);
}

void ItemContainer::propagateHoldingCreatureTo(const ItemPtr& item) const
{
	if (auto childContainer = item->getContainer())
		childContainer->setHoldingCreature(getHoldingCreature());
}

Attr_ReadValue ItemContainer::readAttr(const AttrTypes_t attr, PropStream& propStream)
{
	if (attr == ATTR_CONTAINER_ITEMS)
	{
		if (not propStream.read<uint32_t>(serializationCount))
			return ATTR_READ_ERROR;

		return ATTR_READ_END;
	}

	if (attr == ATTR_DEPOT_ID)
	{
		if (not propStream.read<uint16_t>(depotId))
			return ATTR_READ_ERROR;

		return ATTR_READ_CONTINUE;
	}
	return ATTR_READ_CONTINUE;
}

bool ItemContainer::unserializeItemNode(OTB::Loader& loader, const OTB::Node& node, PropStream&)
{
	auto ownerItem = getOwner();
	for (auto& itemNode : node.children)
	{
		if (itemNode.type != OTBM_ITEM)
			return false;

		PropStream itemPropStream;

		if (not loader.getProps(itemNode, itemPropStream))
			return false;

		auto item = Item::CreateItem(itemPropStream);

		if (not item)
			return false;

		if (not item->unserializeItemNode(loader, itemNode, itemPropStream))
			return false;

		addItem(item);
		updateItemWeight(ownerItem, item->getWeight());
	}
	return true;
}

void ItemContainer::updateItemWeight(const ItemPtr& ownerItem, int32_t diff)
{
	totalWeight += diff;
	auto currentOwner = ownerItem;

	while (currentOwner)
	{
		auto outerItem = currentOwner->getContainerParent();

		if (not outerItem)
			break;

		if (auto outerContainer = outerItem->getContainer())
			outerContainer->totalWeight += diff;

		currentOwner = outerItem;
	}
}

bool ItemContainer::isGenuinelyPlaced(const ItemPtr& ownerItem) const
{
	if (not ownerItem)
		return false;

	if (ownerItem->getContainerParent())
		return true;

	auto realParent = ownerItem->getParent();
	return realParent and realParent != VirtualCylinder::virtualCylinder;
}

std::string ItemContainer::getContentDescription() const
{
	std::ostringstream os;
	return getContentDescription(os).str();
}

std::ostringstream& ItemContainer::getContentDescription(std::ostringstream& os) const
{
	bool firstitem = true;

	for (ContainerIterator it = iterator(); it.hasNext(); it.advance())
	{
		const auto item = *it;
		const auto container = item->getContainer();

		if (container and not container->empty())
			continue;

		if (firstitem)
			firstitem = false;

		else
			os << ", ";

		os << item->getNameDescription();
	}

	if (firstitem)
	{
		os << "nothing";
	}
	return os;
}

ItemPtr ItemContainer::getItemByIndex(size_t index) const
{
	if (index >= size())
		return nullptr;

	return itemlist[index];
}

uint32_t ItemContainer::getItemHoldingCount() const
{
	uint32_t counter = 0;

	for (ContainerIterator it = iterator(); it.hasNext(); it.advance())
		++counter;

	return counter;
}

bool ItemContainer::isHoldingItem(const ItemConstPtr& item) const noexcept
{
	for (ContainerIterator it = iterator(); it.hasNext(); it.advance())
	{
		if ((*it)->equals(item))
			return true;
	}
	return false;
}

bool ItemContainer::isRewardCorpse() const noexcept
{
	for (const auto& subItem : getItemList())
	{
		if (subItem->getID() == ITEM_REWARD_CONTAINER)
			return true;
	}
	return false;
}

void ItemContainer::removeInbox(const ItemPtr& inbox)
{
	auto cit = std::ranges::find(itemlist, inbox);
	if (cit == itemlist.end())
		return;

	itemlist.erase(cit);
}

void ItemContainer::onAddContainerItem(const ItemPtr& ownerItem, ItemPtr& item)
{
	if (not ownerItem)
		return;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, ownerItem->getPosition(), false, true, 1, 1, 1, 1);

	for (const auto& c : spectators.players())
	{
		const auto c_player = std::static_pointer_cast<Player>(c);
		c_player->sendAddContainerItem(ownerItem->getContainer(), item);
	}

	for (const auto& c : spectators.players())
	{
		const auto c_player = std::static_pointer_cast<Player>(c);
		c_player->onAddContainerItem(item);
	}
}

void ItemContainer::onUpdateContainerItem(const ItemPtr& ownerItem, uint32_t index, const ItemPtr& oldItem, const ItemPtr& newItem)
{
	if (not ownerItem)
		return;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, ownerItem->getPosition(), false, true, 1, 1, 1, 1);

	for (const auto& c : spectators.players())
	{
		const auto c_player = std::static_pointer_cast<Player>(c);
		c_player->sendUpdateContainerItem(ownerItem->getContainer(), index, newItem);
	}

	for (const auto& c : spectators.players())
	{
		const auto c_player = std::static_pointer_cast<Player>(c);
		c_player->onUpdateContainerItem(ownerItem->getContainer(), oldItem, newItem);
	}
}

void ItemContainer::onRemoveContainerItem(const ItemPtr& ownerItem, uint32_t index, const ItemPtr& item)
{
	if (not ownerItem)
		return;

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, ownerItem->getPosition(), false, true, 1, 1, 1, 1);

	for (const auto& c : spectators.players())
	{
		const auto c_player = std::static_pointer_cast<Player>(c);
		c_player->sendRemoveContainerItem(ownerItem->getContainer(), index);
	}

	for (const auto& c : spectators.players())
	{
		const auto c_player = std::static_pointer_cast<Player>(c);
		c_player->onRemoveContainerItem(ownerItem->getContainer(), item);
	}
}

ReturnValue ItemContainer::queryAdd(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor/* = nullptr*/)
{
	if (config.Has(ContainerProperty::NeverAcceptsDirectInsert))
		return RETURNVALUE_NOTENOUGHROOM;

	if (config.Has(ContainerProperty::RequiresNoLimitFlagToAccept))
		return queryAddInbox(thing, flags);

	if (config.Has(ContainerProperty::RejectsPlayerInitiatedInserts))
		return actor ? RETURNVALUE_NOTPOSSIBLE : RETURNVALUE_NOERROR;

	if (config.Has(ContainerProperty::RequiresStoreItem))
		return queryAddStoreInbox(thing, flags);

	if (config.Has(ContainerProperty::EnforcesItemCountLimit))
		return queryAddDepotChest(index, thing, count, flags, actor);

	return queryAddGeneric(index, thing, count, flags, actor);
}

ReturnValue ItemContainer::queryAddDepotChest(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor)
{
	auto item = thing->getItem();

	if (item == nullptr)
		return RETURNVALUE_NOTPOSSIBLE;

	bool skipLimit = hasBitSet(FLAG_NOLIMIT, flags);

	if (not skipLimit)
	{
		int32_t addCount = 0;

		if ((item->isStackable() and item->getItemCount() != count))
			addCount = 1;

		if (item->getTopParent().get() != getOwner().get())
		{
			if (const auto container = item->getContainer())
				addCount = container->getItemHoldingCount() + 1;
			else
				addCount = 1;
		}

		if (getItemHoldingCount() + addCount > maxDepotItems)
			return RETURNVALUE_DEPOTISFULL;
	}

	return queryAddGeneric(index, thing, count, flags, actor);
}

ReturnValue ItemContainer::queryAddValidateItem(const ThingPtr& thing, ItemPtr& outItem) const
{
	outItem = thing->getItem();

	if (not outItem)
		return RETURNVALUE_NOTPOSSIBLE;

	if (outItem == getOwner())
		return RETURNVALUE_THISISIMPOSSIBLE;

	if (not outItem->isPickupable())
		return RETURNVALUE_CANNOTPICKUP;

	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::queryAddInbox(const ThingPtr& thing, uint32_t flags) const
{
	if (not hasBitSet(FLAG_NOLIMIT, flags))
		return RETURNVALUE_CONTAINERNOTENOUGHROOM;

	ItemPtr item;
	return queryAddValidateItem(thing, item);
}

ReturnValue ItemContainer::queryAddStoreInbox(const ThingPtr& thing, uint32_t flags) const
{
	ItemPtr item;
	if (auto ret = queryAddValidateItem(thing, item); ret != RETURNVALUE_NOERROR)
		return ret;

	if (not hasBitSet(FLAG_NOLIMIT, flags))
	{
		if (not item->isStoreItem())
			return RETURNVALUE_CANNOTMOVEITEMISNOTSTOREITEM;

		if (const auto& container = item->getContainer(); container and not container->empty())
			return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
	}

	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::queryAddGeneric(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor)
{
	bool childIsOwner = hasBitSet(FLAG_CHILDISOWNER, flags);

	//a child container is querying, since we are the top container (not carried by a player)
	//just return with no error.
	if (childIsOwner)
		return RETURNVALUE_NOERROR;

	if (not unlocked)
		return RETURNVALUE_NOTPOSSIBLE;

	const auto ownerItem = getOwner();
	const auto item = thing->getItem();

	if (item == nullptr)
		return RETURNVALUE_NOTPOSSIBLE;

	if (not item->isPickupable())
		return RETURNVALUE_CANNOTPICKUP;

	if (item == ownerItem)
		return RETURNVALUE_THISISIMPOSSIBLE;

	if (ownerItem->getWeaponType() == WEAPON_QUIVER and item->getWeaponType() != WEAPON_AMMO)
		return RETURNVALUE_QUIVERAMMOONLY;

	if (item->isStoreItem() and not config.Has(ContainerProperty::AcceptsStoreItemsDirectly))
		return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;

	ItemPtr ancestorItem = ownerItem->getContainerParent();
	CylinderPtr ancestorCylinder = ancestorItem ? nullptr : ownerItem->getParent();

	auto advanceAncestor = [&ancestorItem, &ancestorCylinder]()
	{
		if (ancestorItem)
		{
			auto next = ancestorItem->getContainerParent();
			ancestorCylinder = next ? nullptr : ancestorItem->getParent();
			ancestorItem = next;
		}
		else if (ancestorCylinder)
		{
			ancestorCylinder = ancestorCylinder->getParent();
		}
	};

	if (config.Has(ContainerProperty::SkipsOwnNodeInChain))
		advanceAncestor();

	ContainerPtr ancestorContainer = ancestorItem ? ancestorItem->getContainer() : nullptr;

	// don't allow moving items into container that is store item and is in store inbox
	if (ownerItem->isStoreItem() and ancestorContainer and ancestorContainer->config.Has(ContainerProperty::BlocksInsertIntoStoreChild))
	{
		ReturnValue ret = RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;

		if (not item->isStoreItem())
			ret = RETURNVALUE_CANNOTMOVEITEMISNOTSTOREITEM;

		return ret;
	}

	auto atEnd = [&ancestorItem, &ancestorCylinder]()
	{
		return not ancestorItem and not ancestorCylinder;
	};

	if (not hasBitSet(FLAG_NOLIMIT, flags))
	{
		while (not atEnd())
		{
			if ((ancestorItem and ancestorItem == thing) or (ancestorCylinder and ancestorCylinder == thing))
				return RETURNVALUE_THISISIMPOSSIBLE;

			auto currentContainer = ancestorItem ? ancestorItem->getContainer() : nullptr;
			if (currentContainer and currentContainer->config.Has(ContainerProperty::BlocksNestedInsertWithoutNoLimit))
				return RETURNVALUE_CONTAINERNOTENOUGHROOM;

			advanceAncestor();
		}

		if (index == INDEX_WHEREEVER and size() >= capacity() and not hasPagination())
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}
	else
	{
		while (not atEnd())
		{
			if ((ancestorItem and ancestorItem == thing) or (ancestorCylinder and ancestorCylinder == thing))
				return RETURNVALUE_THISISIMPOSSIBLE;

			advanceAncestor();
		}
	}

	const auto topParent = ownerItem->getTopParent();

	if (actor and g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
	{
		if (topParent->getTile()->isHouseTile())
		{
			const auto topCylinder = topParent->getCylinder();
			const bool topParentIsPlayer = topCylinder and topCylinder->getCylinderSubType() == CylinderSubType::Player;
			if (not topParentIsPlayer and not topParent->getTile()->getHouse()->isInvited(actor->getPlayer()))
				return RETURNVALUE_PLAYERISNOTINVITED;
		}
	}

	if (topParent.get() != ownerItem.get())
	{
		if (auto topContainer = topParent->getContainer())
			return topContainer->queryAdd(INDEX_WHEREEVER, item, count, flags | FLAG_CHILDISOWNER, actor);

		if (auto topCylinder = topParent->getCylinder())
			return topCylinder->queryAdd(INDEX_WHEREEVER, item, count, flags | FLAG_CHILDISOWNER, actor);

		return RETURNVALUE_NOERROR;
	}
	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t& maxQueryCount, uint32_t flags)
{
	auto item = thing->getItem();

	if (item == nullptr)
	{
		maxQueryCount = 0;
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (hasBitSet(FLAG_NOLIMIT, flags) or hasPagination())
	{
		maxQueryCount = std::max<uint32_t>(1, count);
		return RETURNVALUE_NOERROR;
	}

	int32_t freeSlots = std::max<int32_t>(capacity() - size(), 0);

	if (item->isStackable())
	{
		uint32_t n = 0;

		if (index == INDEX_WHEREEVER)
		{
			uint32_t slotIndex = 0;

			for (auto& containerItem : itemlist)
			{
				if (containerItem != item and containerItem->equals(item) and containerItem->getItemCount() < 100)
				{
					if (queryAdd(slotIndex++, item, count, flags) == RETURNVALUE_NOERROR)
						n += 100 - containerItem->getItemCount();
				}
			}
		}
		else
		{
			const auto& destItem = getItemByIndex(index);

			if (item->equals(destItem) and destItem->getItemCount() < 100)
			{
				if (queryAdd(index, item, count, flags) == RETURNVALUE_NOERROR)
					n = 100 - destItem->getItemCount();
			}
		}

		maxQueryCount = freeSlots * 100 + n;

		if (maxQueryCount < count)
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}
	else
	{
		maxQueryCount = freeSlots;

		if (maxQueryCount == 0)
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}
	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor /*= nullptr */)
{
	int32_t index = getThingIndex(thing);

	if (index == -1)
		return RETURNVALUE_NOTPOSSIBLE;

	const auto item = thing->getItem();

	if (item == nullptr)
		return RETURNVALUE_NOTPOSSIBLE;

	if (count == 0 or (item->isStackable() and count > item->getItemCount()))
		return RETURNVALUE_NOTPOSSIBLE;

	if (not item->isMoveable() and not hasBitSet(FLAG_IGNORENOTMOVEABLE, flags))
		return RETURNVALUE_NOTMOVEABLE;

	if (actor and g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
	{
		if (auto ground_tile = item->getTile(); ground_tile and ground_tile->isHouseTile())
		{
			const auto topParent = item->getTopParent();

			if (const auto player = actor->getPlayer())
			{
				const auto topCylinder = topParent ? topParent->getCylinder() : nullptr;
				const bool topParentIsPlayer = topCylinder and topCylinder->getCylinderSubType() == CylinderSubType::Player;
				if (not topParent or (not topParentIsPlayer and not ground_tile->getHouse()->isInvited(player)))
					return RETURNVALUE_PLAYERISNOTINVITED;
			}
		}
	}

	return RETURNVALUE_NOERROR;
}

ThingPtr ItemContainer::queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem, uint32_t& flags)
{
	auto ownerItem = getOwner();

	if (not unlocked)
		return ownerItem;

	if (index == 254 /*move up*/)
	{
		index = INDEX_WHEREEVER;

		if (config.Has(ContainerProperty::SkipsOwnNodeInChain))
			return ownerItem;

		if (auto parentContainerItem = ownerItem->getContainerParent())
			return parentContainerItem;

		if (auto realParent = ownerItem->getParent())
			return realParent;

		return ownerItem;
	}

	if (index == 255 /*add wherever*/ or index >= static_cast<int32_t>(capacity()))
		index = INDEX_WHEREEVER;

	ItemPtr item = thing ? thing->getItem() : nullptr;

	if (not item)
		return ownerItem;

	if (index != INDEX_WHEREEVER)
	{
		if (auto itemFromIndex = getItemByIndex(index))
			destItem = itemFromIndex;

		if (destItem and destItem->getContainer())
		{
			index = INDEX_WHEREEVER;
			return destItem;
		}
	}

	bool autoStack = not hasBitSet(FLAG_IGNOREAUTOSTACK, flags);

	if (autoStack and item->isStackable() and item->getContainerParent() != ownerItem)
	{
		if (destItem and destItem->equals(item) and destItem->getItemCount() < 100)
			return ownerItem;

		uint32_t n = 0;

		for (const auto& listItem : itemlist)
		{
			if (listItem != item and listItem->equals(item) and listItem->getItemCount() < 100)
			{
				destItem = listItem;
				index = n;
				return ownerItem;
			}
			++n;
		}
	}
	return ownerItem;
}


void ItemContainer::addThing(ThingPtr thing)
{
	return addThing(0, thing);
}

void ItemContainer::addThing(int32_t index, ThingPtr thing)
{
	if (index >= static_cast<int32_t>(capacity()))
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	ItemPtr item = thing->getItem();

	if (item == nullptr)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	auto ownerItem = getOwner();
	item->setContainerParent(ownerItem);
	propagateHoldingCreatureTo(item);
	itemlist.push_front(item);
	updateItemWeight(ownerItem, item->getWeight());
	ammoCount += item->getItemCount();

	if (isGenuinelyPlaced(ownerItem))
		onAddContainerItem(ownerItem, item);
}

void ItemContainer::addItemBack(ItemPtr& item)
{
	addItem(item);
	auto ownerItem = getOwner();
	updateItemWeight(ownerItem, item->getWeight());
	ammoCount += item->getItemCount();

	if (isGenuinelyPlaced(ownerItem))
		onAddContainerItem(ownerItem, item);
}

void ItemContainer::updateThing(ThingPtr thing, uint16_t itemId, uint32_t count)
{
	int32_t index = getThingIndex(thing);

	if (index == -1)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	const auto item = thing->getItem();

	if (item == nullptr)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	ammoCount += count;
	ammoCount -= item->getItemCount();

	const int32_t oldWeight = item->getWeight();
	item->setID(itemId);
	item->setSubType(count);
	auto ownerItem = getOwner();
	updateItemWeight(ownerItem, -oldWeight + item->getWeight());

	if (isGenuinelyPlaced(ownerItem))
		onUpdateContainerItem(ownerItem, index, item, item);
}

void ItemContainer::replaceThing(uint32_t index, ThingPtr thing)
{
	const auto item = thing->getItem();

	if (not item)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	const auto replacedItem = getItemByIndex(index);

	if (not replacedItem)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	ammoCount -= replacedItem->getItemCount();
	itemlist[index] = item;
	auto ownerItem = getOwner();
	item->setContainerParent(ownerItem);
	propagateHoldingCreatureTo(item);
	updateItemWeight(ownerItem, -static_cast<int32_t>(replacedItem->getWeight()) + item->getWeight());
	ammoCount += item->getItemCount();

	if (isGenuinelyPlaced(ownerItem))
		onUpdateContainerItem(ownerItem, index, replacedItem, item);

	replacedItem->clearParent();
}

void ItemContainer::removeThing(ThingPtr thing, uint32_t count)
{
	auto item = thing->getItem();
	if (item == nullptr)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	int32_t index = getThingIndex(thing);
	if (index == -1)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	auto ownerItem = getOwner();

	if (item->isStackable() and count != item->getItemCount())
	{
		uint8_t newCount = static_cast<uint8_t>(std::max<int32_t>(0, item->getItemCount() - count));
		const int32_t oldWeight = item->getWeight();
		ammoCount -= (item->getItemCount() - newCount);
		item->setItemCount(newCount);
		updateItemWeight(ownerItem, -oldWeight + item->getWeight());

		if (isGenuinelyPlaced(ownerItem))
			onUpdateContainerItem(ownerItem, index, item, item);
	}
	else
	{
		updateItemWeight(ownerItem, -static_cast<int32_t>(item->getWeight()));
		ammoCount -= item->getItemCount();

		if (isGenuinelyPlaced(ownerItem))
			onRemoveContainerItem(ownerItem, index, item);

		item->clearParent();
		itemlist.erase(itemlist.begin() + index);
	}
}

int32_t ItemContainer::getThingIndex(ThingPtr thing)
{
	int32_t index = 0;

	for (const auto& item : itemlist)
	{
		if (item == thing)
			return index;

		++index;
	}
	return -1;
}

size_t ItemContainer::getFirstIndex() const
{
	return 0;
}

size_t ItemContainer::getLastIndex() const
{
	return size();
}

uint32_t ItemContainer::getItemTypeCount(uint16_t itemId, int32_t subType/* = -1*/) const
{
	uint32_t count = 0;

	for (const auto& item : itemlist)
	{
		if (item->getID() == itemId)
			count += Item::countByType(item, subType);
	}
	return count;
}

gtl::btree_map<uint32_t, uint32_t>& ItemContainer::getAllItemTypeCount(gtl::btree_map<uint32_t, uint32_t>& countMap) const
{
	for (const auto item : itemlist)
		countMap[item->getID()] += item->getItemCount();

	return countMap;
}

ThingPtr ItemContainer::getThing(size_t index)
{
	return getItemByIndex(index);
}

void ItemContainer::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t)
{
	if (config.Has(ContainerProperty::NotifiesViaOwnerParent))
	{
		if (auto realParent = getOwner()->getParent())
		{
			auto link = config.Has(ContainerProperty::NotifyLinkIsTopParent) ? LINK_TOPPARENT : LINK_PARENT;
			realParent->postAddNotification(thing, oldParent, index, link);
		}
		return;
	}

	if (config.Has(ContainerProperty::NotifiesViaParentWalk))
	{
		if (auto realParent = getParent())
			realParent->postAddNotification(thing, oldParent, index, LINK_PARENT);

		return;
	}

	postAddNotificationDefault(thing, oldParent, index);
}

void ItemContainer::postAddNotificationDefault(ThingPtr thing, CylinderPtr oldParent, int32_t index)
{
	auto ownerItem = getOwner();

	if (not ownerItem)
		return;

	auto topParent = ownerItem->getTopParent();

	if (auto outerContainer = topParent->getContainer())
	{
		outerContainer->postAddNotification(thing, oldParent, index, LINK_PARENT);
	}
	else if (auto realParent = topParent->getCylinder())
	{
		auto link = realParent->getCylinderSubType() == CylinderSubType::Player ? LINK_TOPPARENT : LINK_NEAR;
		realParent->postAddNotification(thing, oldParent, index, link);
	}
}

void ItemContainer::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	if (config.Has(ContainerProperty::NotifiesViaOwnerParent))
	{
		if (auto realParent = getOwner()->getParent())
		{
			auto link = config.Has(ContainerProperty::NotifyLinkIsTopParent) ? LINK_TOPPARENT : LINK_PARENT;
			realParent->postRemoveNotification(thing, newParent, index, link);
		}
		return;
	}

	if (config.Has(ContainerProperty::NotifiesViaParentWalk))
	{
		if (auto realParent = getParent())
			realParent->postRemoveNotification(thing, newParent, index, LINK_PARENT);

		return;
	}

	postRemoveNotificationDefault(thing, newParent, index);
}

void ItemContainer::postRemoveNotificationDefault(ThingPtr thing, CylinderPtr newParent, int32_t index)
{
	auto ownerItem = getOwner();

	if (not ownerItem)
		return;

	auto topParent = ownerItem->getTopParent();

	if (auto outerContainer = topParent->getContainer())
	{
		outerContainer->postRemoveNotification(thing, newParent, index, LINK_PARENT);
	}
	else if (auto realParent = topParent->getCylinder())
	{
		auto link = realParent->getCylinderSubType() == CylinderSubType::Player ? LINK_TOPPARENT : LINK_NEAR;
		realParent->postRemoveNotification(thing, newParent, index, link);
	}
}

void ItemContainer::internalAddThing(ThingPtr thing)
{
	internalAddThing(0, thing);
}

void ItemContainer::internalAddThing(uint32_t, ThingPtr thing)
{
	auto item = thing->getItem();

	if (item == nullptr)
	{
		return;
	}
	auto ownerItem = getOwner();
	item->setContainerParent(ownerItem);
	propagateHoldingCreatureTo(item);
	itemlist.push_front(item);
	updateItemWeight(ownerItem, item->getWeight());
	ammoCount += item->getItemCount();
}

void ItemContainer::startDecaying()
{
	for (const auto item : itemlist)
		item->startDecaying();
}

ContainerIterator ItemContainer::iterator() const
{
	ContainerIterator cit;

	if (not itemlist.empty())
	{
		if (auto ownerItem = getOwner())
		{
			cit.over.push_back(ownerItem->getContainer());
			cit.cur = itemlist.begin();
		}
	}
	return cit;
}

ItemPtr ContainerIterator::operator*() const
{
	return *cur;
}

void ContainerIterator::advance()
{
	if (const auto& i = *cur)
	{
		if (const auto c = i->getContainer())
		{
			if (not c->empty())
				over.push_back(c);
		}
	}

	++cur;

	if (cur == over.front()->itemlist.end())
	{
		over.pop_front();
		if (not over.empty())
		{
			cur = over.front()->itemlist.begin();
		}
	}
}
