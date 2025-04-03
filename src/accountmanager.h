// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.
#ifndef FS_ACCOUNT_MANAGER_H
#define FS_ACCOUNT_MANAGER_H

#include <vector>

struct CharacterOption {
    // idea, maybe add groups here later? and even guilds? 
    // to enhance possibilities for accounts with VIP or registered to a guild or faction ect..
    int32_t vocation = 0;
    bool premium = false;
    bool allowTowns = true;
    bool needsPosition = true;
    std::vector<int32_t> town_list;
    // this struct needs the data types updated with the size of the map
    struct { int x; int y; int z; } startingPos; // alternative to towns
    // using magic number here, do this a better way
    int32_t outfit[8] = { 0,0,0,0,0,0,0 };
    int32_t skills[7] = { 1,1,1,1,1,1,1 };
    uint32_t magiclevel;
    // todo : add support for configuring starting items 
    // int32_t helmet;
    // int32_t armor;
    // int32_t amulet;
    // int32_t backpack; ect, ect.
    // 
    // ItemID and Count
    // std::vector<int32_t, int32_t> extra_items;
};

namespace AccountManager
{
    static constexpr int32_t ID = 1;
    static constexpr auto NAME = "Account Manager";
    static constexpr auto AUTH = "1"; // make this one the config.lua option
    static constexpr auto toml_path = "/config/accountmanager.toml";
    void initialize();
}

#endif