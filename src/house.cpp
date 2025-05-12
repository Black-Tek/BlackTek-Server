// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "pugicast.h"

#include "house.h"
#include "iologindata.h"
#include "game.h"
#include "configmanager.h"
#include "bed.h"

#include <fmt/format.h>

extern ConfigManager g_config;
extern Game g_game;

House::House(const uint32_t houseId) : id(houseId) {}

void House::addTile(const TilePtr& tile)
{
	tile->setFlag(TILESTATE_PROTECTIONZONE);
	houseTiles.push_back(tile);
}

void House::setOwner(uint32_t guid, bool updateDatabase/* = true*/, const PlayerPtr& player)
{
	if (updateDatabase && owner != guid) {
		Database& db = Database::getInstance();
		db.executeQuery(fmt::format("UPDATE `houses` SET `owner` = {:d}, `bid` = 0, `bid_end` = 0, `last_bid` = 0, `highest_bidder` = 0  WHERE `id` = {:d}", guid, id));
	}

	if (isLoaded && owner == guid) {
		return;
	}

	isLoaded = true;

	if (owner != 0) {
		//send items to depot
		if (player) {
			transferToDepot(player);
		} else {
			transferToDepot();
		}

		for (const auto tile : houseTiles) {
			if (const auto creatures = tile->getCreatures()) {
				for (int32_t i = creatures->size(); --i >= 0;) {
					kickPlayer(nullptr, (*creatures)[i]->getPlayer());
				}
			}
		}

		// Remove players from beds
		for (const auto bed : bedsList) {
			if (bed->getSleeper() != 0) {
				bed->wakeUp(nullptr);
			}
		}

		//clean access lists
		owner = 0;
		ownerAccountId = 0;
		setAccessList(SUBOWNER_LIST, "");
		setAccessList(GUEST_LIST, "");

		for (const auto door : doorSet) {
			door->setAccessList("");
		}
	} else {
		std::string strRentPeriod = asLowerCaseString(g_config.getString(ConfigManager::HOUSE_RENT_PERIOD));
		time_t currentTime = time(nullptr);
		if (strRentPeriod == "yearly") {
			currentTime += 24 * 60 * 60 * 365;
		} else if (strRentPeriod == "monthly") {
			currentTime += 24 * 60 * 60 * 30;
		} else if (strRentPeriod == "weekly") {
			currentTime += 24 * 60 * 60 * 7;
		} else if (strRentPeriod == "daily") {
			currentTime += 24 * 60 * 60;
		} else {
			currentTime = 0;
		}

		paidUntil = currentTime;
	}

	rentWarnings = 0;

	if (guid != 0) {
		if (const std::string name = IOLoginData::getNameByGuid(guid); !name.empty()) {
			owner = guid;
			ownerName = name;
			ownerAccountId = IOLoginData::getAccountIdByPlayerName(name);
		}
	}

	updateDoorDescription();
}

void House::updateDoorDescription() const
{
	const int32_t housePrice = g_config.getNumber(ConfigManager::HOUSE_PRICE);
	for (const auto& it : doorSet) {
		it->setSpecialDescription(fmt::format("It belongs to house '{:s}'. {:s} owns this house.{:s}", houseName, (owner != 0) ? ownerName : "Nobody", g_config.getBoolean(ConfigManager::HOUSE_DOOR_SHOW_PRICE) && (housePrice != -1) && (owner == 0) ? fmt::format(" It costs {:d} gold coins.", (houseTiles.size() * housePrice)) : ""));
	}
}

