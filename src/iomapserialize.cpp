// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "iomapserialize.h"
#include "game.h"
#include "bed.h"

#include <fmt/format.h>

extern Game g_game;

void IOMapSerialize::loadHouseItems(Map* map)
{
	int64_t start = OTSYS_TIME();

	DBResult_ptr result = Database::getInstance().storeQuery("SELECT `data` FROM `tile_store`");
	if (!result) {
		return;
	}

	do {

		auto attr = result->getString("data");
		PropStream propStream;
		propStream.init(attr.data(), attr.size());

		uint16_t x, y;
		uint8_t z;
		if (!propStream.read<uint16_t>(x) || !propStream.read<uint16_t>(y) || !propStream.read<uint8_t>(z)) {
			continue;
		}

		TilePtr tile = map->getTile(x, y, z);
		if (!tile) {
			continue;
		}
		//g_game.addGameTile(tile);
		uint32_t item_count;
		if (!propStream.read<uint32_t>(item_count)) {
			continue;
		}

		while (item_count--) {
			loadItem(propStream, tile);
		}
	} while (result->next());
	std::cout << "> Loaded house items in: " << (OTSYS_TIME() - start) / (1000.) << " s" << std::endl;
}

bool IOMapSerialize::saveHouseItems()
{
	int64_t start = OTSYS_TIME();
	Database& db = Database::getInstance();

	//Start the transaction
	DBTransaction transaction;
	if (!transaction.begin()) {
		return false;
	}

	//clear old tile data
	if (!db.executeQuery("DELETE FROM `tile_store`")) {
		return false;
	}

	DBInsert stmt("INSERT INTO `tile_store` (`house_id`, `data`) VALUES ");

	PropWriteStream stream;
	for (const auto& it : g_game.map.houses.getHouses()) {
		//save house items
		House* house = it.second;
		for (auto tile : house->getTiles()) {
			saveTile(stream, tile);

			if (auto attributes = stream.getStream(); !attributes.empty()) {
				if (!stmt.addRow(fmt::format("{:d}, {:s}", house->getId(), db.escapeString(attributes)))) {
					return false;
				}
				stream.clear();
			}
		}
	}

	if (!stmt.execute()) {
		return false;
	}

	//End the transaction
	bool success = transaction.commit();
	std::cout << "> Saved house items in: " <<
	          (OTSYS_TIME() - start) / (1000.) << " s" << std::endl;
	return success;
}

bool IOMapSerialize::loadContainer(PropStream& propStream, const ContainerPtr& container)
{
	while (container->serializationCount > 0) {
		if (!loadItem(propStream, container)) {
			std::cout << "[Warning - IOMapSerialize::loadContainer] Unserialization error for container item: " << container->getID() << std::endl;
			return false;
		}
		container->serializationCount--;
	}

	uint8_t endAttr;
	if (!propStream.read<uint8_t>(endAttr) || endAttr != 0) {
		std::cout << "[Warning - IOMapSerialize::loadContainer] Unserialization error for container item: " << container->getID() << std::endl;
		return false;
	}
	return true;
}

