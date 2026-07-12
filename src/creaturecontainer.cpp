// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "creaturecontainer.h"
#include "creature.h"
#include "player.h"
#include "monster.h"
#include "npc.h"
#include "tile.h"
#include "combat.h"
#include "game.h"
#include "configmanager.h"

extern Game g_game;
extern ConfigManager g_config;

bool CreatureContainer::hasCreature(const CreatureConstPtr& creature) const
{
    return std::ranges::find(creatureList, creature) != creatureList.end();
}

int32_t CreatureContainer::getCreatureIndex(const CreatureConstPtr& creature) const
{
    int32_t index = 0;
    for (const auto& tileCreature : creatureList)
    {
        if (tileCreature == creature)
        {
            return index;
        }
        ++index;
    }
    return -1;
}

CreaturePtr CreatureContainer::getTopCreature() const
{
    if (not creatureList.empty())
    {
        return *creatureList.begin();
    }
    return nullptr;
}

CreatureConstPtr CreatureContainer::getBottomCreature() const
{
    if (not creatureList.empty())
    {
        return *creatureList.rbegin();
    }
    return nullptr;
}

CreaturePtr CreatureContainer::getTopVisibleCreature(const CreaturePtr& creature) const
{
    for (const auto& tileCreature : creatureList)
    {
        const bool creature_has_sight = (creature and creature->canSeeCreature(tileCreature));
        const bool invisible_creature = (tileCreature->isInvisible() ? true : false) or (tileCreature->getPlayer() and tileCreature->getPlayer()->isInGhostMode());

        if ((creature_has_sight) or (not invisible_creature))
        {
            return tileCreature;
        }
    }
    return nullptr;
}

CreatureConstPtr CreatureContainer::getBottomVisibleCreature(const CreatureConstPtr& creature) const
{
    if (creature)
    {
        for (auto it = creatureList.rbegin(), end = creatureList.rend(); it != end; ++it)
        {
            if (creature->canSeeCreature(*it))
            {
                return *it;
            }
        }
    }
    else
    {
        for (auto it = creatureList.rbegin(), end = creatureList.rend(); it != end; ++it)
        {
            if (not (*it)->isInvisible())
            {
                if (const auto player = (*it)->getPlayer(); not player or not player->isInGhostMode())
                {
                    return *it;
                }
            }
        }
    }
    return nullptr;
}

std::optional<ReturnValue> CreatureContainer::queryAddRestrictions(uint32_t flags) const
{
    const auto tile = getOwner();

    if (hasBitSet(FLAG_NOLIMIT, flags))
        return RETURNVALUE_NOERROR;

    if (hasBitSet(FLAG_PATHFINDING, flags) and tile->hasFlag(TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT))
        return RETURNVALUE_NOTPOSSIBLE;

    if (tile->getGround() == nullptr)
        return RETURNVALUE_NOTPOSSIBLE;

    // If the FLAG_IGNOREBLOCKITEM bit isn't set we dont have to iterate every single item
    if (not hasBitSet(FLAG_IGNOREBLOCKITEM, flags))
    {
        if (tile->hasFlag(TILESTATE_BLOCKSOLID))
            return RETURNVALUE_NOTENOUGHROOM;
    }
    else
    {
        //FLAG_IGNOREBLOCKITEM is set
        if (const auto ground = tile->getGround())
        {
            if (const ItemType& iiType = Item::items[ground->getID()]; iiType.blockSolid and (not iiType.moveable or ground->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)))
                return RETURNVALUE_NOTPOSSIBLE;
        }

        if (const auto items = tile->getItemList())
        {
            for (const auto& item : *items)
            {
                if (const ItemType& iiType = Item::items[item->getID()]; iiType.blockSolid and (not iiType.moveable or item->hasAttribute(ITEM_ATTRIBUTE_UNIQUEID)))
                    return RETURNVALUE_NOTPOSSIBLE;
            }
        }
    }

    return std::nullopt;
}

ReturnValue CreatureContainer::queryAdd(PlayerPtr player, uint32_t flags) const
{
    if (const auto restriction = queryAddRestrictions(flags))
        return *restriction;

    const auto tile = getOwner();

    if (tile->isHouseTile() and not tile->getHouse()->isInvited(player))
        return RETURNVALUE_PLAYERISNOTINVITED;

    // If we aren't a GM/Admin can't walk on a tile that has a creature, if we don't have walkthrough enabled.
    if (not creatureList.empty() and not hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags) and not player->isAccessPlayer())
    {
        for (const auto& tileCreature : creatureList)
        {
            if (not player->canWalkthrough(tileCreature))
                return RETURNVALUE_NOTPOSSIBLE;
        }
    }

    // We are auto-walking, lets not step on a field that would hurt us.
    if (const auto field = tile->getFieldItem())
    {
        if (field->getDamage() != 0 and hasBitSet(FLAG_PATHFINDING, flags) and not hasBitSet(FLAG_IGNOREFIELDDAMAGE, flags))
            return RETURNVALUE_NOTPOSSIBLE;
    }

    // Player is trying to login to a "no logout" tile.
    if (player->getTile() == nullptr and tile->hasFlag(TILESTATE_NOLOGOUT))
        return RETURNVALUE_NOTPOSSIBLE;

    // Player is pz'd, let do some checks.
    const auto playerTile = player->getTile();

    if (playerTile and player->isPzLocked())
    {
        if (not playerTile->hasFlag(TILESTATE_PVPZONE))
        {
            if (tile->hasFlag(TILESTATE_PVPZONE))
                return RETURNVALUE_PLAYERISPZLOCKEDENTERPVPZONE;
        }
        else if (not tile->hasFlag(TILESTATE_PVPZONE))
        {
            return RETURNVALUE_PLAYERISPZLOCKEDLEAVEPVPZONE;
        }

        // player is trying to enter a non-pvp/protection zone while being pz-locked
        if ((not playerTile->hasFlag(TILESTATE_NOPVPZONE) and tile->hasFlag(TILESTATE_NOPVPZONE))
            or (not playerTile->hasFlag(TILESTATE_PROTECTIONZONE) and tile->hasFlag(TILESTATE_PROTECTIONZONE)))
            return RETURNVALUE_PLAYERISPZLOCKED;
    }

    return RETURNVALUE_NOERROR;
}

