// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "iomap.h"
#include "iomapserialize.h"
#include "combat.h"
#include "creature.h"
#include "game.h"
#include "monster.h"

extern Game g_game;

bool Map::loadMap(const std::string& identifier, bool loadHouses)
{
	IOMap loader;
	if (!loader.loadMap(this, identifier)) {
		std::cout << "[Fatal - Map::loadMap] " << loader.getLastErrorString() << std::endl;
		return false;
	}

	//if (!IOMap::loadSpawns(this)) {
	//	std::cout << "[Warning - Map::loadMap] Failed to load spawn data." << std::endl;
	//}

	if (loadHouses) {
		if (!IOMap::loadHouses(this)) {
			std::cout << "[Warning - Map::loadMap] Failed to load house data." << std::endl;
		}

		IOMapSerialize::loadHouseInfo();
		IOMapSerialize::loadHouseItems(this);
	}
	return true;
}

bool Map::save()
{
	bool saved = false;
	for (uint32_t tries = 0; tries < 3; tries++) {
		if (IOMapSerialize::saveHouseInfo()) {
			saved = true;
			break;
		}
	}

	if (!saved) {
		return false;
	}

	saved = false;
	for (uint32_t tries = 0; tries < 3; tries++) {
		if (IOMapSerialize::saveHouseItems()) {
			saved = true;
			break;
		}
	}
	return saved;
}

TilePtr Map::getTile(const uint16_t x, const uint16_t y, const uint8_t z)
{
	if (z >= MAP_MAX_LAYERS) {
		return nullptr;
	}

	auto leaf = QTreeNode::getLeafStatic< QTreeLeafNode*, QTreeNode*>(&root, x, y);
	if (!leaf) {
		return nullptr;
	}

	auto floor = leaf->getFloor(z);
	if (!floor) {
		return nullptr;
	}
	return floor->tiles[x & FLOOR_MASK][y & FLOOR_MASK];
}

void Map::setTile(const uint16_t x, const uint16_t y, const uint8_t z, TilePtr& newTile)
{
	if (z >= MAP_MAX_LAYERS) {
		std::cout << "ERROR: Attempt to set tile on invalid coordinate " << Position(x, y, z) << "!" << std::endl;
		return;
	}

	QTreeLeafNode::newLeaf = false;
	const auto& leaf = root.createLeaf(x, y, 15);

	if (QTreeLeafNode::newLeaf) {
		//update north
		if (const auto& northLeaf = root.getLeaf(x, y - FLOOR_SIZE)) {
			northLeaf->leafS = leaf;
		}

		//update west leaf
		if (const auto& westLeaf = root.getLeaf(x - FLOOR_SIZE, y)) {
			westLeaf->leafE = leaf;
		}

		//update south
		if (const auto& southLeaf = root.getLeaf(x, y + FLOOR_SIZE)) {
			leaf->leafS = southLeaf;
		}

		//update east
		if (const auto& eastLeaf = root.getLeaf(x + FLOOR_SIZE, y)) {
			leaf->leafE = eastLeaf;
		}
	}

	const auto& floor = leaf->createFloor(z);
	const uint32_t offsetX = x & FLOOR_MASK;
	const uint32_t offsetY = y & FLOOR_MASK;

	if (auto& tile = floor->tiles[offsetX][offsetY]) {
		if (const auto& items = newTile->getItemList()) {
			for (auto it = items->rbegin(), end = items->rend(); it != end; ++it) {
				tile->addThing(*it);
			}
			items->clear();
		}

		if (const auto& ground = newTile->getGround()) {
			tile->addThing(ground);
			newTile->setGround(nullptr);
		}
	} else {
		tile = newTile;
	}
}

void Map::removeTile(const uint16_t x, const uint16_t y, const uint8_t z) const
{
	if (z >= MAP_MAX_LAYERS) {
		return;
	}

	const auto& leaf = QTreeNode::getLeafStatic<const QTreeLeafNode*, const QTreeNode*>(&root, x, y);
	if (!leaf) {
		return;
	}

	const auto& floor = leaf->getFloor(z);
	if (!floor) {
		return;
	}

	if (const auto& tile = floor->tiles[x & FLOOR_MASK][y & FLOOR_MASK]) {
		if (const auto& creatures = tile->getCreatures()) {
			for (int32_t i = creatures->size(); --i >= 0;) {
				if (const auto& player = (*creatures)[i]->getPlayer()) {
					g_game.internalTeleport(player, player->getTown()->getTemplePosition(), false, FLAG_NOLIMIT);
				} else {
					g_game.removeCreature((*creatures)[i]);
				}
			}
		}

		if (const auto& items = tile->getItemList()) {
			for (auto it = items->begin(), end = items->end(); it != end; ++it) {
				g_game.internalRemoveItem(*it);
			}
		}

		if (const auto& ground = tile->getGround()) {
			g_game.internalRemoveItem(ground);
			tile->setGround(nullptr);
		}
	}
}

