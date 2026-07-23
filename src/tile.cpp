// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include <boost/range/adaptor/reversed.hpp>
#include <ranges>

#include "tile.h"

#include "creature.h"
#include "creaturecontainer.h"
#include "combat.h"
#include "game.h"
#include "monster.h"
#include "movement.h"
#include "configmanager.h"

extern Game g_game;
extern MoveEvents* g_moveEvents;
extern ConfigManager g_config;

using BlackTek::GameModel;

Tile real_nullptr_tile(0xFFFF, 0xFFFF, 0xFF);

Tile::Tile(uint16_t x, uint16_t y, uint8_t z) : tilePos(x, y, z)
{
	items = std::make_shared<TileItemVector>();
}

Tile::Tile(uint16_t x, uint16_t y, uint8_t z, House* house) : tilePos(x, y, z)
{
	items = std::make_shared<TileItemVector>();
	this->house = house;
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

GameModel Tile::getTopVisibleGameModel(const CreaturePtr& creature)
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

void Tile::onAddTileItem(const ItemPtr& item, std::span<const CreaturePtr> spectators)
{
	if (item->hasProperty(CONST_PROP_MOVEABLE) || item->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			ItemPtr addedItem = item;
			it->second->addItemBack(addedItem);
			item->setContainerParent(it->second->getOwner());
		}
	}

	setTileFlags(item);
	const Position& cylinderMapPos = getPosition();
	TilePtr self = getTile();

	// send to client and event callback
	for (const auto& spectatorPlayer : spectators
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

void Tile::onUpdateTileItem(const ItemPtr& oldItem, const ItemType& oldType, const ItemPtr& newItem, const ItemType& newType, std::span<const CreaturePtr> spectators)
{
	if (newItem->hasProperty(CONST_PROP_MOVEABLE) || newItem->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			if (int32_t index = it->second->getItemIndex(oldItem); index != -1)
			{
				it->second->replaceItem(index, newItem);
				newItem->setContainerParent(it->second->getOwner());
			}
		}
	} else if (oldItem->hasProperty(CONST_PROP_MOVEABLE) || oldItem->getContainer()) {
		auto it = g_game.browseFields.find(getTile());
		if (it != g_game.browseFields.end()) {
			it->second->removeItem(oldItem, oldItem->getItemCount());
			auto oldItemLocation = oldItem->getLocation();
			if (oldItemLocation.tile)
			{
				oldItem->setTileParent(oldItemLocation.tile);
			}
			else if (oldItemLocation.player)
			{
				oldItem->setInventoryOwner(oldItemLocation.player);
			}
			else
			{
				oldItem->clearParent();
			}
		}
	}

	const Position& cylinderMapPos = getPosition();
	const auto& self = getTile();

	//send to client and event callback
	for (const auto& spectatorPlayer : spectators
		| std::views::transform([](auto& c) { return std::static_pointer_cast<Player>(c); }))
	{
		spectatorPlayer->sendUpdateTileItem(self, cylinderMapPos, newItem);
		spectatorPlayer->onUpdateTileItem(self, cylinderMapPos, oldItem, oldType, newItem, newType);
	}
}

void Tile::onRemoveTileItem(std::span<const CreaturePtr> spectators, const std::vector<int32_t>& oldStackPosVector, const ItemPtr& item)
{
	if (item->hasProperty(CONST_PROP_MOVEABLE) || item->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			it->second->removeItem(item, item->getItemCount());
		}
	}

	resetTileFlags(item);

	const Position& cylinderMapPos = getPosition();
	const ItemType& iType = Item::items[item->getID()];

	//send to client and event callback
	size_t i = 0;

	for (const auto& spectatorPlayer : spectators
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

void Tile::onUpdateTile(std::span<const CreaturePtr> spectators)
{
	const Position& cylinderMapPos = getPosition();

	for (const auto& spectatorPlayer : spectators
		| std::views::filter([](const auto& c) { return c->is_player(); })
		| std::views::transform([](auto& c) { return std::static_pointer_cast<Player>(c); }))
	{
		spectatorPlayer->sendUpdateTile(getTile(), cylinderMapPos);
	}
}

ReturnValue Tile::canEnter(PlayerPtr player, uint32_t flags)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->canEnter(player, flags);
	}

	CreatureContainer empty;
	empty.owner = static_shared_this<Tile>();
	return empty.canEnter(player, flags);
}

