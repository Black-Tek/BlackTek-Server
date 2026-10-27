/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * This file is an original BlackTek contribution, created since this project's
 * inception, and contains no code inherited from The Forgotten Server.
 * It is licensed solely under the MIT License (see the licensing/License file
 * in the project root for the full text).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef FS_ACCOUNT_MANAGER_H
#define FS_ACCOUNT_MANAGER_H

#include <vector>
#include <string>

#include "declarations.h"

struct CharacterOption {
    // idea, maybe add groups here later? and even guilds? 
    // to enhance possibilities for accounts with VIP or registered to a guild or faction ect..
    int32_t id = 0; // internal use only
    std::string name = "";
    int32_t vocation = 0;
    bool sex = false;
    uint32_t level;
    uint32_t baseHealth;
    uint32_t baseMana;
    uint32_t baseCapacity;
    bool premium = false;
    bool showPremiumOption = false;
    bool allowTowns = true;
    bool needsPosition = true;
    std::vector<int32_t> town_list;
    // this struct needs the data types updated with the size of the map
    struct { int32_t x; int32_t y; int32_t z; } startingPos; // alternative to towns
    // using magic number here, do this a better way
    int32_t outfit[8] = { 0,0,0,0,0,0,0 };
    int32_t skills[7] = { 1,1,1,1,1,1,1 };
    uint32_t magiclevel = 0;
    // todo : add support for configuring starting items 
    // int32_t helmet;
    // int32_t armor;
    // int32_t amulet;
    // int32_t backpack; ect, ect.
    // 
    // ItemID and Count
    // std::vector<int32_t, int32_t> extra_items;
};

enum ButtonID : uint8_t {
    NO_ROLE,
    PRIMARY,
    SECONDARY,
    TERTIARY,
    QUATERNY
};

enum ChoiceID : uint8_t {
    NO_CHOICE,
    FIRST,
    SECOND,
    THIRD,
    FOURTH
};

class AccountManager
{
public:
    // No Constructors! Purely static class.
    AccountManager() = delete;
    ~AccountManager() = delete;
    AccountManager(const AccountManager&) = delete;
    AccountManager& operator=(const AccountManager&) = delete;
    AccountManager(AccountManager&&) = delete;
    AccountManager& operator=(AccountManager&&) = delete;

    enum Buttons : uint8_t {
        DEFAULT_NONE,
        DEFAULT_YES,
        DEFAULT_NO,
        DEFAULT_RESET,

        DEFAULT_ACCEPT = DEFAULT_YES,
        DEFAULT_DECLINE = DEFAULT_NO
    };


    enum TextBox : uint8_t {
        NO_TEXT_WINDOW,
        ACCOUNT_NAME_TEXT_BOX,
        PASSWORD_TEXT_BOX,
        CONFIRMATION_TEXT_BOX,
        CHARACTER_NAME_TEXT_BOX,
        PASSWORD_RESET,
        PASSWORD_RESET_CONFIRM
    };

    enum Window : uint8_t {
        NO_INPUT_WINDOW,
        COMMON_LOGIN,
        COMMON_ACCOUNT,
        COMMON_PASSWORD,
        COMMON_CONFIRMATION,
        COMMON_SUCCESS,
        COMMON_ACCOUNT_FAILED,
        COMMON_PASSWORD_FAILED,
        COMMON_CONFIRMATION_FAILED,
        COMMON_CANCEL,

        PRIVATE_LOGIN,
        PRIVATE_CHARACTER_VOCATION,
        PRIVATE_CHARACTER_TOWN,
        PRIVATE_CHARACTER,
        PRIVATE_CHARACTER_FAILED,
        PRIVATE_CHARACTER_PREMIUM_REQUIRED,
        PRIVATE_CHARACTER_CONFIRMATION,
        PRIVATE_CHARACTER_SUCCESS,

        PRIVATE_MAIN_MENU,
        PRIVATE_ACCOUNT,
        PRIVATE_ACCOUNT_FAILED,
        PRIVATE_ACCOUNT_SUCCESS,
        PRIVATE_PASSWORD_WINDOW,
        PRIVATE_PASSWORD_CONFIRMATION,
        PRIVATE_PASSWORD_SUCCESS,
        PRIVATE_PASSWORD_FAILED,
        PRIVATE_PASSWORD_MISMATCH,
        PRIVATE_PASSWORD_RESET,
        PRIVATE_PASSWORD_RESET_CONFIRMATION,
        PRIVATE_PASSWORD_RESET_SUCCESS,
        PRIVATE_PASSWORD_RESET_FAILED,
        PRIVATE_PASSWORD_RESET_MISMATCH
    };


    static void initialize();
    static constexpr int32_t ID = 1;
    static constexpr auto NAME = "Account Manager";
    static constexpr auto AUTH = "1"; // make this one the config.lua option
    static constexpr auto toml_path = "/config/accountmanager.toml";
};

#endif