bool Map::placeCreature(const Position& centerPos, CreaturePtr creature, bool extendedPos/* = false*/, bool forceLogin/* = false*/)
{
	bool foundTile;
	bool placeInPZ;

	auto tile = getTile(centerPos.x, centerPos.y, centerPos.z);
	if (tile) {
		placeInPZ = tile->hasFlag(TILESTATE_PROTECTIONZONE);
		uint32_t flags = FLAG_IGNOREBLOCKITEM;
		if (const auto& player = creature->getPlayer()) {
			if (player->isAccountManager()) {
				flags |= FLAG_IGNOREBLOCKCREATURE;
			}
		}
		const ReturnValue ret = tile->queryAdd(creature, FLAG_IGNOREBLOCKITEM);
		foundTile = forceLogin || ret == RETURNVALUE_NOERROR || ret == RETURNVALUE_PLAYERISNOTINVITED;
	} else {
		placeInPZ = false;
		foundTile = false;
	}

	if (!foundTile) {
		static std::vector<std::pair<int32_t, int32_t>> extendedRelList {
			                   {0, -2},
			         {2,   0}, {0,  2}, {-2, 0},
			{-1, -1},{0,  -1},          {1, -1}, {-1, 0},
			         {1,   1}, {-1, 1}, {0,  1},
			                   {1,  1}
		};

		static std::vector<std::pair<int32_t, int32_t>> normalRelList {
			{-1, -1}, {0, -1}, {1, -1},
			{-1,  0},          {1,  0},
			{-1,  1}, {0,  1}, {1,  1}
		};

		std::vector<std::pair<int32_t, int32_t>>& relList = (extendedPos ? extendedRelList : normalRelList);

		if (extendedPos) {
			std::shuffle(relList.begin(), relList.begin() + 4, getRandomGenerator());
			std::shuffle(relList.begin() + 4, relList.end(), getRandomGenerator());
		} else {
			std::ranges::shuffle(relList, getRandomGenerator());
		}

		for (const auto& it : relList) {
			Position tryPos(centerPos.x + it.first, centerPos.y + it.second, centerPos.z);

			tile = getTile(tryPos.x, tryPos.y, tryPos.z);
			if (!tile || (placeInPZ && !tile->hasFlag(TILESTATE_PROTECTIONZONE))) {
				continue;
			}

			if (tile->queryAdd(creature, 0) == RETURNVALUE_NOERROR) {
				if (!extendedPos || isSightClear(centerPos, tryPos, false)) {
					foundTile = true;
					break;
				}
			}
		}

		if (!foundTile) {
			return false;
		}
	}

	int32_t index = 0;
	uint32_t flags = 0;
	ItemPtr toItem = nullptr;
	auto toCylinder = tile->queryDestination(index, creature, toItem, flags);
	toCylinder->internalAddThing(creature);

	const Position& dest = toCylinder->getPosition();
	getQTNode(dest.x, dest.y)->addCreature(creature);
	return true;
}

