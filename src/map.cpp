// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "iomap.h"
#include "iomapserialize.h"
#include "combat.h"
#include "creature.h"
#include "creaturecontainer.h"
#include "game.h"
#include "monster.h"
#include "simd_kernels.h"

extern Game g_game;

namespace
{
    // Shared sentinel so tile lookups can return by const reference without refcount traffic on misses.
    const TilePtr null_tile = nullptr;

    thread_local BlackTek::World::ChunkCoord last_chunk_coord{};
    thread_local const BlackTek::World::Chunk* last_chunk = nullptr;
}

bool Map::loadMap(const std::string& identifier, bool loadHouses)
{
	IOMap loader;
    if (!loader.loadMap(this, identifier, g_game.getRawMapBlock(), g_game.getMapBlock()))
	{
		std::cout << "[Fatal - Map::loadMap] " << loader.getLastErrorString() << std::endl;
		return false;
	}

	if (not IOMap::loadSpawns(this))
	{
		std::cout << "[Warning - Map::loadMap] Failed to load spawn data." << std::endl;
	}

	if (loadHouses)
	{
		if (not IOMap::loadHouses(this))
		{
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

const TilePtr& Map::getTile(const uint16_t x, const uint16_t y, const uint8_t z) const
{
    using namespace BlackTek::World;

    if (z >= MaxLayers)
        return null_tile;

    const ChunkCoord chunkCoord = ToChunkCoord(x, y);
    const Chunk* chunk = last_chunk;

    if (not chunk or chunkCoord != last_chunk_coord)
    {
        const auto chunkHandle = chunk_grid.FindChunk(chunkCoord);
        chunk = chunk_grid.GetChunk(chunkHandle);

        if (chunk)
        {
            last_chunk_coord = chunkCoord;
            last_chunk = chunk;
        }
    }

    if (not chunk)
        return null_tile;

    const auto floorHandle = chunk->floor_handles[z];
    const auto* floor = floor_pool.Get(floorHandle);

    if (not floor)
        return null_tile;

    return floor->tiles[x & Floor::Mask][y & Floor::Mask];
}

const TilePtr& Map::getTileInChunk(const BlackTek::World::Chunk* chunk, const uint16_t x, const uint16_t y, const uint8_t z) const
{
    using namespace BlackTek::World;

    if (not chunk or z >= MaxLayers)
        return null_tile;

    const auto* floor = floor_pool.Get(chunk->floor_handles[z]);
    if (not floor)
        return null_tile;

    return floor->tiles[x & Floor::Mask][y & Floor::Mask];
}

void Map::setTile(const uint16_t x, const uint16_t y, const uint8_t z, TilePtr& newTile)
{
    using namespace BlackTek::World;

	if (z >= MaxLayers)
    {
		std::cout << "ERROR: Attempt to set tile on invalid coordinate " << Position(x, y, z) << "!" << std::endl;
		return;
	}

	const ChunkHandle chunkHandle = chunk_grid.GetOrCreateChunk(ToChunkCoord(x, y));
	Chunk* chunk = chunk_grid.GetChunk(chunkHandle);
	FloorHandle floorHandle = chunk->floor_handles[z];

	if (not floorHandle.IsValid()) 
    {
		floorHandle = floor_pool.Allocate();
		chunk->floor_handles[z] = floorHandle;
	}

	Floor* floor = floor_pool.Get(floorHandle);
	const uint32_t offsetX = x & Floor::Mask;
	const uint32_t offsetY = y & Floor::Mask;

	if (auto& tile = floor->tiles[offsetX][offsetY]) 
    {
		if (const auto& items = newTile->getItemList()) 
        {
			for (auto it = items->rbegin(), end = items->rend(); it != end; ++it)
				tile->addItem(*it);

			items->clear();
		}

		if (const auto& ground = newTile->getGround())
        {
			tile->addItem(ground);
			newTile->setGround(nullptr);
		}
	}
    else
    {
		newTile->setOwningChunk(chunkHandle);
		tile = newTile;

		chunk->SetTileBlockState(offsetX, offsetY, z,
			newTile->hasFlag(TILESTATE_BLOCKPATH), newTile->hasFlag(TILESTATE_BLOCKSOLID), newTile->hasProperty(CONST_PROP_BLOCKPROJECTILE));
	}
}

void Map::removeTile(const uint16_t x, const uint16_t y, const uint8_t z) const
{
    using namespace BlackTek::World;

	if (z >= MaxLayers)
		return;

	const auto chunkHandle = chunk_grid.FindChunk(ToChunkCoord(x, y));
	const auto* chunk = chunk_grid.GetChunk(chunkHandle);

	if (not chunk)
		return;

	const auto floorHandle = chunk->floor_handles[z];
	const auto* floor = floor_pool.Get(floorHandle);

	if (not floor)
		return;

	if (const auto& tile = floor->tiles[x & Floor::Mask][y & Floor::Mask])
    {
		if (const auto& creatures = tile->getCreatures())
        {
			for (int32_t i = creatures->size(); --i >= 0;)
            {
				if (const auto& player = creatures->getList()[i]->getPlayer())
					g_game.internalTeleport(player, player->getTown()->getTemplePosition(), false, FLAG_NOLIMIT);
				else 
					g_game.removeCreature(creatures->getList()[i]);
			}
		}

		if (const auto& items = tile->getItemList()) 
        {
			for (auto it = items->begin(), end = items->end(); it != end; ++it)
				g_game.internalRemoveItem(*it);
		}

		if (const auto& ground = tile->getGround())
        {
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
        ReturnValue ret = RETURNVALUE_NOERROR;
        if (creature->getCreatureSubType() == CreatureSubType::Player)
        {
            ret = tile->canEnter(std::static_pointer_cast<Player>(creature), FLAG_IGNOREBLOCKITEM);
        }
        else if (creature->getCreatureSubType() == CreatureSubType::Monster)
        {
            ret = tile->canEnter(std::static_pointer_cast<Monster>(creature), FLAG_IGNOREBLOCKITEM);
        }
        else if (creature->getCreatureSubType() == CreatureSubType::Npc)
        {
            ret = tile->canEnter(std::static_pointer_cast<Npc>(creature), FLAG_IGNOREBLOCKITEM);
        }
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

            ReturnValue ret = RETURNVALUE_NOERROR;
            if (creature->getCreatureSubType() == CreatureSubType::Player)
            {
                ret = tile->canEnter(std::static_pointer_cast<Player>(creature), 0);
            }
            else if (creature->getCreatureSubType() == CreatureSubType::Monster)
            {
                ret = tile->canEnter(std::static_pointer_cast<Monster>(creature), 0);
            }
            else if (creature->getCreatureSubType() == CreatureSubType::Npc)
            {
                ret = tile->canEnter(std::static_pointer_cast<Npc>(creature), 0);
            }

			if (ret == RETURNVALUE_NOERROR) {
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

	uint32_t flags = 0;
	TilePtr destTile = tile->resolveCreatureDestination(creature, flags);
	destTile->ensureCreatures().addCreatureSilently(creature);

	const Position& dest = destTile->getPosition();
    if (auto* chunk = getChunk(dest.x, dest.y))
        chunk->AddCreature(creature);
    else
        BlackTek::Console::DebugLog("Creature placed on non-existent tile, and has escaped spectator tracking as a result!");

	return true;
}

void Map::moveCreature(CreaturePtr& creature, const TilePtr& newTile, bool forceTeleport/* = false*/)
{
	const auto& oldTile = creature->getTile();

	if (not oldTile->hasCreature(creature))
		return;

	Position oldPos = oldTile->getPosition();
	Position newPos = newTile->getPosition();

	bool teleport = forceTeleport or not newTile->getGround() or not Position::areInRange<1, 1, 0>(oldPos, newPos);

	SpectatorVec spectators;
	if (not teleport)
    {
		getSpectators(spectators, oldPos, true, false, maxViewportX + 1, maxViewportX + 1, maxViewportY + 1, maxViewportY + 1);
	}
    else
    {
		SpectatorVec newPosSpectators;
		getSpectators(spectators, oldPos, true);
		getSpectators(newPosSpectators, newPos, true);
		spectators.addSpectators(newPosSpectators);
	}

	const Tile::UniformStackIndex oldStackIndex = oldTile->getUniformClientIndexOfCreature(creature);

	thread_local std::vector<int32_t> oldStackPosVector;
	oldStackPosVector.clear();

	for (const auto& c : spectators.players())
    {
		const auto& tmpPlayer = std::static_pointer_cast<Player>(c);

		if (not tmpPlayer->canSeeCreature(creature))
		{
			oldStackPosVector.push_back(-1);
			continue;
		}

		oldStackPosVector.push_back(oldStackIndex.uniform ? oldStackIndex.index : oldTile->getClientIndexOfCreature(tmpPlayer, creature));
	}

	if (const auto creatures = oldTile->getCreatures())
	{
		creatures->removeCreature(creature);
	}

	newTile->ensureCreatures().addCreature(creature);

	using BlackTek::World::Chunk;
	using BlackTek::World::ChunkHandle;

	const ChunkHandle oldChunkHandle = oldTile->getOwningChunk();
	const ChunkHandle newChunkHandle = newTile->getOwningChunk();

	if (oldChunkHandle == newChunkHandle)
    {
		if (Chunk* chunk = getChunk(newChunkHandle)) 
			chunk->UpdateCreaturePosition(creature, newPos);
	} 
    else
    {
		if (Chunk* oldChunk = getChunk(oldChunkHandle))
			oldChunk->RemoveCreature(creature);

		if (Chunk* newChunk = getChunk(newChunkHandle)) 
			newChunk->AddCreature(creature);

        else 
            BlackTek::Console::DebugLog("Creature placed on non-existent tile, and has escaped spectator tracking as a result!");
	}

	if (not teleport)
    {
		if (oldPos.y > newPos.y)
			creature->setDirection(DIRECTION_NORTH);

        else if (oldPos.y < newPos.y)
			creature->setDirection(DIRECTION_SOUTH);

		if (oldPos.x < newPos.x)
			creature->setDirection(DIRECTION_EAST);

		else if (oldPos.x > newPos.x)
			creature->setDirection(DIRECTION_WEST);
	}

	const Tile::UniformStackIndex newStackIndex = newTile->getUniformClientIndexOfCreature(creature);

	size_t i = 0;
	for (const auto& c : spectators.players())
    {
		const auto& tmpPlayer = std::static_pointer_cast<Player>(c);

		if (const int32_t& stackpos = oldStackPosVector[i++]; stackpos != -1)
		{
			const int32_t newStackpos = newStackIndex.uniform ? newStackIndex.index : newTile->getClientIndexOfCreature(tmpPlayer, creature);
			tmpPlayer->sendCreatureMove(creature, newPos, newStackpos, oldPos, stackpos, teleport);
		}
	}

    if (creature->getCreatureSubType() == CreatureSubType::Player or (creature->getCreatureSubType() == CreatureSubType::Monster and creature->getMaster() and creature->getMaster()->getCreatureSubType() == CreatureSubType::Player))
    {
        for (const auto& c : spectators.monsters())
            static_cast<Monster*>(c.get())->setIdle(false);
    }

	const std::span<const CreaturePtr> spectators_span(spectators.begin(), spectators.size());

	for (const auto& spectator : spectators)
	{
		switch (spectator->getCreatureSubType())
		{
			case CreatureSubType::Player:
				static_cast<Player*>(spectator.get())->onCreatureMove(creature, newTile, newPos, oldTile, oldPos, teleport);
				break;
			case CreatureSubType::Monster:
				static_cast<Monster*>(spectator.get())->onCreatureMove(creature, newTile, newPos, oldTile, oldPos, teleport, spectators_span);
				break;
			case CreatureSubType::Npc:
				static_cast<Npc*>(spectator.get())->onCreatureMove(creature, newTile, newPos, oldTile, oldPos, teleport);
				break;
			default:
				break;
		}
	}

	oldTile->notifyCreatureRemoved(creature, newTile, spectators_span);
	newTile->notifyCreatureAdded(creature, oldTile, spectators_span);
}

void Map::getSpectatorsInternal(SpectatorVec& spectators, const Position& centerPos, const int32_t minRangeX, const int32_t maxRangeX, const int32_t minRangeY, const int32_t maxRangeY, const int32_t minRangeZ, const int32_t maxRangeZ, const bool onlyPlayers) const
{
    using namespace BlackTek::World;
    using namespace BlackTek::SIMD;

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

    const int32_t base_yMin = min_y + minoffset;
    const int32_t base_yMax = max_y + maxoffset;
    const int32_t base_xMin = min_x + minoffset;
    const int32_t base_xMax = max_x + maxoffset;

    struct ZBounds { int32_t yMin, yMax, xMin, xMax; };
    ZBounds zbounds[MaxLayers];

    for (int32_t z = 0; z < MaxLayers; ++z)
    {
        if (z < minRangeZ or z > maxRangeZ)
        {
            zbounds[z] = { 1, 0, 1, 0 };
            continue;
        }
        const int32_t off = static_cast<int32_t>(centerPos.getZ()) - z;
        zbounds[z] = { base_yMin + off, base_yMax + off, base_xMin + off, base_xMax + off };
    }

    const int32_t startx1 = x1 - (x1 % Floor::Size);
    const int32_t starty1 = y1 - (y1 % Floor::Size);
    const int32_t endx2 = x2 - (x2 % Floor::Size);
    const int32_t endy2 = y2 - (y2 % Floor::Size);

    const int32_t startChunkX = startx1 >> Floor::Bits;
    const int32_t startChunkY = starty1 >> Floor::Bits;
    const int32_t endChunkX = endx2 >> Floor::Bits;
    const int32_t endChunkY = endy2 >> Floor::Bits;

    ChunkHandle rowStartHandle = chunk_grid.FindChunk({ startChunkX, startChunkY });

    thread_local std::vector<uint8_t> spectatorMaskScratch;

    const auto clampToCoord = [](int32_t value) noexcept -> MapCoord
    {
        return static_cast<MapCoord>(std::clamp(value, 0, 0xFFFF));
    };

    for (int32_t chunkY = startChunkY; chunkY <= endChunkY; ++chunkY)
    {
        ChunkHandle current = rowStartHandle;
        for (int32_t chunkX = startChunkX; chunkX <= endChunkX; ++chunkX)
        {
            if (const Chunk* chunk = chunk_grid.GetChunk(current))
            {
                const size_t creatureCount = onlyPlayers ? chunk->player_count : chunk->creature_ptrs.size();
                const bool sameFloor = creatureCount > 0 and chunk->AllCreaturesSameFloor();
                const uint8_t firstZ = sameFloor ? chunk->creature_z[0] : 0;

                if (sameFloor and creatureCount > 0)
                {
                    if (const auto& b = zbounds[firstZ];
                        b.yMin <= b.yMax and b.xMin <= b.xMax and
                        b.xMax >= 0 and b.xMin <= 0xFFFF and
                        b.yMax >= 0 and b.yMin <= 0xFFFF)
                    {
                        spectatorMaskScratch.resize(creatureCount);
                        compute_spectator_mask(chunk->creature_x.data(), chunk->creature_y.data(), creatureCount, clampToCoord(b.xMin), clampToCoord(b.xMax),clampToCoord(b.yMin), clampToCoord(b.yMax), spectatorMaskScratch.data());

                        for (size_t i = 0; i < creatureCount; ++i)
                        {
                            if (spectatorMaskScratch[i])
                                spectators.emplace_back(chunk->creature_ptrs[i]);
                        }
                    }
                }
                else
                {
                    for (size_t i = 0; i < creatureCount; ++i)
                    {
                        const uint16_t cx = chunk->creature_x[i];
                        const uint16_t cy = chunk->creature_y[i];
                        const uint8_t cz = chunk->creature_z[i];
                        const auto& b = zbounds[cz];

                        if (b.yMin > cy or b.yMax < cy or b.xMin > cx or b.xMax < cx)
                            continue;

                        spectators.emplace_back(chunk->creature_ptrs[i]);
                    }
                }

                current = chunk->east_neighbor;
            }
            else
            {
                current = chunk_grid.FindChunk({ chunkX + 1, chunkY });
            }
        }

        if (const Chunk* rowStartChunk = chunk_grid.GetChunk(rowStartHandle))
            rowStartHandle = rowStartChunk->south_neighbor;

        else
            rowStartHandle = chunk_grid.FindChunk({ startChunkX, chunkY + 1 });
    }

    if (onlyPlayers)
        spectators.setPlayersOnlyMode();

    else
        spectators.partitionByType();
}

void Map::getSpectators(SpectatorVec& spectators, const Position& centerPos, const bool multifloor /*= false*/, const bool onlyPlayers /*= false*/, int32_t minRangeX /*= 0*/, int32_t maxRangeX /*= 0*/, int32_t minRangeY /*= 0*/, int32_t maxRangeY /*= 0*/)
{
    if (centerPos.z >= BlackTek::World::MaxLayers)
    {
        return;
    }

    minRangeX = (minRangeX == 0 ? -maxViewportX : -minRangeX);
    maxRangeX = (maxRangeX == 0 ? maxViewportX : maxRangeX);
    minRangeY = (minRangeY == 0 ? -maxViewportY : -minRangeY);
    maxRangeY = (maxRangeY == 0 ? maxViewportY : maxRangeY);

    int32_t minRangeZ;
    int32_t maxRangeZ;

    if (multifloor)
    {
        if (centerPos.z > 7)
        {
            // underground (8->15)
            minRangeZ = std::max<int32_t>(centerPos.getZ() - 2, 0);
            maxRangeZ = std::min<int32_t>(centerPos.getZ() + 2, BlackTek::World::MaxLayers - 1);
        }
        else if (centerPos.z == 6)
        {
            minRangeZ = 0;
            maxRangeZ = 8;
        }
        else if (centerPos.z == 7)
        {
            minRangeZ = 0;
            maxRangeZ = 9;
        }
        else
        {
            minRangeZ = 0;
            maxRangeZ = 7;
        }
    }
    else
    {
        minRangeZ = centerPos.z;
        maxRangeZ = centerPos.z;
    }

    getSpectatorsInternal(spectators, centerPos, minRangeX, maxRangeX, minRangeY, maxRangeY, minRangeZ, maxRangeZ, onlyPlayers);
}

// Todo: Handroll this implementation out, building custom constructors for the spectators if we must, which will allow us to utilize the 
// reserve for which the underlying vector has, in a way that we can keep our work at bare minimal during creation while also passing
// all the criteria for building the vector with the data already in it at creation, rather than create then pass and build like getspectators currently does
SpectatorVec Map::fetchSpectators(const Position& centerPos, const bool multifloor, const bool onlyPlayers, int32_t minRangeX, int32_t maxRangeX, int32_t minRangeY, int32_t maxRangeY)
{
    SpectatorVec spectators;
    getSpectators(spectators, centerPos, multifloor, onlyPlayers, minRangeX, maxRangeX, minRangeY, maxRangeY);
    return spectators;
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
    using namespace BlackTek::World;

    if (not blockFloor and z < MaxLayers)
    {
        const ChunkCoord chunkCoord = ToChunkCoord(x, y);
        const Chunk* chunk = last_chunk;

        if (not chunk or chunkCoord != last_chunk_coord)
        {
            const auto chunkHandle = chunk_grid.FindChunk(chunkCoord);
            chunk = chunk_grid.GetChunk(chunkHandle);

            if (chunk)
            {
                last_chunk_coord = chunkCoord;
                last_chunk = chunk;
            }
        }

        if (not chunk)
            return true;

        const uint32_t bitIndex = (y & Floor::Mask) * Floor::Size + (x & Floor::Mask);
        return not ((chunk->block_projectile_mask[z] >> bitIndex) & 1u);
    }

    const auto& tile = getTile(x, y, z);
    if (not tile)
    {
        return true;
    }

    if (blockFloor and tile->getGround())
    {
        return false;
    }

    return not tile->hasProperty(CONST_PROP_BLOCKPROJECTILE);
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

const TilePtr& Map::canWalkTo(CreaturePtr& creature, const Position& pos)
{
    int32_t walkCache = 2;
    if (creature->getCreatureSubType() == CreatureSubType::Monster)
    {
        walkCache = std::static_pointer_cast<Monster>(creature)->getWalkCache(pos);
    }

    if (walkCache == 0)
    {
        return null_tile;
    }

    if (walkCache == 1)
    {
        return getTile(pos.x, pos.y, pos.z);
    }

    // used for non-cached tiles
    const auto& tile = getTile(pos.x, pos.y, pos.z);
    if (creature->getTile() != tile)
    {
        if (not tile)
        {
            return null_tile;
        }

        uint32_t flags = FLAG_PATHFINDING;
        const auto subtype = creature->getCreatureSubType();
        if (subtype != CreatureSubType::Player)
        {
            flags |= FLAG_IGNOREFIELDDAMAGE;
        }

        ReturnValue ret = RETURNVALUE_NOERROR;
        if (subtype == CreatureSubType::Player)
        {
            ret = tile->canEnter(std::static_pointer_cast<Player>(creature), flags);
        }
        else if (subtype == CreatureSubType::Monster)
        {
            ret = tile->canEnter(std::static_pointer_cast<Monster>(creature), flags);
        }
        else if (subtype == CreatureSubType::Npc)
        {
            ret = tile->canEnter(std::static_pointer_cast<Npc>(creature), flags);
        }

        if (ret != RETURNVALUE_NOERROR)
        {
            return null_tile;
        }
    }
    return tile;
}

namespace
{
    static constexpr uint32_t ASTAR_HASH_BITS = 10u;
    static constexpr uint32_t ASTAR_HASH_SIZE = (1u << ASTAR_HASH_BITS);  // 1024
    static constexpr uint32_t ASTAR_HASH_MASK = ASTAR_HASH_SIZE - 1u;
    static constexpr uint32_t FIB_MULT = 2654435761u;
    static constexpr uint16_t ASTAR_INVALID = 0xFFFFu;

    struct AStarWorkspace
    {
        AStarNode nodes[MAX_NODES];
        uint16_t heap[MAX_NODES];
        uint16_t node_to_heap[MAX_NODES];
        uint32_t h_table_keys[ASTAR_HASH_SIZE];
        uint16_t h_table_values[ASTAR_HASH_SIZE];
        uint16_t used_nodes[MAX_NODES];
        uint16_t used_count = 0;
        uint16_t previous_node = 0;

        AStarWorkspace()
        {
            std::memset(h_table_values, 0xFF, sizeof(h_table_values));
            std::memset(node_to_heap, 0xFF, sizeof(node_to_heap));
        }
    };

    thread_local AStarWorkspace threaded_workspace;
}

bool Map::getPathMatching(CreaturePtr& creature, std::vector<Direction>& dirList, const FrozenPathingConditionCall& pathCondition, const FindPathParams& fpp)
{
    Position end_position;
    auto position = creature->getPosition();
    auto nodes = AStarNodes(position.x, position.y);
    const auto& target_position = pathCondition.getTargetPos();
    const auto manhattan_heuristic = [&](const int_fast32_t nx, const int_fast32_t ny) -> int_fast32_t
    {
        return (std::abs(nx - static_cast<int_fast32_t>(target_position.x)) +
                std::abs(ny - static_cast<int_fast32_t>(target_position.y))) * MAP_NORMALWALKCOST;
    };

    int32_t best_match = 0;

    static int_fast32_t dirNeighbors[8][5][2] = 
    {
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

    const Position start_position = position;

    AStarNode* found = nullptr;
    while (fpp.maxSearchDist != 0 or nodes.GetClosedNodes() < 100)
    {
        auto* node = nodes.GetBestNode();
        if (not node)
        {
            if (found)
            {
                break;
            }
            return false;
        }

        const int_fast32_t x = node->x;
        const int_fast32_t y = node->y;
        position.x = x;
        position.y = y;

        if (pathCondition(start_position, position, fpp, best_match))
        {
            found = node;
            end_position = position;
            if (best_match == 0)
            {
                break;
            }
        }

        uint_fast32_t direction_count;
        int_fast32_t* neighbors;

        if (node->parent)
        {
            const int_fast32_t x_offset = node->parent->x - x;
            const int_fast32_t y_offset = node->parent->y - y;

            if (y_offset == 0)
            {
                neighbors = (x_offset == -1) ? *dirNeighbors[DIRECTION_WEST]
                                            : *dirNeighbors[DIRECTION_EAST];
            }
            else if (not fpp.allowDiagonal or x_offset == 0)
            {
                neighbors = (y_offset == -1) ? *dirNeighbors[DIRECTION_NORTH]
                                            : *dirNeighbors[DIRECTION_SOUTH];
            }
            else if (y_offset == -1)
            {
                neighbors = (x_offset == -1) ? *dirNeighbors[DIRECTION_NORTHWEST]
                                            : *dirNeighbors[DIRECTION_NORTHEAST];
            }
            else
            {
                neighbors = (x_offset == -1) ? *dirNeighbors[DIRECTION_SOUTHWEST]
                                            : *dirNeighbors[DIRECTION_SOUTHEAST];
            }

            direction_count = fpp.allowDiagonal ? 5 : 3;
        }
        else
        {
            direction_count = 8;
            neighbors = *allNeighbors;
        }

        const int_fast32_t parent_g_score = node->g_score;

        for (uint_fast32_t i = 0; i < direction_count; ++i)
        {
            position.x = x + *neighbors++;
            position.y = y + *neighbors++;

            if (fpp.maxSearchDist != 0
                and (Position::getDistanceX(start_position, position) > fpp.maxSearchDist
                or Position::getDistanceY(start_position, position) > fpp.maxSearchDist))
            {
                continue;
            }

            if (fpp.keepDistance and not pathCondition.isInRange(start_position, position, fpp))
            {
                continue;
            }

            auto* neighbor_node = nodes.GetNodeByPosition(position.x, position.y);
            const TilePtr& tile = neighbor_node ? getTile(position.x, position.y, position.z) : canWalkTo(creature, position);

            if (not tile)
            {
                continue;
            }

            const int_fast32_t cost = AStarNodes::GetMapWalkCost(node, position);
            const int_fast32_t extra_cost = AStarNodes::GetTileWalkCost(creature, tile);
            const int_fast32_t neighbor_g_score = parent_g_score + cost + extra_cost;
            const int_fast32_t neighbor_h_score = manhattan_heuristic(position.x, position.y);
            const int_fast32_t neighbor_f_score = neighbor_g_score + neighbor_h_score;

            if (neighbor_node)
            {
                if (neighbor_node->f <= neighbor_f_score)
                {
                    // Existing path is at least as cheap so lets skip it
                    continue;
                }
                neighbor_node->f = neighbor_f_score;
                neighbor_node->g_score = neighbor_g_score;
                neighbor_node->parent = node;
                // Sifts the node up in the min-heap
                nodes.OpenNode(neighbor_node);
            }
            else
            {
                neighbor_node = nodes.CreateOpenNode(node, position.x, position.y, neighbor_f_score, neighbor_g_score);
                if (not neighbor_node)
                {
                    if (found)
                    {
                        break;
                    }
                    return false;
                }
            }
        }

        nodes.CloseNode(node);
    }

    if (not found)
    {
        return false;
    }

    int_fast32_t prevx = end_position.x;
    int_fast32_t prevy = end_position.y;
    found = found->parent;

    while (found)
    {
        position.x = found->x;
        position.y = found->y;

        const int_fast32_t dx = position.getX() - prevx;
        const int_fast32_t dy = position.getY() - prevy;

        prevx = position.x;
        prevy = position.y;

        if (dx == 1 and dy == 1)
        {
            dirList.push_back(DIRECTION_NORTHWEST);
        }
        else if (dx == -1 and dy == 1)
        {
            dirList.push_back(DIRECTION_NORTHEAST);
        }
        else if (dx == 1 and dy == -1)
        {
            dirList.push_back(DIRECTION_SOUTHWEST);
        }
        else if (dx == -1 and dy == -1)
        {
            dirList.push_back(DIRECTION_SOUTHEAST);
        }
        else if (dx == 1)
        {
            dirList.push_back(DIRECTION_WEST);
        }
        else if (dx == -1)
        {
            dirList.push_back(DIRECTION_EAST);
        }
        else if (dy == 1)
        {
            dirList.push_back(DIRECTION_NORTH);
        }
        else if (dy == -1)
        {
            dirList.push_back(DIRECTION_SOUTH);
        }

        found = found->parent;
    }
    return true;
}

AStarNodes::AStarNodes(uint32_t x, uint32_t y) : heap_size(1), current_node(1), closed_nodes(0)
{
    auto& workspace = threaded_workspace;

    // We only clear slots touched by the previous call to avoid memsetting the workspace everytime
    for (uint16_t i = 0; i < workspace.used_count; ++i)
    {
        workspace.h_table_values[workspace.used_nodes[i]] = ASTAR_INVALID;
    }
    for (uint16_t i = 0; i < workspace.previous_node; ++i)
    {
        workspace.node_to_heap[i] = ASTAR_INVALID;
    }

    workspace.used_count = 0;
    auto& start = workspace.nodes[0];
    start.parent  = nullptr;
    start.x = static_cast<uint16_t>(x);
    start.y = static_cast<uint16_t>(y);
    start.f = 0;
    start.g_score = 0;
    workspace.heap[0] = 0;
    workspace.node_to_heap[0] = 0;
    Insert((x << 16) | y, 0);
}

AStarNodes::~AStarNodes()
{
    // We record how many nodes were used so the next call can reset cheaply.
    threaded_workspace.previous_node = current_node;
}

void AStarNodes::SiftUp(uint16_t pos)
{
    auto& workspace = threaded_workspace;
    const uint16_t node_index = workspace.heap[pos];
    const int_fast32_t f = workspace.nodes[node_index].f;

    while (pos > 0)
    {
        const uint16_t parentPos = (pos - 1u) / 2u;
        if (workspace.nodes[workspace.heap[parentPos]].f <= f)
        {
            break;
        }
        workspace.heap[pos] = workspace.heap[parentPos];
        workspace.node_to_heap[workspace.heap[pos]] = pos;
        pos = parentPos;
    }

    workspace.heap[pos] = node_index;
    workspace.node_to_heap[node_index] = pos;
}

// We return the final position of the element, so the caller can decide
// whether a siftUp is also needed (element didn't move downward).
uint16_t AStarNodes::SiftDown(uint16_t pos)
{
    auto& workspace = threaded_workspace;
    const uint16_t node_index = workspace.heap[pos];
    const int_fast32_t f = workspace.nodes[node_index].f;

    while (true)
    {
        uint16_t child = 2u * pos + 1u;

        if (child >= heap_size)
        {
            break;
        }

        if (child + 1u < heap_size and
            workspace.nodes[workspace.heap[child + 1u]].f < workspace.nodes[workspace.heap[child]].f)
        {
            ++child;
        }
        if (workspace.nodes[workspace.heap[child]].f >= f)
        {
            break;
        }
        workspace.heap[pos] = workspace.heap[child];
        workspace.node_to_heap[workspace.heap[pos]] = pos;
        pos = child;
    }

    workspace.heap[pos] = node_index;
    workspace.node_to_heap[node_index] = pos;
    return pos;
}

void AStarNodes::Insert(uint32_t key, uint16_t node_index)
{
    auto& workspace = threaded_workspace;
    uint32_t slot = (key * FIB_MULT) >> (32u - ASTAR_HASH_BITS);

    while (workspace.h_table_values[slot] != ASTAR_INVALID)
    {
        slot = (slot + 1u) & ASTAR_HASH_MASK;
    }

    workspace.h_table_keys[slot] = key;
    workspace.h_table_values[slot] = node_index;
    // Track which slots are occupied so they can be cleared cheaply next call.
    workspace.used_nodes[workspace.used_count++] = static_cast<uint16_t>(slot);
}

uint16_t AStarNodes::Find(uint32_t key) const
{
    const auto& workspace = threaded_workspace;
    uint32_t slot = (key * FIB_MULT) >> (32u - ASTAR_HASH_BITS);

    while (workspace.h_table_values[slot] != ASTAR_INVALID)
    {
        if (workspace.h_table_keys[slot] == key)
        {
            return workspace.h_table_values[slot];
        }
        slot = (slot + 1u) & ASTAR_HASH_MASK;
    }
    return ASTAR_INVALID;
}

AStarNode* AStarNodes::CreateOpenNode(AStarNode* parent, uint32_t x, uint32_t y, int_fast32_t f, int_fast32_t g_score)
{
    if (current_node >= MAX_NODES)
    {
        return nullptr;
    }

    auto& workspace = threaded_workspace;
    const uint16_t node_index = current_node++;
    auto& node = workspace.nodes[node_index];
    node.parent  = parent;
    node.x = static_cast<uint16_t>(x);
    node.y = static_cast<uint16_t>(y);
    node.f = f;
    node.g_score = g_score;
    Insert((x << 16) | y, node_index);
    workspace.heap[heap_size] = node_index;
    workspace.node_to_heap[node_index] = heap_size;
    ++heap_size;
    SiftUp(heap_size - 1u);
    return &node;
}

// This should now be O(1) since the best node should always be at the root of the heap.
AStarNode* AStarNodes::GetBestNode()
{
    if (heap_size == 0)
    {
        return nullptr;
    }
    auto& workspace = threaded_workspace;
    return &workspace.nodes[workspace.heap[0]];
}

void AStarNodes::CloseNode(const AStarNode* node)
{
    auto& workspace = threaded_workspace;
    const uint16_t node_index = static_cast<uint16_t>(node - workspace.nodes);
    const uint16_t position = workspace.node_to_heap[node_index];

    assert(position != ASTAR_INVALID);

    workspace.node_to_heap[node_index] = ASTAR_INVALID;
    ++closed_nodes;
    --heap_size;

    if (position == heap_size)
    {
        return;
    }

    // Replace the removed slot with the current last element and reheapify.
    const uint16_t last_index = workspace.heap[heap_size];
    workspace.heap[position] = last_index;
    workspace.node_to_heap[last_index] = position;
    const uint16_t new_position = SiftDown(position);

    if (new_position == position)
    {
        SiftUp(position);
    }
}

void AStarNodes::OpenNode(AStarNode* node)
{
    auto& workspace = threaded_workspace;
    const uint16_t node_index = static_cast<uint16_t>(node - workspace.nodes);

    if (workspace.node_to_heap[node_index] == ASTAR_INVALID)
    {
        workspace.heap[heap_size] = node_index;
        workspace.node_to_heap[node_index] = heap_size;
        ++heap_size;
        SiftUp(heap_size - 1u);
        --closed_nodes;
    }
    else
    {
        // The node is already open and its f decreased so we sift up to restore order
        SiftUp(workspace.node_to_heap[node_index]);
    }
}

int_fast32_t AStarNodes::GetClosedNodes() const
{
    return closed_nodes;
}

AStarNode* AStarNodes::GetNodeByPosition(uint32_t x, uint32_t y)
{
    const uint16_t index = Find((x << 16) | y);
    if (index == ASTAR_INVALID)
    {
        return nullptr;
    }
    return &threaded_workspace.nodes[index];
}

int_fast32_t AStarNodes::GetMapWalkCost(const AStarNode* node, const Position& neighborPos)
{
    if (std::abs(node->x - neighborPos.x) == std::abs(node->y - neighborPos.y))
    {
        return MAP_DIAGONALWALKCOST;
    }
    return MAP_NORMALWALKCOST;
}

int_fast32_t AStarNodes::GetTileWalkCost(const CreaturePtr& creature, const TileConstPtr& tile)
{
    int_fast32_t cost = 0;

    if (tile->getTopVisibleCreature(creature) != nullptr)
    {
        cost += MAP_NORMALWALKCOST * 3;
    }

    if (const auto& field = tile->getFieldItem())
    {
        const CombatType_t combatType = field->getCombatType();
        if (const auto& monster = creature->getMonster();
            not creature->isImmune(combatType)
            and not creature->hasCondition(BlackTek::Combat::DamageToConditionType(combatType))
            and (monster and not monster->canWalkOnFieldType(combatType)))
        {
            cost += MAP_NORMALWALKCOST * 18;
        }
    }

    return cost;
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