AccessHouseLevel_t House::getHouseAccessLevel(const PlayerConstPtr& player) const
{
	if (!player) {
		return HOUSE_OWNER;
	}

	if (g_config.getBoolean(ConfigManager::HOUSE_OWNED_BY_ACCOUNT)) {
		if (ownerAccountId == player->getAccount()) {
			return HOUSE_OWNER;
		}
	}

	if (player->hasFlag(PlayerFlag_CanEditHouses)) {
		return HOUSE_OWNER;
	}

	if (player->getGUID() == owner) {
		return HOUSE_OWNER;
	}

	if (subOwnerList.isInList(player)) {
		return HOUSE_SUBOWNER;
	}

	if (guestList.isInList(player)) {
		return HOUSE_GUEST;
	}

	return HOUSE_NOT_INVITED;
}

bool House::kickPlayer(const PlayerPtr& player, const PlayerPtr& target)
{
	if (!target) {
		return false;
	}

	if (!target->getTile()->isHouseTile() || target->getTile()->getHouse() != this) {
		return false;
	}

	if (getHouseAccessLevel(player) < getHouseAccessLevel(target) || target->hasFlag(PlayerFlag_CanEditHouses)) {
		return false;
	}

	Position oldPosition = target->getPosition();
	if (g_game.internalTeleport(target, getEntryPosition()) == RETURNVALUE_NOERROR) {
		g_game.addMagicEffect(oldPosition, CONST_ME_POFF);
		g_game.addMagicEffect(getEntryPosition(), CONST_ME_TELEPORT);
	}
	return true;
}

void House::setAccessList(const uint32_t listId, const std::string_view textlist)
{
	if (listId == GUEST_LIST) {
		guestList.parseList(textlist);
	} else if (listId == SUBOWNER_LIST) {
		subOwnerList.parseList(textlist);
	} else {
		if (const auto door = getDoorByNumber(listId)) {
			door->setAccessList(textlist);
		}

		// We do not have to kick anyone
		return;
	}

	//kick uninvited players
	for (const auto tile : houseTiles) {
		if (const auto creatures = tile->getCreatures()) {
			for (int32_t i = creatures->size(); --i >= 0;) {
				const auto player = (*creatures)[i]->getPlayer();
				if (player && !isInvited(player)) {
					kickPlayer(nullptr, player);
				}
			}
		}
	}
}

bool House::transferToDepot() const
{
	if (townId == 0 || owner == 0) {
		return false;
	}

	if (const auto player = g_game.getPlayerByGUID(owner)) {
		transferToDepot(player);
	} else {
		const PlayerPtr tmpPlayer = std::make_shared<Player>(nullptr);
		if (!IOLoginData::loadPlayerById(tmpPlayer, owner)) {
			return false;
		}

		transferToDepot(tmpPlayer);
		IOLoginData::savePlayer(tmpPlayer);
	}
	return true;
}

bool House::transferToDepot(const PlayerPtr& player) const
{
	if (townId == 0 || owner == 0) {
		return false;
	}

	ItemList moveItemList;
	for (const auto tile : houseTiles) {
		if (const auto items = tile->getItemList()) {
			for (const auto item : *items) {
				if (item->isPickupable()) {
					moveItemList.push_back(item);
				} else {
					if (const auto container = item->getContainer()) {
						for (const auto containerItem : container->getItemList()) {
							moveItemList.push_back(containerItem);
						}
					}
				}
			}
		}
	}
	
	CylinderPtr inbox = player->getInbox();
	for (const auto item : moveItemList) {
		CylinderPtr parent = item->getParent();
		g_game.internalMoveItem(parent, inbox, INDEX_WHEREEVER, item, item->getItemCount(), std::nullopt, FLAG_NOLIMIT);
	}
	return true;
}

bool House::getAccessList(const uint32_t listId, std::string& list) const
{
	if (listId == GUEST_LIST) {
		guestList.getList(list);
		return true;
	} else if (listId == SUBOWNER_LIST) {
		subOwnerList.getList(list);
		return true;
	}

	const auto door = getDoorByNumber(listId);
	if (!door) {
		return false;
	}

	return door->getAccessList(list);
}

bool House::isInvited(const PlayerConstPtr& player) const
{
	return getHouseAccessLevel(player) != HOUSE_NOT_INVITED;
}