void Map::moveCreature(CreaturePtr& creature, const TilePtr& newTile, bool forceTeleport/* = false*/)
{
	const auto& oldTile = creature->getTile();
	// If the tile does not have the creature it means that the creature is ready for elimination, we skip the move.
	if (!oldTile->hasCreature(creature)) {
		return;
	}
	Position oldPos = oldTile->getPosition();
	Position newPos = newTile->getPosition();

	bool teleport = forceTeleport || !newTile->getGround() || !Position::areInRange<1, 1, 0>(oldPos, newPos);

	SpectatorVec spectators, newPosSpectators;
	getSpectators(spectators, oldPos, true);
	getSpectators(newPosSpectators, newPos, true);
	spectators.addSpectators(newPosSpectators);
	std::vector<int32_t> oldStackPosVector;
	size_t i = 0;

	for (const auto& spectator : spectators) 
	{
		if (const auto& tmpPlayer = spectator->getPlayer()) 
		{
			if (tmpPlayer->canSeeCreature(creature)) 
			{
				oldStackPosVector.push_back(oldTile->getClientIndexOfCreature(tmpPlayer, creature));
			} 
			else 
			{
				oldStackPosVector.push_back(-1);
			}
		}
	}

	//remove the creature
	oldTile->removeThing(creature, 0);

	const auto& leaf = getQTNode(oldPos.x, oldPos.y);
	const auto& new_leaf = getQTNode(newPos.x, newPos.y);

	// Switch the node ownership
	if (leaf != new_leaf) {
		leaf->removeCreature(creature);
		new_leaf->addCreature(creature);
	}

	//add the creature
	newTile->addThing(creature);

	if (!teleport) {
		if (oldPos.y > newPos.y) {
			creature->setDirection(DIRECTION_NORTH);
		} else if (oldPos.y < newPos.y) {
			creature->setDirection(DIRECTION_SOUTH);
		}

		if (oldPos.x < newPos.x) {
			creature->setDirection(DIRECTION_EAST);
		} else if (oldPos.x > newPos.x) {
			creature->setDirection(DIRECTION_WEST);
		}
	}


	for (const auto& spectator : spectators) 
	{
		if (const auto& monster = spectator->getMonster(); monster) 
		{
			monster->setIdle(false);
		}

		if (const auto& tmpPlayer = spectator->getPlayer()) 
		{ 
			// Use the correct stackpos
			if (const int32_t& stackpos = oldStackPosVector[i++]; stackpos != -1) 
			{
				tmpPlayer->sendCreatureMove(creature, newPos, newTile->getClientIndexOfCreature(tmpPlayer, creature), oldPos, stackpos, teleport); // send to client
			}
		}

		spectator->onCreatureMove(creature, newTile, newPos, oldTile, oldPos, teleport); //event method
	}

	oldTile->postRemoveNotification(creature, newTile, 0);
	newTile->postAddNotification(creature, oldTile, 0);
}

void Map::getSpectatorsInternal(SpectatorVec& spectators, const Position& centerPos, const int32_t minRangeX, const int32_t maxRangeX, const int32_t minRangeY, const int32_t maxRangeY, const int32_t minRangeZ, const int32_t maxRangeZ, const bool onlyPlayers) const
{
    const auto min_y = centerPos.y + minRangeY;
    const auto min_x = centerPos.x + minRangeX;
    const auto max_y = centerPos.y + maxRangeY;
    const auto max_x = centerPos.x + maxRangeX;

    const int32_t minoffset = centerPos.getZ() - maxRangeZ;
    const uint16_t x1 = std::min<uint32_t>(0xFFFF, std::max<int32_t>(0, (min_x + minoffset)));
    const uint16_t y1 = std::min<uint32_t>(0xFFFF, std::max<int32_t>(0, (min_y + minoffset)));

    const int32_t maxoffset = centerPos.getZ() - minRangeZ;
    const uint16_t x2 = std::min<uint32_t>(0xFFFF, std::max<int32_t>(0, (max_x + maxoffset)));
    const uint16_t y2 = std::min<uint32_t>(0xFFFF, std::max<int32_t>(0, (max_y + maxoffset)));

    const std::array<int32_t, 4> cache_values{
        min_y + minoffset,
        max_y + maxoffset,
        min_x + minoffset,
        max_x + maxoffset
    };

    const int32_t startx1 = x1 - (x1 % FLOOR_SIZE);
    const int32_t starty1 = y1 - (y1 % FLOOR_SIZE);
    const int32_t endx2 = x2 - (x2 % FLOOR_SIZE);
    const int32_t endy2 = y2 - (y2 % FLOOR_SIZE);

    const auto& startLeaf = QTreeNode::getLeafStatic<const QTreeLeafNode*, const QTreeNode*>(&root, startx1, starty1);
    auto leafS = startLeaf;

    for (int_fast32_t ny = starty1; ny <= endy2; ny += FLOOR_SIZE) {
        const QTreeLeafNode* leafE = leafS;
        for (int_fast32_t nx = startx1; nx <= endx2; nx += FLOOR_SIZE) {
            if (leafE) {
                const auto& node_list = (onlyPlayers ? leafE->player_list : leafE->creature_list);
                std::ranges::for_each(node_list, [&](const CreaturePtr& creature) {
                    const Position& cpos = creature->getPosition();
                    if (minRangeZ > cpos.z || maxRangeZ < cpos.z) {
                        return;
                    }

                    const int_fast16_t offsetZ = Position::getOffsetZ(centerPos, cpos);
                    if ((cache_values[0] + offsetZ) > cpos.y || (cache_values[1] + offsetZ) < cpos.y || (cache_values[2] + offsetZ) > cpos.x || (cache_values[3] + offsetZ) < cpos.x) {
                        return;
                    }

                    spectators.emplace_back(creature);
                });
                leafE = leafE->leafE;
            } else {
                leafE = QTreeNode::getLeafStatic<const QTreeLeafNode*, const QTreeNode*>(&root, nx + FLOOR_SIZE, ny);
            }
        }

        if (leafS) {
            leafS = leafS->leafS;
        } else {
            leafS = QTreeNode::getLeafStatic<const QTreeLeafNode*, const QTreeNode*>(&root, startx1, ny + FLOOR_SIZE);
        }
    }
}