bool IOMapSerialize::loadItem(PropStream& propStream, const CylinderPtr& parent)
{
	uint16_t id;
	if (!propStream.read<uint16_t>(id)) {
		return false;
	}

	TilePtr tile = nullptr;
	if (parent->getParent() == nullptr) {
		tile = parent->getTile();
	}

	const ItemType& iType = Item::items[id];
	if (iType.moveable || iType.forceSerialize || !tile) {
		//create a new item
		if (auto item = Item::CreateItem(id)) {
			if (item->unserializeAttr(propStream)) {
				const auto& container = item->getContainer();
				if (container && !loadContainer(propStream, container)) {
					return false;
				}

				parent->internalAddThing(item);
				item->startDecaying();
			} else {
				std::cout << "WARNING: Unserialization error in IOMapSerialize::loadItem()" << id << std::endl;
				return false;
			}
		}
	} else {
		// Stationary items like doors/beds/blackboards/bookcases
		ItemPtr item = nullptr;
		if (const auto items = tile->getItemList()) {
			for (auto findItem : *items) {
				if (findItem->getID() == id) {
					item = findItem;
					break;
				} else if (iType.isDoor() && findItem->getDoor()) {
					item = findItem;
					break;
				} else if (iType.isBed() && findItem->getBed()) {
					item = findItem;
					break;
				}
			}
		}

		if (item) {
			if (item->unserializeAttr(propStream)) {
				auto container = item->getContainer();
				if (container && !loadContainer(propStream, container)) {
					return false;
				}

				auto newItem = g_game.transformItem(item, id);
			} else {
				std::cout << "WARNING: Unserialization error in IOMapSerialize::loadItem()" << id << std::endl;
			}
		} else {
			//The map changed since the last save, just read the attributes
			auto dummy = Item::CreateItem(id);
			if (dummy) {
				dummy->unserializeAttr(propStream);
				if (const auto container = dummy->getContainer()) {
					if (!loadContainer(propStream, container)) {
						return false;
					}
				} else if (auto bedItem = dynamic_cast<BedItem*>(dummy.get())) {
					uint32_t sleeperGUID = bedItem->getSleeper();
					if (sleeperGUID != 0) {
						g_game.removeBedSleeper(sleeperGUID);
					}
				}
			}
		}
	}
	return true;
}

void IOMapSerialize::saveItem(PropWriteStream& stream, const ItemPtr& item)
{
	const auto container = item->getContainer();

	// Write ID & props
	stream.write<uint16_t>(item->getID());
	item->serializeAttr(stream);

	if (container) {
		// Hack our way into the attributes
		stream.write<uint8_t>(ATTR_CONTAINER_ITEMS);
		stream.write<uint32_t>(container->size());
		for (auto it = container->getReversedItems(), end = container->getReversedEnd(); it != end; ++it) {
			saveItem(stream, *it);
		}
	}

	stream.write<uint8_t>(0x00); // attr end
}

void IOMapSerialize::saveTile(PropWriteStream& stream, const TilePtr& tile)
{
	const auto tileItems = tile->getItemList();
	if (!tileItems) {
		return;
	}

	std::forward_list<ItemPtr> items;
	uint16_t count = 0;
	for (auto item : *tileItems) {
		const ItemType& it = Item::items[item->getID()];

		// Note that these are NEGATED, ie. these are the items that will be saved.
		if (!(it.moveable || it.forceSerialize || item->getDoor() || (item->getContainer() && !item->getContainer()->empty()) || it.canWriteText || item->getBed())) {
			continue;
		}

		items.push_front(item);
		++count;
	}

	if (!items.empty()) {
		const Position& tilePosition = tile->getPosition();
		stream.write<uint16_t>(tilePosition.x);
		stream.write<uint16_t>(tilePosition.y);
		stream.write<uint8_t>(tilePosition.z);

		stream.write<uint32_t>(count);
		for (const auto item : items) {
			saveItem(stream, item);
		}
	}
}

bool IOMapSerialize::loadHouseInfo()
{
	Database& db = Database::getInstance();

	DBResult_ptr result = db.storeQuery("SELECT `id`, `owner`, `paid`, `warnings` FROM `houses`");
	if (!result) {
		return false;
	}

	do {
		if (const auto house = g_game.map.houses.getHouse(result->getNumber<uint32_t>("id"))) {
			house->setOwner(result->getNumber<uint32_t>("owner"), false);
			house->setPaidUntil(result->getNumber<time_t>("paid"));
			house->setPayRentWarnings(result->getNumber<uint32_t>("warnings"));
		}
	} while (result->next());

	result = db.storeQuery("SELECT `house_id`, `listid`, `list` FROM `house_lists`");
	if (result) {
		do {
			if (const auto house = g_game.map.houses.getHouse(result->getNumber<uint32_t>("house_id"))) {
				house->setAccessList(result->getNumber<uint32_t>("listid"), result->getString("list"));
			}
		} while (result->next());
	}
	return true;
}

