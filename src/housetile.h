// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_HOUSETILE_H
#define FS_HOUSETILE_H

#include "tile.h"

class House;

class HouseTile final : public DynamicTile
{
	public:
		HouseTile(int32_t x, int32_t y, int32_t z, House* house);

		//cylinder implementations
		ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                     uint32_t flags, CreaturePtr actor = nullptr) override;

		CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem,
				uint32_t& flags) override; // another optional ref wrapper

		ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr actor = nullptr) override;

		void addThing(int32_t index, ThingPtr thing) override;
		void internalAddThing(uint32_t index, ThingPtr thing) override;

		House* getHouse() const {
			return house;
		}

		HouseTilePtr getHouseTilePtr() {
			return dynamic_shared_this<HouseTile>();
		}

		HouseTileConstPtr getHouseTileConstPtr() {
			return dynamic_shared_this<const HouseTile>();
		}

	private:
		void updateHouse(const ItemPtr& item);

		House* house;
};

#endif
