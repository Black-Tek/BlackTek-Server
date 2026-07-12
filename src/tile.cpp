// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include <boost/range/adaptor/reversed.hpp>

#include "tile.h"

#include "creature.h"
#include "creaturecontainer.h"
#include "combat.h"
#include "game.h"
#include "mailbox.h"
#include "monster.h"
#include "movement.h"
#include "teleport.h"
#include "trashholder.h"
#include "configmanager.h"

extern Game g_game;
extern MoveEvents* g_moveEvents;
extern ConfigManager g_config;

Tile real_nullptr_tile(0xFFFF, 0xFFFF, 0xFF);

Tile::Tile(uint16_t x, uint16_t y, uint8_t z) : tilePos(x, y, z)
{
	items = std::make_shared<TileItemVector>();
	thing_subtype = ThingSubType::Tile;
	cylinder_subtype = CylinderSubType::Tile;
}

Tile::Tile(uint16_t x, uint16_t y, uint8_t z, House* house) : tilePos(x, y, z)
{
	items = std::make_shared<TileItemVector>();
	this->house = house;
	thing_subtype = ThingSubType::Tile;
	cylinder_subtype = CylinderSubType::Tile;
}

Tile::~Tile()
{
	ground.reset();
}

CreatureContainer& Tile::ensureCreatures()
{
	if (not creatures)
	{
		creatures = std::make_unique<CreatureContainer>();
		creatures->owner = static_shared_this<Tile>();
	}
	return *creatures;
}

void Tile::applyItemProperties(const ItemConstPtr& item)
{
	for (uint32_t p = 0; p <= static_cast<uint32_t>(CONST_PROP_SUPPORTHANGABLE); ++p) {
		if (item->hasProperty(static_cast<ITEMPROPERTY>(p))) {
			itemProperties |= (1u << p);
		}
	}
}

void Tile::recalculateItemProperties()
{
	itemProperties = 0;
	if (ground) {
		applyItemProperties(ground);
	}
	if (const auto itemList = getItemList()) {
		for (const auto& item : *itemList) {
			applyItemProperties(item);
		}
	}
}

bool Tile::hasProperty(const ItemPtr& exclude, ITEMPROPERTY prop) const
{
	if (ground and ground != exclude and ground->hasProperty(prop)) {
		return true;
	}
	if (const auto itemList = getItemList()) {
		for (const auto& item : *itemList) {
			if (item != exclude and item->hasProperty(prop)) {
				return true;
			}
		}
	}
	return false;
}

bool Tile::hasHeight(const uint32_t n) const
{
	uint32_t height = 0;

	if (ground) {
		if (ground->hasProperty(CONST_PROP_HASHEIGHT)) {
			++height;
		}

		if (n == height) {
			return true;
		}
	}

	if (const auto items = getItemList()) {
		for (const auto item : *items) {
			if (item->hasProperty(CONST_PROP_HASHEIGHT)) {
				++height;
			}

			if (n == height) {
				return true;
			}
		}
	}
	return false;
}

size_t Tile::getCreatureCount() const
{
	if (const auto creatures = getCreatures()) {
		return creatures->size();
	}
	return 0;
}

size_t Tile::getItemCount() const
{
	if (const auto items = getItemList()) {
		return items->size();
	}
	return 0;
}

uint32_t Tile::getTopItemCount() const
{
	if (const auto items = getItemList()) {
		return items->getTopItemCount();
	}
	return 0;
}

uint32_t Tile::getDownItemCount() const
{
	if (const auto items = getItemList()) {
		return items->getDownItemCount();
	}
	return 0;
}

std::string Tile::getDescription(int32_t)
{
	return "You dont know why, but you cant see anything!";
}

TeleportPtr Tile::getTeleportItem() const
{
	if (!hasFlag(TILESTATE_TELEPORT)) {
		return nullptr;
	}

	if (const auto items = getItemList()) {
		for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
			if ((*it)->getTeleport()) {
				return (*it)->getTeleport();
			}
		}
	}
	return nullptr;
}

MagicFieldPtr Tile::getFieldItem() const
{
	if (!hasFlag(TILESTATE_MAGICFIELD)) {
		return nullptr;
	}

	if (ground && ground->getMagicField()) {
		return ground->getMagicField();
	}

	if (const auto items = getItemList()) {
		for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
			if ((*it)->getMagicField()) {
				return (*it)->getMagicField();
			}
		}
	}
	return nullptr;
}

TrashHolderPtr Tile::getTrashHolder() const
{
	if (!hasFlag(TILESTATE_TRASHHOLDER)) {
		return nullptr;
	}

	if (ground && ground->getTrashHolder()) {
		return ground->getTrashHolder();
	}

	if (const auto items = getItemList()) {
		for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
			if ((*it)->getTrashHolder()) {
				return (*it)->getTrashHolder();
			}
		}
	}
	return nullptr;
}

MailboxPtr Tile::getMailbox() const
{
	if (!hasFlag(TILESTATE_MAILBOX)) {
		return nullptr;
	}

	if (ground && ground->getMailbox()) {
		return ground->getMailbox();
	}

	if (const auto items = getItemList()) {
		for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
			if ((*it)->getMailbox()) {
				return (*it)->getMailbox();
			}
		}
	}
	return nullptr;
}

