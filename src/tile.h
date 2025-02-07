// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_TILE_H
#define FS_TILE_H

#include "cylinder.h"
#include "item.h"
#include "tools.h"
#include "spectators.h"
#include "declarations.h"

enum tileflags_t : uint32_t {
	TILESTATE_NONE = 0,

	TILESTATE_FLOORCHANGE_DOWN = 1 << 0,
	TILESTATE_FLOORCHANGE_NORTH = 1 << 1,
	TILESTATE_FLOORCHANGE_SOUTH = 1 << 2,
	TILESTATE_FLOORCHANGE_EAST = 1 << 3,
	TILESTATE_FLOORCHANGE_WEST = 1 << 4,
	TILESTATE_FLOORCHANGE_SOUTH_ALT = 1 << 5,
	TILESTATE_FLOORCHANGE_EAST_ALT = 1 << 6,
	TILESTATE_PROTECTIONZONE = 1 << 7,
	TILESTATE_NOPVPZONE = 1 << 8,
	TILESTATE_NOLOGOUT = 1 << 9,
	TILESTATE_PVPZONE = 1 << 10,
	TILESTATE_TELEPORT = 1 << 11,
	TILESTATE_MAGICFIELD = 1 << 12,
	TILESTATE_MAILBOX = 1 << 13,
	TILESTATE_TRASHHOLDER = 1 << 14,
	TILESTATE_BED = 1 << 15,
	TILESTATE_DEPOT = 1 << 16,
	TILESTATE_BLOCKSOLID = 1 << 17,
	TILESTATE_BLOCKPATH = 1 << 18,
	TILESTATE_IMMOVABLEBLOCKSOLID = 1 << 19,
	TILESTATE_IMMOVABLEBLOCKPATH = 1 << 20,
	TILESTATE_IMMOVABLENOFIELDBLOCKPATH = 1 << 21,
	TILESTATE_NOFIELDBLOCKPATH = 1 << 22,
	TILESTATE_SUPPORTS_HANGABLE = 1 << 23,

	TILESTATE_FLOORCHANGE = TILESTATE_FLOORCHANGE_DOWN | TILESTATE_FLOORCHANGE_NORTH | TILESTATE_FLOORCHANGE_SOUTH | TILESTATE_FLOORCHANGE_EAST | TILESTATE_FLOORCHANGE_WEST | TILESTATE_FLOORCHANGE_SOUTH_ALT | TILESTATE_FLOORCHANGE_EAST_ALT,
};

enum ZoneType_t {
	ZONE_PROTECTION,
	ZONE_NOPVP,
	ZONE_PVP,
	ZONE_NOLOGOUT,
	ZONE_NORMAL,
};

class TileItemVector : public ItemVector
{
	public:
		using ItemVector::begin;
		using ItemVector::end;
		using ItemVector::rbegin;
		using ItemVector::rend;
		using ItemVector::size;
		using ItemVector::clear;
		using ItemVector::at;
		using ItemVector::insert;
		using ItemVector::erase;
		using ItemVector::push_back;
		using ItemVector::value_type;
		using ItemVector::iterator;
		using ItemVector::const_iterator;
		using ItemVector::reverse_iterator;
		using ItemVector::const_reverse_iterator;
		using ItemVector::empty;

		iterator getBeginDownItem() {
			return begin();
		}
	
		const_iterator getBeginDownItem() const {
			return begin();
		}
	
		iterator getEndDownItem() {
			return begin() + downItemCount;
		}
	
		const_iterator getEndDownItem() const {
			return begin() + downItemCount;
		}
	
		iterator getBeginTopItem() {
			return getEndDownItem();
		}
	
		const_iterator getBeginTopItem() const {
			return getEndDownItem();
		}
	
		iterator getEndTopItem() {
			return end();
		}
	
		const_iterator getEndTopItem() const {
			return end();
		}

		uint32_t getTopItemCount() const {
			return size() - downItemCount;
		}
	
		uint32_t getDownItemCount() const {
			return downItemCount;
		}
	
		ItemPtr getTopTopItem() const {
			if (getTopItemCount() == 0) {
				return nullptr;
			}
			return *(getEndTopItem() - 1);
		}
	
