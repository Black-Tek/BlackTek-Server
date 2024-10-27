--- File is called 1crafting_mod.lua , because it needs to be the first file of the folder, if not, then you get errors of nil value about categorys crafting (armorsmith, etc)


Crafting = {}

local CODE_CRAFTING = 91
local fetchLimit = 10

local categories = {"herbalist", "woodcutting", "mining", "generalcrafting", "armorsmith", "weaponsmith", "jewelsmith"}


local ActionEvent = Action()

function ActionEvent.onUse(player)
  player:showCrafting()
  return true
end


ActionEvent:aid(38820)
ActionEvent:register()

local LoginEvent = CreatureEvent("CraftingLogin")

function LoginEvent.onLogin(player)
  player:registerEvent("CraftingExtended")
  return true
end

LoginEvent:type("login")
LoginEvent:register()

local ExtendedEvent = CreatureEvent("CraftingExtended")

function ExtendedEvent.onExtendedOpcode(player, opcode, buffer)
  if opcode == CODE_CRAFTING then
    local status, json_data =
      pcall(
      function()
        return json.decode(buffer)
      end
    )

    if not status then
      return false
    end

    local action = json_data.action
    local data = json_data.data

    if action == "fetch" then
      Crafting:sendMoney(player)
      for _, category in ipairs(categories) do
        Crafting:sendCrafts(player, category)
      end
    elseif action == "craft" then
      Crafting:craft(player, data.category, data.craftId)
    end
  end
  return true
end

ExtendedEvent:type("extendedopcode")
ExtendedEvent:register()

function Crafting:sendCrafts(player, category)
  local data = {}

  for i = 1, #Crafting[category] do
    local craft = {}
    for key, value in pairs(Crafting[category][i]) do
      if key == "materials" then
        craft.materials = {}
        for indx, material in ipairs(value) do
          local itemType = ItemType(material.id)
          craft.materials[indx] = {
            id = material.id,
            count = material.count,
            player = player:getItemCount(material.id)
          }
        end
      else
        craft[key] = value
      end
    end

    local itemType = ItemType(craft.id)
    craft.clientId = itemType:getClientId()

    for x = 1, #craft.materials do
      craft.materials[x].id = ItemType(craft.materials[x].id):getClientId()
    end
    table.insert(data, craft)
  end

  if #data >= fetchLimit then
    local x = 1
    for i = 1, math.floor(#data / fetchLimit) do
      player:sendExtendedOpcode(
        CODE_CRAFTING,
        json.encode({action = "fetch", data = {category = category, crafts = {unpack(data, x, math.min(x + fetchLimit - 1, #data))}}})
      )
      x = x + fetchLimit
    end

    if x < #data then
      player:sendExtendedOpcode(CODE_CRAFTING, json.encode({action = "fetch", data = {category = category, crafts = {unpack(data, x, #data)}}}))
    end
  else
    player:sendExtendedOpcode(CODE_CRAFTING, json.encode({action = "fetch", data = {category = category, crafts = data}}))
  end
end

function Crafting:craft(player, category, craftId)
  local craft = Crafting[category][craftId]

  local money = player:getTotalMoney()
  local money = player:getMoney()

  if money < craft.cost then
    player:popupFYI(
        string.format("You don't have enough money: " .. craft.cost .. ".")
      )
    return
  end
  
  if player:getLevel() < craft.level then
    player:popupFYI(
        string.format("You don't have the required Level." .. craft.level .. ".")
      )
    return
  end

  for i = 1, #craft.materials do
    local material = craft.materials[i]
    if player:getItemCount(material.id) < material.count then
      player:popupFYI(
        string.format("You don't have enough materials.")
      )
      return
    end
  end

  local item = Game.createItem(craft.id, craft.count)
  if item then
    if player:addItemEx(item) then
      player:removeTotalMoney(craft.cost)

      for i = 1, #craft.materials do
        local material = craft.materials[i]
        player:removeItem(material.id, material.count)
      end

      Crafting:sendMoney(player)
      Crafting:sendMaterials(player, category)
      player:sendExtendedOpcode(CODE_CRAFTING, json.encode({action = "crafted"}))
    end
  end
end

function Crafting:sendMaterials(player, category)
  local data = {}

  for i = 1, #Crafting[category] do
    local materials = {}
    for key, value in pairs(Crafting[category][i]) do
      if key == "materials" then
        for matId, matData in ipairs(value) do
          materials[matId] = player:getItemCount(matData.id)
        end
      end
    end
    table.insert(data, materials)
  end

  if #data >= fetchLimit then
    local x = 1
    for i = 1, math.floor(#data / fetchLimit) do
      player:sendExtendedOpcode(
        CODE_CRAFTING,
        json.encode({action = "materials", data = {category = category, from = x, materials = {unpack(data, x, math.min(x + fetchLimit - 1, #data))}}})
      )
      x = x + fetchLimit
    end

    if x < #data then
      player:sendExtendedOpcode(
        CODE_CRAFTING,
        json.encode({action = "materials", data = {category = category, from = x, materials = {unpack(data, x, #data)}}})
      )
    end
  else
    player:sendExtendedOpcode(CODE_CRAFTING, json.encode({action = "materials", data = {category = category, from = 1, materials = data}}))
  end
end

function Crafting:sendMoney(player)
  player:sendExtendedOpcode(CODE_CRAFTING, json.encode({action = "money", data = player:getMoney()}))
end

function Player:showCrafting()
  Crafting:sendMoney(self)
  for _, category in ipairs(categories) do
    Crafting:sendMaterials(self, category)
  end
  self:sendExtendedOpcode(CODE_CRAFTING, json.encode({action = "show"}))
end

