// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include <boost/range/adaptor/reversed.hpp>

#include "tile.h"

#include "creature.h"
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

bool Tile::hasProperty(ITEMPROPERTY prop) const
{
	if (ground && ground->hasProperty(prop)) {
		return true;
	}

	if (const auto items = getItemList()) {
		for (const auto item : *items) {
			if (item->hasProperty(prop)) {
				return true;
			}
		}
	}
	return false;
}

bool Tile::hasProperty(const ItemPtr& exclude, ITEMPROPERTY prop) const
{
	assert(exclude);

	if (ground && exclude != ground && ground->hasProperty(prop)) {
		return true;
	}

	if (const auto items = getItemList()) {
		for (const auto item : *items) {
			if (item != exclude && item->hasProperty(prop)) {
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

std::string Tile::getDescription(int32_t) const
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
		if (!creatures->empty()) {
			return *creatures->begin();
		}
	}
	return nullptr;
}

CreatureConstPtr Tile::getBottomCreature() const
{
	if (const auto creatures = getCreatures()) {
		if (!creatures->empty()) {
			return *creatures->rbegin();
		}
	}
	return nullptr;
}

CreaturePtr Tile::getTopVisibleCreature(const CreaturePtr creature) const
{
	if (const auto creatures = getCreatures()) {
		if (creature) {
			for (const auto tileCreature : *creatures) {
				if (creature->canSeeCreature(tileCreature)) {
					return tileCreature;
				}
			}
		} else {
			for (const auto tileCreature : *creatures) {
				if (!tileCreature->isInvisible()) {
					const auto player = tileCreature->getPlayer();
					if (!player || !player->isInGhostMode()) {
						return tileCreature;
					}
				}
			}
		}
	}
	return nullptr;
}

CreatureConstPtr Tile::getBottomVisibleCreature(const CreatureConstPtr& creature) const
{
	if (const auto creatures = getCreatures()) {
		if (creature) {
			for (auto it = creatures->rbegin(), end = creatures->rend(); it != end; ++it) {
				if (creature->canSeeCreature(*it)) {
					return *it;
				}
			}
		} else {
			for (auto it = creatures->rbegin(), end = creatures->rend(); it != end; ++it) {
				if (!(*it)->isInvisible()) {
					if (const auto player = (*it)->getPlayer(); !player || !player->isInGhostMode()) {
						return *it;
					}
				}
			}
		}
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

ThingPtr Tile::getTopVisibleThing(const CreaturePtr creature)
{
	if (const auto thing = getTopVisibleCreature(creature)) {
		return thing;
	}

	if (const auto items = getItemList()) {
		for (ItemVector::const_iterator it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it) {
			if (const ItemType& iit = Item::items[(*it)->getID()]; !iit.lookThrough) {
				return (*it);
			}
		}

		for (auto it = ItemVector::const_reverse_iterator(items->getEndTopItem()), end = ItemVector::const_reverse_iterator(items->getBeginTopItem()); it != end; ++it) {
			if (const ItemType& iit = Item::items[(*it)->getID()]; !iit.lookThrough) {
				return (*it);
			}
		}
	}

	return ground;
}

void Tile::onAddTileItem(ItemPtr& item)
{
	if (item->hasProperty(CONST_PROP_MOVEABLE) || item->getContainer()) {
		if (const auto it = g_game.browseFields.find(getTile()); it != g_game.browseFields.end()) {
			it->second->addItemBack(item);
			item->setParent(it->second);
		}
	}

	setTileFlags(item);

	const Position& cylinderMapPos = getPosition();

	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, cylinderMapPos, true);

	//send to client
	for (const auto spectator : spectators) {
		if (const auto spectatorPlayer = spectator->getPlayer()) {
			spectatorPlayer->sendAddTileItem(getTile(), cylinderMapPos, item);
		}
	}

	//event methods
	for (const auto spectator : spectators) {
		TilePtr tp = this->getTile();
		spectator->onAddTileItem(tp, cylinderMapPos);
	}

	if ((!hasFlag(TILESTATE_PROTECTIONZONE) || g_config.getBoolean(ConfigManager::CLEAN_PROTECTION_ZONES)) && item->isCleanable()) {
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
				newItem->setParent(it->second);
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

	//send to client
	for (const auto spectator : spectators) {
		if (const auto spectatorPlayer = spectator->getPlayer()) {
			spectatorPlayer->sendUpdateTileItem(getTile(), cylinderMapPos, newItem);
		}
	}

	//event methods
	for (const auto spectator : spectators) {
		spectator->onUpdateTileItem(getTile(), cylinderMapPos, oldItem, oldType, newItem, newType);
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

	//send to client
	size_t i = 0;
	for (const auto spectator : spectators) {
		if (const auto tmpPlayer = spectator->getPlayer()) {
			tmpPlayer->sendRemoveTileThing(cylinderMapPos, oldStackPosVector[i++]);
		}
	}

	//event methods
	for (const auto spectator : spectators) {
		spectator->onRemoveTileItem(getTile(), cylinderMapPos, iType, item);
	}

	if (!hasFlag(TILESTATE_PROTECTIONZONE) || g_config.getBoolean(ConfigManager::CLEAN_PROTECTION_ZONES)) {
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

	//send to clients
	for (const auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->sendUpdateTile(getTile(), cylinderMapPos);
	}
}

ReturnValue Tile::queryAdd(CreaturePtr creature, uint32_t flags)
{
    ReturnValue results = RETURNVALUE_NOERROR;

    if (hasBitSet(FLAG_NOLIMIT, flags)) {
        return RETURNVALUE_NOERROR;
    }

    if (hasBitSet(FLAG_PATHFINDING, flags) && hasFlag(TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT)) {
        return RETURNVALUE_NOTPOSSIBLE;
    }

    if (ground == nullptr) {
        return RETURNVALUE_NOTPOSSIBLE;
    }

	const auto creatures = getCreatures();

	if (creatures && !creatures->empty() && !hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags)) {
		for (const auto tileCreature : *creatures) {
			if (!tileCreature->isInGhostMode() && (tileCreature->getPlayer() && !tileCreature->getPlayer()->isAccessPlayer() )) {
				if (creature->getPlayer() && !creature->getPlayer()->isAccessPlayer() && !creature->getPlayer()->canWalkthrough(tileCreature)) {
					return RETURNVALUE_NOTENOUGHROOM;
				}
			}
		}
	}

    if (!hasBitSet(FLAG_IGNOREBLOCKITEM, flags)) {
        //If the FLAG_IGNOREBLOCKITEM bit isn't set we dont have to iterate every single item
        if (hasFlag(TILESTATE_BLOCKSOLID)) {
            return RETURNVALUE_NOTENOUGHROOM;
        }
    } else {
        //FLAG_IGNOREBLOCKITEM is set
        if (ground) {
	        if (const ItemType& iiType = Item::items[ground->getID()]; iiType.blockSolid && (!iiType.moveable || ground->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID))) {
                return RETURNVALUE_NOTPOSSIBLE;
            }
        }

        if (const auto items = getItemList()) {
            for (const auto item : *items) {
	            if (const ItemType& iiType = Item::items[item->getID()]; iiType.blockSolid && (!iiType.moveable || item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID))) {
                    return RETURNVALUE_NOTPOSSIBLE;
                }
            }
        }
    }

	if (auto player = std::dynamic_pointer_cast<Player>(creature)) {
		results = queryAdd(player, flags);
	} else if (auto monster = std::dynamic_pointer_cast<Monster>(creature)) {
		results = queryAdd(monster, flags);
	}

	return results;
}


ReturnValue Tile::queryAdd(PlayerPtr player, uint32_t flags) {

	const auto creatures = getCreatures();

	if (isHouseTile() && !house->isInvited(player)) {
		return RETURNVALUE_PLAYERISNOTINVITED;
	}

	// If we aren't a GM/Admin can't walk on a tile that has a creature, if we don't have walkthrough enabled.
	if (creatures && !creatures->empty() && !hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags) && !player->isAccessPlayer()) {
		for (const auto tileCreature : *creatures) {
			if (!player->canWalkthrough(tileCreature)) {
				return RETURNVALUE_NOTPOSSIBLE;
			}
		}
	}

	// We are auto-walking, lets not step on a field that would hurt us.
	if (const auto field = getFieldItem()) {
		if (field->getDamage() != 0 && hasBitSet(FLAG_PATHFINDING, flags) &&
			!hasBitSet(FLAG_IGNOREFIELDDAMAGE, flags)) {
			return RETURNVALUE_NOTPOSSIBLE;
		}
	}

	// Player is trying to login to a "no logout" tile.
	// note: might need to check for std::nullopt here as well actually.
	if (player->getParent() == nullptr && hasFlag(TILESTATE_NOLOGOUT)) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	// Player is pz'd, let do some checks.
	const auto playerTile = player->getTile();
	if (playerTile && player->isPzLocked()) {
		if (!playerTile->hasFlag(TILESTATE_PVPZONE)) {
			//player is trying to enter a pvp zone while being pz-locked
			if (hasFlag(TILESTATE_PVPZONE)) {
				return RETURNVALUE_PLAYERISPZLOCKEDENTERPVPZONE;
			}
		} else if (!hasFlag(TILESTATE_PVPZONE)) {
			// player is trying to leave a pvp zone while being pz-locked
			return RETURNVALUE_PLAYERISPZLOCKEDLEAVEPVPZONE;
		}

		if ((!playerTile->hasFlag(TILESTATE_NOPVPZONE) && hasFlag(TILESTATE_NOPVPZONE)) ||
			(!playerTile->hasFlag(TILESTATE_PROTECTIONZONE) && hasFlag(TILESTATE_PROTECTIONZONE))) {
			// player is trying to enter a non-pvp/protection zone while being pz-locked
			return RETURNVALUE_PLAYERISPZLOCKED;
		}
	}

	return RETURNVALUE_NOERROR;
}


ReturnValue Tile::queryAdd(MonsterPtr monster, uint32_t flags) {
	// Monsters
	// Monsters cannot enter pz, jump floors, or step into teleports
	if (hasFlag(TILESTATE_PROTECTIONZONE | TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT)) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (isHouseTile()) { // should we hardcode these things? I don't think so.
		return RETURNVALUE_NOTPOSSIBLE;
	}

	// Something immovable blocking the way.
	if (hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID)) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	// Monster is searching for a path and tile has immovable field in the way.
	if (hasBitSet(FLAG_PATHFINDING, flags) && hasFlag(TILESTATE_IMMOVABLENOFIELDBLOCKPATH)) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	// Monster is looking for a clear path, some stuff is blocking it.
	if (hasFlag(TILESTATE_BLOCKSOLID) || (hasBitSet(FLAG_PATHFINDING, flags) && hasFlag(TILESTATE_NOFIELDBLOCKPATH))) {
		if (!(monster->canPushItems() || hasBitSet(FLAG_IGNOREBLOCKITEM, flags))) {
			return RETURNVALUE_NOTPOSSIBLE;
		}
	}

	const auto creatures = getCreatures();

	// We have creatures on the tile we are trying to step on
	if (creatures && !creatures->empty()) {
		for (const auto tileCreature : *creatures) {
			// creature is not in ghost mode
			if (!tileCreature->isInGhostMode()) {
				return RETURNVALUE_NOTENOUGHROOM;
			}
			
			if (monster->canPushCreatures() && !monster->isSummon()) {
				// the creature is a player in ghost mode.
				if (tileCreature->getPlayer() && tileCreature->getPlayer()->isInGhostMode()) {
					continue;
				}
				// the creature is a monster this monster can't push.
				const auto creatureMonster = tileCreature->getMonster();
				if (!creatureMonster || !tileCreature->isPushable() || (creatureMonster->isSummon() && creatureMonster->getMaster()->getPlayer())) {
					return RETURNVALUE_NOTPOSSIBLE;
				}
			}
		}
	}

	// If the magic field is safe, return early
    const auto field = getFieldItem();
    if (!field || field->isBlocking() || field->getDamage() == 0) {
        return RETURNVALUE_NOERROR;
    }

    CombatType_t combatType = field->getCombatType();

    //There is 3 options for a monster to enter a magic field
    //1) Monster is immune
    if (!monster->isImmune(combatType)) {
        //1) Monster is able to walk over field type
        //2) Being attacked while random stepping will make it ignore field damages
        if (hasBitSet(FLAG_IGNOREFIELDDAMAGE, flags)) {
            if (!(monster->canWalkOnFieldType(combatType) || monster->isIgnoringFieldDamage())) {
                return RETURNVALUE_NOTPOSSIBLE;
            }
        } else {
            return RETURNVALUE_NOTPOSSIBLE;
        }
    }

    return RETURNVALUE_NOERROR;
}


ReturnValue Tile::queryAdd(NpcPtr npc, uint32_t flags) {
	if (npc->isPhaseable()) {
		return RETURNVALUE_NOERROR;
	}

	if (g_config.getBoolean(ConfigManager::NPC_PZ_WALKTHROUGH) and this->hasFlag(TILESTATE_PVPZONE)) {
		return RETURNVALUE_NOERROR;
	}

	const auto creatures = getCreatures();

	if (creatures and not creatures->empty() and not hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags)) {
		return RETURNVALUE_NOTENOUGHROOM;
	}

	// Here we can add more options specifically for NPC's
	// realistically, there is likely more use cases not considered for NPC's that should probably
	// be handled here, like perhaps fields and such, but I don't think its a huge concernt atm.

	return RETURNVALUE_NOERROR;
}

