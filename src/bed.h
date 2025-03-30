// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_BED_H
#define FS_BED_H

#include "item.h"

class House;
class Player;

class BedItem final : public Item
{
	public:
		explicit BedItem(uint16_t id);

		BedItemPtr getBed() override {
			return static_shared_this<BedItem>();
		}
	
		BedItemConstPtr getBed() const override {
			return static_shared_this<const BedItem>();
		}

		Attr_ReadValue readAttr(AttrTypes_t attr, PropStream& propStream) override;
		void serializeAttr(PropWriteStream& propWriteStream) const override;

		bool canRemove() const override {
			return house == nullptr;
		}

		uint32_t getSleeper() const {
			return sleeperGUID;
		}

		House* getHouse() const {
			return house;
		}
	
		void setHouse(House* h) {
			house = h;
		}

		bool canUse(PlayerPtr player) const;

		bool trySleep(const PlayerPtr& player);
		bool sleep(const PlayerPtr& player);
		void wakeUp(const PlayerPtr& player);

		BedItemPtr getNextBedItem() const;

	private:
		void updateAppearance(const PlayerConstPtr& player);
		void regeneratePlayer(const PlayerPtr& player) const;
		void internalSetSleeper(const PlayerConstPtr& player);
		void internalRemoveSleeper();

		House* house = nullptr;
		uint64_t sleepStart;
		uint32_t sleeperGUID;
};

#endif
