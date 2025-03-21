// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_CYLINDER_H
#define FS_CYLINDER_H

#include "enums.h"
#include "thing.h"

#include <btree.hpp>

static constexpr int32_t INDEX_WHEREEVER = -1;

enum cylinderflags_t {
	FLAG_NOLIMIT = 1 << 0, //Bypass limits like capacity/container limits, blocking items/creatures etc.
	FLAG_IGNOREBLOCKITEM = 1 << 1, //Bypass movable blocking item checks
	FLAG_IGNOREBLOCKCREATURE = 1 << 2, //Bypass creature checks
	FLAG_CHILDISOWNER = 1 << 3, //Used by containers to query capacity of the carrier (player)
	FLAG_PATHFINDING = 1 << 4, //An additional check is done for floor changing/teleport items
	FLAG_IGNOREFIELDDAMAGE = 1 << 5, //Bypass field damage checks
	FLAG_IGNORENOTMOVEABLE = 1 << 6, //Bypass check for mobility
	FLAG_IGNOREAUTOSTACK = 1 << 7, //queryDestination will not try to stack items together
};

enum cylinderlink_t {
	LINK_OWNER,
	LINK_PARENT,
	LINK_TOPPARENT,
	LINK_NEAR,
};

class Cylinder : virtual public Thing
{
	public:
		/**
		  * Query if the cylinder can add an object
		  * \param index points to the destination index (inventory slot/container position)
			* -1 is a internal value and means add to a empty position, with no destItem
		  * \param thing the object to move/add
		  * \param count is the amount that we want to move/add
		  * \param flags if FLAG_CHILDISOWNER if set the query is from a child-cylinder (check cap etc.)
			* if FLAG_NOLIMIT is set blocking items/container limits is ignored
		  * \param actor the creature trying to add the thing
		  * \returns ReturnValue holds the return value
		  */
		virtual ReturnValue queryAdd(int32_t index, const ThingPtr& thing, uint32_t count,
		                             uint32_t flags, CreaturePtr actor = nullptr) = 0;

		/**
		  * Query the cylinder how much it can accept
		  * \param index points to the destination index (inventory slot/container position)
			* -1 is a internal value and means add to a empty position, with no destItem
		  * \param thing the object to move/add
		  * \param count is the amount that we want to move/add
		  * \param maxQueryCount is the max amount that the cylinder can accept
		  * \param flags optional flags to modify the default behaviour
		  * \returns ReturnValue holds the return value
		  */
		virtual ReturnValue queryMaxCount(int32_t index, const ThingPtr& thing, uint32_t count, uint32_t& maxQueryCount,
				uint32_t flags) = 0;

		/**
		  * Query if the cylinder can remove an object
		  * \param thing the object to move/remove
		  * \param count is the amount that we want to remove
		  * \param flags optional flags to modify the default behaviour
		  * \returns ReturnValue holds the return value
		  */
		virtual ReturnValue queryRemove(const ThingPtr& thing, uint32_t count, uint32_t flags, CreaturePtr = nullptr) = 0;

		/**
		  * Query the destination cylinder
		  * \param index points to the destination index (inventory slot/container position),
			* -1 is a internal value and means add to a empty position, with no destItem
			* this method can change the index to point to the new cylinder index
		  * \param destItem is the destination object
		  * \param flags optional flags to modify the default behaviour
			* this method can modify the flags
		  * \returns Cylinder returns the destination cylinder
		  */
		virtual CylinderPtr queryDestination(int32_t& index, const ThingPtr& thing, ItemPtr& destItem, // maybe make optional here?
				uint32_t& flags) = 0;

		/**
		  * Add the object to the cylinder
		  * \param thing is the object to add
		  */
		virtual void addThing(ThingPtr thing) = 0;

		/**
		  * Add the object to the cylinder
		  * \param index points to the destination index (inventory slot/container position)
		  * \param thing is the object to add
		  */
		virtual void addThing(int32_t index, ThingPtr thing) = 0;

		/**
		  * Update the item count or type for an object
		  * \param thing is the object to update
		  * \param itemId is the new item id
		  * \param count is the new count value
		  */
		virtual void updateThing(ThingPtr thing, uint16_t itemId, uint32_t count) = 0;

		/**
		  * Replace an object with a new
		  * \param index is the position to change (inventory slot/container position)
		  * \param thing is the object to update
		  */
		virtual void replaceThing(uint32_t index, ThingPtr thing) = 0;

		/**
		  * Remove an object
		  * \param thing is the object to delete
		  * \param count is the new count value
		  */
		virtual void removeThing(ThingPtr thing, uint32_t count) = 0;

