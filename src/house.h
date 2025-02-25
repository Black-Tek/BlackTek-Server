// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_HOUSE_H
#define FS_HOUSE_H

#include <ranges>
#include <set>
#include <unordered_set>

#include "container.h"
#include "position.h"

class House;
class BedItem;
class Player;

class AccessList
{
	public:
		void parseList(std::string_view list);
		void addPlayer(const std::string& name);
		void addGuild(const std::string& name);
		void addGuildRank(const std::string& name, const std::string& rankName);

		bool isInList(const PlayerConstPtr& player) const;

		void getList(std::string& list) const;

	private:
		std::string list;
		std::unordered_set<uint32_t> playerList;
		std::unordered_set<uint32_t> guildRankList;
		bool allowEveryone = false;
};

class Door final : public Item
{
	public:
		explicit Door(uint16_t type);

		// non-copyable
		Door(const Door&) = delete;
		Door& operator=(const Door&) = delete;

		DoorPtr getDoor() override {
			return dynamic_shared_this<Door>();
		}
	
		DoorConstPtr getDoor() const override {
			return dynamic_shared_this<Door>();
		}

		House* getHouse() const {
			return house;
		}

		//serialization
		Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream) override;
		void serializeAttr(PropWriteStream&) const override {}

		void setDoorId(uint32_t doorId) {
			setIntAttr(ITEM_ATTRIBUTE_DOORID, doorId);
		}
	
		uint32_t getDoorId() const {
			return getIntAttr(ITEM_ATTRIBUTE_DOORID);
		}

		bool canUse(const PlayerConstPtr& player) const;

		void setAccessList(std::string_view textlist);
		bool getAccessList(std::string& list) const;

		void onRemoved() override;

	private:
		void setHouse(House* house);

		House* house = nullptr;
		std::unique_ptr<AccessList> accessList;
		friend class House;
};

enum AccessList_t {
	GUEST_LIST = 0x100,
	SUBOWNER_LIST = 0x101,
};

enum AccessHouseLevel_t {
	HOUSE_NOT_INVITED = 0,
	HOUSE_GUEST = 1,
	HOUSE_SUBOWNER = 2,
	HOUSE_OWNER = 3,
};

using HouseTileList = std::list<TilePtr>;
using HouseBedItemList = std::list<BedItemPtr>;

class HouseTransferItem;
using HouseTransferItemPtr = std::shared_ptr<HouseTransferItem>;
using HouseTransferItemConstPtr = std::shared_ptr<const HouseTransferItem>;

class HouseTransferItem final : public Item
{
	public:
		static HouseTransferItemPtr createHouseTransferItem(House* house);

		explicit HouseTransferItem(House* house) : Item(0), house(house) {}

		void onTradeEvent(TradeEvents_t event, const PlayerPtr& owner) override;
		bool canTransform() const override {
			return false;
		}

	private:
		House* house;
};

class House
{
	public:
		explicit House(const uint32_t houseId);

		void addTile(const TilePtr& tile);
		void updateDoorDescription() const;

		bool canEditAccessList(uint32_t listId, const PlayerConstPtr& player) const;
		// listId special values:
		// GUEST_LIST	 guest list
		// SUBOWNER_LIST subowner list
		void setAccessList(uint32_t listId, std::string_view textlist);
		bool getAccessList(uint32_t listId, std::string& list) const;

		bool isInvited(const PlayerConstPtr& player) const;

		AccessHouseLevel_t getHouseAccessLevel(const PlayerConstPtr& player) const;
		bool kickPlayer(const PlayerPtr& player, const PlayerPtr& target);

		void setEntryPos(const Position pos) {
			posEntry = pos;
		}
	
		const Position& getEntryPosition() const {
			return posEntry;
		}

		void setName(const std::string& houseName) {
			this->houseName = houseName;
		}
	
		const std::string& getName() const {
			return houseName;
		}

		void setOwner(uint32_t guid, bool updateDatabase = true, const PlayerPtr& player = nullptr);
	
		uint32_t getOwner() const {
			return owner;
		}

		void setPaidUntil(const time_t paid) {
			paidUntil = paid;
		}
	
		time_t getPaidUntil() const {
			return paidUntil;
		}

		void setRent(const uint32_t rent) {
			this->rent = rent;
		}
	
		uint32_t getRent() const {
			return rent;
		}

		void setPayRentWarnings(const uint32_t warnings) {
			rentWarnings = warnings;
		}
	
		uint32_t getPayRentWarnings() const {
			return rentWarnings;
		}

		void setTownId(const uint32_t townId) {
			this->townId = townId;
		}
	
		uint32_t getTownId() const {
			return townId;
		}

		uint32_t getId() const {
			return id;
		}

		void addDoor(const DoorPtr& door);
		void removeDoor(const DoorPtr& door);
		DoorPtr getDoorByNumber(uint32_t doorId) const;
		DoorPtr getDoorByPosition(const Position& pos);

		HouseTransferItemPtr getTransferItem();
		void resetTransferItem();
		bool executeTransfer(const HouseTransferItemPtr& item, const PlayerPtr& newOwner);

		const HouseTileList& getTiles() const {
			return houseTiles;
		}

		const std::set<DoorPtr>& getDoors() const {
			return doorSet;
		}

		void addBed(const BedItemPtr& bed);
	
		const HouseBedItemList& getBeds() const {
			return bedsList;
		}
	
		uint32_t getBedCount() const {
			return static_cast<uint32_t>(std::ceil(bedsList.size() / 2.)); //each bed takes 2 sqms of space, ceil is just for bad maps
		}

	private:
		bool transferToDepot() const;
		bool transferToDepot(const PlayerPtr& player) const;

		AccessList guestList;
		AccessList subOwnerList;

		Container transfer_container{ITEM_LOCKER1};

		HouseTileList houseTiles;
		std::set<DoorPtr> doorSet;
		HouseBedItemList bedsList;

		std::string houseName;
		std::string ownerName;

		HouseTransferItemPtr transferItem = nullptr;

		time_t paidUntil = 0;

		uint32_t id;
		uint32_t owner = 0;
		uint32_t ownerAccountId = 0;
		uint32_t rentWarnings = 0;
		uint32_t rent = 0;
		uint32_t townId = 0;

		Position posEntry = {};

		bool isLoaded = false;
};

using HouseMap = std::map<uint32_t, House*>;

enum RentPeriod_t {
	RENTPERIOD_DAILY,
	RENTPERIOD_WEEKLY,
	RENTPERIOD_MONTHLY,
	RENTPERIOD_YEARLY,
	RENTPERIOD_NEVER,
};

class Houses
{
	public:
		Houses() = default;
		~Houses() {
			for (const auto& val : houseMap | std::views::values) {
				delete val;
			}
		}

		// non-copyable
		Houses(const Houses&) = delete;
		Houses& operator=(const Houses&) = delete;

		House* addHouse(const uint32_t id) {
			auto it = houseMap.find(id);
			if (it != houseMap.end()) {
				return it->second;
			}

			auto house = new House(id);
			houseMap[id] = house;
			return house;
		}

		House* getHouse(uint32_t houseId) {
			auto it = houseMap.find(houseId);
			if (it == houseMap.end()) {
				return nullptr;
			}
			return it->second;
		}

		House* getHouseByPlayerId(uint32_t playerId) const;

		bool loadHousesXML(const std::string& filename);

		void payHouses(RentPeriod_t rentPeriod) const;

		const HouseMap& getHouses() const {
			return houseMap;
		}

	private:
		HouseMap houseMap;
};

#endif