void Map::getSpectators(SpectatorVec& spectators, const Position& centerPos, const bool multifloor /*= false*/, bool onlyPlayers /*= false*/, int32_t minRangeX /*= 0*/, int32_t maxRangeX /*= 0*/, int32_t minRangeY /*= 0*/, int32_t maxRangeY /*= 0*/)
{
    if (centerPos.z >= MAP_MAX_LAYERS) {
        return;
    }

    bool foundCache = false;
    bool cacheResult = false;

    minRangeX = (minRangeX == 0 ? -maxViewportX : -minRangeX);
    maxRangeX = (maxRangeX == 0 ? maxViewportX : maxRangeX);
    minRangeY = (minRangeY == 0 ? -maxViewportY : -minRangeY);
    maxRangeY = (maxRangeY == 0 ? maxViewportY : maxRangeY);

    chunkKey.minRangeX = minRangeX;
	chunkKey.maxRangeX = maxRangeX;
	chunkKey.minRangeY = minRangeY;
	chunkKey.maxRangeY = maxRangeY;
	chunkKey.x = centerPos.x;
	chunkKey.y = centerPos.y;
	chunkKey.z = centerPos.z;
	chunkKey.multifloor = multifloor;
	chunkKey.onlyPlayers = onlyPlayers;

    if (const auto it = chunksSpectatorCache.find(chunkKey); it != chunksSpectatorCache.end()) {
		if (!spectators.empty()) {
			spectators.addSpectators(it->second);
		} else {
			spectators = it->second;
		}
		foundCache = true;
	} else {
		cacheResult = true;
	}

    if (minRangeX == -maxViewportX && maxRangeX == maxViewportX && minRangeY == -maxViewportY && maxRangeY == maxViewportY && multifloor) {
        if (onlyPlayers) {
	        if (const auto it = playersSpectatorCache.find(centerPos); it != playersSpectatorCache.end()) {
                spectators.addSpectators(it->second);
                foundCache = true;
            }
        }

        if (!foundCache) {
	        if (const auto it = spectatorCache.find(centerPos); it != spectatorCache.end()) {
                if (!onlyPlayers) {
                    const SpectatorVec& cachedSpectators = it->second;
                    spectators.addSpectators(cachedSpectators);
                } else {
                    for (const auto& spectator : it->second) {
                        if (spectator->getPlayer()) {
                            spectators.emplace_back(spectator);
                        }
                    }
                }
                foundCache = true;
            } else {
                cacheResult = true;
            }
        }
    }

    if (!foundCache) {
        int32_t minRangeZ;
        int32_t maxRangeZ;

        if (multifloor) {
            if (centerPos.z > 7) {
                //underground (8->15)
                minRangeZ = std::max<int32_t>(centerPos.getZ() - 2, 0);
                maxRangeZ = std::min<int32_t>(centerPos.getZ() + 2, MAP_MAX_LAYERS - 1);
            } else if (centerPos.z == 6) {
                minRangeZ = 0;
                maxRangeZ = 8;
            } else if (centerPos.z == 7) {
                minRangeZ = 0;
                maxRangeZ = 9;
            } else {
                minRangeZ = 0;
                maxRangeZ = 7;
            }
        } else {
            minRangeZ = centerPos.z;
            maxRangeZ = centerPos.z;
        }

        getSpectatorsInternal(spectators, centerPos, minRangeX, maxRangeX, minRangeY, maxRangeY, minRangeZ, maxRangeZ, onlyPlayers);

        if (cacheResult) {
            chunksSpectatorCache.emplace(chunkKey, spectators);
        }
    }
}

void Map::clearSpectatorCache()
{
	spectatorCache.clear();
}

void Map::clearPlayersSpectatorCache()
{
	playersSpectatorCache.clear();
}

bool Map::canThrowObjectTo(const Position& fromPos, const Position& toPos, const bool checkLineOfSight /*= true*/, const bool sameFloor /*= false*/,
                           const int32_t rangex /*= Map::maxClientViewportX*/, const int32_t rangey /*= Map::maxClientViewportY*/)
{
	if (Position::getDistanceX(fromPos, toPos) > rangex || Position::getDistanceY(fromPos, toPos) > rangey) {
		return false;
	}

	return !checkLineOfSight || isSightClear(fromPos, toPos, sameFloor);
}