ReturnValue CreatureContainer::queryAdd(MonsterPtr monster, uint32_t flags) const
{
    if (const auto restriction = queryAddRestrictions(flags))
        return *restriction;

    const auto tile = getOwner();

    // Monsters
    // Monsters cannot enter pz, jump floors, or step into teleports
    if (tile->hasFlag(TILESTATE_PROTECTIONZONE | TILESTATE_FLOORCHANGE | TILESTATE_TELEPORT))
        return RETURNVALUE_NOTPOSSIBLE;

    if (tile->isHouseTile()) // should we hardcode these things? I don't think so.
        return RETURNVALUE_NOTPOSSIBLE;

    // Something immovable blocking the way.
    if (tile->hasFlag(TILESTATE_IMMOVABLEBLOCKSOLID))
        return RETURNVALUE_NOTPOSSIBLE;

    // Monster is searching for a path and tile has immovable field in the way.
    if (hasBitSet(FLAG_PATHFINDING, flags) and tile->hasFlag(TILESTATE_IMMOVABLENOFIELDBLOCKPATH))
        return RETURNVALUE_NOTPOSSIBLE;

    // Monster is looking for a clear path, some stuff is blocking it.
    if (tile->hasFlag(TILESTATE_BLOCKSOLID) or (hasBitSet(FLAG_PATHFINDING, flags) and tile->hasFlag(TILESTATE_NOFIELDBLOCKPATH)))
    {
        if (not (monster->canPushItems() or hasBitSet(FLAG_IGNOREBLOCKITEM, flags)))
            return RETURNVALUE_NOTPOSSIBLE;
    }

    // We have creatures on the tile we are trying to step on
    if (not creatureList.empty())
    {
        for (const auto& tileCreature : creatureList)
        {
            // creature is not in ghost mode
            if (not tileCreature->isInGhostMode())
                return RETURNVALUE_NOTENOUGHROOM;

            if (monster->canPushCreatures() and not monster->isSummon())
            {
                // the creature is a player in ghost mode.
                if (tileCreature->getPlayer() and tileCreature->getPlayer()->isInGhostMode())
                    continue;

                // the creature is a monster this monster can't push.
                const auto creatureMonster = tileCreature->getMonster();

                if (not creatureMonster or not tileCreature->isPushable() or (creatureMonster->isSummon() and creatureMonster->getMaster()->getPlayer()))
                    return RETURNVALUE_NOTPOSSIBLE;
            }
        }
    }

    // If the magic field is safe, return early
    const auto field = tile->getFieldItem();

    if (not field or field->isBlocking() or field->getDamage() == 0)
        return RETURNVALUE_NOERROR;

    auto combatType = field->getCombatType();

    if (not monster->isImmune(combatType))
    {
        if (hasBitSet(FLAG_IGNOREFIELDDAMAGE, flags))
        {
            if (not (monster->canWalkOnFieldType(combatType) or monster->isIgnoringFieldDamage()))
                return RETURNVALUE_NOTPOSSIBLE;
            else
                return RETURNVALUE_NOERROR;
        }
    }

    return RETURNVALUE_NOERROR;
}

ReturnValue CreatureContainer::queryAdd(NpcPtr npc, uint32_t flags) const
{
    if (const auto restriction = queryAddRestrictions(flags))
        return *restriction;

    const auto tile = getOwner();

    if (npc->isPhaseable())
        return RETURNVALUE_NOERROR;

    if (g_config.GetBoolean(ConfigManager::NPC_PZ_WALKTHROUGH) and tile->hasFlag(TILESTATE_PVPZONE))
        return RETURNVALUE_NOERROR;

    if (not creatureList.empty() and not hasBitSet(FLAG_IGNOREBLOCKCREATURE, flags))
        return RETURNVALUE_NOTENOUGHROOM;

    // Here we can add more options specifically for NPC's
    // realistically, there is likely more use cases not considered for NPC's that should probably
    // be handled here, like perhaps fields and such, but I don't think its a huge concernt atm.

    return RETURNVALUE_NOERROR;
}

void CreatureContainer::addCreature(const CreaturePtr& creature)
{
    g_game.map.clearChunkSpectatorCache();
    creature->setCurrentTile(getOwner());
    creatureList.insert(creatureList.begin(), creature);
}

void CreatureContainer::internalAddCreature(const CreaturePtr& creature)
{
    g_game.map.clearChunkSpectatorCache();
    creature->setCurrentTile(getOwner());
    creatureList.insert(creatureList.begin(), creature);
}

void CreatureContainer::removeCreature(const CreaturePtr& creature)
{
    if (const auto it = std::ranges::find(creatureList, creature); it != creatureList.end())
    {
        g_game.map.clearChunkSpectatorCache();
        creatureList.erase(it);
    }
}
