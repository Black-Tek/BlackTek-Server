local handlers = require("data/scripts/store/handlers")
local events = require("data/scripts/store/events")
local offers = require("data/scripts/store/offers")

local DONATION_URL = os.getenv("DONATION_URL") or ""
local CODE_GAMESTORE = 102
local GAME_STORE = nil

local LoginEvent = CreatureEvent("GameStoreLogin")
local ExtendedEvent = CreatureEvent("GameStoreExtended")

function LoginEvent.onLogin(player)
    player:registerEvent("GameStoreExtended")
    return true
end

function gameStoreInitialize()
    GAME_STORE = {
        categories = {},
        offers = {}
    }

    -- Items
    offers.addCategory(GAME_STORE, "Items", "Tools, Dolls & Boxes.", "item", 2331)
    offers.addItem(GAME_STORE, "Items", "VIP Medal (30 days)",
        "Earn 10% more exp, Access To Gambling\nAnd Gold Dump(Obtainable ingame)", 10135, 1, 200)
    offers.addItem(GAME_STORE, "Items", "Outfit Doll", "Use this to obtain one outfit with full Addons.", 8982, 1, 300)
    offers.addItem(GAME_STORE, "Items", "Gender Doll", "Can be used to change your gender.", 13581, 1, 200)
    offers.addItem(GAME_STORE, "Items", "Name Doll", "Can be used to change your name.", 12666, 1, 400)
    offers.addItem(GAME_STORE, "Items", "Frag Remover", "Use this to remove your frags & Red/Black Skull.", 16105, 1,
        500)
    offers.addItem(GAME_STORE, "Items", "Stamina Refiller", "Can be used to fully recharge your stamina.", 21705, 1, 400)
    offers.addItem(GAME_STORE, "Items", "Squeezing Gear of Girlpower", "Multitool", 10513, 1, 200)

    -- Mounts
    offers.addCategory(GAME_STORE, "Mounts", "Giddy up", "mount", 426)
    offers.addMount(GAME_STORE, "Mounts", "Blackpelt", "Wont be taking any damage with that armor..", 58, 651, 400)
    offers.addMount(GAME_STORE, "Mounts", "Doombringer", "Shall curse your enemies with doom.", 53, 644, 350)
    offers.addMount(GAME_STORE, "Mounts", "Walker", "Mechanical experiment.", 43, 606, 300)
    offers.addMount(GAME_STORE, "Mounts", "Ladybug", "Lets out a mighty roar.", 27, 447, 300)

    -- Equipment
    offers.addCategory(GAME_STORE, "Equipment", "Beginner Item Sets.", "item", 2331)
    offers.addItem(GAME_STORE, "Equipment", "Dwarven Legs", "", 2504, 1, 5)
    offers.addItem(GAME_STORE, "Equipment", "Mastermind Shield", "", 2514, 1, 5)
    offers.addItem(GAME_STORE, "Equipment", "Boots of Haste", "", 2195, 1, 5)
    offers.addItem(GAME_STORE, "Equipment", "Demon Helmet", "", 2493, 1, 5)
    offers.addItem(GAME_STORE, "Equipment", "Demon Armour", "", 2494, 1, 5)

    offers.addItem(GAME_STORE, "Equipment", "Mage Starter Box", "Contains a full mage beginner set.", 29223, 1, 500)
    offers.addItem(GAME_STORE, "Equipment", "Paladin Starter Box", "Contains a full paladin beginner set.", 29224, 1,
        500)
    offers.addItem(GAME_STORE, "Equipment", "Knight Starter Box", "Contains a full knight beginner set.", 29227, 1, 500)

    -- Outfits
    offers.addCategory(GAME_STORE, "Outfits", "Contains all addons.", "outfit", {
        mount = 0,
        type = 577,
        addons = 3,
        head = 9,
        body = 126,
        legs = 124,
        feet = 116
    })
    offers.addOutfit(GAME_STORE, "Outfits", "Dream Warden Outfit", "Dream Warden", {
        mount = 0,
        type = 577,
        addons = 3,
        head = 9,
        body = 126,
        legs = 124,
        feet = 116
    }, {
        mount = 0,
        type = 578,
        addons = 3,
        head = 9,
        body = 126,
        legs = 124,
        feet = 116
    }, 400)

    offers.addOutfit(GAME_STORE, "Outfits", "Beggar Outfit", "Beggar", {
        mount = 0,
        type = 153,
        addons = 3,
        head = 9,
        body = 126,
        legs = 124,
        feet = 116
    }, {
        mount = 0,
        type = 157,
        addons = 3,
        head = 9,
        body = 126,
        legs = 124,
        feet = 116
    }, 400)
end

function ExtendedEvent.onExtendedOpcode(player, opcode, buffer)
    if opcode == CODE_GAMESTORE then
        if not GAME_STORE then
            gameStoreInitialize()
            events.refreshPlayersPoints(CODE_GAMESTORE)
        end

        local status, json_data = pcall(function()
            return json.decode(buffer)
        end)
        if not status then
            return
        end

        local action = json_data.action
        local data = json_data.data
        if not action or not data then
            return
        end

        if action == "fetch" then
            handlers.gameStoreFetch(player, GAME_STORE, DONATION_URL, CODE_GAMESTORE)
        elseif action == "purchase" then
            handlers.gameStorePurchase(player, data, GAME_STORE, CODE_GAMESTORE)
        elseif action == "gift" then
            handlers.gameStorePurchaseGift(player, data, GAME_STORE, CODE_GAMESTORE)
        end
    end
end

LoginEvent:type("login")
LoginEvent:register()
ExtendedEvent:type("extendedopcode")
ExtendedEvent:register()