BedItemPtr Tile::getBedItem() const
{
	if (!hasFlag(TILESTATE_BED)) {
		return nullptr;
	}

	if (ground && ground->getBed()) {
		return ground->getBed();
	}

	if (const auto items = getItemList()) {
		for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
			if ((*it)->getBed()) {
				return (*it)->getBed();
			}
		}
	}
	return nullptr;
}

CreaturePtr Tile::getTopCreature() const
{
	if (const auto creatures = getCreatures()) {
		return creatures->getTopCreature();
	}
	return nullptr;
}

CreatureConstPtr Tile::getBottomCreature() const
{
	if (const auto creatures = getCreatures()) {
		return creatures->getBottomCreature();
	}
	return nullptr;
}

CreaturePtr Tile::getTopVisibleCreature(const CreaturePtr creature) const
{
	if (const auto creatures = getCreatures())
	{
		return creatures->getTopVisibleCreature(creature);
	}
	return nullptr;
}

CreatureConstPtr Tile::getBottomVisibleCreature(const CreatureConstPtr& creature) const
{
	if (const auto creatures = getCreatures()) {
		return creatures->getBottomVisibleCreature(creature);
	}
	return nullptr;
}

ItemPtr Tile::getTopDownItem() const
{
	if (const auto items = getItemList()) {
		return items->getTopDownItem();
	}
	return nullptr;
}

ItemPtr Tile::getTopTopItem() const
{
	if (const auto items = getItemList()) {
		return items->getTopTopItem();
	}
	return nullptr;
}

ItemPtr Tile::getItemByTopOrder(int32_t topOrder)
{
	//topOrder:
	//1: borders
	//2: ladders, signs, splashes
	//3: doors etc
	//4: creatures
	if (const auto items = getItemList()) {
		for (auto it = ItemVector::const_reverse_iterator(items->getEndTopItem()), end = ItemVector::const_reverse_iterator(items->getBeginTopItem()); it != end; ++it) {
			if (Item::items[(*it)->getID()].alwaysOnTopOrder == topOrder) {
				return (*it);
			}
		}
	}
	return nullptr;
}

StackposResolution Tile::getTopVisibleThing(const CreaturePtr& creature)
{
	if (const auto visibleCreature = getTopVisibleCreature(creature))
	{
		return { visibleCreature, nullptr };
	}

	if (const auto items = getItemList()) {
		for (ItemVector::const_iterator it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it) {
			if (const ItemType& iit = Item::items[(*it)->getID()]; !iit.lookThrough) {
				return { nullptr, *it };
			}
		}

		for (auto it = ItemVector::const_reverse_iterator(items->getEndTopItem()), end = ItemVector::const_reverse_iterator(items->getBeginTopItem()); it != end; ++it) {
			if (const ItemType& iit = Item::items[(*it)->getID()]; !iit.lookThrough) {
				return { nullptr, *it };
			}
		}
	}

	return { nullptr, ground };
}

void Tile::onAddTileItem(ItemPtr& item)
{
	if (item->hasProperty(CONST_PROP_MOVEABLE) || item->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			it->second->addItemBack(item);
			item->setContainerParent(it->second->getOwner());
		}
	}

	setTileFlags(item);
	const Position& cylinderMapPos = getPosition();
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, cylinderMapPos, true);
	TilePtr self = getTile();

	// send to client and event callback
	for (const auto& spectatorPlayer : spectators.players()
		| std::views::transform([](auto& c) { return std::static_pointer_cast<Player>(c); }))
	{
		spectatorPlayer->sendAddTileItem(getTile(), cylinderMapPos, item);
		spectatorPlayer->onAddTileItem(self, cylinderMapPos);
	}

	if ((!hasFlag(TILESTATE_PROTECTIONZONE) || g_config.GetBoolean(ConfigManager::CLEAN_PROTECTION_ZONES)) && item->isCleanable()) {
		if (!isHouseTile()) {
			g_game.addTileToClean(getTile());
		}
	}
}

void Tile::onUpdateTileItem(const ItemPtr& oldItem, const ItemType& oldType, const ItemPtr& newItem, const ItemType& newType)
{
	if (newItem->hasProperty(CONST_PROP_MOVEABLE) || newItem->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			if (int32_t index = it->second->getThingIndex(oldItem); index != -1) {
				it->second->replaceThing(index, newItem);
				newItem->setContainerParent(it->second->getOwner());
			}
		}
	} else if (oldItem->hasProperty(CONST_PROP_MOVEABLE) || oldItem->getContainer()) {
		auto it = g_game.browseFields.find(getTile());
		if (it != g_game.browseFields.end()) {
			it->second->removeThing(oldItem, oldItem->getItemCount());
			oldItem->setParent(oldItem->getParent());
		}
	}

	const Position& cylinderMapPos = getPosition();
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, cylinderMapPos, true);
	const auto& self = getTile();

	//send to client and event callback
	for (const auto& spectatorPlayer : spectators.players()
		| std::views::transform([](auto& c) { return std::static_pointer_cast<Player>(c); }))
	{
		spectatorPlayer->sendUpdateTileItem(self, cylinderMapPos, newItem);
		spectatorPlayer->onUpdateTileItem(self, cylinderMapPos, oldItem, oldType, newItem, newType);
	}
}

