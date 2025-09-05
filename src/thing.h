// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_THING_H
#define FS_THING_H

#include "position.h"
#include <memory>
#include <string>
#include "sharedobject.h"
#include "declarations.h"


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