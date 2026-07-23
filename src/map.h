// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_MAP_H
#define FS_MAP_H

#include "position.h"
#include "item.h"
#include "fileloader.h"

#include "tools.h"
#include "tile.h"
#include "town.h"
#include "house.h"
#include "spawn.h"

#include "chunk.h"
#include "chunkgrid.h"
#include "floorpool.h"

class Creature;
class Player;
class Game;
class Tile;
class Map;

struct FindPathParams;
struct AStarNode {
	AStarNode* parent;
	int_fast32_t f;        // f = g_score + h_score, used for heap ordering
	int_fast32_t g_score;  // actual accumulated cost from start to this node
	uint16_t x, y;
};

static constexpr int32_t MAX_NODES = 512;

static constexpr int32_t MAP_NORMALWALKCOST = 10;
static constexpr int32_t MAP_DIAGONALWALKCOST = 25;

class AStarNodes
{
    public:
        AStarNodes(uint32_t x, uint32_t y);
        ~AStarNodes();

        // non-copyable
        AStarNodes(const AStarNodes&) = delete;
        AStarNodes& operator=(const AStarNodes&) = delete;

        AStarNode* CreateOpenNode(AStarNode* parent, uint32_t x, uint32_t y, int_fast32_t f, int_fast32_t g_score);
        AStarNode* GetBestNode();
        void CloseNode(const AStarNode* node);
        void OpenNode(AStarNode* node);
        int_fast32_t GetClosedNodes() const;
        AStarNode* GetNodeByPosition(uint32_t x, uint32_t y);

        static int_fast32_t GetMapWalkCost(const AStarNode* node, const Position& neighborPos);
        static int_fast32_t GetTileWalkCost(const CreaturePtr& creature, const TileConstPtr& tile);

    private:
        void SiftUp(uint16_t pos);
        uint16_t SiftDown(uint16_t pos);
        void Insert(uint32_t key, uint16_t nodeIdx);
        uint16_t Find(uint32_t key) const;

        uint16_t heap_size;
        uint16_t current_node;
        int_fast32_t closed_nodes;
};

class FrozenPathingConditionCall;

class Map
{
	public:
		static constexpr int32_t maxViewportX = 11; //min value: maxClientViewportX + 1
		static constexpr int32_t maxViewportY = 11; //min value: maxClientViewportY + 1
		static constexpr int32_t maxClientViewportX = 8;
		static constexpr int32_t maxClientViewportY = 6;

		static uint32_t clean();
		bool loadMap(const std::string& identifier, bool loadHouses);
		static bool save();
		const TilePtr& getTile(uint16_t x, uint16_t y, uint8_t z) const;
		const TilePtr& getTile(const Position& pos) const
		{
			return getTile(pos.x, pos.y, pos.z);
		}

		const TilePtr& getTileInChunk(const BlackTek::World::Chunk* chunk, uint16_t x, uint16_t y, uint8_t z) const;

		void setTile(uint16_t x, uint16_t y, uint8_t z, TilePtr& newTile);
		void setTile(const Position& pos, TilePtr& newTile) { setTile(pos.x, pos.y, pos.z, newTile); }
		void removeTile(uint16_t x, uint16_t y, uint8_t z) const;
		void removeTile(const Position& pos) const { removeTile(pos.x, pos.y, pos.z); }


		bool placeCreature(const Position& centerPos, CreaturePtr creature, bool extendedPos = false, bool forceLogin = false);
		void moveCreature(CreaturePtr& creature, const TilePtr& newTile, bool forceTeleport = false);

		void getSpectators(SpectatorVec& spectators, const Position& centerPos, bool multifloor = false, bool onlyPlayers = false,
		                   int32_t minRangeX = 0, int32_t maxRangeX = 0,
		                   int32_t minRangeY = 0, int32_t maxRangeY = 0);

		[[nodiscard]] SpectatorVec fetchSpectators(const Position& centerPos, bool multifloor = false, bool onlyPlayers = false, int32_t minRangeX = 0, int32_t maxRangeX = 0, int32_t minRangeY = 0, int32_t maxRangeY = 0);
		bool canThrowObjectTo(const Position& fromPos, const Position& toPos, bool checkLineOfSight = true, bool sameFloor = false, int32_t rangex = Map::maxClientViewportX, int32_t rangey = Map::maxClientViewportY);
		bool isTileClear(uint16_t x, uint16_t y, uint8_t z, bool blockFloor = false);
		bool isSightClear(const Position& fromPos, const Position& toPos, bool sameFloor = false);
		static bool checkSightLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t z);
		const TilePtr& canWalkTo(CreaturePtr& creature, const Position& pos);
		bool getPathMatching(CreaturePtr& creature, std::vector<Direction>& dirList, const FrozenPathingConditionCall& pathCondition, const FindPathParams& fpp);

		std::map<std::string, Position> waypoints;

		BlackTek::World::Chunk* getChunk(uint16_t x, uint16_t y)
		{
			const auto handle = chunk_grid.FindChunk(BlackTek::World::ToChunkCoord(x, y));
			return chunk_grid.GetChunk(handle);
		}

		BlackTek::World::Chunk* getChunk(BlackTek::World::ChunkHandle handle)
		{
			return chunk_grid.GetChunk(handle);
		}

		Spawns spawns;
		Towns towns;
		Houses houses;

	private:
		BlackTek::World::ChunkGrid chunk_grid;
		BlackTek::World::FloorPool floor_pool;

		std::filesystem::path spawnfile;
		std::filesystem::path housefile;

		uint32_t width = 0;
		uint32_t height = 0;

		// Actually scans the map for spectators
		void getSpectatorsInternal(SpectatorVec& spectators, const Position& centerPos,
		                           int32_t minRangeX, int32_t maxRangeX,
		                           int32_t minRangeY, int32_t maxRangeY,
		                           int32_t minRangeZ, int32_t maxRangeZ, bool onlyPlayers) const;

		friend class Game;
		friend class IOMap;
};

#endif