ReturnValue Tile::canEnter(MonsterPtr monster, uint32_t flags)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->canEnter(monster, flags);
	}

	CreatureContainer empty;
	empty.owner = static_shared_this<Tile>();
	return empty.canEnter(monster, flags);
}

ReturnValue Tile::canEnter(NpcPtr npc, uint32_t flags)
{
	if (const auto creatures = getCreatures())
	{
		return creatures->canEnter(npc, flags);
	}

	CreatureContainer empty;
	empty.owner = static_shared_this<Tile>();
	return empty.canEnter(npc, flags);
}

ReturnValue Tile::canAddItem(const ItemPtr& item, uint32_t flags, const CreaturePtr& mover)
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

ReturnValue Tile::checkAddCapacity(int32_t, const ItemPtr&, const uint32_t count, uint32_t& acceptedCount, uint32_t)
{
	acceptedCount = std::max<uint32_t>(1, count);
	return RETURNVALUE_NOERROR;
}

ReturnValue Tile::canRemoveItem(const ItemPtr& item, const uint32_t count, uint32_t flags, CreaturePtr actor/*= nullptr */)
{
	const int32_t index = getItemStackIndex(item);

	if (index == -1)
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

TilePtr Tile::resolveCreatureDestination(const CreaturePtr& creature, uint32_t& flags)
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
				{
					static const Position nowherePosition(0xFFFF, 0xFFFF, BlackTek::World::MaxLayers - 1);
					destTile = g_game.map.getTile(nowherePosition);
					if (not destTile)
					{
						destTile = std::make_shared<Tile>(nowherePosition.x, nowherePosition.y, nowherePosition.z);
						g_game.map.setTile(nowherePosition, destTile);
					}
				}
			}

			return destTile;
		}
	}

	return resolveFloorChangeDestination(flags);
}

TilePtr Tile::resolveItemDestination(ItemPtr& destItem, uint32_t& flags)
{
	TilePtr destTile = resolveFloorChangeDestination(flags);

	if (destTile)
	{
		if (auto topDownItem = destTile->getTopDownItem())
			destItem = topDownItem;
	}

	return destTile;
}


SpectatorVec Tile::addItem(const ItemPtr& item)
{
	updateHouse(item);

	TileItemsPtr items = getItemList();
	if (items and items->size() >= 0xFFFF)
	{
		return {} /*RETURNVALUE_NOTPOSSIBLE*/;
	}
	item->setTileParent(getTile());
	if (auto itemContainer = item->getContainer())
	{
		itemContainer->setHoldingCreature(nullptr);
	}

	updateHouse(item);

	const ItemType& itemType = Item::items[item->getID()];
	if (itemType.isGroundTile())
	{
		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true, true);
		const std::span<const CreaturePtr> spectators_span(spectators.begin(), spectators.size());

		if (ground == nullptr)
		{
			ground = item;
			onAddTileItem(item, spectators_span);
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
			onUpdateTileItem(oldGround, oldType, item, itemType, spectators_span);
			notifyItemRemoved(oldGround, {}, 0, spectators_span);
		}

		return spectators;
	}
	else if (itemType.alwaysOnTop)
	{
		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true, true);
		const std::span<const CreaturePtr> spectators_span(spectators.begin(), spectators.size());

		if (itemType.isSplash() and items)
		{
			for (ItemVector::const_iterator it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
				const auto oldSplash = *it;
				if (not Item::items[oldSplash->getID()].isSplash())
				{
					continue;
				}

				removeItem(oldSplash, 1, spectators_span);
				oldSplash->clearParent();
				notifyItemRemoved(oldSplash, {}, 0, spectators_span);
				break;
			}
		}

		bool isInserted = false;

		if (items)
		{
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
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

		onAddTileItem(item, spectators_span);
		return spectators;
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
							SpectatorVec spectators;
							g_game.map.getSpectators(spectators, getPosition(), true, true);
							const std::span<const CreaturePtr> spectators_span(spectators.begin(), spectators.size());

							removeItem(oldField, 1, spectators_span);
							oldField->clearParent();
							notifyItemRemoved(oldField, {}, 0, spectators_span);

							items->insert(items->getBeginDownItem(), item);
							items->addDownItemCount(1);
							onAddTileItem(item, spectators_span);
							return spectators;
						}
						else
						{
							//This magic field cannot be replaced.
							item->clearParent();
							return {};
						}
					}
				}
			}
		}

		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true, true);
		const std::span<const CreaturePtr> spectators_span(spectators.begin(), spectators.size());

		items->insert(items->getBeginDownItem(), item);
		items->addDownItemCount(1);
		onAddTileItem(item, spectators_span);
		return spectators;
	}
}