bool IOMapSerialize::saveHouseInfo()
{
	Database& db = Database::getInstance();

	DBTransaction transaction;
	if (!transaction.begin()) {
		return false;
	}

	if (!db.executeQuery("DELETE FROM `house_lists`")) {
		return false;
	}

	for (const auto& val : g_game.map.houses.getHouses() | std::views::values) {
		const auto house = val;
		if (DBResult_ptr result = db.storeQuery(fmt::format("SELECT `id` FROM `houses` WHERE `id` = {:d}", house->getId()))) {
			db.executeQuery(fmt::format("UPDATE `houses` SET `owner` = {:d}, `paid` = {:d}, `warnings` = {:d}, `name` = {:s}, `town_id` = {:d}, `rent` = {:d}, `size` = {:d}, `beds` = {:d} WHERE `id` = {:d}", house->getOwner(), house->getPaidUntil(), house->getPayRentWarnings(), db.escapeString(house->getName()), house->getTownId(), house->getRent(), house->getTiles().size(), house->getBedCount(), house->getId()));
		} else {
			db.executeQuery(fmt::format("INSERT INTO `houses` (`id`, `owner`, `paid`, `warnings`, `name`, `town_id`, `rent`, `size`, `beds`) VALUES ({:d}, {:d}, {:d}, {:d}, {:s}, {:d}, {:d}, {:d}, {:d})", house->getId(), house->getOwner(), house->getPaidUntil(), house->getPayRentWarnings(), db.escapeString(house->getName()), house->getTownId(), house->getRent(), house->getTiles().size(), house->getBedCount()));
		}
	}

	DBInsert stmt("INSERT INTO `house_lists` (`house_id` , `listid` , `list`) VALUES ");

	for (const auto& val : g_game.map.houses.getHouses() | std::views::values) {
		const auto house = val;

		std::string listText;
		if (house->getAccessList(GUEST_LIST, listText) && !listText.empty()) {
			if (!stmt.addRow(fmt::format("{:d}, {:d}, {:s}", house->getId(), Titan::to_underlying(GUEST_LIST), db.escapeString(listText)))) {
				return false;
			}

			listText.clear();
		}

		if (house->getAccessList(SUBOWNER_LIST, listText) && !listText.empty()) {
			if (!stmt.addRow(fmt::format("{:d}, {:d}, {:s}", house->getId(), Titan::to_underlying(SUBOWNER_LIST), db.escapeString(listText)))) {
				return false;
			}

			listText.clear();
		}

		for (const auto door : house->getDoors()) {
			if (door->getAccessList(listText) && !listText.empty()) {
				if (!stmt.addRow(fmt::format("{:d}, {:d}, {:s}", house->getId(), door->getDoorId(), db.escapeString(listText)))) {
					return false;
				}

				listText.clear();
			}
		}
	}

	if (!stmt.execute()) {
		return false;
	}

	return transaction.commit();
}

bool IOMapSerialize::saveHouse(House* house)
{
	Database& db = Database::getInstance();

	//Start the transaction
	DBTransaction transaction;
	if (!transaction.begin()) {
		return false;
	}

	uint32_t houseId = house->getId();
	
	//clear old tile data
	if (!db.executeQuery(fmt::format("DELETE FROM `tile_store` WHERE `house_id` = {:d}", houseId))) {
		return false;
	}

	DBInsert stmt("INSERT INTO `tile_store` (`house_id`, `data`) VALUES ");

	PropWriteStream stream;
	for (const auto tile : house->getTiles()) {
		saveTile(stream, tile);

		if (auto attributes = stream.getStream(); attributes.size() > 0) {
			if (!stmt.addRow(fmt::format("{:d}, {:s}", houseId, db.escapeString(attributes)))) {
				return false;
			}
			stream.clear();
		}
	}

	if (!stmt.execute()) {
		return false;
	}

	//End the transaction
	return transaction.commit();
}