ReturnValue Tile::queryAdd(ItemPtr item, uint32_t flags, CreaturePtr mover) {
    // Tile's item stack is at its numeric limit can't add anything
    const auto& items = getItemList();
    if (items && items->size() >= 0xFFFF) {
        return RETURNVALUE_NOTPOSSIBLE;
    }

    // Tile has no limit flag
    if (hasBitSet(FLAG_NOLIMIT, flags)) {
        return RETURNVALUE_NOERROR;
    }

    // Can't move store items to tiles
    if (item->isStoreItem()) {
        return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
    }

    // If its a wall, but not a hangable item.
    bool itemIsHangable = item->isHangable();
    if (ground == nullptr && !itemIsHangable) {
        return RETURNVALUE_NOTPOSSIBLE;
    }

	if (isHouseTile()) {
		if (item->isStoreItem() && !item->hasAttribute(ITEM_ATTRIBUTE_WRAPID)) {
			return RETURNVALUE_ITEMCANNOTBEMOVEDTHERE;
		}

		if (mover && g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
			if (!house->isInvited(mover->getPlayer())) {
				return RETURNVALUE_PLAYERISNOTINVITED;
			}
		}
	}

    // If there is any creature there, who is not in ghost mode... don't think this should be here...
    const auto& creatures = getCreatures();
    if (creatures && !creatures->empty() && item->isBlocking() && !hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags)) {
        for (const auto tileCreature : *creatures) {
            if (!tileCreature->isInGhostMode()) {
                return RETURNVALUE_NOTENOUGHROOM;
            }
        }
    }

	//////////////////////////////////////////////////////////////
	// Can move the hangable check inside the loop instead, get rid of the else,
	// and then merge the other if(items) check and loop into this one.

    // If we have a hangable item and the tile supports hangables
    if (itemIsHangable && hasFlag(TILESTATE_SUPPORTS_HANGABLE)) {
        if (items) {
            for (const auto tileItem : *items) {
				// there is already a hangable there
                if (tileItem->isHangable()) {
                    return RETURNVALUE_NEEDEXCHANGE;
                }
            }
        }
	// I believe this 'else' is not needed, and potentially limiting where we don't want to be.
    } else {
        if (ground) {
            const ItemType& iiType = Item::items[ground->getID()];
            if (iiType.blockSolid) {
                if (!iiType.allowPickupable || item->isMagicField() || item->isBlocking()) {
                    if (!item->isPickupable()) {
                        return RETURNVALUE_NOTENOUGHROOM;
                    }

                    if (!iiType.hasHeight || iiType.pickupable || iiType.isBed()) {
                        return RETURNVALUE_NOTENOUGHROOM;
                    }
                }
            }
        }
		// We can move this into the previous check and combine the loops. 
        if (items) {
            for (const auto& tileItem : *items) {
                const ItemType& iiType = Item::items[tileItem->getID()];
                if (!iiType.blockSolid) {
                    continue;
                }

                if (iiType.allowPickupable && !item->isMagicField() && !item->isBlocking()) {
                    continue;
                }

                if (!item->isPickupable()) {
                    return RETURNVALUE_NOTENOUGHROOM;
                }

                if (!iiType.hasHeight || iiType.pickupable || iiType.isBed()) {
                    return RETURNVALUE_NOTENOUGHROOM;
                }
            }
        }
    }
    return RETURNVALUE_NOERROR;
}

