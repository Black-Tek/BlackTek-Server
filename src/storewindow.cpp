// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "storewindow.h"
#include "luascript.h"
#include "console.h"

namespace BlackTek {

// ---------------------------------------------------------------------------
// StoreCategory
// ---------------------------------------------------------------------------

void StoreCategory::addProduct(
    uint32_t            id,
    const std::string&  name,
    uint32_t            price,
    const std::string&  icon,
    const std::string&  description)
{
    StoreProduct p;
    p.id          = id;
    p.name        = name;
    p.price       = price;
    p.description = description;
    p.icons       = { icon };
    products.emplace_back(std::move(p));
}

const StoreProduct* StoreCategory::getProductById(uint32_t id) const
{
    for (const auto& p : products)
    {
        if (p.id == id)
        {
            return &p;
        }
    }
    return nullptr;
}

bool StoreCategory::executePurchase(
    const PlayerPtr&    player,
    uint32_t            productId,
    uint8_t             offerType,
    const std::string&  param) const
{
    if (onPurchaseScriptId == -1 or not scriptInterface)
    {
        return false;
    }

    if (not scriptInterface->reserveScriptEnv())
    {
        BlackTek::Console::Error("[StoreCategory::executePurchase] Call stack overflow");
        return false;
    }

    ScriptEnvironment* env = scriptInterface->getScriptEnv();
    env->setScriptId(onPurchaseScriptId, scriptInterface);

    lua_State* L = scriptInterface->getLuaState();
    scriptInterface->pushFunction(onPurchaseScriptId);

    LuaScriptInterface::pushSharedPtr(L, player);
    LuaScriptInterface::setMetatable(L, -1, "Player");
    lua_pushinteger(L, static_cast<lua_Integer>(productId));
    lua_pushinteger(L, static_cast<lua_Integer>(offerType));
    LuaScriptInterface::pushString(L, param);

    return scriptInterface->callFunction(4);
}

bool StoreCategory::executeCanPurchase(
    const PlayerPtr&    player,
    uint32_t            productId,
    std::string&        outReason) const
{
    if (canPurchaseScriptId == -1 or not scriptInterface)
    {
        return true;
    }

    if (not scriptInterface->reserveScriptEnv())
    {
        BlackTek::Console::Error("[StoreCategory::executeCanPurchase] Call stack overflow");
        return true;
    }

    ScriptEnvironment* env = scriptInterface->getScriptEnv();
    env->setScriptId(canPurchaseScriptId, scriptInterface);

    lua_State* L = scriptInterface->getLuaState();
    scriptInterface->pushFunction(canPurchaseScriptId);

    LuaScriptInterface::pushSharedPtr(L, player);
    LuaScriptInterface::setMetatable(L, -1, "Player");
    lua_pushinteger(L, static_cast<lua_Integer>(productId));

    if (LuaScriptInterface::protectedCall(L, 2, 2) != 0)
    {
        LuaScriptInterface::reportError(nullptr, LuaScriptInterface::getString(L, -1));
        lua_pop(L, 1);
        scriptInterface->resetScriptEnv();
        return true;
    }

    bool allowed = LuaScriptInterface::getBoolean(L, -2, true);
    if (not allowed)
    {
        outReason = LuaScriptInterface::getString(L, -1);
    }
    lua_pop(L, 2);
    scriptInterface->resetScriptEnv();
    return allowed;
}

// ---------------------------------------------------------------------------
// StoreWindow
// ---------------------------------------------------------------------------

StoreWindow::StoreWindow(std::string title)
    : title(std::move(title))
{}

StoreCategory* StoreWindow::addCategory(const std::string& name, const std::string& icon)
{
    auto cat    = std::make_unique<StoreCategory>();
    cat->name   = name;
    cat->icon   = icon;
    auto* ptr   = cat.get();
    categories.emplace_back(std::move(cat));
    return ptr;
}

StoreCategory* StoreWindow::getCategoryByName(const std::string& name)
{
    for (const auto& cat : categories)
    {
        if (cat->name == name)
        {
            return cat.get();
        }
    }
    return nullptr;
}

void StoreWindow::executeOnOpen(const PlayerPtr& player) const
{
    if (onOpenScriptId == -1 or not scriptInterface)
    {
        return;
    }

    if (not scriptInterface->reserveScriptEnv())
    {
        BlackTek::Console::Error("[StoreWindow::executeOnOpen] Call stack overflow");
        return;
    }

    ScriptEnvironment* env = scriptInterface->getScriptEnv();
    env->setScriptId(onOpenScriptId, scriptInterface);

    lua_State* L = scriptInterface->getLuaState();
    scriptInterface->pushFunction(onOpenScriptId);

    LuaScriptInterface::pushSharedPtr(L, player);
    LuaScriptInterface::setMetatable(L, -1, "Player");

    scriptInterface->callVoidFunction(1);
}

// ---------------------------------------------------------------------------
// StoreManager
// ---------------------------------------------------------------------------

StoreManager& StoreManager::getInstance()
{
    static StoreManager instance;
    return instance;
}

bool StoreManager::registerWindow(StoreWindow* window)
{
    if (not window)
    {
        return false;
    }
    windows[static_cast<uint8_t>(window->getRequiredAccountType())] = window;
    return true;
}

StoreWindow* StoreManager::getWindowForAccountType(AccountType_t type) const
{
    auto it = windows.find(static_cast<uint8_t>(type));
    if (it != windows.end())
    {
        return it->second;
    }
    // Fall back to the ACCOUNT_TYPE_NORMAL window so privileged accounts
    // (GOD, GM, etc.) still see the default store when no dedicated window
    // is registered for their account type.
    auto fallback = windows.find(static_cast<uint8_t>(ACCOUNT_TYPE_NORMAL));
    if (fallback != windows.end())
    {
        return fallback->second;
    }
    return nullptr;
}

void StoreManager::clear()
{
    for (auto& [key, window] : windows)
    {
        delete window;
    }
    windows.clear();
}

} // namespace BlackTek
