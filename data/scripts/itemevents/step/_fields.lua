-- stepOnIds and addFieldIds are separate lists; items 7359/7360 have no addFieldIds counterpart.
local stepOnIds = {
	-- Campfires
	1423, 1424, 1425,
	-- Fields
	1487, 1488, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500,
	1501, 1502, 1503, 1504, 1505, 1506, 1507, 7359,
	-- Magic Wall NoPvp / Wild Growth NoPvp
	20669, 20670,
}

local addFieldIds = {
	-- Campfires
	1423, 1424, 1425,
	-- Fields
	1487, 1488, 1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500,
	1501, 1502, 1503, 1504, 1505, 1506, 1507, 7360,
}

for _, id in ipairs(stepOnIds) do
	local ev = ItemEvent()
	ev:type("stepon")
	ev:nativeFunction("onsteponfield")
	ev:id(id)
	ev:register()
end

for _, id in ipairs(addFieldIds) do
	local ev = ItemEvent()
	ev:type("additem")
	ev:nativeFunction("onaddfield")
	ev:id(id)
	ev:register()
end
