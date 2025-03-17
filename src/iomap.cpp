// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "iomap.h"

#include "bed.h"

#include <fmt/format.h>

/*
	OTBM_ROOTV1
	|
	|--- OTBM_MAP_DATA
	|	|
	|	|--- OTBM_TILE_AREA
	|	|	|--- OTBM_TILE
	|	|	|--- OTBM_TILE_SQUARE (not implemented)
	|	|	|--- OTBM_TILE_REF (not implemented)
	|	|	|--- OTBM_HOUSETILE
	|	|
	|	|--- OTBM_SPAWNS (not implemented)
	|	|	|--- OTBM_SPAWN_AREA (not implemented)
	|	|	|--- OTBM_MONSTER (not implemented)
	|	|
	|	|--- OTBM_TOWNS
	|	|	|--- OTBM_TOWN
	|	|
	|	|--- OTBM_WAYPOINTS
	|		|--- OTBM_WAYPOINT
	|
	|--- OTBM_ITEM_DEF (not implemented)
*/

TilePtr IOMap::createTile(ItemPtr& ground, uint16_t x, uint16_t y, uint8_t z)
{
	if (!ground) {
		return std::make_shared<Tile>(x, y, z);
	}

	TilePtr	tile = std::make_shared<Tile>(x, y, z);

	tile->internalAddThing(ground);
	ground->startDecaying();
	ground.reset();
	return tile;
}

bool IOMap::loadMap(Map* map, const std::filesystem::path& fileName)
{
	const auto start = OTSYS_TIME();
	try {
		OTB::Loader loader{ fileName.string(), OTB::Identifier{{'O', 'T', 'B', 'M'}} };

		const auto& root = loader.parseTree();
		PropStream propStream;
		if (!loader.getProps(root, propStream)) {
			setLastErrorString("Could not read root property.");
			return false;
		}

		OTBM_root_header root_header{};
		if (!propStream.read(root_header)) {
			setLastErrorString("Could not read header.");
			return false;
		}

		const uint32_t headerVersion = root_header.version;
		if (headerVersion == 0 || headerVersion > 2) {
			setLastErrorString(headerVersion == 0
				? "This map need to be upgraded by using the latest map editor version to be able to load correctly."
				: "Unknown OTBM version detected.");
			return false;
		}

		if (root_header.majorVersionItems < 3 ||
			root_header.majorVersionItems > Item::items.majorVersion ||
			root_header.minorVersionItems < CLIENT_VERSION_810) {

			if (root_header.majorVersionItems < 3 || root_header.minorVersionItems < CLIENT_VERSION_810) {
				setLastErrorString("This map need to be upgraded by using the latest map editor version to be able to load correctly.");
			}
			else {
				setLastErrorString("The map was saved with a different items.otb version, an upgraded items.otb is required.");
			}
			return false;
		}

		if (root_header.minorVersionItems > Item::items.minorVersion) {
			std::cout << "[Warning - IOMap::loadMap] This map needs an updated items.otb." << std::endl;
		}

		map->width = root_header.width;
		map->height = root_header.height;

		std::cout << "> Map size: " << root_header.width << "x" << root_header.height << '.' << std::endl;

		if (root.children.empty() || root.children[0].type != OTBM_MAP_DATA) {
			setLastErrorString("Could not read data node.");
			return false;
		}

		const auto& mapNode = root.children[0];

		[[unlikely]] if (!parseMapDataAttributes(loader, mapNode, *map, fileName)) {
			return false;
		}

		for (const auto& mapDataNode : mapNode.children) {
			switch (mapDataNode.type) {
			case OTBM_TILE_AREA:
				[[unlikely]] if (!parseTileArea(loader, mapDataNode, *map)) {
					return false;
				}
				break;
			case OTBM_TOWNS:
				[[unlikely]] if (!parseTowns(loader, mapDataNode, *map)) {
					return false;
				}
				break;
			case OTBM_WAYPOINTS:
				[[unlikely]] if (headerVersion > 1 && !parseWaypoints(loader, mapDataNode, *map)) {
					return false;
				}
				break;
			[[unlikely]] default:
				setLastErrorString("Unknown map node.");
				return false;
			}
		}
	}
	catch (const OTB::InvalidOTBFormat& err) {
		setLastErrorString(err.what());
		return false;
	}

	std::cout << "> Map loading time: " << (OTSYS_TIME() - start) / (1000.) << " seconds." << std::endl;
	return true;
}