		ItemPtr getTopDownItem() const {
			if (downItemCount == 0) {
				return nullptr;
			}
			return *getBeginDownItem();
		}
	
		void addDownItemCount(int32_t increment) {
			downItemCount += increment;
		}

	private:
		uint16_t downItemCount = 0;
};


class Tile : public Cylinder, public SharedObject
{
	public:
		Tile(uint16_t x, uint16_t y, uint8_t z) : tilePos(x, y, z) {}
		virtual ~Tile() {
			ground.reset();
		};

		// non-copyable
		Tile(const Tile&) = delete;
		Tile& operator=(const Tile&) = delete;

		virtual TileItemsPtr getItemList() = 0;
		virtual TileItemsConstPtr getItemList() const = 0;

		virtual TileCreaturesPtr getCreatures() = 0;
		virtual TileCreaturesConstPtr getCreatures() const = 0;

		int32_t getThrowRange() const override final {
			return 0;
		}
	
		bool isPushable() const override final {
			return false;
		}

		MagicFieldPtr getFieldItem() const;
		TeleportPtr getTeleportItem() const;
		TrashHolderPtr getTrashHolder() const;
		MailboxPtr getMailbox() const;
		BedItemPtr getBedItem() const;

		CreaturePtr getTopCreature() const;
		CreatureConstPtr getBottomCreature() const;
		CreaturePtr getTopVisibleCreature(const CreaturePtr creature) const;
		CreatureConstPtr getBottomVisibleCreature(const CreatureConstPtr& creature) const;
		ItemPtr getTopTopItem() const;
		ItemPtr getTopDownItem() const;
		bool isMoveableBlocking() const;
		ThingPtr getTopVisibleThing(const CreaturePtr creature);
		ItemPtr getItemByTopOrder(int32_t topOrder);

		size_t getThingCount() const {
			size_t thingCount = getCreatureCount() + getItemCount();
			if (ground) {
				thingCount++;
			}
			return thingCount;
		}
	
		// If these return != 0 the associated vectors are guaranteed to exists
		size_t getCreatureCount() const;
		size_t getItemCount() const;
		uint32_t getTopItemCount() const;
		uint32_t getDownItemCount() const;

		bool hasProperty(ITEMPROPERTY prop) const;
		bool hasProperty(const ItemPtr& exclude, ITEMPROPERTY prop) const;

		bool hasFlag(uint32_t flag) const {
			return hasBitSet(flag, flags);
		}

		void setFlag(uint32_t flag) {
			this->flags |= flag;
		}

		void resetFlag(uint32_t flag) {
			this->flags &= ~flag;
		}

		ZoneType_t getZone() const {
			if (hasFlag(TILESTATE_PROTECTIONZONE)) {
				return ZONE_PROTECTION;
			} else if (hasFlag(TILESTATE_NOPVPZONE)) {
				return ZONE_NOPVP;
			} else if (hasFlag(TILESTATE_PVPZONE)) {
				return ZONE_PVP;
			} else if (hasFlag(TILESTATE_NOLOGOUT)) {
				return ZONE_NOLOGOUT;
			} else {
				return ZONE_NORMAL;
			}
		}

		bool hasHeight(uint32_t n) const;

		std::string getDescription(int32_t lookDistance) const override final;

