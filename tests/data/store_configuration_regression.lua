local category, purchaseGate, purchase = {}, nil, nil
function category:product() end
function category:canPurchase(callback) purchaseGate = callback end
function category:onPurchase(callback) purchase = callback end
StoreWindow = function()
	return {
		accountType = function() end,
		coins = function() end,
		onOpen = function() end,
		category = function() return category end,
		register = function() end,
	}
end
dofile("data/scripts/store/store.lua")
local grants = 0
local player = {
	hasBlessing = function() return false end,
	addBlessing = function() grants = grants + 1 end,
	sendTextMessage = function() end,
}
local failures = 0
for _, id in ipairs({4200, 4201, 9999}) do
	local allowed, message = purchaseGate(player, id)
	if allowed ~= false or type(message) ~= "string" or not message:find("configured", 1, true) then
		failures = failures + 1
		print("FAIL unconfigured store did not reject product " .. id .. " with configuration message")
	end
	grants = 0
	if purchase(player, id, 0, "") ~= false or grants ~= 0 then
		failures = failures + 1
		print("FAIL unconfigured purchase granted a reward for product " .. id)
	end
end
assert(failures == 0, tostring(failures) .. " unconfigured store failures")
print("PASS unconfigured store rejects advertised and unknown products without granting rewards")