ReturnValue Tile::queryAdd(int32_t, const ThingPtr& thing, uint32_t, uint32_t flags, CreaturePtr mover) {
	if (auto creature = std::dynamic_pointer_cast<Creature>(thing)) {
		return queryAdd(creature, flags);
	}

	if (auto item = std::dynamic_pointer_cast<Item>(thing)) {
		return queryAdd(item, flags, mover);
	}

	std::cout << "|| WARNING || Tile::queryAdd() passed the object "<< typeid(thing).name() << ", that is not a creature or item! " << "\n";

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
	if (index == -1) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	const auto item = thing->getItem();
	if (item == nullptr) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (actor && g_config.getBoolean(ConfigManager::ONLY_INVITED_CAN_MOVE_HOUSE_ITEMS)) {
		if (isHouseTile() && !house->isInvited(actor->getPlayer())) {
			return RETURNVALUE_PLAYERISNOTINVITED;
		}
	}

	if (count == 0 || (item->isStackable() && count > item->getItemCount())) {
		return RETURNVALUE_NOTPOSSIBLE;
	}

	if (!item->isMoveable() && !hasBitSet(FLAG_IGNORENOTMOVEABLE, flags)) {
		return RETURNVALUE_NOTMOVEABLE;
	}

	return RETURNVALUE_NOERROR;
}