void Tile::onRemoveTileItem(const SpectatorVec& spectators, const std::vector<int32_t>& oldStackPosVector, const ItemPtr& item)
{
	if (item->hasProperty(CONST_PROP_MOVEABLE) || item->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			it->second->removeThing(item, item->getItemCount());
		}
	}

	resetTileFlags(item);

	const Position& cylinderMapPos = getPosition();
	const ItemType& iType = Item::items[item->getID()];

	//send to client and event callback
	size_t i = 0;

	for (const auto& spectatorPlayer : spectators.players()
		| std::views::transform([](auto& c) { return std::static_pointer_cast<Player>(c); }))
	{
		spectatorPlayer->sendRemoveTileThing(cylinderMapPos, oldStackPosVector[i++]);
		spectatorPlayer->onRemoveTileItem(getTile(), cylinderMapPos, iType, item);
	}

	if (!hasFlag(TILESTATE_PROTECTIONZONE) || g_config.GetBoolean(ConfigManager::CLEAN_PROTECTION_ZONES)) {
		const auto items = getItemList();
		if (!items || items->empty()) {
			g_game.removeTileToClean(getTile());
			return;
		}

		bool ret = false;
		for (const auto toCheck : *items) {
			if (toCheck->isCleanable()) {
				ret = true;
				break;
			}
		}

		if (!ret) {
			g_game.removeTileToClean(getTile());
		}
	}
}

void Tile::onUpdateTile(const SpectatorVec& spectators)
{
	const Position& cylinderMapPos = getPosition();

	for (const auto& spectator : spectators.players()) {
		std::static_pointer_cast<Player>(spectator)->sendUpdateTile(getTile(), cylinderMapPos);
	}
}

ReturnValue Tile::queryAdd(PlayerPtr player, uint32_t flags)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->queryAdd(player, flags);
	}

	CreatureContainer empty;
	empty.owner = static_shared_this<Tile>();
	return empty.queryAdd(player, flags);
}

ReturnValue Tile::queryAdd(MonsterPtr monster, uint32_t flags)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->queryAdd(monster, flags);
	}

	CreatureContainer empty;
	empty.owner = static_shared_this<Tile>();
	return empty.queryAdd(monster, flags);
}

ReturnValue Tile::queryAdd(NpcPtr npc, uint32_t flags)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->queryAdd(npc, flags);
	}

	CreatureContainer empty;
	empty.owner = static_shared_this<Tile>();
	return empty.queryAdd(npc, flags);
}

ReturnValue Tile::queryAdd(ItemPtr item, uint32_t flags, CreaturePtr mover)
{
    const auto& items = getItemList();
    if (items and items->size() >= 0xFFFF)
        return RETURNVALUE_NOTPOSSIBLE;

    // Tile has no limit flag
    if (hasBitSet(FLAG_NOLIMIT, flags))
        return RETURNVALUE_NOERROR;

    // If its a wall, but not a hangable item.
    bool itemIsHangable = item->isHangable();
    if (ground == nullptr and not itemIsHangable)
        return RETURNVALUE_NOTPOSSIBLE;

	if (isHouseTile())
	{
		// House tiles allow store items only if they carry a wrapid (placed via wrapping).
		if (item->isStoreItem() and not item->hasAttribute(ITEM_ATTRIBUTE_WRAPID))
			return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;

		if (mover and g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
		{
			if (not house->isInvited(mover->getPlayer()))
				return RETURNVALUE_PLAYERISNOTINVITED;
		}
	}
	else if (item->isStoreItem())
	{
		return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
	}


    // If there is any creature there, who is not in ghost mode... don't think this should be here...
    const auto& creatures = getCreatures();

    if (creatures and not creatures->empty() and item->isBlocking() and not hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags))
	{
		for (const auto& tileCreature : creatures->getList())
		{
			if (not tileCreature->isInGhostMode())
				return RETURNVALUE_NOTENOUGHROOM;
		}
    }

    if (itemIsHangable and hasFlag(TILESTATE_SUPPORTS_HANGABLE))
	{
        if (items)
		{
			for (const auto& tileItem : *items)
			{
				if (tileItem->isHangable())
					return RETURNVALUE_NEEDEXCHANGE;
			}
        }
    }
	else
	{
		auto isSolidBlocked = [&](const ItemType& iiType) -> bool
		{
			if (not iiType.blockSolid)
				return false;
			if (iiType.allowPickupable and not item->isMagicField() and not item->isBlocking())
				return false;
			return not item->isPickupable() or not iiType.hasHeight or iiType.pickupable or iiType.isBed();
		};

		if (ground and isSolidBlocked(Item::items[ground->getID()]))
			return RETURNVALUE_NOTENOUGHROOM;

		if (items)
		{
			for (const auto& tileItem : *items)
			{
				if (isSolidBlocked(Item::items[tileItem->getID()]))
					return RETURNVALUE_NOTENOUGHROOM;
			}
		}
	}

    return RETURNVALUE_NOERROR;
}

ReturnValue Tile::queryAdd(int32_t, const ThingPtr& thing, uint32_t, uint32_t flags, CreaturePtr mover)
{
	if (hasBitSet(FLAG_NOLIMIT, flags))
		return RETURNVALUE_NOERROR;

	if (auto item = thing->getItem())
		return queryAdd(item, flags, mover);

	std::cout << "|| WARNING || Tile::queryAdd() received unknown ThingSubType: "
	          << static_cast<int>(thing->getThingSubType()) << "\n";

	return RETURNVALUE_NOERROR;
}