bool Map::isTileClear(const uint16_t x, const uint16_t y, const uint8_t z, const bool blockFloor /*= false*/)
{
	auto tile = getTile(x, y, z);
	if (!tile) {
		return true;
	}

	if (blockFloor && tile->getGround()) {
		return false;
	}

	return !tile->hasProperty(CONST_PROP_BLOCKPROJECTILE);
}

namespace {

bool checkSteepLine(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint8_t z)
{
	const float dx = x1 - x0;
	const float slope = (dx == 0) ? 1 : (y1 - y0) / dx;
	float yi = y0 + slope;

	for (uint16_t x = x0 + 1; x < x1; ++x) {
		//0.1 is necessary to avoid loss of precision during calculation
		if (!g_game.map.isTileClear(std::floor(yi + 0.1), x, z)) {
			return false;
		}
		yi += slope;
	}

	return true;
}

bool checkSlightLine(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint8_t z)
{
	const float dx = x1 - x0;
	const float slope = (dx == 0) ? 1 : (y1 - y0) / dx;
	float yi = y0 + slope;

	for (uint16_t x = x0 + 1; x < x1; ++x) {
		//0.1 is necessary to avoid loss of precision during calculation
		if (!g_game.map.isTileClear(x, std::floor(yi + 0.1), z)) {
			return false;
		}
		yi += slope;
	}

	return true;
}

}

bool Map::checkSightLine(const uint16_t x0, const uint16_t y0, const uint16_t x1, const uint16_t y1, const uint8_t z)
{
	if (x0 == x1 && y0 == y1) {
		return true;
	}

	if (std::abs(y1 - y0) > std::abs(x1 - x0)) {
		if (y1 > y0) {
			return checkSteepLine(y0, x0, y1, x1, z);
		}
		return checkSteepLine(y1, x1, y0, x0, z);
	}

	if (x0 > x1) {
		return checkSlightLine(x1, y1, x0, y0, z);
	}

	return checkSlightLine(x0, y0, x1, y1, z);
}

bool Map::isSightClear(const Position& fromPos, const Position& toPos, const bool sameFloor /*= false*/)
{
	// Return immediately true if the destination is the same as the current location
	if (fromPos == toPos) {
		return true;
	}

	// Cache differences to reduce function calls to position::getDistance
	const auto diffX = std::abs(fromPos.x - toPos.x);
	const auto diffY = std::abs(fromPos.y - toPos.y);
	const auto diffZ = std::abs(fromPos.z - toPos.z);

	// Destination is on a different floor and sameFloor requirement is true
	if (sameFloor && diffZ != 0) {
		return false;
	}

	// Destination is on the same floor
	if (diffZ == 0) {
		// Close proximity check
		if (diffX < 2 && diffY < 2) {
			return true;
		}
		// Check horizontal line of sight
		const bool sightClear = checkSightLine(fromPos.x, fromPos.y, toPos.x, toPos.y, fromPos.z);
		if (sightClear || sameFloor) {
			return sightClear;
		}

		// No obstacles above floor 0 so we can throw above the obstacle
		if (fromPos.z == 0) {
			return true;
		}

		// Check if tiles above us and the target are clear and check for a clear sight between them
		uint8_t newZ = fromPos.z - 1;
		return isTileClear(fromPos.x, fromPos.y, newZ, true) &&
			isTileClear(toPos.x, toPos.y, newZ, true) &&
			checkSightLine(fromPos.x, fromPos.y, toPos.x, toPos.y, newZ);
	}

	// Destination is on a different floor
	if (sameFloor) {
		return false;
	}

	// Skip checks for sight line in case fromPos and toPos cross the ground floor
	if ((fromPos.z < 8 && toPos.z > 7) || (fromPos.z > 7 && toPos.z < 8)) {
		return false;
	}

	// Destination is above us
	if (fromPos.z > toPos.z) {
		if (diffZ > 1) {
			return false;
		}

		// Check a tile above us and the path to the target
		const uint8_t newZ = fromPos.z - 1;
		return isTileClear(fromPos.x, fromPos.y, newZ, true) &&
			checkSightLine(fromPos.x, fromPos.y, toPos.x, toPos.y, newZ);
	}

	// Target is below us, check if tiles above the target are clear
	for (uint8_t z = fromPos.z; z < toPos.z; ++z) {
		if (!isTileClear(toPos.x, toPos.y, z, true)) {
			return false;
		}
	}

	// Check if we can throw to the tile above the target
	return checkSightLine(fromPos.x, fromPos.y, toPos.x, toPos.y, fromPos.z);
}