void Tile::updateItem(const ItemPtr& item, uint16_t itemId, uint32_t count)
{
	int32_t index = getItemStackIndex(item);
	if (index == -1)
	{
		return /*RETURNVALUE_NOTPOSSIBLE*/;
	}

	const ItemType& oldType = Item::items[item->getID()];
	const ItemType& newType = Item::items[itemId];
	resetTileFlags(item);
	item->setID(itemId);
	item->setSubType(count);
	setTileFlags(item);

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	onUpdateTileItem(item, oldType, item, newType, std::span<const CreaturePtr>(spectators.begin(), spectators.size()));
}

void Tile::replaceItem(uint32_t index, const ItemPtr& item)
{
	int32_t pos = index;

	ItemPtr oldItem = nullptr;
	bool isInserted = false;

	if (ground)
	{
		if (pos == 0)
		{
			oldItem = ground;
			ground = item;
			isInserted = true;
		}

		--pos;
	}

	const auto items = getItemList();
	if (items and not isInserted)
	{
		int32_t topItemSize = getTopItemCount();
		if (pos < topItemSize)
		{
			auto it = items->getBeginTopItem();
			it += pos;

			oldItem = (*it);
			it = items->erase(it);
			items->insert(it, item);
			isInserted = true;
		}

		pos -= topItemSize;
	}

	if (const auto creatures = getCreatures())
	{
		if (not isInserted and pos < static_cast<int32_t>(creatures->size()))
		{
			return /*RETURNVALUE_NOTPOSSIBLE*/;
		}

		pos -= static_cast<uint32_t>(creatures->size());
	}

	if (items and not isInserted)
	{
		int32_t downItemSize = getDownItemCount();
		if (pos < downItemSize)
		{
			auto it = items->getBeginDownItem() + pos;
			oldItem = *it;
			it = items->erase(it);
			items->insert(it, item);
			isInserted = true;
		}
	}

	if (isInserted)
	{
		item->setTileParent(getTile());

		resetTileFlags(oldItem);
		setTileFlags(item);
		const ItemType& oldType = Item::items[oldItem->getID()];
		const ItemType& newType = Item::items[item->getID()];

		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true, true);
		onUpdateTileItem(oldItem, oldType, item, newType, std::span<const CreaturePtr>(spectators.begin(), spectators.size()));

		oldItem->clearParent();
		return /*RETURNVALUE_NOERROR*/;
	}
}

void Tile::removeItem(const ItemPtr& item, uint32_t count)
{
	const int32_t index = getItemStackIndex(item);
	if (index == -1)
	{
		return;
	}

	if (item == ground)
	{
		ground->clearParent();
		ground = nullptr;

		SpectatorVec spectators;
		g_game.map.getSpectators(spectators, getPosition(), true, true);
		onRemoveTileItem(std::span<const CreaturePtr>(spectators.begin(), spectators.size()), std::vector<int32_t>(spectators.size(), 0), item);
		return;
	}

	if (not getItemList())
	{
		return;
	}

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	removeItem(item, count, std::span<const CreaturePtr>(spectators.begin(), spectators.size()));
}