ReturnValue Tile::queryMaxCount(int32_t, const ThingPtr&, const uint32_t count, uint32_t& maxQueryCount, uint32_t)
{
	maxQueryCount = std::max<uint32_t>(1, count);
	return RETURNVALUE_NOERROR;
}

ReturnValue Tile::queryRemove(const ThingPtr& thing, const uint32_t count, uint32_t flags, CreaturePtr actor/*= nullptr */)
{
	int32_t index = getThingIndex(thing);

	if (index == -1)
		return RETURNVALUE_NOTPOSSIBLE;

	const auto item = thing->getItem();

	if (item == nullptr)
		return RETURNVALUE_NOTPOSSIBLE;

	if (actor and g_config.GetBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS))
	{
		if (isHouseTile() and not house->isInvited(actor->getPlayer()))
			return RETURNVALUE_PLAYERISNOTINVITED;
	}

	if (count == 0 or (item->isStackable() and count > item->getItemCount()))
		return RETURNVALUE_NOTPOSSIBLE;

	if (not item->isMoveable() and not hasBitSet(FLAG_IGNORENOTMOVEABLE, flags))
		return RETURNVALUE_NOTMOVEABLE;

	return RETURNVALUE_NOERROR;
}

TilePtr Tile::resolveFloorChangeDestination(uint32_t& flags)
{
	TilePtr destTile;

	if (hasFlag(TILESTATE_FLOORCHANGE_DOWN))
	{
		uint16_t dx = tilePos.x;
		uint16_t dy = tilePos.y;
		uint8_t dz = tilePos.z + 1;

		if (auto southDownTile = g_game.map.getTile(dx, dy - 1, dz); southDownTile and southDownTile->hasFlag(TILESTATE_FLOORCHANGE_SOUTH_ALT))
		[[unlikely]]
		{
			dy -= 2;
			destTile = g_game.map.getTile(dx, dy, dz);
		}
		else if (auto eastDownTile = g_game.map.getTile(dx - 1, dy, dz); eastDownTile and eastDownTile->hasFlag(TILESTATE_FLOORCHANGE_EAST_ALT))
		[[unlikely]]
		{
			dx -= 2;
			destTile = g_game.map.getTile(dx, dy, dz);
		}
		else
		[[likely]]
		{
			if (auto downTile = g_game.map.getTile(dx, dy, dz))
			[[likely]]
			{
				if (downTile->hasFlag(TILESTATE_FLOORCHANGE_NORTH)) ++dy;
				if (downTile->hasFlag(TILESTATE_FLOORCHANGE_SOUTH)) --dy;
				if (downTile->hasFlag(TILESTATE_FLOORCHANGE_SOUTH_ALT)) dy -= 2;
				if (downTile->hasFlag(TILESTATE_FLOORCHANGE_EAST)) --dx;
				if (downTile->hasFlag(TILESTATE_FLOORCHANGE_EAST_ALT)) dx -= 2;
				if (downTile->hasFlag(TILESTATE_FLOORCHANGE_WEST)) ++dx;

				destTile = g_game.map.getTile(dx, dy, dz);
			}
		}
	}
	else if (hasFlag(TILESTATE_FLOORCHANGE))
	{
		uint16_t dx = tilePos.x;
		uint16_t dy = tilePos.y;
		uint8_t dz = tilePos.z - 1;

		if (hasFlag(TILESTATE_FLOORCHANGE_NORTH)) --dy;
		if (hasFlag(TILESTATE_FLOORCHANGE_SOUTH)) ++dy;
		if (hasFlag(TILESTATE_FLOORCHANGE_EAST)) ++dx;
		if (hasFlag(TILESTATE_FLOORCHANGE_WEST)) --dx;
		if (hasFlag(TILESTATE_FLOORCHANGE_SOUTH_ALT)) dy += 2;
		if (hasFlag(TILESTATE_FLOORCHANGE_EAST_ALT)) dx += 2;

		destTile = g_game.map.getTile(dx, dy, dz);
	}

	if (not destTile)
		destTile = this->getTile();
	else
		flags |= FLAG_NOLIMIT;

	return destTile;
}

TilePtr Tile::queryCreatureDestination(const CreaturePtr& creature, uint32_t& flags)
{
	if (const auto player = creature->getPlayer())
	{
		if (isHouseTile() and not house->isInvited(player))
		{
			const Position& entryPos = house->getEntryPosition();
			auto destTile = g_game.map.getTile(entryPos);

			if (not destTile)
			[[unlikely]]
			{
				destTile = g_game.map.getTile(player->getTemplePosition());

				if (not destTile) [[unlikely]]
					destTile = std::make_shared<Tile>(0xFFFF, 0xFFFF, 0xFF);
			}

			return destTile;
		}
	}

	return resolveFloorChangeDestination(flags);
}

ThingPtr Tile::queryDestination(int32_t&, const ThingPtr& thingPtr, ItemPtr& destItem, uint32_t& flags)
{
	if (not thingPtr)
		return getTile();

	TilePtr destTile = resolveFloorChangeDestination(flags);

	if (destTile)
	{
		if (auto destThing = destTile->getTopDownItem())
			destItem = destThing->getItem();
	}

	return destTile;
}