void House::addDoor(const DoorPtr& door)
{
	doorSet.insert(door);
	door->setHouse(this);
	updateDoorDescription();
}

void House::removeDoor(const DoorPtr& door)
{
	if (const auto it = doorSet.find(door); it != doorSet.end()) {
		doorSet.erase(it);
	}
}

void House::addBed(const BedItemPtr& bed)
{
	bedsList.push_back(bed);
	bed->setHouse(this);
}

DoorPtr House::getDoorByNumber(const uint32_t doorId) const
{
	for (const auto door : doorSet) {
		if (door->getDoorId() == doorId) {
			return door;
		}
	}
	return nullptr;
}

DoorPtr House::getDoorByPosition(const Position& pos)
{
	for (const auto door : doorSet) {
		if (door->getPosition() == pos) {
			return door;
		}
	}
	return nullptr;
}

bool House::canEditAccessList(const uint32_t listId, const PlayerConstPtr& player) const
{
	switch (getHouseAccessLevel(player)) {
		case HOUSE_OWNER:
			return true;

		case HOUSE_SUBOWNER:
			return listId == GUEST_LIST;

		default:
			return false;
	}
}

HouseTransferItemPtr House::getTransferItem()
{
	if (transferItem != nullptr) {
		return nullptr;
	}

	transfer_container.clearParent();
	transferItem = HouseTransferItem::createHouseTransferItem(this);
	transfer_container.addThing(transferItem);
	return transferItem;
}

void House::resetTransferItem()
{
	if (transferItem) {
		ItemPtr tmpItem = transferItem;
		transferItem = nullptr;
		transfer_container.clearParent();

		transfer_container.removeThing(tmpItem, tmpItem->getItemCount());
		// g_game.ReleaseItem(tmpItem);
	}
}

HouseTransferItemPtr HouseTransferItem::createHouseTransferItem(House* house)
{
	HouseTransferItemPtr transferItem = std::make_shared<HouseTransferItem>(house);
	transferItem->setID(ITEM_DOCUMENT_RO);
	transferItem->setSubType(1);
	transferItem->setSpecialDescription(fmt::format("It is a house transfer document for '{:s}'.", house->getName()));
	return transferItem;
}

void HouseTransferItem::onTradeEvent(const TradeEvents_t event, const PlayerPtr& owner)
{
	if (event == ON_TRADE_TRANSFER) {
		if (house) {
			house->executeTransfer(static_cast<HouseTransferItemPtr>(this), owner);
		}

		g_game.internalRemoveItem(static_cast<ItemPtr>(this), 1);
	} else if (event == ON_TRADE_CANCEL) {
		if (house) {
			house->resetTransferItem();
		}
	}
}

bool House::executeTransfer(const HouseTransferItemPtr& item, const PlayerPtr& newOwner)
{
	if (transferItem != item) {
		return false;
	}

	setOwner(newOwner->getGUID());
	transferItem = nullptr;
	return true;
}

void AccessList::parseList(const std::string_view list)
{
	playerList.clear();
	guildRankList.clear();
	allowEveryone = false;
	this->list = list;
	if (list.empty()) {
		return;
	}

	std::istringstream listStream{ this->list };
	std::string line;

	uint16_t lineNo = 1;
	while (getline(listStream, line)) {
		if (++lineNo > 100) {
			break;
		}

		trimString(line);
		trim_left(line, '\t');
		trim_right(line, '\t');
		trimString(line);

		if (line.empty() || line.front() == '#' || line.length() > 100) {
			continue;
		}

		toLowerCaseString(line);

		if (const std::string::size_type at_pos = line.find("@"); at_pos != std::string::npos) {
			if (at_pos == 0) {
				addGuild(line.substr(1));
			} else {
				addGuildRank(line.substr(0, at_pos - 1), line.substr(at_pos + 1));
			}
		} else if (line == "*") {
			allowEveryone = true;
		} else if (line.find("!") != std::string::npos || line.find("*") != std::string::npos || line.find("?") != std::string::npos) {
			continue; // regexp no longer supported
		} else {
			addPlayer(line);
		}
	}
}