		/**
		  * Is sent after an operation (move/add) to update internal values
		  * \param thing is the object that has been added
		  * \param index is the objects new index value
		  * \param link holds the relation the object has to the cylinder
		  */
		virtual void postAddNotification(ThingPtr thing, CylinderPtr oldParent, int32_t index, cylinderlink_t link = LINK_OWNER) = 0;

		/**
		  * Is sent after an operation (move/remove) to update internal values
		  * \param thing is the object that has been removed
		  * \param index is the previous index of the removed object
		  * \param link holds the relation the object has to the cylinder
		  */
		virtual void postRemoveNotification(ThingPtr thing, CylinderPtr newParent, int32_t index, cylinderlink_t link = LINK_OWNER) = 0;

		/**
		  * Gets the index of an object
		  * \param thing the object to get the index value from
		  * \returns the index of the object, returns -1 if not found
		  */
		virtual int32_t getThingIndex(ThingPtr thing);

		/**
		  * Returns the first index
		  * \returns the first index, if not implemented 0 is returned
		  */
		virtual size_t getFirstIndex() const;

		/**
		  * Returns the last index
		  * \returns the last index, if not implemented 0 is returned
		  */
		virtual size_t getLastIndex() const;

		/**
		  * Gets the object based on index
		  * \returns the object, returns nullptr if not found
		  */
		virtual ThingPtr getThing(size_t index);

		/**
		  * Get the amount of items of a certain type
		  * \param itemId is the item type to the get the count of
		  * \param subType is the extra type an item can have such as charges/fluidtype, -1 means not used
		  * \returns the amount of items of the asked item type
		  */
		virtual uint32_t getItemTypeCount(uint16_t itemId, int32_t subType = -1) const;

		/**
		  * Get the amount of items of a all types
		  * \param countMap a map to put the itemID:count mapping in
		  * \returns a map mapping item id to count (same as first argument)
		  */
		virtual gtl::btree_map<uint32_t, uint32_t>& getAllItemTypeCount(gtl::btree_map<uint32_t, uint32_t>& countMap) const;

		/**
		  * Adds an object to the cylinder without sending to the client(s)
		  * \param thing is the object to add
		  */
		virtual void internalAddThing(ThingPtr thing);

		/**
		  * Adds an object to the cylinder without sending to the client(s)
		  * \param thing is the object to add
		  * \param index points to the destination index (inventory slot/container position)
		  */
		virtual void internalAddThing(uint32_t index, ThingPtr thing);

		virtual void startDecaying();
};

class VirtualCylinder;
using VirtualCylinderPtr = std::shared_ptr<VirtualCylinder>;
using VirtualCylinderConstPtr = std::shared_ptr<const VirtualCylinder>;

class VirtualCylinder final : public Cylinder
{
	public:
		static VirtualCylinderPtr virtualCylinder;

		virtual ReturnValue queryAdd(int32_t, const ThingPtr&, uint32_t, uint32_t, CreaturePtr = nullptr) override {
			return RETURNVALUE_NOTPOSSIBLE;
		}
	
		virtual ReturnValue queryMaxCount(int32_t, const ThingPtr&, uint32_t, uint32_t&, uint32_t) override {
			return RETURNVALUE_NOTPOSSIBLE;
		}
	
		virtual ReturnValue queryRemove(const ThingPtr&, uint32_t, uint32_t, CreaturePtr = nullptr ) override {
			return RETURNVALUE_NOTPOSSIBLE;
		}
	
		virtual CylinderPtr queryDestination(int32_t&, const ThingPtr&, ItemPtr&, uint32_t&) override {
			return nullptr;
		}

		virtual void addThing(ThingPtr) override {}
		virtual void addThing(int32_t, ThingPtr) override {}
		virtual void updateThing(ThingPtr, uint16_t, uint32_t) override {}
		virtual void replaceThing(uint32_t, ThingPtr) override {}
		virtual void removeThing(ThingPtr, uint32_t) override {}

		virtual void postAddNotification(ThingPtr,  CylinderPtr, int32_t, cylinderlink_t = LINK_OWNER) override {}
		virtual void postRemoveNotification(ThingPtr,  CylinderPtr, int32_t, cylinderlink_t = LINK_OWNER) override {}

		bool isPushable() const override {
			return false;
		}
	
		int32_t getThrowRange() const override {
			return 1;
		}
	
		std::string getDescription(int32_t) const override {
			return {};
		}
	
		bool isRemoved() const override {
			return false;
		}
};



#endif