void Tile::addThing(ThingPtr thing)
{
	addThing(0, thing);
}

void Tile::addThing(int32_t, ThingPtr thing)
{
	auto item = thing->getItem();
	if (item == nullptr)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	updateHouse(item);

	TileItemsPtr items = getItemList();
	if (items and items->size() >= 0xFFFF)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}
	item->setParent(getTile());
	if (auto itemContainer = item->getContainer())
	{
		itemContainer->setHoldingCreature(nullptr);
	}

	updateHouse(item);

	const ItemType& itemType = Item::items[item->getID()];
	if (itemType.isGroundTile())
	{
		if (ground == nullptr)
		{
			ground = item;
			onAddTileItem(item);
		}
		else
		{
			const ItemType& oldType = Item::items[ground->getID()];

			const auto oldGround = ground;
			ground->clearParent();
			// g_game.ReleaseItem(ground);
			ground = item;
			resetTileFlags(oldGround);
			setTileFlags(item);
			onUpdateTileItem(oldGround, oldType, item, itemType);
			postRemoveNotification(oldGround, nullptr, 0);
		}
	}
	else if (itemType.alwaysOnTop)
	{
		if (itemType.isSplash() and items)
		{
			for (ItemVector::const_iterator it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
				const auto oldSplash = *it;
				if (not Item::items[oldSplash->getID()].isSplash())
				{
					continue;
				}

				removeThing(oldSplash, 1);
				oldSplash->clearParent();
				postRemoveNotification(oldSplash, nullptr, 0);
				break;
			}
		}

		bool isInserted = false;

		if (items)
		{
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
				//Note: this is different from internalAddThing
				if (itemType.alwaysOnTopOrder <= Item::items[(*it)->getID()].alwaysOnTopOrder)
				{
					items->insert(it, item);
					isInserted = true;
					break;
				}
			}
		}
		else
		{
			// std::unreachable()
		}

		if (not isInserted)
		{
			items->push_back(item);
		}

		onAddTileItem(item);
	}
	else
	{
		if (itemType.isMagicField())
		{
			//remove old field item if exists
			if (items)
			{
				for (ItemVector::const_iterator it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it)
				{
					if (const auto oldField = (*it)->getMagicField())
					{
						if (oldField->isReplaceable())
						{
							removeThing(oldField, 1);
							oldField->clearParent();
							postRemoveNotification(oldField, nullptr, 0);
							break;
						}
						else
						{
							//This magic field cannot be replaced.
							item->clearParent();
							return;
						}
					}
				}
			}
		}

		items->insert(items->getBeginDownItem(), item);
		items->addDownItemCount(1);
		onAddTileItem(item);
	}
}

void Tile::updateThing(ThingPtr thing, uint16_t itemId, uint32_t count)
{
	int32_t index = getThingIndex(thing);
	if (index == -1) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	const auto item = thing->getItem();
	if (item == nullptr) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	const ItemType& oldType = Item::items[item->getID()];
	const ItemType& newType = Item::items[itemId];
	resetTileFlags(item);
	item->setID(itemId);
	item->setSubType(count);
	setTileFlags(item);
	onUpdateTileItem(item, oldType, item, newType);
}

void Tile::replaceThing(uint32_t index, ThingPtr thing)
{
	int32_t pos = index;

	const auto item = thing->getItem();
	if (item == nullptr) {
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	ItemPtr oldItem = nullptr;
	bool isInserted = false;

	if (ground) {
		if (pos == 0) {
			oldItem = ground;
			ground = item;
			isInserted = true;
		}

		--pos;
	}

	const auto items = getItemList();
	if (items && !isInserted) {
		int32_t topItemSize = getTopItemCount();
		if (pos < topItemSize) {
			auto it = items->getBeginTopItem();
			it += pos;

			oldItem = (*it);
			it = items->erase(it);
			items->insert(it, item);
			isInserted = true;
		}

		pos -= topItemSize;
	}

	if (const auto creatures = getCreatures()) {
		if (!isInserted && pos < static_cast<int32_t>(creatures->size())) {
			return /*RETURNVALUE_NOTPOSSIBLE*/;
		}

		pos -= static_cast<uint32_t>(creatures->size());
	}

	if (items && !isInserted) {
		int32_t downItemSize = getDownItemCount();
		if (pos < downItemSize) {
			auto it = items->getBeginDownItem() + pos;
			oldItem = *it;
			it = items->erase(it);
			items->insert(it, item);
			isInserted = true;
		}
	}

	if (isInserted) {
		item->setParent(getTile());

		resetTileFlags(oldItem);
		setTileFlags(item);
		const ItemType& oldType = Item::items[oldItem->getID()];
		const ItemType& newType = Item::items[item->getID()];
		onUpdateTileItem(oldItem, oldType, item, newType);

		oldItem->clearParent();
		return /*RETURNVALUE_NOERROR*/;
	}
}

void Tile::removeThing(ThingPtr thing, uint32_t count)
{
	const auto item = thing->getItem();
	if (!item) {
		return;
	}

	const int32_t index = getThingIndex(item);
	if (index == -1) {
		return;
	}

	if (item == ground) {
		ground->clearParent();
		ground = nullptr;

		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true);
		onRemoveTileItem(spectators, std::vector<int32_t>(spectators.size(), 0), item);
		return;
	}

	const auto items = getItemList();
	if (not items) {
		return;
	}



	const ItemType& itemType = Item::items[item->getID()];
	if (itemType.alwaysOnTop) {
		const auto it = std::find(items->getBeginTopItem(), items->getEndTopItem(), item);
		if (it == items->getEndTopItem()) {
			return;
		}

		std::vector<int32_t> oldStackPosVector;

		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true);

		for (const auto& c : spectators.players()) {
			const auto spectatorPlayer = std::static_pointer_cast<Player>(c);
			oldStackPosVector.push_back(getStackposOfItem(spectatorPlayer, item));
		}

		item->clearParent();
		items->erase(it);
		onRemoveTileItem(spectators, oldStackPosVector, item);
	} else {
		const auto it = std::find(items->getBeginDownItem(), items->getEndDownItem(), item);
		if (it == items->getEndDownItem()) {
			return;
		}

		if (itemType.stackable && count != item->getItemCount()) {
			const uint8_t newCount = static_cast<uint8_t>(std::max<int32_t>(0, static_cast<int32_t>(item->getItemCount() - count)));
			item->setItemCount(newCount);
			onUpdateTileItem(item, itemType, item, itemType);
		} else {
			std::vector<int32_t> oldStackPosVector;

			SpectatorVec spectators;
			g_game.map.getSpectators(spectators, getPosition(), true);
			for (const auto& c : spectators.players()) {
				const auto spectatorPlayer = std::static_pointer_cast<Player>(c);
				oldStackPosVector.push_back(getStackposOfItem(spectatorPlayer, item));
			}

			item->clearParent();
			items->erase(it);
			items->addDownItemCount(-1);
			onRemoveTileItem(spectators, oldStackPosVector, item);
		}
	}
}

