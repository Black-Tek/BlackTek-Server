// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_IOMAPSERIALIZE_H
#define FS_IOMAPSERIALIZE_H

#include "database.h"
#include "map.h"
#include "house.h"

class IOMapSerialize
{
	public:
		static void loadHouseItems(Map* map);
		static bool saveHouseItems();
		static bool loadHouseInfo();
		static bool saveHouseInfo();

		static bool saveHouse(House* house);

	private:
		static void saveItem(PropWriteStream& stream, const ItemPtr& item);
		static void saveTile(PropWriteStream& stream, const TilePtr& tile);

		static bool loadContainer(PropStream& propStream, const ContainerPtr& container);
		static bool loadItem(PropStream& propStream, const CylinderPtr& parent);
};

#endif