CylinderPtr Tile::queryDestination(int32_t& someInt, const ThingPtr& thingPtr, ItemPtr& destItem, uint32_t& flags) {
	TilePtr destTile;
	destItem.reset();

	if (const auto creature = thingPtr->getCreature()) {
		if (const auto player = creature->getPlayer()) {
			if (isHouseTile() && !house->isInvited(player)) {
				const Position& entryPos = house->getEntryPosition();
				auto destTile = g_game.map.getTile(entryPos);
				if (!destTile) {
					destTile = g_game.map.getTile(player->getTemplePosition());
					if (!destTile) {
						destTile = std::make_shared<Tile>(0xFFFF, 0xFFFF, 0xFF);
					}
				}

				someInt = -1;
				destItem.reset();
				return destTile;
			}
		}
	}

	if (!thingPtr) {
		return getTile();
	}

	if (hasFlag(TILESTATE_FLOORCHANGE_DOWN)) {
		uint16_t dx = tilePos.x;
		uint16_t dy = tilePos.y;
		uint8_t dz = tilePos.z + 1;

		if (auto southDownTile = g_game.map.getTile(dx, dy - 1, dz); southDownTile && southDownTile->hasFlag(TILESTATE_FLOORCHANGE_SOUTH_ALT)) {
			dy -= 2;
			destTile = g_game.map.getTile(dx, dy, dz);
		}
		else if (auto eastDownTile = g_game.map.getTile(dx - 1, dy, dz); eastDownTile && eastDownTile->hasFlag(TILESTATE_FLOORCHANGE_EAST_ALT)) {
			dx -= 2;
			destTile = g_game.map.getTile(dx, dy, dz);
		}
		else {
			if (auto downTile = g_game.map.getTile(dx, dy, dz)) {
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
	else if (hasFlag(TILESTATE_FLOORCHANGE)) {
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

	if (!destTile) {
		destTile = this->getTile();
	}
	else {
		flags |= FLAG_NOLIMIT;
	}

	if (destTile) {
		if (auto destThing = destTile->getTopDownItem()) {
			destItem = destThing->getItem();
		}
	}

	return destTile;
}


void Tile::addThing(ThingPtr thing)
{
	addThing(0, thing);
}

void Tile::addThing(int32_t, ThingPtr thing)
{
	if (const auto& creature = thing->getCreature()) {
		g_game.map.clearChunkSpectatorCache();
		creature->setParent(getTile());
		const auto& creatures = getCreatures();
		creatures->insert(creatures->begin(), creature);
	} else {
		auto item = thing->getItem();
		if (item == nullptr) {
			return /*RETURNVALUE_NOTPOSSIBLE*/;
		}

		updateHouse(item);

		TileItemsPtr items = getItemList();
		if (items && items->size() >= 0xFFFF) {
			return /*RETURNVALUE_NOTPOSSIBLE*/;
		}
		item->setParent(getTile());

		updateHouse(item);

		const ItemType& itemType = Item::items[item->getID()];
		if (itemType.isGroundTile()) {
			if (ground == nullptr) {
				ground = item;
				onAddTileItem(item);
			} else {
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
		} else if (itemType.alwaysOnTop) {
			if (itemType.isSplash() && items) {
				for (ItemVector::const_iterator it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it) {
					const auto oldSplash = *it;
					if (!Item::items[oldSplash->getID()].isSplash()) {
						continue;
					}

					removeThing(oldSplash, 1);
					oldSplash->clearParent();
					postRemoveNotification(oldSplash, nullptr, 0);
					break;
				}
			}

			bool isInserted = false;

			if (items) {
				for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it) {
					//Note: this is different from internalAddThing
					if (itemType.alwaysOnTopOrder <= Item::items[(*it)->getID()].alwaysOnTopOrder) {
						items->insert(it, item);
						isInserted = true;
						break;
					}
				}
			} else {
				// std::unreachable()
			}

			if (!isInserted) {
				items->push_back(item);
			}

			onAddTileItem(item);
		} else {
			if (itemType.isMagicField()) {
				//remove old field item if exists
				if (items) {
					for (ItemVector::const_iterator it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it) {
						if (const auto oldField = (*it)->getMagicField()) {
							if (oldField->isReplaceable()) {
								removeThing(oldField, 1);
								oldField->clearParent();
								postRemoveNotification(oldField, nullptr, 0);
								break;
							} else {
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
	if (const auto creature = thing->getCreature()) {
		if (const auto creatures = getCreatures()) {
			if (const auto it = std::ranges::find(*creatures, thing); it != creatures->end()) {
				g_game.map.clearChunkSpectatorCache();

				creatures->erase(it);
			}
		}
		return;
	}

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
	if (!items) {
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
		for (const auto& spectator : spectators) {
			if (const auto& spectatorPlayer = spectator->getPlayer()) {
				oldStackPosVector.push_back(getStackposOfItem(spectatorPlayer, item));
			}
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
			for (const auto& spectator : spectators) {
				if (const auto& spectatorPlayer = spectator->getPlayer()) {
					oldStackPosVector.push_back(getStackposOfItem(spectatorPlayer, item));
				}
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
	if (const auto& creatures = getCreatures()) {
		return std::ranges::find(*creatures, creature) != creatures->end();
	}
	return false;
}

void Tile::removeCreature(CreaturePtr& creature)
{
	g_game.map.getQTNode(tilePos.x, tilePos.y)->removeCreature(creature);
	removeThing(creature, 0);
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

	if (const auto& creatures = getCreatures()) {
		if (thing->getCreature()) {
			for (const auto& creature : *creatures) {
				++n;
				if (creature == thing) {
					return n;
				}
			}
		} else {
			n += creatures->size();
		}
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

int32_t Tile::getClientIndexOfCreature(const PlayerConstPtr& player, const CreatureConstPtr& creature) const
{
	int32_t n;
	if (ground) {
		n = 1;
	} else {
		n = 0;
	}

	if (const auto& items = getItemList()) {
		n += items->getTopItemCount();
	}

	if (const auto& creatures = getCreatures()) {
		for (const auto& c : boost::adaptors::reverse(*creatures)) {
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
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it) {
				if (*it == item) {
					return n;
				} else if (++n == 10) {
					return -1;
				}
			}
		} else {
			n += items->getTopItemCount();
			if (n >= 10) {
				return -1;
			}
		}
	}

	if (const auto& creatures = getCreatures()) {
		for (const auto& creature : *creatures) {
			if (player->canSeeCreature(creature)) {
				if (++n >= 10) {
					return -1;
				}
			}
		}
	}

	if (items && !item->isAlwaysOnTop()) {
		for (auto it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it) {
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
	if (ground) {
		if (index == 0) {
			return ground;
		}

		--index;
	}

	const auto& items = getItemList();
	if (items) {
		const uint32_t topItemSize = items->getTopItemCount();
		if (index < topItemSize) {
			return items->at(items->getDownItemCount() + index);
		}
		index -= topItemSize;
	}

	if (const auto& creatures = getCreatures()) {
		if (index < creatures->size()) {
			return (*creatures)[index];
		}
		index -= creatures->size();
	}

	if (items && index < items->getDownItemCount()) {
		return items->at(index);
	}
	return nullptr;
}

void Tile::postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link /*= LINK_OWNER*/)
{
	SpectatorVec spectators;
	g_game.map.getSpectators(spectators, getPosition(), true, true);
	for (auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->postAddNotification(thing, oldParent, index, LINK_NEAR);
	}

	//add a reference to this item, it may be deleted after being added (mailbox for example)
	auto creature = thing->getCreature();
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


		// this one and player are the only two postAddNotifications using the thing as a creature.
		if (creature) {
			g_moveEvents->onCreatureMove(creature, getTile(), MOVE_EVENT_STEP_IN);
		} else if (item) {
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

	for (auto spectator : spectators) {
		assert(std::dynamic_pointer_cast<Player>(spectator) != nullptr);
		std::static_pointer_cast<Player>(spectator)->postRemoveNotification(thing, newParent, index, LINK_NEAR);
	}

	//calling movement scripts
	if (auto creature = thing->getCreature()) {
		g_moveEvents->onCreatureMove(creature, getTile(), MOVE_EVENT_STEP_OUT);
	} else {
		if (auto item = thing->getItem()) {
			g_moveEvents->onItemMove(item, getTile(), false);
		}
	}
}

void Tile::internalAddThing(ThingPtr thing)
{
	internalAddThing(0, thing);
}

void Tile::internalAddThing(uint32_t, ThingPtr thing)
{
	thing->setParent(getTile());

	if (const auto& creature = thing->getCreature()) {
		g_game.map.clearChunkSpectatorCache();

		const auto& creatures = getCreatures();
		creatures->insert(creatures->begin(), creature);
	} else {
		const auto& item = thing->getItem();
		if (item == nullptr) {
			return;
		}
		updateHouse(item);
		const ItemType& itemType = Item::items[item->getID()];
		if (itemType.isGroundTile()) {
			if (ground == nullptr) {
				ground = item;
				setTileFlags(item);
			}
			return;
		}

		auto items = getItemList();
		if (items->size() >= 0xFFFF) {
			return /*RETURNVALUE_NOTPOSSIBLE*/;
		}

		if (itemType.alwaysOnTop) {
			bool isInserted = false;
			for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it) {
				if (Item::items[(*it)->getID()].alwaysOnTopOrder > itemType.alwaysOnTopOrder) {
					items->insert(it, item);
					isInserted = true;
					break;
				}
			}

			if (!isInserted) {
				items->push_back(item);
			}
		} else {
			items->insert(items->getBeginDownItem(), item);
			items->addDownItemCount(1);
		}

		setTileFlags(item);
	}
}

void Tile::setTileFlags(const ItemConstPtr& item)
{
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
	if (container && container->getDepotLocker()) {
		setFlag(TILESTATE_DEPOT);
	}

	if (item->hasProperty(CONST_PROP_SUPPORTHANGABLE)) {
		setFlag(TILESTATE_SUPPORTS_HANGABLE);
	}
}

void Tile::resetTileFlags(const ItemPtr& item)
{
	if (const ItemType& it = Item::items[item->getID()]; it.floorChange != 0) {
		resetFlag(TILESTATE_FLOORCHANGE);
	}

	if (item->hasProperty(CONST_PROP_BLOCKSOLID) && !hasProperty(item, CONST_PROP_BLOCKSOLID)) {
		resetFlag(TILESTATE_BLOCKSOLID);
	}

	if (item->hasProperty(CONST_PROP_IMMOVABLEBLOCKSOLID) && !hasProperty(item, CONST_PROP_IMMOVABLEBLOCKSOLID)) {
		resetFlag(TILESTATE_IMMOVABLEBLOCKSOLID);
	}

	if (item->hasProperty(CONST_PROP_BLOCKPATH) && !hasProperty(item, CONST_PROP_BLOCKPATH)) {
		resetFlag(TILESTATE_BLOCKPATH);
	}

	if (item->hasProperty(CONST_PROP_NOFIELDBLOCKPATH) && !hasProperty(item, CONST_PROP_NOFIELDBLOCKPATH)) {
		resetFlag(TILESTATE_NOFIELDBLOCKPATH);
	}

	if (item->hasProperty(CONST_PROP_IMMOVABLEBLOCKPATH) && !hasProperty(item, CONST_PROP_IMMOVABLEBLOCKPATH)) {
		resetFlag(TILESTATE_IMMOVABLEBLOCKPATH);
	}

	if (item->hasProperty(CONST_PROP_IMMOVABLENOFIELDBLOCKPATH) && !hasProperty(item, CONST_PROP_IMMOVABLENOFIELDBLOCKPATH)) {
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

	if (const auto& container = item->getContainer(); container && container->getDepotLocker()) {
		resetFlag(TILESTATE_DEPOT);
	}

	if (item->hasProperty(CONST_PROP_SUPPORTHANGABLE)) {
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