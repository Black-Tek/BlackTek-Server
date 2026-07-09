// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include "declarations.h"
#include "enums.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class LuaScriptInterface;

namespace BlackTek {

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

    bool          registerWindow(StoreWindow* window);
    StoreWindow*  getWindowForAccountType(AccountType_t type) const;
    void         clear();

private:
    StoreManager() = default;
    std::unordered_map<uint8_t, StoreWindow*> windows;
};

} // namespace BlackTek

#define g_storeManager (BlackTek::StoreManager::getInstance())
