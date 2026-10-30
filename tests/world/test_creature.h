#pragma once

#include "creature.h"

namespace BlackTek::World::Tests
{
    class TestCreature final : public Creature
    {
    public:
        explicit TestCreature(CreatureSubType subtype = CreatureSubType::None, Position pos = {})
        {
            creature_subtype = subtype;
            position = pos;
        }

        std::string getDescription(int32_t) override { return name; }
        const std::string& getRegisteredName() const override { return name; }
        const std::string& getName() const override { return name; }
        const std::string& getNameDescription() const override { return name; }
        CreatureType_t getType(CreaturePtr = nullptr) const override { return CREATURETYPE_ATTACKABLE; }
        void setID() override {}
        void removeList() override {}
        void addList() override {}

    private:
        std::string name = "test creature";
    };
}