TilePtr Map::canWalkTo(CreaturePtr& creature, const Position& pos)
{
	const int32_t& walkCache = creature->getWalkCache(pos);
	if (walkCache == 0) {
		return nullptr;
	} else if (walkCache == 1) {
		return getTile(pos.x, pos.y, pos.z);
	}

	//used for non-cached tiles
	const auto& tile = getTile(pos.x, pos.y, pos.z);
	if (creature->getTile() != tile) {
		if (!tile) {
			return nullptr;
		}

		uint32_t flags = FLAG_PATHFINDING;
		if (!creature->getPlayer()) {
			flags |= FLAG_IGNOREFIELDDAMAGE;
		}

		if (tile->queryAdd(creature, flags) != RETURNVALUE_NOERROR) {
			return nullptr;
		}
	}
	return tile;
}

bool Map::getPathMatching(CreaturePtr& creature, std::vector<Direction>& dirList, const FrozenPathingConditionCall& pathCondition, const FindPathParams& fpp)
{
	Position pos = creature->getPosition();
	Position endPos;

	AStarNodes nodes(pos.x, pos.y);

	int32_t bestMatch = 0;

	static int_fast32_t dirNeighbors[8][5][2] = {
		{{-1, 0}, {0, 1}, {1, 0}, {1, 1}, {-1, 1}},
		{{-1, 0}, {0, 1}, {0, -1}, {-1, -1}, {-1, 1}},
		{{-1, 0}, {1, 0}, {0, -1}, {-1, -1}, {1, -1}},
		{{0, 1}, {1, 0}, {0, -1}, {1, -1}, {1, 1}},
		{{1, 0}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}},
		{{-1, 0}, {0, -1}, {-1, -1}, {1, -1}, {-1, 1}},
		{{0, 1}, {1, 0}, {1, -1}, {1, 1}, {-1, 1}},
		{{-1, 0}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}}
	};
	static int_fast32_t allNeighbors[8][2] = {
		{-1, 0}, {0, 1}, {1, 0}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}
	};

	const Position startPos = pos;

	AStarNode* found = nullptr;
	while (fpp.maxSearchDist != 0 || nodes.getClosedNodes() < 100) {
		const auto& n = nodes.getBestNode();
		if (!n) {
			if (found) {
				break;
			}
			return false;
		}

		const int_fast32_t x = n->x;
		const int_fast32_t y = n->y;
		pos.x = x;
		pos.y = y;
		if (pathCondition(startPos, pos, fpp, bestMatch)) {
			found = n;
			endPos = pos;
			if (bestMatch == 0) {
				break;
			}
		}

		uint_fast32_t dirCount;
		int_fast32_t* neighbors;
		if (n->parent) {
			const int_fast32_t offset_x = n->parent->x - x;
			const int_fast32_t offset_y = n->parent->y - y;
			if (offset_y == 0) {
				if (offset_x == -1) {
					neighbors = *dirNeighbors[DIRECTION_WEST];
				} else {
					neighbors = *dirNeighbors[DIRECTION_EAST];
				}
			} else if (!fpp.allowDiagonal || offset_x == 0) {
				if (offset_y == -1) {
					neighbors = *dirNeighbors[DIRECTION_NORTH];
				} else {
					neighbors = *dirNeighbors[DIRECTION_SOUTH];
				}
			} else if (offset_y == -1) {
				if (offset_x == -1) {
					neighbors = *dirNeighbors[DIRECTION_NORTHWEST];
				} else {
					neighbors = *dirNeighbors[DIRECTION_NORTHEAST];
				}
			} else if (offset_x == -1) {
				neighbors = *dirNeighbors[DIRECTION_SOUTHWEST];
			} else {
				neighbors = *dirNeighbors[DIRECTION_SOUTHEAST];
			}
			dirCount = fpp.allowDiagonal ? 5 : 3;
		} else {
			dirCount = 8;
			neighbors = *allNeighbors;
		}

		const int_fast32_t f = n->f;
		for (uint_fast32_t i = 0; i < dirCount; ++i) {
			pos.x = x + *neighbors++;
			pos.y = y + *neighbors++;

			if (fpp.maxSearchDist != 0 && (Position::getDistanceX(startPos, pos) > fpp.maxSearchDist || Position::getDistanceY(startPos, pos) > fpp.maxSearchDist)) {
				continue;
			}

			if (fpp.keepDistance && !pathCondition.isInRange(startPos, pos, fpp)) {
				continue;
			}

			auto neighborNode = nodes.getNodeByPosition(pos.x, pos.y);
			TilePtr tile = neighborNode ? getTile(pos.x, pos.y, pos.z) : canWalkTo(creature, pos);

			if (!tile) 
			{
				continue;
			}
			
			//The cost (g) for this neighbor
			const int_fast32_t cost = AStarNodes::getMapWalkCost(n, pos);
			const int_fast32_t extraCost = AStarNodes::getTileWalkCost(creature, tile);
			const int_fast32_t newf = f + cost + extraCost;

			if (neighborNode) {
				if (neighborNode->f <= newf) {
					//The node on the closed/open list is cheaper than this one
					continue;
				}

				neighborNode->f = newf;
				neighborNode->parent = n;
				nodes.openNode(neighborNode);
			} else {
				//Does not exist in the open/closed list, create a new node
				neighborNode = nodes.createOpenNode(n, pos.x, pos.y, newf);
				if (!neighborNode) {
					if (found) {
						break;
					}
					return false;
				}
			}
		}

		nodes.closeNode(n);
	}

	if (!found) {
		return false;
	}

	int_fast32_t prevx = endPos.x;
	int_fast32_t prevy = endPos.y;

	found = found->parent;
	while (found) {
		pos.x = found->x;
		pos.y = found->y;

		const int_fast32_t dx = pos.getX() - prevx;
		const int_fast32_t dy = pos.getY() - prevy;

		prevx = pos.x;
		prevy = pos.y;

		if (dx == 1 && dy == 1) {
			dirList.push_back(DIRECTION_NORTHWEST);
		} else if (dx == -1 && dy == 1) {
			dirList.push_back(DIRECTION_NORTHEAST);
		} else if (dx == 1 && dy == -1) {
			dirList.push_back(DIRECTION_SOUTHWEST);
		} else if (dx == -1 && dy == -1) {
			dirList.push_back(DIRECTION_SOUTHEAST);
		} else if (dx == 1) {
			dirList.push_back(DIRECTION_WEST);
		} else if (dx == -1) {
			dirList.push_back(DIRECTION_EAST);
		} else if (dy == 1) {
			dirList.push_back(DIRECTION_NORTH);
		} else if (dy == -1) {
			dirList.push_back(DIRECTION_SOUTH);
		}

		found = found->parent;
	}
	return true;
}