void Tile::removeItem(const ItemPtr& item, uint32_t count, std::span<const CreaturePtr> spectators)
{
	const auto items = getItemList();
	if (not items)
	{
		return;
	}

	const ItemType& itemType = Item::items[item->getID()];
	if (itemType.alwaysOnTop)
	{
		const auto it = std::find(items->getBeginTopItem(), items->getEndTopItem(), item);
		if (it == items->getEndTopItem())
		{
			return;
		}

		std::vector<int32_t> oldStackPosVector;

		for (const auto& c : spectators)
		{
			const auto spectatorPlayer = std::static_pointer_cast<Player>(c);
			oldStackPosVector.push_back(getStackposOfItem(spectatorPlayer, item));
		}

		item->clearParent();
		items->erase(it);
		onRemoveTileItem(spectators, oldStackPosVector, item);
	}
	else
	{
		const auto it = std::find(items->getBeginDownItem(), items->getEndDownItem(), item);
		if (it == items->getEndDownItem())
		{
			return;
		}

		if (itemType.stackable and count != item->getItemCount())
		{
			const uint8_t newCount = static_cast<uint8_t>(std::max<int32_t>(0, static_cast<int32_t>(item->getItemCount() - count)));
			item->setItemCount(newCount);
			onUpdateTileItem(item, itemType, item, itemType, spectators);
		}
		else
		{
			std::vector<int32_t> oldStackPosVector;

			for (const auto& c : spectators)
			{
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
	if (auto* chunk = g_game.map.getChunk(owning_chunk))
	{
		chunk->RemoveCreature(creature);
	}
	if (const auto creatures = getCreatures())
	{
		creatures->removeCreature(creature);
	}
}

int32_t Tile::getItemStackIndex(const ItemConstPtr& item)
{
	int32_t n = -1;
	if (ground)
	{
		if (ground == item)
		{
			return 0;
		}
		++n;
	}

	const auto items = getItemList();
	if (items)
	{
		if (item->isAlwaysOnTop())
		{
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			{
				++n;
				if (*it == item)
				{
					return n;
				}
			}
		}
		else
		{
			n += items->getTopItemCount();
		}
	}

	if (const auto creatures = getCreatures())
	{
		n += creatures->size();
	}

	if (items)
	{
		if (not item->isAlwaysOnTop())
		{
			for (auto it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it)
			{
				++n;
				if (*it == item)
				{
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

Tile::UniformStackIndex Tile::getUniformClientIndexOfCreature(const CreatureConstPtr& creature) const
{
	UniformStackIndex result;
	result.index = ground ? 1 : 0;

	if (const auto& items = getItemList())
	{
		for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
			++result.index;
	}

	if (const auto creatures = getCreatures())
	{
		for (const auto& c : creatures->getList() | std::views::reverse)
		{
			if (c == creature)
				return result;

			if (c->isInvisible() or c->isInGhostMode())
				result.uniform = false;

			++result.index;
		}
	}

	result.index = -1;
	return result;
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

GameModel Tile::getGameModelAt(size_t index)
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

void Tile::notifyItemAdded(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, NotifyLink link /*= LINK_OWNER*/)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	notifyItemAdded(item, oldLocation, index, std::span<const CreaturePtr>(spectators.begin(), spectators.size()), link);
}

void Tile::notifyItemAdded(const ItemPtr& item, const BlackTek::ItemLocation& oldLocation, int32_t index, std::span<const CreaturePtr> spectators, NotifyLink link /*= LINK_OWNER*/)
{
	for (auto& spectator : spectators)
	{
		std::static_pointer_cast<Player>(spectator)->notifyItemAdded(item, oldLocation, index, LINK_NEAR);
	}

	if (link == LINK_OWNER)
	{
		if (item)
		{
			if (hasFlag(TILESTATE_TELEPORT))
			{
				if (const auto& teleport = getTeleportItem())
				{
					teleport->teleportItem(item);
				}
			}
			else if (hasFlag(TILESTATE_TRASHHOLDER))
			{
				if (const auto& trashholder = getTrashHolder())
				{
					trashholder->consumeItem(item);
				}
			}
			else if (hasFlag(TILESTATE_MAILBOX))
			{
				if (const auto mailbox = getMailbox())
				{
					mailbox->deliverItem(item);
				}
			}

			if (TilePtr tile = item->getTile())
			{
				g_moveEvents->onItemMove(item, tile, true);
			}
		}
	}
}

void Tile::notifyItemRemoved(const ItemPtr& item, const BlackTek::ItemLocation& newLocation, int32_t index, NotifyLink link)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	notifyItemRemoved(item, newLocation, index, std::span<const CreaturePtr>(spectators.begin(), spectators.size()), link);
}

void Tile::notifyItemRemoved(const ItemPtr& item, const BlackTek::ItemLocation& newLocation, int32_t index, std::span<const CreaturePtr> spectators, NotifyLink)
{
	if (getStackSize() > 8)
	{
		onUpdateTile(spectators);
	}

	// Final test location, if the spectators internals were ever failing to filter only players
	// then npcs would trigger, spawns would trigger, and moving things to and from tiles, would trigger
	// and we would know about it pretty quickly, howevever if this doesn't happen, and this new RTTI tagging
	// works as well as anticipated, I will cleanup the view changes, and apply the same system for thing, cylinder and item based classes.

	for (auto& spectator : spectators)
	{
		std::static_pointer_cast<Player>(spectator)->notifyItemRemoved(item, newLocation, index, LINK_NEAR);
	}

	//calling movement scripts
	if (item)
	{
		g_moveEvents->onItemMove(item, getTile(), false);
	}
}

void Tile::notifyCreatureAdded(const CreaturePtr& creature, const TilePtr& oldTile)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	notifyCreatureAdded(creature, oldTile, std::span<const CreaturePtr>(spectators.begin(), spectators.size()));
}

void Tile::notifyCreatureAdded(const CreaturePtr& creature, const TilePtr&, std::span<const CreaturePtr> spectators)
{
	for (const auto& spectatorPlayer : spectators
		| std::views::filter([](const auto& c) { return c->is_player(); })
		| std::views::transform([](auto& c) { return std::static_pointer_cast<Player>(c); }))
	{
		spectatorPlayer->onNearbyCreatureMoved(creature);
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

void Tile::notifyCreatureRemoved(const CreaturePtr& creature, const TilePtr& newTile)
{
	SpectatorVec spectators;
	if (getStackSize() > 8)
	{
		g_game.map.getSpectators(spectators, getPosition(), true, true);
	}
	notifyCreatureRemoved(creature, newTile, std::span<const CreaturePtr>(spectators.begin(), spectators.size()));
}

void Tile::notifyCreatureRemoved(const CreaturePtr& creature, const TilePtr&, std::span<const CreaturePtr> spectators)
{
	if (getStackSize() > 8)
	{
		onUpdateTile(spectators);
	}

	g_moveEvents->onCreatureMove(creature, getTile(), MOVE_EVENT_STEP_OUT);
}

void Tile::addItemSilently(const ItemPtr& item)
{
	item->setTileParent(getTile());

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

	syncChunkFlags();
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

	syncChunkFlags();
}

void Tile::syncChunkFlags()
{
	if (not owning_chunk.IsValid())
		return;

	auto* chunk = g_game.map.getChunk(owning_chunk);
	if (not chunk)
		return;

	const uint32_t offsetX = tilePos.x & BlackTek::World::Floor::Mask;
	const uint32_t offsetY = tilePos.y & BlackTek::World::Floor::Mask;

	chunk->SetTileBlockState(offsetX, offsetY, tilePos.z,
		hasFlag(TILESTATE_BLOCKPATH), hasFlag(TILESTATE_BLOCKSOLID), hasProperty(CONST_PROP_BLOCKPROJECTILE));
}

bool Tile::isMoveableBlocking() const
{
	return !ground || hasFlag(TILESTATE_BLOCKSOLID);
}

ItemPtr Tile::getUseItem(const int32_t)
{
	// no items, get ground
	const auto& items = getItemList();
	if (!items || items->size() == 0) {
		return ground;
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
	const auto location = item->getLocation();
	if (location.tile or location.player)
	{
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
