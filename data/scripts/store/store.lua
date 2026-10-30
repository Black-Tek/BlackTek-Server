-- Player game store
-- Loaded automatically by the revscript system from /data/scripts/

local store = StoreWindow("Black Tek Store")

store:accountType(ACCOUNT_TYPE_NORMAL)
store:coins(0, 0)

-- onOpen fires every time a player opens the store; use it to refresh
-- the displayed coin balance from the player's actual account data.
store:onOpen(function(player)
    -- Replace with real coin-balance lookup once coin storage is added.
    store:setCoins(0, 0)
end)

-- ---- Blessings category -----------------------------------------------

local blessings = store:category("Blessings", "Category_Blessings.png")

blessings:product(4200, "Death Redemption",    100, "Death_Redemption.png",    "Protects you from losing items upon death.")
blessings:product(4201, "Adventurer's Blessing", 80, "Adv_Blessings.png",       "A bundle of the five standard blessings.")

blessings:canPurchase(function(player, productId)
    -- Product prices are display metadata. The purchase callbacks must check
    -- and debit a real currency balance before this example can accept orders.
    return false, "Store purchases are not configured. Please contact the server administrator."
end)

blessings:onPurchase(function(player, productId, offerType, param)
    -- Keep reward delivery disabled until payment is implemented as well.
    return false
end)

-- ---- Register ---------------------------------------------------------

store:register()
