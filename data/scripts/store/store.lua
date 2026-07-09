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
    -- Example gate: block re-purchase of Death Redemption if already owned.
    if productId == 4200 and player:hasBlessing(5) then
        return false, "You already have Death Redemption."
    end
    return true
end)

blessings:onPurchase(function(player, productId, offerType, param)
    if productId == 4200 then
        player:addBlessing(5)
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "You received Death Redemption.")
        return true
    elseif productId == 4201 then
        for i = 1, 5 do
            player:addBlessing(i)
        end
        player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "You received the Adventurer's Blessing.")
        return true
    end
    return false
end)

-- ---- Register ---------------------------------------------------------

store:register()
