// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_THING_H
#define FS_THING_H

#include "position.h"
#include <memory>
#include <string>
#include "sharedobject.h"


class Tile;
class Cylinder;
class Item;
class Creature;
class Container;

using ThingPtr = std::shared_ptr<class Thing>;
using ThingConstPtr = std::shared_ptr<const Thing>;

using TilePtr = std::shared_ptr<Tile>;
using TileConstPtr = std::shared_ptr<const Tile>;
using TileWeakPtr = std::weak_ptr<Tile>;

using CylinderPtr = std::shared_ptr<Cylinder>;
using CylinderConstPtr = std::shared_ptr<const Cylinder>;
using CylinderWeakPtr = std::weak_ptr<Cylinder>;

using ItemPtr = std::shared_ptr<Item>;
using ItemConstPtr = std::shared_ptr<const Item>;

using CreaturePtr = std::shared_ptr<Creature>;
using CreatureConstPtr = std::shared_ptr<const Creature>;

using ContainerPtr = std::shared_ptr<Container>;
using ContainerConstPtr = std::shared_ptr<const Container>;

class Thing
{
    public:
        virtual ~Thing() = default;

        // non-copyable
        Thing(const Thing&) = delete;
        Thing& operator=(const Thing&) = delete;

        // Factory method to create shared instances
        template<typename T, typename... Args>
        static std::shared_ptr<T> createThing(Args&&... args) {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        virtual std::string getDescription(int32_t lookDistance) const = 0;

        virtual CylinderPtr getParent() {
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
    
    protected:
        constexpr Thing() = default;
};

#endif