void AccessList::addPlayer(const std::string& name)
{
	if (const auto player = g_game.getPlayerByName(name)) {
		playerList.insert(player->getGUID());
	} else {
		if (const uint32_t guid = IOLoginData::getGuidByName(name); guid != 0) {
			playerList.insert(guid);
		}
	}
}

namespace {

const Guild_ptr getGuildByName(const std::string& name)
{
	uint32_t guildId = IOGuild::getGuildIdByName(name);
	if (guildId == 0) {
		return nullptr;
	}

	if (const auto guild = g_game.getGuild(guildId)) {
		return guild;
	}

	return IOGuild::loadGuild(guildId);
}

}

void AccessList::addGuild(const std::string& name)
{
	if (const auto guild = getGuildByName(name)) {
		for (const auto rank : guild->getRanks()) {
			guildRankList.insert(rank->id);
		}
	}
}

void AccessList::addGuildRank(const std::string& name, const std::string& rankName)
{
	if (const auto guild = getGuildByName(name)) {
		if (const GuildRank_ptr rank = guild->getRankByName(rankName)) {
			guildRankList.insert(rank->id);
		}
	}
}

bool AccessList::isInList(const PlayerConstPtr& player) const
{
	if (allowEveryone) {
		return true;
	}

	auto playerIt = playerList.find(player->getGUID());
	if (playerIt != playerList.end()) {
		return true;
	}

	const GuildRank_ptr rank = player->getGuildRank();
	return rank && guildRankList.contains(rank->id);
}

void AccessList::getList(std::string& list) const
{
	list = this->list;
}

Door::Door(const uint16_t type) :	Item(type) {}

Attr_ReadValue Door::readAttr(const AttrTypes_t attr, PropStream& propStream)
{
	if (attr == ATTR_HOUSEDOORID) {
		uint8_t doorId;
		if (!propStream.read<uint8_t>(doorId)) {
			return ATTR_READ_ERROR;
		}

		setDoorId(doorId);
		return ATTR_READ_CONTINUE;
	}
	return Item::readAttr(attr, propStream);
}

void Door::setHouse(House* house)
{
	if (this->house != nullptr) {
		return;
	}

	this->house = house;

	if (!accessList) {
		accessList.reset(new AccessList());
	}
}

bool Door::canUse(const PlayerConstPtr& player) const
{
	if (!house) {
		return true;
	}

	if (house->getHouseAccessLevel(player) >= HOUSE_SUBOWNER) {
		return true;
	}

	return accessList->isInList(player);
}

void Door::setAccessList(const std::string_view textlist)
{
	if (!accessList) {
		accessList.reset(new AccessList());
	}

	accessList->parseList(textlist);
}

bool Door::getAccessList(std::string& list) const
{
	if (!house) {
		return false;
	}

	accessList->getList(list);
	return true;
}

void Door::onRemoved()
{
	Item::onRemoved();

	if (house) {
		house->removeDoor(static_cast<DoorPtr>(this));
	}
}

House* Houses::getHouseByPlayerId(const uint32_t playerId) const
{
	for (const auto& val : houseMap | std::views::values) {
		if (val->getOwner() == playerId) {
			return val;
		}
	}
	return nullptr;
}

