// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "itemcontainer.h"
#include "iomap.h"
#include "game.h"
#include "movement.h"

extern Game g_game;
extern MoveEvents* g_moveEvents;

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
	return ownerItem->getID() != ITEM_BROWSEFIELD and not realParent.player;
}

BlackTek::ItemLocation ItemContainer::getParent() const
{
	auto current = getOwner();

	if (not current)
		return {};

	while (auto currentContainer = current->getContainer())
	{
		if (not currentContainer->config.Has(ContainerProperty::SkipsOwnNodeInChain))
			break;

		auto next = current->getContainerParent();

		if (not next)
			break;

		current = next;
	}

	return current->getLocation();
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

	return static_cast<bool>(ownerItem->getLocation());
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

	onAddContainerItem(ownerItem, item, spectators);
}

void ItemContainer::onAddContainerItem(const ItemPtr& ownerItem, ItemPtr& item, const SpectatorVec& spectators)
{
	if (not ownerItem)
		return;

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

ReturnValue ItemContainer::canAddItem(int32_t index, const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor/* = nullptr*/)
{
	if (config.Has(ContainerProperty::NeverAcceptsDirectInsert))
		return RETURNVALUE_NOTENOUGHROOM;

	if (config.Has(ContainerProperty::RequiresNoLimitFlagToAccept))
		return canAddItemInbox(item, flags);

	if (config.Has(ContainerProperty::RejectsPlayerInitiatedInserts))
		return actor ? RETURNVALUE_NOTPOSSIBLE : RETURNVALUE_NOERROR;

	if (config.Has(ContainerProperty::RequiresStoreItem))
		return canAddItemStoreInbox(item, flags);

	if (config.Has(ContainerProperty::EnforcesItemCountLimit))
		return canAddItemDepotChest(index, item, count, flags, actor);

	return canAddItemStandard(index, item, count, flags, actor);
}

ReturnValue ItemContainer::canAddItemDepotChest(int32_t index, const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor)
{
	if (item == nullptr)
		return RETURNVALUE_NOTPOSSIBLE;

	bool skipLimit = hasBitSet(FLAG_NOLIMIT, flags);

	if (not skipLimit)
	{
		int32_t addCount = 0;

		if ((item->isStackable() and item->getItemCount() != count))
			addCount = 1;

		bool sameOwnerChain = false;

		if (auto topOfChain = item->getContainerParent())
		{
			while (auto owner = topOfChain->getContainerParent())
			{
				topOfChain = owner;
			}

			sameOwnerChain = (topOfChain == getOwner());
		}

		if (not sameOwnerChain)
		{
			if (const auto container = item->getContainer())
				addCount = container->getItemHoldingCount() + 1;
			else
				addCount = 1;
		}

		if (getItemHoldingCount() + addCount > maxDepotItems)
			return RETURNVALUE_DEPOTISFULL;
	}

	return canAddItemStandard(index, item, count, flags, actor);
}

ReturnValue ItemContainer::validateAddItem(const ItemPtr& item) const
{
	if (not item)
		return RETURNVALUE_NOTPOSSIBLE;

	if (item == getOwner())
		return RETURNVALUE_THISISIMPOSSIBLE;

	if (not item->isPickupable())
		return RETURNVALUE_CANNOTPICKUP;

	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::canAddItemInbox(const ItemPtr& item, uint32_t flags) const
{
	if (not hasBitSet(FLAG_NOLIMIT, flags))
		return RETURNVALUE_CONTAINERNOTENOUGHROOM;

	return validateAddItem(item);
}

ReturnValue ItemContainer::canAddItemStoreInbox(const ItemPtr& item, uint32_t flags) const
{
	if (auto ret = validateAddItem(item); ret != RETURNVALUE_NOERROR)
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

ReturnValue ItemContainer::canAddItemStandard(int32_t index, const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor)
{
	bool childIsOwner = hasBitSet(FLAG_CHILDISOWNER, flags);

	//a child container is querying, since we are the top container (not carried by a player)
	//just return with no error.
	if (childIsOwner)
		return RETURNVALUE_NOERROR;

	if (not unlocked)
		return RETURNVALUE_NOTPOSSIBLE;

	const auto ownerItem = getOwner();

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
	BlackTek::ItemLocation ancestorLocation = ancestorItem ? BlackTek::ItemLocation{} : ownerItem->getLocation();

	auto advanceAncestor = [&ancestorItem, &ancestorLocation]()
	{
		if (ancestorItem)
		{
			auto next = ancestorItem->getContainerParent();
			ancestorLocation = next ? BlackTek::ItemLocation{} : ancestorItem->getLocation();
			ancestorItem = next;
		}
		else if (ancestorLocation)
		{
			ancestorLocation = {};
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

	auto atEnd = [&ancestorItem, &ancestorLocation]()
	{
		return not ancestorItem and not ancestorLocation;
	};

	if (not hasBitSet(FLAG_NOLIMIT, flags))
	{
		while (not atEnd())
		{
			if (ancestorItem and ancestorItem == item)
				return RETURNVALUE_THISISIMPOSSIBLE;

			auto currentContainer = ancestorItem ? ancestorItem->getContainer() : nullptr;
			if (currentContainer and currentContainer->config.Has(ContainerProperty::BlocksNestedInsertWithoutNoLimit))
				return RETURNVALUE_CONTAINERNOTENOUGHROOM;

			advanceAncestor();
		}

		if (index == INDEX_ANYWHERE and size() >= capacity() and not hasPagination())
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}
	else
	{
		while (not atEnd())
		{
			if (ancestorItem and ancestorItem == item)
				return RETURNVALUE_THISISIMPOSSIBLE;

			advanceAncestor();
		}
	}

	ItemPtr topContainerItem = ownerItem;

	while (auto containerOwner = topContainerItem->getContainerParent())
	{
		topContainerItem = containerOwner;
	}

	const bool nested = (topContainerItem != ownerItem);

	if (actor and g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
	{
		if (topContainerItem->getTile()->isHouseTile())
		{
			const bool topParentIsPlayer = (not nested) and static_cast<bool>(topContainerItem->getLocation().player);
			if (not topParentIsPlayer and not topContainerItem->getTile()->getHouse()->isInvited(actor->getPlayer()))
				return RETURNVALUE_PLAYERISNOTINVITED;
		}
	}

	if (nested)
	{
		if (auto topContainer = topContainerItem->getContainer())
			return topContainer->canAddItem(INDEX_ANYWHERE, item, count, flags | FLAG_CHILDISOWNER, actor);

		return RETURNVALUE_NOERROR;
	}

	const auto topLocation = topContainerItem->getLocation();

	if (topLocation.player)
	{
		ReturnValue ret = topLocation.player->canAddItem(INDEX_ANYWHERE, item, count, flags | FLAG_CHILDISOWNER, actor);

		if (ret == RETURNVALUE_NOERROR)
			ret = g_moveEvents->onPlayerEquip(topLocation.player, item, static_cast<slots_t>(INDEX_ANYWHERE), true);

		return ret;
	}

	if (topLocation.tile)
		return topLocation.tile->canAddItem(item, flags | FLAG_CHILDISOWNER, actor);

	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::checkAddCapacity(int32_t index, const ItemPtr& item, uint32_t count, uint32_t& acceptedCount, uint32_t flags)
{
	if (item == nullptr)
	{
		acceptedCount = 0;
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (hasBitSet(FLAG_NOLIMIT, flags) or hasPagination())
	{
		acceptedCount = std::max<uint32_t>(1, count);
		return RETURNVALUE_NOERROR;
	}

	int32_t freeSlots = std::max<int32_t>(capacity() - size(), 0);

	if (item->isStackable())
	{
		uint32_t n = 0;

		if (index == INDEX_ANYWHERE)
		{
			uint32_t slotIndex = 0;

			for (auto& containerItem : itemlist)
			{
				if (containerItem != item and containerItem->equals(item) and containerItem->getItemCount() < 100)
				{
					if (canAddItem(slotIndex++, item, count, flags) == RETURNVALUE_NOERROR)
						n += 100 - containerItem->getItemCount();
				}
			}
		}
		else
		{
			const auto& destItem = getItemByIndex(index);

			if (item->equals(destItem) and destItem->getItemCount() < 100)
			{
				if (canAddItem(index, item, count, flags) == RETURNVALUE_NOERROR)
					n = 100 - destItem->getItemCount();
			}
		}

		acceptedCount = freeSlots * 100 + n;

		if (acceptedCount < count)
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}
	else
	{
		acceptedCount = freeSlots;

		if (acceptedCount == 0)
			return RETURNVALUE_CONTAINERNOTENOUGHROOM;
	}
	return RETURNVALUE_NOERROR;
}

ReturnValue ItemContainer::canRemoveItem(const ItemPtr& item, uint32_t count, uint32_t flags, CreaturePtr actor /*= nullptr */)
{
	int32_t index = getItemIndex(item);

	if (index == -1)
		return RETURNVALUE_NOTPOSSIBLE;

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
			if (const auto player = actor->getPlayer())
			{
				ItemPtr topContainerItem = item;
				while (auto containerOwner = topContainerItem->getContainerParent())
				{
					topContainerItem = containerOwner;
				}

				const bool nested = (topContainerItem != item);
				const bool topParentIsPlayer = (not nested) and static_cast<bool>(topContainerItem->getLocation().player);
				if (not topParentIsPlayer and not ground_tile->getHouse()->isInvited(player))
					return RETURNVALUE_PLAYERISNOTINVITED;
			}
		}
	}

	return RETURNVALUE_NOERROR;
}

BlackTek::ItemLocation ItemContainer::resolveItemDestination(int32_t& index, const ItemPtr& item, ItemPtr& destItem, uint32_t& flags)
{
	auto ownerItem = getOwner();

	if (not unlocked)
		return { .containerItem = ownerItem };

	if (index == 254 /*move up*/)
	{
		index = INDEX_ANYWHERE;

		if (config.Has(ContainerProperty::SkipsOwnNodeInChain))
			return { .containerItem = ownerItem };

		if (auto parentLocation = ownerItem->getLocation())
			return parentLocation;

		return { .containerItem = ownerItem };
	}

	if (index == 255 /*add wherever*/ or index >= static_cast<int32_t>(capacity()))
		index = INDEX_ANYWHERE;

	if (not item)
		return { .containerItem = ownerItem };

	if (index != INDEX_ANYWHERE)
	{
		if (auto itemFromIndex = getItemByIndex(index))
			destItem = itemFromIndex;

		if (destItem and destItem->getContainer())
		{
			index = INDEX_ANYWHERE;
			return { .containerItem = destItem };
		}
	}

	bool autoStack = not hasBitSet(FLAG_IGNOREAUTOSTACK, flags);

	if (autoStack and item->isStackable() and item->getContainerParent() != ownerItem)
	{
		if (destItem and destItem->equals(item) and destItem->getItemCount() < 100)
			return { .containerItem = ownerItem };

		uint32_t n = 0;

		for (const auto& listItem : itemlist)
		{
			if (listItem != item and listItem->equals(item) and listItem->getItemCount() < 100)
			{
				destItem = listItem;
				index = n;
				return { .containerItem = ownerItem };
			}
			++n;
		}
	}
	return { .containerItem = ownerItem };
}


void ItemContainer::addItemAt(int32_t index, const ItemPtr& item)
{
	if (index >= static_cast<int32_t>(capacity()))
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	if (item == nullptr)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	auto ownerItem = getOwner();
	item->setContainerParent(ownerItem);
	propagateHoldingCreatureTo(item);
	itemlist.push_front(item);
	updateItemWeight(ownerItem, item->getWeight());
	ammoCount += item->getItemCount();

	if (isGenuinelyPlaced(ownerItem))
	{
		ItemPtr addedItem = item;
		onAddContainerItem(ownerItem, addedItem);
	}
}

void ItemContainer::addItemAt(int32_t index, const ItemPtr& item, const SpectatorVec& spectators)
{
	if (index >= static_cast<int32_t>(capacity()))
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	if (item == nullptr)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

	auto ownerItem = getOwner();
	item->setContainerParent(ownerItem);
	propagateHoldingCreatureTo(item);
	itemlist.push_front(item);
	updateItemWeight(ownerItem, item->getWeight());
	ammoCount += item->getItemCount();

	if (isGenuinelyPlaced(ownerItem))
	{
		ItemPtr addedItem = item;
		onAddContainerItem(ownerItem, addedItem, spectators);
	}
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

void ItemContainer::updateItem(const ItemPtr& item, uint16_t itemId, uint32_t count)
{
	int32_t index = getItemIndex(item);

	if (index == -1)
		return /*RETURNVALUE_NOTPOSSIBLE*/;

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

void ItemContainer::replaceItem(uint32_t index, const ItemPtr& item)
{
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

void ItemContainer::removeItem(const ItemPtr& item, uint32_t count)
{
	if (item == nullptr)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	int32_t index = getItemIndex(item);
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

int32_t ItemContainer::getItemIndex(const ItemConstPtr& item) const
{
	int32_t index = 0;

	for (const auto& listItem : itemlist)
	{
		if (listItem == item)
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

void ItemContainer::notifyItemAdded(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, NotifyLink)
{
	if (config.Has(ContainerProperty::NotifiesViaOwnerParent))
	{
		auto realParent = getOwner()->getLocation();
		auto link = config.Has(ContainerProperty::NotifyLinkIsTopParent) ? LINK_TOPPARENT : LINK_PARENT;

		if (realParent.tile)
			realParent.tile->notifyItemAdded(item, oldLocation, index, link);
		else if (realParent.player)
			realParent.player->notifyItemAdded(item, oldLocation, index, link);

		return;
	}

	if (config.Has(ContainerProperty::NotifiesViaParentWalk))
	{
		auto realParent = getParent();

		if (realParent.tile)
			realParent.tile->notifyItemAdded(item, oldLocation, index, LINK_PARENT);
		else if (realParent.player)
			realParent.player->notifyItemAdded(item, oldLocation, index, LINK_PARENT);

		return;
	}

	notifyItemAddedDefault(item, oldLocation, index);
}

void ItemContainer::notifyItemAdded(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, const SpectatorVec& spectators, NotifyLink)
{
	if (config.Has(ContainerProperty::NotifiesViaOwnerParent))
	{
		auto realParent = getOwner()->getLocation();
		auto link = config.Has(ContainerProperty::NotifyLinkIsTopParent) ? LINK_TOPPARENT : LINK_PARENT;

		if (realParent.tile)
			realParent.tile->notifyItemAdded(item, oldLocation, index, spectators, link);
		else if (realParent.player)
			realParent.player->notifyItemAdded(item, oldLocation, index, link);

		return;
	}

	if (config.Has(ContainerProperty::NotifiesViaParentWalk))
	{
		auto realParent = getParent();

		if (realParent.tile)
			realParent.tile->notifyItemAdded(item, oldLocation, index, spectators, LINK_PARENT);
		else if (realParent.player)
			realParent.player->notifyItemAdded(item, oldLocation, index, LINK_PARENT);

		return;
	}

	notifyItemAddedDefault(item, oldLocation, index, spectators);
}

void ItemContainer::notifyItemAddedDefault(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index)
{
	auto ownerItem = getOwner();

	if (not ownerItem)
		return;

	ItemPtr topContainerItem = ownerItem;

	while (auto containerOwner = topContainerItem->getContainerParent())
	{
		topContainerItem = containerOwner;
	}

	if (topContainerItem != ownerItem)
	{
		if (auto outerContainer = topContainerItem->getContainer())
			outerContainer->notifyItemAdded(item, oldLocation, index, LINK_PARENT);

		return;
	}

	const auto topLocation = topContainerItem->getLocation();

	if (topLocation.player)
		topLocation.player->notifyItemAdded(item, oldLocation, index, LINK_TOPPARENT);
	else if (topLocation.tile)
		topLocation.tile->notifyItemAdded(item, oldLocation, index, LINK_NEAR);
}

void ItemContainer::notifyItemAddedDefault(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, const SpectatorVec& spectators)
{
	auto ownerItem = getOwner();

	if (not ownerItem)
		return;

	ItemPtr topContainerItem = ownerItem;

	while (auto containerOwner = topContainerItem->getContainerParent())
	{
		topContainerItem = containerOwner;
	}

	if (topContainerItem != ownerItem)
	{
		if (auto outerContainer = topContainerItem->getContainer())
			outerContainer->notifyItemAdded(item, oldLocation, index, spectators, LINK_PARENT);

		return;
	}

	const auto topLocation = topContainerItem->getLocation();

	if (topLocation.player)
		topLocation.player->notifyItemAdded(item, oldLocation, index, LINK_TOPPARENT);
	else if (topLocation.tile)
		topLocation.tile->notifyItemAdded(item, oldLocation, index, spectators, LINK_NEAR);
}

void ItemContainer::notifyItemRemoved(const ItemPtr& item, const BlackTek::ItemLocation& newLocation, int32_t index, NotifyLink)
{
	if (config.Has(ContainerProperty::NotifiesViaOwnerParent))
	{
		auto realParent = getOwner()->getLocation();
		auto link = config.Has(ContainerProperty::NotifyLinkIsTopParent) ? LINK_TOPPARENT : LINK_PARENT;

		if (realParent.tile)
			realParent.tile->notifyItemRemoved(item, newLocation, index, link);
		else if (realParent.player)
			realParent.player->notifyItemRemoved(item, newLocation, index, link);

		return;
	}

	if (config.Has(ContainerProperty::NotifiesViaParentWalk))
	{
		auto realParent = getParent();

		if (realParent.tile)
			realParent.tile->notifyItemRemoved(item, newLocation, index, LINK_PARENT);
		else if (realParent.player)
			realParent.player->notifyItemRemoved(item, newLocation, index, LINK_PARENT);

		return;
	}

	notifyItemRemovedDefault(item, newLocation, index);
}

void ItemContainer::notifyItemRemovedDefault(const ItemPtr& item, const BlackTek::ItemLocation& newLocation, int32_t index)
{
	auto ownerItem = getOwner();

	if (not ownerItem)
		return;

	ItemPtr topContainerItem = ownerItem;

	while (auto containerOwner = topContainerItem->getContainerParent())
	{
		topContainerItem = containerOwner;
	}

	if (topContainerItem != ownerItem)
	{
		if (auto outerContainer = topContainerItem->getContainer())
			outerContainer->notifyItemRemoved(item, newLocation, index, LINK_PARENT);

		return;
	}

	const auto topLocation = topContainerItem->getLocation();

	if (topLocation.player)
		topLocation.player->notifyItemRemoved(item, newLocation, index, LINK_TOPPARENT);
	else if (topLocation.tile)
		topLocation.tile->notifyItemRemoved(item, newLocation, index, LINK_NEAR);
}

void ItemContainer::addItemSilently(const ItemPtr& item)
{
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