bool Tile::hasCreature(CreaturePtr& creature)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->hasCreature(creature);
	}
	return false;
}

void Tile::removeCreature(CreaturePtr& creature)
{
	g_game.map.getQTNode(tilePos.x, tilePos.y)->removeCreature(creature);
	if (const auto creatures = getCreatures())
	{
		creatures->removeCreature(creature);
	}
}

int32_t Tile::getThingIndex(ThingPtr thing)
{
	int32_t n = -1;
	if (ground) {
		if (ground == thing) {
			return 0;
		}
		++n;
	}

	const auto items = getItemList();
	if (items) {
		if (const auto& item = thing->getItem(); item && item->isAlwaysOnTop()) {
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it) {
				++n;
				if (*it == item) {
					return n;
				}
			}
		} else {
			n += items->getTopItemCount();
		}
	}

	if (const auto creatures = getCreatures())
	{
		n += creatures->size();
	}

	if (items) {
		const auto& item = thing->getItem();
		if (item && !item->isAlwaysOnTop()) {
			for (auto it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it) {
				++n;
				if (*it == item) {
					return n;
				}
			}
		}
	}
	return -1;
}

int32_t Tile::getCreatureStackIndex(const CreatureConstPtr& creature) const
{
	int32_t n = -1;
	if (ground)
	{
		++n;
	}

	if (const auto& items = getItemList())
	{
		n += items->getTopItemCount();
	}

	if (const auto creatures = getCreatures())
	{
		for (const auto& tileCreature : creatures->getList())
		{
			++n;
			if (tileCreature == creature)
			{
				return n;
			}
		}
	}

	return -1;
}

int32_t Tile::getClientIndexOfCreature(const PlayerConstPtr& player, const CreatureConstPtr& creature) const
{
	int32_t n;
	if (ground) {
		n = 1;
	} else {
		n = 0;
	}

	if (const auto& items = getItemList()) {
		for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
		{
			++n;
		}
	}

	if (const auto creatures = getCreatures())
	{
		for (const auto& c : boost::adaptors::reverse(creatures->getList()))
		{
			if (c == creature) {
				return n;
			} else if (player->canSeeCreature(c)) {
				++n;
			}
		}
	}
	return -1;
}

int32_t Tile::getStackposOfItem(const PlayerConstPtr& player, const ItemConstPtr& item) const
{
	int32_t n = 0;
	if (ground) {
		if (ground == item) {
			return n;
		}
		++n;
	}

	const auto& items = getItemList();
	if (items) {
		if (item->isAlwaysOnTop()) {
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
				if (*it == item) {
					return n;
				} else if (++n == 10) {
					return -1;
				}
			}
		} else {
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
				if (++n >= 10) {
					return -1;
				}
			}
		}
	}

	if (const auto creatures = getCreatures())
	{
		for (const auto& creature : creatures->getList())
		{
			if (player->canSeeCreature(creature)) {
				if (++n >= 10) {
					return -1;
				}
			}
		}
	}

	if (items && !item->isAlwaysOnTop()) {
		for (auto it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it)
		{
			if (*it == item) {
				return n;
			} else if (++n >= 10) {
				return -1;
			}
		}
	}
	return -1;
}

size_t Tile::getFirstIndex() const
{
	return 0;
}

size_t Tile::getLastIndex() const
{
	return getThingCount();
}

uint32_t Tile::getItemTypeCount(const uint16_t itemId, int32_t subType /*= -1*/) const
{
	uint32_t count = 0;
	if (ground && ground->getID() == itemId) {
		count += Item::countByType(ground, subType);
	}

	if (const auto& items = getItemList()) {
		for (const auto& item : *items) {
			if (item->getID() == itemId) {
				count += Item::countByType(item, subType);
			}
		}
	}
	return count;
}