bool Houses::loadHousesXML(const std::string& filename)
{
	pugi::xml_document doc;
	if (const pugi::xml_parse_result result = doc.load_file(filename.c_str()); !result) {
		printXMLError("Error - Houses::loadHousesXML", filename, result);
		return false;
	}

	for (auto houseNode : doc.child("houses").children()) {
		pugi::xml_attribute houseIdAttribute = houseNode.attribute("houseid");
		if (!houseIdAttribute) {
			return false;
		}

		const int32_t houseId = pugi::cast<int32_t>(houseIdAttribute.value());

		House* house = getHouse(houseId);
		if (!house) {
			std::cout << "Error: [Houses::loadHousesXML] Unknown house, id = " << houseId << std::endl;
			return false;
		}

		house->setName(houseNode.attribute("name").as_string());

		Position entryPos(
			pugi::cast<uint16_t>(houseNode.attribute("entryx").value()),
			pugi::cast<uint16_t>(houseNode.attribute("entryy").value()),
			pugi::cast<uint16_t>(houseNode.attribute("entryz").value())
		);
		if (entryPos.x == 0 && entryPos.y == 0 && entryPos.z == 0) {
			std::cout << "[Warning - Houses::loadHousesXML] House entry not set"
					    << " - Name: " << house->getName()
					    << " - House id: " << houseId << std::endl;
		}
		house->setEntryPos(entryPos);

		house->setRent(pugi::cast<uint32_t>(houseNode.attribute("rent").value()));
		house->setTownId(pugi::cast<uint32_t>(houseNode.attribute("townid").value()));

		house->setOwner(0, false);
	}
	return true;
}

void Houses::payHouses(const RentPeriod_t rentPeriod) const
{
	if (rentPeriod == RENTPERIOD_NEVER) {
		return;
	}

	time_t currentTime = time(nullptr);
	for (const auto& val : houseMap | std::views::values) {
		House* house = val;
		if (house->getOwner() == 0) {
			continue;
		}

		const uint32_t rent = house->getRent();
		if (rent == 0 || house->getPaidUntil() > currentTime) {
			continue;
		}

		const uint32_t ownerId = house->getOwner();
		const auto town = g_game.map.towns.getTown(house->getTownId());
		if (!town) {
			continue;
		}

		PlayerPtr player = std::make_shared<Player>(nullptr);
		if (!IOLoginData::loadPlayerById(player, ownerId)) {
			// Player doesn't exist, reset house owner
			house->setOwner(0);
			continue;
		}

		if (player->getBankBalance() >= rent) {
			player->setBankBalance(player->getBankBalance() - rent);

			time_t paidUntil = currentTime;
			switch (rentPeriod) {
				case RENTPERIOD_DAILY:
					paidUntil += 24 * 60 * 60;
					break;
				case RENTPERIOD_WEEKLY:
					paidUntil += 24 * 60 * 60 * 7;
					break;
				case RENTPERIOD_MONTHLY:
					paidUntil += 24 * 60 * 60 * 30;
					break;
				case RENTPERIOD_YEARLY:
					paidUntil += 24 * 60 * 60 * 365;
					break;
				default:
					break;
			}

			house->setPaidUntil(paidUntil);
			house->setPayRentWarnings(0);
		} else {
			if (house->getPayRentWarnings() < 7) {
				int32_t daysLeft = 7 - house->getPayRentWarnings();

				auto letter = Item::CreateItem(ITEM_LETTER_STAMPED);
				std::string period;

				switch (rentPeriod) {
					case RENTPERIOD_DAILY:
						period = "daily";
						break;

					case RENTPERIOD_WEEKLY:
						period = "weekly";
						break;

					case RENTPERIOD_MONTHLY:
						period = "monthly";
						break;

					case RENTPERIOD_YEARLY:
						period = "annual";
						break;

					default:
						break;
				}

				letter->setText(fmt::format("Warning! \nThe {:s} rent of {:d} gold for your house \"{:s}\" is payable. Have it within {:d} days or you will lose this house.", period, house->getRent(), house->getName(), daysLeft));
				CylinderPtr inbox = player->getInbox();
				g_game.internalAddItem(inbox, letter, INDEX_WHEREEVER, FLAG_NOLIMIT);
				house->setPayRentWarnings(house->getPayRentWarnings() + 1);
			} else {
				house->setOwner(0, true, player);
			}
		}

		IOLoginData::savePlayer(player);
	}
}
