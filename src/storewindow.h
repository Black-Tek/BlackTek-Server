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

#pragma once

#include "declarations.h"
#include "enums.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class LuaScriptInterface;

namespace BlackTek {

class StoreWindow;
using StoreWindowPtr = std::shared_ptr<StoreWindow>;

struct StoreProduct
{
    uint32_t    id          = 0;
    uint32_t    price       = 0;
    uint8_t     state       = 0;
    bool        enabled     = true;
    std::string name;
    std::string description;
    std::vector<std::string> icons;
};

class StoreCategory
{
public:
    void addProduct(uint32_t id, const std::string& name, uint32_t price,
                    const std::string& icon, const std::string& description = "");
    const StoreProduct* getProductById(uint32_t id) const;

    bool executePurchase(const PlayerPtr& player, uint32_t productId,
                         uint8_t offerType, const std::string& param) const;
    bool executeCanPurchase(const PlayerPtr& player, uint32_t productId,
                            std::string& outReason) const;

    std::string name;
    std::string parentName;
    bool        highlighted = false;
    std::string icon;
    std::vector<StoreProduct> products;

    int32_t              onPurchaseScriptId  = -1;
    int32_t              canPurchaseScriptId = -1;
    LuaScriptInterface*  scriptInterface     = nullptr;
};

class StoreWindow
{
public:
    explicit StoreWindow(std::string title);
    ~StoreWindow() = default;

    void            setAccountType(AccountType_t type)      { accountType = type; }
    void            setCoins(uint32_t bal, uint32_t trans)  { coins = bal; transferableCoins = trans; }
    StoreCategory*  addCategory(const std::string& name, const std::string& icon);
    StoreCategory*  getCategoryByName(const std::string& name);

    const std::string& getTitle() const                                     { return title; }
    AccountType_t      getRequiredAccountType() const                       { return accountType; }
    uint32_t           getCoins() const                                     { return coins; }
    uint32_t           getTransferableCoins() const                         { return transferableCoins; }
    const std::vector<std::unique_ptr<StoreCategory>>& getCategories() const { return categories; }

    void executeOnOpen(const PlayerPtr& player) const;

    bool fromLua = false;

    int32_t              onOpenScriptId = -1;
    LuaScriptInterface*  scriptInterface = nullptr;

private:
    std::string    title;
    AccountType_t  accountType      = ACCOUNT_TYPE_NORMAL;
    uint32_t       coins            = 0;
    uint32_t       transferableCoins = 0;
    std::vector<std::unique_ptr<StoreCategory>> categories;
};

class StoreManager
{
public:
    static StoreManager& getInstance();

    bool          registerWindow(const StoreWindowPtr& window);
    StoreWindowPtr getWindowForAccountType(AccountType_t type) const;
    void         clear();

private:
    StoreManager() = default;
    std::unordered_map<uint8_t, StoreWindowPtr> windows;
    mutable std::mutex windows_mutex;
};

} // namespace BlackTek

#define g_storeManager (BlackTek::StoreManager::getInstance())