ThingPtr Tile::getThing(size_t index)
{
	return getThingAt(index).item;
}

StackposResolution Tile::getThingAt(size_t index)
{
	if (ground) {
		if (index == 0) {
			return { nullptr, ground };
		}

		--index;
	}

	const auto& items = getItemList();
	if (items) {
		const uint32_t topItemSize = items->getTopItemCount();
		if (index < topItemSize) {
			return { nullptr, items->at(items->getDownItemCount() + index) };
		}
		index -= topItemSize;
	}

	if (const auto creatures = getCreatures())
	{
		if (index < creatures->size()) {
			return { creatures->getList()[index], nullptr };
		}
		index -= creatures->size();
	}

	if (items && index < items->getDownItemCount()) {
		return { nullptr, items->at(index) };
	}
	return {};
}

void Tile::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);

	// another test location

	for (auto& spectator : spectators.players())
	{
		std::static_pointer_cast<Player>(spectator)->postAddNotification(thing, oldParent, index, LINK_NEAR);
	}

	//add a reference to this item, it may be deleted after being added (mailbox for example)
	auto item = thing->getItem();

	if (link == LINK_OWNER) {
		if (hasFlag(TILESTATE_TELEPORT)) {
			if (const auto& teleport = getTeleportItem()) {
				teleport->addThing(thing);
			}
		} else if (hasFlag(TILESTATE_TRASHHOLDER)) {
			if (const auto& trashholder = getTrashHolder()) {
				trashholder->addThing(thing);
			}
		} else if (hasFlag(TILESTATE_MAILBOX)) {
			if (const auto mailbox = getMailbox()) {
				mailbox->addThing(thing);
			}
		}

		if (item)
		{
			if (TilePtr tile = item->getTile()) {
				g_moveEvents->onItemMove(item, tile, true);
			}
		}
	}
}

void Tile::postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	if (getThingCount() > 8) {
		onUpdateTile(spectators);
	}

	// Final test location, if the spectators internals were ever failing to filter only players
	// then npcs would trigger, spawns would trigger, and moving things to and from tiles, would trigger
	// and we would know about it pretty quickly, howevever if this doesn't happen, and this new RTTI tagging
	// works as well as anticipated, I will cleanup the view changes, and apply the same system for thing, cylinder and item based classes.

	for (auto& spectator : spectators.players())
	{
		std::static_pointer_cast<Player>(spectator)->postRemoveNotification(thing, newParent, index, LINK_NEAR);
	}

	//calling movement scripts
	if (auto item = thing->getItem())
	{
		g_moveEvents->onItemMove(item, getTile(), false);
	}
}

void Tile::postAddCreatureNotification(const CreaturePtr& creature, const TilePtr& oldTile)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	postAddCreatureNotification(creature, oldTile, spectators);
}

void Tile::postAddCreatureNotification(const CreaturePtr& creature, const TilePtr&, const SpectatorVec& spectators)
{
	for (auto& spectator : spectators.players())
	{
		std::static_pointer_cast<Player>(spectator)->onNearbyCreatureMoved(creature);
	}

	if (hasFlag(TILESTATE_TELEPORT))
	{
		if (const auto& teleport = getTeleportItem())
		{
			teleport->teleportCreature(creature);
		}
	}

	g_moveEvents->onCreatureMove(creature, getTile(), MOVE_EVENT_STEP_IN);
}

void Tile::postRemoveCreatureNotification(const CreaturePtr& creature, const TilePtr& newTile)
{
	SpectatorVec spectators;
	if (getThingCount() > 8)
	{
		g_game.map.getSpectators(spectators, getPosition(), true, true);
	}
	postRemoveCreatureNotification(creature, newTile, spectators);
}

void Tile::postRemoveCreatureNotification(const CreaturePtr& creature, const TilePtr&, const SpectatorVec& spectators)
{
	if (getThingCount() > 8)
	{
		onUpdateTile(spectators);
	}

	g_moveEvents->onCreatureMove(creature, getTile(), MOVE_EVENT_STEP_OUT);
}

void Tile::internalAddThing(ThingPtr thing)
{
	internalAddThing(0, thing);
}

void Tile::internalAddThing(uint32_t, ThingPtr thing)
{
	thing->setParent(getTile());

	const auto& item = thing->getItem();
	if (item == nullptr)
	{
		return;
	}
	if (auto itemContainer = item->getContainer())
	{
		itemContainer->setHoldingCreature(nullptr);
	}
	updateHouse(item);
	const ItemType& itemType = Item::items[item->getID()];
	if (itemType.isGroundTile())
	{
		if (ground == nullptr)
		{
			ground = item;
			setTileFlags(item);
		}
		return;
	}

	auto items = getItemList();
	if (items->size() >= 0xFFFF)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	if (itemType.alwaysOnTop)
	{
		bool isInserted = false;
		for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
		{
			if (Item::items[(*it)->getID()].alwaysOnTopOrder > itemType.alwaysOnTopOrder)
			{
				items->insert(it, item);
				isInserted = true;
				break;
			}
		}

		if (not isInserted)
		{
			items->push_back(item);
		}
	}
	else
	{
		items->insert(items->getBeginDownItem(), item);
		items->addDownItemCount(1);
	}

	setTileFlags(item);
}

