local lockedDoors = {
	1209, 1212, 1231, 1234, 1249, 1252, 3535, 3544, 4913, 4916, 5098, 5107, 5116, 5125, 5134, 5137, 5140,
	5143, 5278, 5281, 5732, 5735, 6192, 6195, 6249, 6252, 6891, 6900, 7033, 7042, 8541, 8544, 9165, 9168,
	9267, 9270, 10268, 10271, 10775, 10784, 12092, 12099, 12188, 12197, 19840, 19849, 19980, 19989, 20273,
	20282, 22814, 22823, 25283, 25290
}

local closedNormalDoors = {
	[1210] = 1211, [1232] = 1233, [1250] = 1251, [3545] = 3546, [4914] = 4915, [5108] = 5109,
	[5126] = 5127, [5141] = 5142, [5144] = 5145, [5282] = 5283, [5736] = 5737, [6193] = 6194,
	[6250] = 6251, [6901] = 6902, [7043] = 7044, [1213] = 1214, [1235] = 1236, [1253] = 1254,
	[3536] = 3537, [4917] = 4918, [5099] = 5100, [5117] = 5118, [5135] = 5136, [5138] = 5139,
	[5279] = 5280, [5733] = 5734, [6196] = 6197, [6253] = 6254, [6892] = 6893, [7034] = 7035,
	[8542] = 8543, [9166] = 9167, [9268] = 9269, [10269] = 10270, [10469] = 10470, [10766] = 10777,
	[12093] = 12094, [12100] = 12101, [12189] = 12190, [19841] = 19842, [19981] = 19982, [20274] = 20275,
	[22815] = 22816, [25284] = 25285
}

local openVerticalDoors = {
	[1211] = 1210, [1233] = 1232, [1251] = 1250, [3546] = 3545, [4915] = 4914, [5109] = 5108,
	[5127] = 5126, [5142] = 5141, [5145] = 5144, [5283] = 5282, [5737] = 5736, [6194] = 6193,
	[6251] = 6250, [6902] = 6901, [7044] = 7043, [8543] = 8542, [9167] = 9166, [9269] = 9268,
	[10270] = 10269, [10470] = 10469, [10777] = 10766, [12094] = 12093, [12101] = 12100, [12190] = 12189,
	[19842] = 19841, [19982] = 19981, [20275] = 20274, [22816] = 22815, [25285] = 25284
}

local openHorizontalDoors = {
	[1214] = 1213, [1236] = 1235, [1254] = 1253, [3537] = 3536, [4918] = 4917, [5100] = 5099,
	[5118] = 5117, [5136] = 5135, [5139] = 5138, [5280] = 5279, [5734] = 5733, [6197] = 6196,
	[6254] = 6253, [6893] = 6892, [7035] = 7034, [8546] = 8545, [9169] = 9168, [9272] = 9271,
	[10273] = 10272, [10479] = 10478, [10786] = 10785, [12101] = 12100, [12199] = 12198, [19851] = 19850,
	[19991] = 19990, [20284] = 20283, [22825] = 22824, [25292] = 25291
}

local levelDoors = {
	[1227] = 1228, [1229] = 1230, [1245] = 1246, [1247] = 1248, [1259] = 1260, [1261] = 1262,
	[3540] = 3541, [3549] = 3550, [5103] = 5104, [5112] = 5113, [5121] = 5122, [5130] = 5131,
	[5292] = 5293, [5294] = 5295, [6206] = 6207, [6208] = 6209, [6263] = 6264, [6265] = 6266,
	[6896] = 6897, [6905] = 6906, [7038] = 7039, [7047] = 7048, [9653] = 9654, [9655] = 9656
}

local questDoors = {
	[1224] = 1223, [1226] = 1225, [1242] = 1241, [1244] = 1243, [1256] = 1255, [1258] = 1257,
	[3543] = 3542, [3552] = 3551, [5106] = 5105, [5115] = 5114, [5124] = 5123, [5133] = 5132,
	[5289] = 5288, [5291] = 5290, [5746] = 5745, [5749] = 5748, [6203] = 6202, [6205] = 6204,
	[6260] = 6259, [6262] = 6261, [6899] = 6898, [6908] = 6907, [7041] = 7040, [7050] = 7049,
	[9650] = 9649, [9652] = 9651
}

local passthrough = {
	[1634] = 1635, [1635] = 1634, [1636] = 1637, [1637] = 1636, [1638] = 1639, [1639] = 1638,
	[1640] = 1641, [1641] = 1640
}

function onUse(player, item, fromPosition, target, toPosition)
	if table.contains(lockedDoors, item:getId()) then
		player:sendTextMessage(MESSAGE_INFO_DESCR, "It is locked.")
		return true
	end

	local closedDoor = closedNormalDoors[item:getId()]
	if closedDoor then
		item:transform(closedDoor, 1)
		return true
	end

	local verticalDoor = openVerticalDoors[item:getId()]
	if verticalDoor then
		local doorCreature = Tile(item:getPosition()):getTopCreature()
		if doorCreature then
			doorCreature:teleportTo(item:getPosition():moveRel(1, 0, 0), true)
		end
		item:transform(verticalDoor, 1)
		return true
	end

	local horizontalDoor = openHorizontalDoors[item:getId()]
	if horizontalDoor then
		local doorCreature = Tile(item:getPosition()):getTopCreature()
		if doorCreature then
			doorCreature:teleportTo(item:getPosition():moveRel(0, 1, 0), true)
		end
		item:transform(horizontalDoor, 1)
		return true
	end

	local levelDoor = levelDoors[item:getId()]
	if levelDoor then
		if item.actionid > 0 and player:getLevel() >= item.actionid - 1000 or player:getGroup():getAccess() then
			player:teleportTo(item:getPosition(), true)
			item:transform(levelDoor, 1)
		else
			player:sendTextMessage(MESSAGE_INFO_DESCR, "You are not worthy.")
		end
		return true
	end

	local questDoor = questDoors[item:getId()]
	if questDoor then
		if player:getStorageValue(item.actionid) ~= -1 or player:getGroup():getAccess() then
			player:sendTextMessage(MESSAGE_INFO_DESCR, "The door seems to be sealed against unwanted intruders.")
			return true
		end

		player:teleportTo(item:getPosition(), true)
		item:transform(questDoor, 1)
		return true
	end

	local door = passthrough[item:getId()]
	if door then
		item:transform(door, 1)
		return true
	end
	return true
end
