// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_THING_H
#define FS_THING_H

#include "position.h"
#include <memory>
#include <string>
#include "sharedobject.h"
#include "declarations.h"


enum class ThingSubType : uint8_t
{
	None,
	// Cylinder-only
	Tile,
	Virtual,
	// Item + Cylinder
	Container,
	DepotChest,
	DepotLocker,
	Inbox,
	StoreInbox,
	RewardChest,
	Teleport,
	TrashHolder,
	Mailbox,
	// Item-only
	Item,
	Door,
	MagicField,
	BedItem,
	HouseTransferItem,
	// Creature + Cylinder
	Player,
	// Creature-only
	Monster,
	Npc,
};

class Thing
{
    public:
        virtual ~Thing() = default;

        // non-copyable
        Thing(const Thing&) = delete;
        Thing& operator=(const Thing&) = delete;
       
        virtual std::string getDescription(int32_t lookDistance) = 0;

        virtual CylinderPtr getParent() {
            return nullptr;
        }

        virtual CylinderConstPtr getParent() const {
            return nullptr;
        }
    
        virtual CylinderPtr getRealParent() {
            return getParent();
        }

        virtual void setParent(std::weak_ptr<Cylinder> cylinder) {
            // Implementation in derived classes
        }

        virtual void clearParent() {

        }

        virtual TilePtr getTile();
        virtual TileConstPtr getTile() const;

        virtual const Position& getPosition() const;
        virtual int32_t getThrowRange() const = 0;
        virtual bool isPushable() const = 0;

        virtual CylinderPtr getCylinder() {
            return nullptr;
        }

        virtual CylinderConstPtr getCylinder() const {
            return nullptr;
        }

        virtual ContainerPtr getContainer() {
            return nullptr;
        }

        virtual ContainerConstPtr getContainer() const {
            return nullptr;
        }
    
        virtual ItemPtr getItem() {
            return nullptr;
        }
    
        virtual ItemConstPtr getItem() const {
            return nullptr;
        }
    
        virtual CreaturePtr getCreature() {
            return nullptr;
        }
    
        virtual CreatureConstPtr getCreature() const {
            return nullptr;
        }

        virtual bool isRemoved() const {
            return true;
        }

        bool is_player() const { return thing_subtype == ThingSubType::Player; }
        bool is_monster() const { return thing_subtype == ThingSubType::Monster; }
        bool is_npc() const { return thing_subtype == ThingSubType::Npc; }
        bool is_creature() const { return is_player() or is_monster() or is_npc(); }
        bool is_item() const { return thing_subtype == ThingSubType::Item; }
        bool is_door() const { return thing_subtype == ThingSubType::Door; }
        bool is_magic_field() const { return thing_subtype == ThingSubType::MagicField; }
        bool is_bed_item() const { return thing_subtype == ThingSubType::BedItem; }
        bool is_house_transfer_item() const { return thing_subtype == ThingSubType::HouseTransferItem; }
        bool is_container() const { return thing_subtype == ThingSubType::Container; }
        bool is_depot_chest() const { return thing_subtype == ThingSubType::DepotChest; }
        bool is_depot_locker() const { return thing_subtype == ThingSubType::DepotLocker; }
        bool is_inbox() const { return thing_subtype == ThingSubType::Inbox; }
        bool is_store_inbox() const { return thing_subtype == ThingSubType::StoreInbox; }
        bool is_reward_chest() const { return thing_subtype == ThingSubType::RewardChest; }
        bool is_teleport() const { return thing_subtype == ThingSubType::Teleport; }
        bool is_trash_holder() const { return thing_subtype == ThingSubType::TrashHolder; }
        bool is_mailbox() const { return thing_subtype == ThingSubType::Mailbox; }
        bool is_tile() const { return thing_subtype == ThingSubType::Tile; }
        bool is_virtual_type() const { return thing_subtype == ThingSubType::Virtual; }
        
        ThingSubType getThingSubType() const {
            return thing_subtype;
        }

        // BlackTek Instance System
        uint32_t getInstanceID() const { return instanceID; }
        void setInstanceID(uint32_t id) { instanceID = id; }
        bool compareInstance(uint32_t id) const { return instanceID == id; }

    protected:
        constexpr Thing() = default;
        ThingSubType thing_subtype = ThingSubType::None;
    private:
        // BlackTek Instance System
        uint32_t instanceID = 0; // 0 = global instance
};

#endif