bool IOMap::parseMapDataAttributes(OTB::Loader& loader, const OTB::Node& mapNode, Map& map,	const std::filesystem::path& fileName)
{
	PropStream propStream;
	if (!loader.getProps(mapNode, propStream)) {
		setLastErrorString("Could not read map data attributes.");
		return false;
	}

	uint8_t attribute;
	while (propStream.read<uint8_t>(attribute)) {
		switch (attribute) {
			case OTBM_ATTR_DESCRIPTION: {
				auto [mapDescription, ok] = propStream.readString();
				if (!ok) {
					setLastErrorString("Invalid description tag.");
					return false;
				}
				break;
			}
			case OTBM_ATTR_EXT_SPAWN_FILE: {
				auto [spawnFile, ok] = propStream.readString();
				if (!ok) {
					setLastErrorString("Invalid spawn tag.");
					return false;
				}

				map.spawnfile = fileName.parent_path() / spawnFile;
				break;
			}
			case OTBM_ATTR_EXT_HOUSE_FILE: {
				auto [houseFile, ok] = propStream.readString();
				if (!ok) {
					setLastErrorString("Invalid house tag.");
					return false;
				}

				map.housefile = fileName.parent_path() / houseFile;
				break;
			}
			default:
				setLastErrorString("Unknown header node.");
				return false;
		}
	}
	return true;
}

bool IOMap::parseTileArea(OTB::Loader& loader, const OTB::Node& tileAreaNode, Map& map)
{
	PropStream propStream;
	if (!loader.getProps(tileAreaNode, propStream)) {
		setLastErrorString("Invalid map node.");
		return false;
	}

	OTBM_Destination_coords area_coord;
	if (!propStream.read(area_coord)) {
		setLastErrorString("Invalid map node.");
		return false;
	}

	const uint16_t base_x = area_coord.x;
	const uint16_t base_y = area_coord.y;
	const uint16_t z = area_coord.z;

	for (const auto& tileNode : tileAreaNode.children | std::views::all) {
		if (tileNode.type != OTBM_TILE && tileNode.type != OTBM_HOUSETILE) {
			setLastErrorString("Unknown tile node.");
			return false;
		}

		if (!loader.getProps(tileNode, propStream)) {
			setLastErrorString("Could not read node data.");
			return false;
		}

		OTBM_Tile_coords tile_coord;
		if (!propStream.read(tile_coord)) {
			setLastErrorString("Could not read tile position.");
			return false;
		}

		uint16_t x = base_x + tile_coord.x;
		uint16_t y = base_y + tile_coord.y;

		bool isHouseTile = (tileNode.type == OTBM_HOUSETILE);
		TilePtr tile = nullptr;
		ItemPtr ground_item = nullptr;
		uint32_t tileflags = TILESTATE_NONE;

		if (isHouseTile) {
			uint32_t houseId;
			if (!propStream.read<uint32_t>(houseId)) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Could not read house id.", x, y, z));
				return false;
			}

			const auto house = map.houses.addHouse(houseId);
			if (!house) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Could not create house id: {:d}", x, y, z, houseId));
				return false;
			}
			
			auto houseTile = std::make_shared<Tile>(x, y, z, house);
			tile = houseTile;
			house->addTile(houseTile);
		}

		uint8_t attribute;
		//read tile attributes
		while (propStream.read<uint8_t>(attribute)) {
			switch (attribute) {
				case OTBM_ATTR_TILE_FLAGS: {
					uint32_t flags;
					if (!propStream.read<uint32_t>(flags)) {
						setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to read tile flags.", x, y, z));
						return false;
					}

					if ((flags & OTBM_TILEFLAG_PROTECTIONZONE) != 0) {
						tileflags |= TILESTATE_PROTECTIONZONE;
					} else if ((flags & OTBM_TILEFLAG_NOPVPZONE) != 0) {
						tileflags |= TILESTATE_NOPVPZONE;
					} else if ((flags & OTBM_TILEFLAG_PVPZONE) != 0) {
						tileflags |= TILESTATE_PVPZONE;
					}

					if ((flags & OTBM_TILEFLAG_NOLOGOUT) != 0) {
						tileflags |= TILESTATE_NOLOGOUT;
					}
					break;
				}

				case OTBM_ATTR_ITEM: {
					auto item = Item::CreateItem(propStream);
					if (!item) {
						setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to create item.", x, y, z));
						return false;
					}

					if (isHouseTile && item->isMoveable()) {
						std::cout << "[Warning - IOMap::loadMap] Moveable item with ID: " << item->getID() << ", at position [x: " << x << ", y: " << y << ", z: " << z << "]." << std::endl;;
					} else {
						if (item->getItemCount() == 0) {
							item->setItemCount(1);
						}

						if (tile) {
							tile->internalAddThing(item);
							item->startDecaying();
							item->setLoadedFromMap(true);
						} else if (item->isGroundTile()) {
							ground_item = std::move(item);
						} else {
							tile = createTile(ground_item, x, y, z);
							tile->internalAddThing(item);
							item->startDecaying();
							item->setLoadedFromMap(true);
						}
					}
					break;
				}

				default:
					setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Unknown tile attribute.", x, y, z));
					return false;
			}
		}

		for (const auto& itemNode : tileNode.children | std::views::all) {
			if (itemNode.type != OTBM_ITEM) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Unknown node type.", x, y, z));
				return false;
			}

			PropStream stream;
			if (!loader.getProps(itemNode, stream)) {
				setLastErrorString("Invalid item node.");
				return false;
			}

			auto item = Item::CreateItem(stream);
			if (!item) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to create item.", x, y, z));
				return false;
			}

			if (!item->unserializeItemNode(loader, itemNode, stream)) {
				setLastErrorString(fmt::format("[x:{:d}, y:{:d}, z:{:d}] Failed to load item {:d}.", x, y, z, item->getID()));
				return false;
			}

			if (isHouseTile && item->isMoveable()) {
				std::cout << "[Warning - IOMap::loadMap] Moveable item with ID: " << item->getID() << ", at position [x: " << x << ", y: " << y << ", z: " << z << "]." << std::endl;
			} else {
				if (item->getItemCount() == 0) {
					item->setItemCount(1);
				}

				if (tile) {
					tile->internalAddThing(item);
					item->startDecaying();
					item->setLoadedFromMap(true);
				} else if (item->isGroundTile()) {
					ground_item = std::move(item);
				} else {
					tile = createTile(ground_item, x, y, z);
					tile->internalAddThing(item);
					item->startDecaying();
					item->setLoadedFromMap(true);
				}
			}
		}

		if (!tile) {
			tile = createTile(ground_item, x, y, z);
		}

		tile->setFlag(static_cast<tileflags_t>(tileflags));

		map.setTile(x, y, z, tile);
	}
	return true;
}