// AStarNodes

AStarNodes::AStarNodes(uint32_t x, uint32_t y)
	: nodes(), openNodes()
{
	curNode = 1;
	closedNodes = 0;
	openNodes[0] = true;

	AStarNode& startNode = nodes[0];
	startNode.parent = nullptr;
	startNode.x = x;
	startNode.y = y;
	startNode.f = 0;
	nodeTable[(x << 16) | y] = nodes;
}

AStarNode* AStarNodes::createOpenNode(AStarNode* parent, uint32_t x, uint32_t y, int_fast32_t f)
{
	if (curNode >= MAX_NODES) {
		return nullptr;
	}

	const size_t retNode = curNode++;
	openNodes[retNode] = true;

	AStarNode* node = nodes + retNode;
	nodeTable[(x << 16) | y] = node;
	node->parent = parent;
	node->x = x;
	node->y = y;
	node->f = f;
	return node;
}

AStarNode* AStarNodes::getBestNode()
{
	if (curNode == 0) {
		return nullptr;
	}

	int32_t best_node_f = std::numeric_limits<int32_t>::max();
	int32_t best_node = -1;
	for (size_t i = 0; i < curNode; i++) {
		if (openNodes[i] && nodes[i].f < best_node_f) {
			best_node_f = nodes[i].f;
			best_node = i;
		}
	}

	if (best_node >= 0) {
		return nodes + best_node;
	}
	return nullptr;
}

void AStarNodes::closeNode(const AStarNode* node)
{
	size_t index = node - nodes;
	assert(index < MAX_NODES);
	openNodes[index] = false;
	++closedNodes;
}

void AStarNodes::openNode(const AStarNode* node)
{
	size_t index = node - nodes;
	assert(index < MAX_NODES);
	if (!openNodes[index]) {
		openNodes[index] = true;
		--closedNodes;
	}
}

int_fast32_t AStarNodes::getClosedNodes() const
{
	return closedNodes;
}

AStarNode* AStarNodes::getNodeByPosition(const uint32_t x, const uint32_t y)
{
	auto it = nodeTable.find((x << 16) | y);
	if (it == nodeTable.end()) {
		return nullptr;
	}
	return it->second;
}