void Tile::setTileFlags(const ItemConstPtr& item)
{
	applyItemProperties(item);

	if (!hasFlag(TILESTATE_FLOORCHANGE)) {
		if (const ItemType& it = Item::items[item->getID()]; it.floorChange != 0) {
			setFlag(it.floorChange);
		}
	}

	if (item->hasProperty(CONST_PROP_IMMOVABLEBLOCKSOLID)) {
		setFlag(TILESTATE_IMMOVABLEBLOCKSOLID);
	}

	if (item->hasProperty(CONST_PROP_BLOCKPATH)) {
		setFlag(TILESTATE_BLOCKPATH);
	}

	if (item->hasProperty(CONST_PROP_NOFIELDBLOCKPATH)) {
		setFlag(TILESTATE_NOFIELDBLOCKPATH);
	}

	if (item->hasProperty(CONST_PROP_IMMOVABLENOFIELDBLOCKPATH)) {
		setFlag(TILESTATE_IMMOVABLENOFIELDBLOCKPATH);
	}

	if (item->getTeleport()) {
		setFlag(TILESTATE_TELEPORT);
	}

	if (item->getMagicField()) {
		setFlag(TILESTATE_MAGICFIELD);
	}

	if (item->getMailbox()) {
		setFlag(TILESTATE_MAILBOX);
	}

	if (item->getTrashHolder()) {
		setFlag(TILESTATE_TRASHHOLDER);
	}

	if (item->hasProperty(CONST_PROP_BLOCKSOLID)) {
		setFlag(TILESTATE_BLOCKSOLID);
	}

	if (item->getBed()) {
		setFlag(TILESTATE_BED);
	}

	const auto& container = item->getContainer();
	if (container and container->isDepotLocker()) {
		setFlag(TILESTATE_DEPOT);
	}

	if (item->hasProperty(CONST_PROP_SUPPORTHANGABLE)) {
		setFlag(TILESTATE_SUPPORTS_HANGABLE);
	}
}

void Tile::resetTileFlags(const ItemPtr& item)
{
	recalculateItemProperties();

	if (const ItemType& it = Item::items[item->getID()]; it.floorChange != 0) {
		resetFlag(TILESTATE_FLOORCHANGE);
	}

	if (!hasProperty(CONST_PROP_BLOCKSOLID)) {
		resetFlag(TILESTATE_BLOCKSOLID);
	}

	if (!hasProperty(CONST_PROP_IMMOVABLEBLOCKSOLID)) {
		resetFlag(TILESTATE_IMMOVABLEBLOCKSOLID);
	}

	if (!hasProperty(CONST_PROP_BLOCKPATH)) {
		resetFlag(TILESTATE_BLOCKPATH);
	}

	if (!hasProperty(CONST_PROP_NOFIELDBLOCKPATH)) {
		resetFlag(TILESTATE_NOFIELDBLOCKPATH);
	}

	if (!hasProperty(CONST_PROP_IMMOVABLEBLOCKPATH)) {
		resetFlag(TILESTATE_IMMOVABLEBLOCKPATH);
	}

	if (!hasProperty(CONST_PROP_IMMOVABLENOFIELDBLOCKPATH)) {
		resetFlag(TILESTATE_IMMOVABLENOFIELDBLOCKPATH);
	}

	if (item->getTeleport()) {
		resetFlag(TILESTATE_TELEPORT);
	}

	if (item->getMagicField()) {
		resetFlag(TILESTATE_MAGICFIELD);
	}

	if (item->getMailbox()) {
		resetFlag(TILESTATE_MAILBOX);
	}

	if (item->getTrashHolder()) {
		resetFlag(TILESTATE_TRASHHOLDER);
	}

	if (item->getBed()) {
		resetFlag(TILESTATE_BED);
	}

	if (const auto& container = item->getContainer(); container and container->isDepotLocker()) {
		resetFlag(TILESTATE_DEPOT);
	}

	if (!hasProperty(CONST_PROP_SUPPORTHANGABLE)) {
		resetFlag(TILESTATE_SUPPORTS_HANGABLE);
	}
}

bool Tile::isMoveableBlocking() const
{
	return !ground || hasFlag(TILESTATE_BLOCKSOLID);
}

ItemPtr Tile::getUseItem(const int32_t index)
{
	// no items, get ground
	const auto& items = getItemList();
	if (!items || items->size() == 0) {
		return ground;
	}

	// try getting thing by index
	if (auto thing = getThing(index)) {
		if (const auto& thingItem = thing->getItem()) {
			return thingItem;
		}
	}

	// try getting top usable item
	if (const auto& topDownItem = getTopDownItem()) {
		return topDownItem;
	}

	// try getting door
	for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
		if ((*it)->getDoor()) {
			return *it;
		}
	}

	return nullptr;
}


void Tile::updateHouse(const ItemPtr& item)
{
	if (item->getParent() != getParent()) {
		return;
	}
	if (isHouseTile()) {
		if (auto door = item->getDoor()) {
			if (door->getDoorId() != 0) {
				house->addDoor(door);
			}
		}
		else {
			if (const auto bed = item->getBed()) {
				house->addBed(bed);
			}
		}
	}
}