bool IOMap::parseTowns(OTB::Loader& loader, const OTB::Node& townsNode, Map& map)
{
	for (const auto& townNode : townsNode.children | std::views::all) {
		PropStream propStream;
		if (townNode.type != OTBM_TOWN) {
			setLastErrorString("Unknown town node.");
			return false;
		}

		if (!loader.getProps(townNode, propStream)) {
			setLastErrorString("Could not read town data.");
			return false;
		}

		uint32_t townId;
		if (!propStream.read<uint32_t>(townId)) {
			setLastErrorString("Could not read town id.");
			return false;
		}

		auto town = map.towns.getTown(townId);
		if (!town) {
			town = new Town(townId);
			map.towns.addTown(townId, town);
		}

		auto [townName, ok] = propStream.readString();
		if (!ok) {
			setLastErrorString("Could not read town name.");
			return false;
		}

		town->setName(townName);

		OTBM_Destination_coords town_coords;
		if (!propStream.read(town_coords)) {
			setLastErrorString("Could not read town coordinates.");
			return false;
		}

		town->setTemplePos(Position(town_coords.x, town_coords.y, town_coords.z));
	}
	return true;
}

bool IOMap::parseWaypoints(OTB::Loader& loader, const OTB::Node& waypointsNode, Map& map)
{
	PropStream propStream;
	for (const auto& node : waypointsNode.children | std::views::all) {
		if (node.type != OTBM_WAYPOINT) {
			setLastErrorString("Unknown waypoint node.");
			return false;
		}

		if (!loader.getProps(node, propStream)) {
			setLastErrorString("Could not read waypoint data.");
			return false;
		}

		auto [name, ok] = propStream.readString();
		if (!ok) {
			setLastErrorString("Could not read waypoint name.");
			return false;
		}

		OTBM_Destination_coords waypoint_coords;
		if (!propStream.read(waypoint_coords)) {
			setLastErrorString("Could not read waypoint coordinates.");
			return false;
		}

		map.waypoints[std::string{ name }] = Position(waypoint_coords.x, waypoint_coords.y, waypoint_coords.z);
	}
	return true;
}