		int32_t getClientIndexOfCreature(const PlayerConstPtr& player, const CreatureConstPtr& creature) const;
		int32_t getStackposOfItem(const PlayerConstPtr& player, const ItemConstPtr& item) const;

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;
		ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count,
				uint32_t& maxQueryCount, uint32_t flags) override final;
		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;
		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem, uint32_t& flags) override; // another optional wrap ref

		ReturnValue queryAdd(CreaturePtr creature, uint32_t flags);
		ReturnValue queryAdd(ItemPtr item, uint32_t flags);
		ReturnValue queryAdd(PlayerPtr player, uint32_t flags);
		ReturnValue queryAdd(MonsterPtr monster, uint32_t flags);

		void addThing(ThingPtr thing) override final;
		void addThing(int32_t index, ThingPtr thing) override;

		void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) override final;
		void replaceThing(uint32_t index, ThingPtr thing) override final;

		void removeThing(ThingPtr thing, uint32_t count) override final;
		bool hasCreature(CreaturePtr& creature);
		void removeCreature(CreaturePtr& creature);

		int32_t getThingIndex(ThingPtr thing) override final;
		size_t getFirstIndex() const override final;
		size_t getLastIndex() const override final;
		uint32_t getItemTypeCount(uint16_t itemId, int32_t subType = -1) const override final;
		ThingPtr getThing(size_t index) override final;

		void postAddNotification(ThingPtr thing,  CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;
		void postRemoveNotification(ThingPtr thing,  CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) override;

		void internalAddThing(ThingPtr thing) override final;
		void internalAddThing(uint32_t index, ThingPtr thing) override;

		const Position& getPosition() const override final {
			return tilePos;
		}

		bool isRemoved() const override final {
			return false;
		}

		ItemPtr getUseItem(int32_t index);

		ItemPtr getGround() const {
			return ground;
		}

		TilePtr getTile() final {
			return static_shared_this<Tile>();
		}
	
		void setGround(const ItemPtr& item) {
			ground = item;
		}

	private:
		void onAddTileItem(ItemPtr& item);
		void onUpdateTileItem(const ItemPtr& oldItem, const ItemType& oldType, const ItemPtr& newItem, const ItemType& newType);
		void onRemoveTileItem(const SpectatorVec& spectators, const std::vector<int32_t>& oldStackPosVector, const ItemPtr& item);
		void onUpdateTile(const SpectatorVec& spectators);

		void setTileFlags(const ItemConstPtr& item);
		void resetTileFlags(const ItemPtr& item);

		ItemPtr ground = nullptr;
		Position tilePos;
		uint32_t flags = 0;
};

// Used for walkable tiles, where there is high likeliness of
// items being added/removed
class DynamicTile : public Tile
{
	TileItemsPtr items;
	TileCreaturesPtr creatures;

public:
	DynamicTile(uint16_t x, uint16_t y, uint8_t z) : Tile(x, y, z)
	{
		items = std::make_shared<TileItemVector>();
		creatures = std::make_shared<CreatureVector>();
	}

	~DynamicTile() {
		for (auto& item : *items) {
			item.reset();
		}
	}

	// non-copyable
	DynamicTile(const DynamicTile&) = delete;
	DynamicTile& operator=(const DynamicTile&) = delete;

	TileItemsPtr getItemList() override {
		return items;
	}

	TileItemsConstPtr getItemList() const override {
		return items;
	}

	TileCreaturesPtr getCreatures() override {
		return creatures;
	}


	TileCreaturesConstPtr getCreatures() const override {
		return creatures;
	}
};

// For blocking tiles, where we very rarely actually have items
class StaticTile final : public Tile
{
	TileItemsPtr items;
	TileCreaturesPtr creatures;

	public:
		StaticTile(uint16_t x, uint16_t y, uint8_t z) : Tile(x, y, z) 
		{
			items = std::make_shared<TileItemVector>();
			creatures = std::make_shared<CreatureVector>();
		}

		~StaticTile() {
			if (items) {
				for (auto& item : *items) {
					item.reset();
				}
			}
		}

		// non-copyable
		StaticTile(const StaticTile&) = delete;
		StaticTile& operator=(const StaticTile&) = delete;

		TileItemsPtr getItemList() override {
			return items;
		}
	
		TileItemsConstPtr getItemList() const override {
			return items;
		}
	
		TileCreaturesPtr getCreatures() override {
			return creatures;
		}
	
		TileCreaturesConstPtr getCreatures() const override {
			return creatures;
		}

	void postAddNotification(ThingPtr thing, CylinderPtr parent, int32_t index, cylinderlink_t link) override {
			Tile::postAddNotification(thing, parent, index, link);
		}

	void postRemoveNotification(ThingPtr thing, CylinderPtr parent, int32_t index, cylinderlink_t link) override {
			Tile::postRemoveNotification(thing, parent, index, link);
		}

};

#endif