int_fast32_t AStarNodes::getMapWalkCost(const AStarNode* node, const Position& neighborPos)
{
	if (std::abs(node->x - neighborPos.x) == std::abs(node->y - neighborPos.y)) {
		//diagonal movement extra cost
		return MAP_DIAGONALWALKCOST;
	}
	return MAP_NORMALWALKCOST;
}

int_fast32_t AStarNodes::getTileWalkCost(const CreaturePtr creature, const TileConstPtr& tile)
{
	int_fast32_t cost = 0;
	if (tile->getTopVisibleCreature(creature) != nullptr) {
		//destroy creature cost
		cost += MAP_NORMALWALKCOST * 3;
	}

	if (const auto& field = tile->getFieldItem()) {
		const CombatType_t combatType = field->getCombatType();
		if (const auto& monster = creature->getMonster(); !creature->isImmune(combatType) && !creature->hasCondition(Combat::DamageToConditionType(combatType)) && (monster && !monster->canWalkOnFieldType(combatType))) {
			cost += MAP_NORMALWALKCOST * 18;
		}
	}
	return cost;
}

// Floor
Floor::~Floor()
{
	for (auto& row : tiles) {
		for (auto& tile : row) {
			tile.reset();
		}
	}
}

// QTreeNode
QTreeNode::~QTreeNode()
{
	for (const auto* ptr : child) {
		delete ptr;
	}
}

QTreeLeafNode* QTreeNode::getLeaf(const uint32_t x, const uint32_t y)
{
	if (leaf) {
		return static_cast<QTreeLeafNode*>(this);
	}

	const auto& node = child[((x & 0x8000) >> 15) | ((y & 0x8000) >> 14)];
	if (!node) {
		return nullptr;
	}
	return node->getLeaf(x << 1, y << 1);
}

QTreeLeafNode* QTreeNode::createLeaf(const uint32_t x, const uint32_t y, const uint32_t level)
{
	if (!isLeaf()) {
		const uint32_t index = ((x & 0x8000) >> 15) | ((y & 0x8000) >> 14);
		if (!child[index]) {
			if (level != FLOOR_BITS) {
				child[index] = new QTreeNode();
			} else {
				child[index] = new QTreeLeafNode();
				QTreeLeafNode::newLeaf = true;
			}
		}
		return child[index]->createLeaf(x * 2, y * 2, level - 1);
	}
	return static_cast<QTreeLeafNode*>(this);
}

// QTreeLeafNode
bool QTreeLeafNode::newLeaf = false;

QTreeLeafNode::~QTreeLeafNode()
{
	for (const auto* ptr : array) {
		delete ptr;
	}
}

Floor* QTreeLeafNode::createFloor(const uint32_t z)
{
	if (!array[z]) {
		array[z] = new Floor();
	}
	return array[z];
}

void QTreeLeafNode::addCreature(const CreaturePtr& c)
{
	creature_list.push_back(c);

	if (c->getPlayer()) {
		player_list.push_back(c);
	}
}

void QTreeLeafNode::removeCreature(const CreaturePtr& c)
{
	auto iter = std::ranges::find(creature_list, c);
	assert(iter != creature_list.end());
	*iter = creature_list.back();
	creature_list.pop_back();

	if (c->getPlayer()) {
		iter = std::ranges::find(player_list, c);
		assert(iter != player_list.end());
		*iter = player_list.back();
		player_list.pop_back();
	}
}

uint32_t Map::clean()
{
	const uint64_t start = OTSYS_TIME();
	size_t tiles = 0;

	if (g_game.getGameState() == GAME_STATE_NORMAL) {
		g_game.setGameState(GAME_STATE_MAINTAIN);
	}

	std::vector<ItemPtr> toRemove;

	for (const auto& tile : g_game.getTilesToClean()) {
		if (!tile) {
			continue;
		}

		if (const auto& items = tile->getItemList()) {
			++tiles;
			for (const auto& item : *items) {
				if (item->isCleanable()) {
					toRemove.emplace_back(item);
				}
			}
		}
	}

	for (const auto& item : toRemove) {
		g_game.internalRemoveItem(item, -1);
	}

	const size_t count = toRemove.size();
	g_game.clearTilesToClean();

	if (g_game.getGameState() == GAME_STATE_MAINTAIN) {
		g_game.setGameState(GAME_STATE_NORMAL);
	}

	std::cout << "> CLEAN: Removed " << count << " item" << (count != 1 ? "s" : "")
		<< " from " << tiles << " tile" << (tiles != 1 ? "s" : "") << " in "
		<< (OTSYS_TIME() - start) / (1000.) << " seconds." << std::endl;
	return count;
}
