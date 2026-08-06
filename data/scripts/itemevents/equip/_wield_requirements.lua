-- {name, false} restricts wielding to that vocation without showing it in the description.
local VOC_SORCERER_DRUID = {{"Sorcerer"}, {"Master Sorcerer", false}, {"Druid"}, {"Elder Druid", false}}
local VOC_KNIGHT_PALADIN = {{"Knight"}, {"Elite Knight", false}, {"Paladin"}, {"Royal Paladin", false}}
local VOC_SORCERER_ONLY = {{"Sorcerer"}, {"Master Sorcerer", false}}
local VOC_DRUID_ONLY = {{"Druid"}, {"Elder Druid", false}}
local VOC_KNIGHT_ONLY = {{"Knight"}, {"Elite Knight", false}}
local VOC_PALADIN_ONLY = {{"Paladin"}, {"Royal Paladin", false}}
local VOC_SORCERER_DRUID_PLAIN = {{"Sorcerer"}, {"Druid"}} -- item 23771: no promoted forms in source
local VOC_DRUID_SORCERER = {{"Druid"}, {"Elder Druid", false}, {"Sorcerer"}, {"Master Sorcerer", false}} -- 25190/25192/25193: reversed order in source

-- Equip and DeEquip are kept as separate lists since some items only have one of the two.
local equipRequirements = {
	-- Boots
	{ id = 2195, slot = "feet" },
	{ id = 2640, slot = "feet" },
	{ id = 6132, slot = "feet" },
	{ id = 7886, slot = "feet", level = 35, vocations = VOC_SORCERER_DRUID },
	{ id = 7891, slot = "feet", level = 35, vocations = VOC_SORCERER_DRUID },
	{ id = 7892, slot = "feet", level = 35, vocations = VOC_SORCERER_DRUID },
	{ id = 7893, slot = "feet", level = 35, vocations = VOC_SORCERER_DRUID },
	{ id = 9932, slot = "feet", level = 130 },
	{ id = 9933, slot = "feet", level = 130 },
	{ ids = {11117, 11118}, slot = "feet", level = 70, vocations = VOC_KNIGHT_PALADIN },
	{ id = 11240, slot = "feet", level = 70, vocations = VOC_KNIGHT_PALADIN },
	{ id = 11303, slot = "feet" },
	{ id = 12646, slot = "feet", level = 80, vocations = VOC_KNIGHT_PALADIN },
	{ id = 15410, slot = "feet", level = 150, vocations = VOC_KNIGHT_ONLY },
	{ id = 18406, slot = "feet", level = 150, vocations = VOC_PALADIN_ONLY },
	{ id = 23540, slot = "feet", level = 50, vocations = VOC_KNIGHT_PALADIN },
	{ id = 24637, slot = "feet", level = 80, vocations = VOC_SORCERER_DRUID },
	{ id = 24742, slot = "feet", level = 60 },
	{ id = 25429, slot = "feet", level = 100 },
	{ id = 26132, slot = "feet", level = 150 },

	-- Amulets
	{ id = 2161, slot = "necklace" },
	{ id = 2170, slot = "necklace" },
	{ id = 2172, slot = "necklace" },
	{ id = 2173, slot = "necklace" },
	{ id = 2197, slot = "necklace" },
	{ id = 2198, slot = "necklace" },
	{ id = 2199, slot = "necklace" },
	{ id = 2200, slot = "necklace" },
	{ id = 2201, slot = "necklace" },
	{ id = 7887, slot = "necklace", level = 60 },
	{ id = 7888, slot = "necklace", level = 60 },
	{ id = 7889, slot = "necklace", level = 60 },
	{ id = 7890, slot = "necklace", level = 60 },
	{ id = 8266, slot = "necklace" },
	{ id = 10218, slot = "necklace", level = 80 },
	{ id = 10219, slot = "necklace", level = 80 },
	{ id = 10220, slot = "necklace", level = 80 },
	{ id = 10221, slot = "necklace", level = 80 },
	{ id = 11374, slot = "necklace" },
	{ id = 15403, slot = "necklace", level = 120 },
	{ id = 18402, slot = "necklace", level = 150 },
	{ id = 18407, slot = "necklace", level = 150 },
	{ id = 21691, slot = "necklace", level = 150 },
	{ id = 23541, slot = "necklace", level = 75 },
	{ id = 23554, slot = "necklace", level = 75, vocations = VOC_SORCERER_DRUID },
	{ id = 24851, slot = "necklace", level = 60 },
	{ id = 25423, slot = "necklace", level = 100 },
	{ id = 25424, slot = "necklace", level = 100 },
	{ id = 26198, slot = "necklace", level = 150, vocations = VOC_PALADIN_ONLY },
	{ id = 26199, slot = "necklace", level = 150, vocations = VOC_SORCERER_DRUID },
	{ id = 26200, slot = "necklace", level = 150, vocations = VOC_KNIGHT_ONLY },

	-- Rings (asymmetric: some ring ids intentionally carry no matching DeEquip entry - see deEquipItems)
	{ id = 2164, slot = "ring" },
	{ id = 2165, slot = "ring" },
	{ id = 2166, slot = "ring" },
	{ id = 2167, slot = "ring" },
	{ id = 2168, slot = "ring" },
	{ id = 2169, slot = "ring" },
	{ id = 2202, slot = "ring" },
	{ id = 2203, slot = "ring" },
	{ id = 2204, slot = "ring" },
	{ id = 2205, slot = "ring" },
	{ id = 2206, slot = "ring" },
	{ id = 2207, slot = "ring" },
	{ id = 2208, slot = "ring" },
	{ id = 2209, slot = "ring" },
	{ id = 2210, slot = "ring" },
	{ id = 2211, slot = "ring" },
	{ id = 2212, slot = "ring" },
	{ id = 2213, slot = "ring" },
	{ id = 2214, slot = "ring" },
	{ id = 2215, slot = "ring" },
	{ id = 2216, slot = "ring" },
	{ id = 6300, slot = "ring" },
	{ id = 6301, slot = "ring" },
	{ id = 18408, slot = "ring", level = 120 },
	{ id = 18528, slot = "ring", level = 120 },
	{ id = 22516, slot = "ring", level = 200 },
	{ id = 26185, slot = "ring", level = 100, vocations = VOC_PALADIN_ONLY },
	{ id = 26187, slot = "ring", level = 100, vocations = VOC_SORCERER_DRUID },
	{ id = 26189, slot = "ring", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 10309, slot = "ring", level = 100 },
	{ id = 10311, slot = "ring", level = 100 },
	{ id = 10312, slot = "ring", level = 100 },
	{ id = 10314, slot = "ring", level = 100 },

	-- Helmets
	{ id = 2323, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 2343, slot = "head" },
	{ id = 2474, slot = "head" },
	{ id = 2502, slot = "head" },
	{ id = 2664, slot = "head", vocations = VOC_PALADIN_ONLY },
	{ id = 5461, slot = "head" },
	{ id = 7459, slot = "head" },
	{ id = 7900, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 7901, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 7902, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 7903, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 8820, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 9778, slot = "head", level = 80, vocations = VOC_SORCERER_DRUID },
	{ id = 10016, slot = "head", level = 50, vocations = VOC_SORCERER_DRUID },
	{ id = 10570, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 11302, slot = "head", vocations = VOC_KNIGHT_PALADIN },
	{ id = 11368, slot = "head", level = 60, vocations = VOC_SORCERER_DRUID },
	{ id = 12630, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 12645, slot = "head", level = 100, vocations = VOC_PALADIN_ONLY },
	{ id = 13756, slot = "head", vocations = VOC_SORCERER_DRUID },
	{ id = 15408, slot = "head", level = 150 },
	{ id = 18398, slot = "head", level = 150, vocations = VOC_SORCERER_DRUID },
	{ id = 18403, slot = "head", level = 150, vocations = VOC_KNIGHT_ONLY },
	{ id = 23536, slot = "head", level = 70, vocations = VOC_SORCERER_DRUID },
	{ id = 24261, slot = "head", level = 80 },
	{ id = 25413, slot = "head", level = 150 },
	{ id = 26130, slot = "head", vocations = VOC_SORCERER_DRUID },

	-- Armors
	{ id = 2466, slot = "armor", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2472, slot = "armor", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2476, slot = "armor", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2487, slot = "armor", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2492, slot = "armor", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2500, slot = "armor", level = 60, vocations = VOC_PALADIN_ONLY },
	{ id = 2503, slot = "armor" },
	{ id = 2660, slot = "armor", vocations = VOC_PALADIN_ONLY },
	{ id = 7884, slot = "armor", level = 50, vocations = VOC_SORCERER_DRUID },
	{ id = 7897, slot = "armor", level = 50, vocations = VOC_SORCERER_DRUID },
	{ id = 7898, slot = "armor", level = 50, vocations = VOC_SORCERER_DRUID },
	{ id = 7899, slot = "armor", level = 50, vocations = VOC_SORCERER_DRUID },
	{ id = 8819, slot = "armor", vocations = VOC_SORCERER_DRUID },
	{ id = 8821, slot = "armor", level = 50 },
	{ id = 8865, slot = "armor", level = 65, vocations = VOC_SORCERER_ONLY },
	{ id = 8866, slot = "armor", level = 75, vocations = VOC_DRUID_ONLY },
	{ id = 8867, slot = "armor", level = 75, vocations = VOC_SORCERER_ONLY },
	{ id = 8868, slot = "armor", level = 75, vocations = VOC_SORCERER_ONLY },
	{ id = 8869, slot = "armor", level = 75, vocations = VOC_DRUID_ONLY },
	{ id = 8870, slot = "armor", vocations = VOC_SORCERER_DRUID },
	{ id = 8871, slot = "armor", vocations = VOC_SORCERER_DRUID },
	{ id = 8872, slot = "armor", vocations = VOC_PALADIN_ONLY },
	{ id = 8877, slot = "armor", level = 60, vocations = VOC_KNIGHT_PALADIN },
	{ id = 8878, slot = "armor", level = 60, vocations = VOC_KNIGHT_PALADIN },
	{ id = 8879, slot = "armor", level = 60, vocations = VOC_KNIGHT_PALADIN },
	{ id = 8880, slot = "armor", level = 60, vocations = VOC_KNIGHT_PALADIN },
	{ id = 8881, slot = "armor", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8882, slot = "armor", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8883, slot = "armor", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8884, slot = "armor", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8885, slot = "armor", level = 75, vocations = VOC_PALADIN_ONLY },
	{ id = 8886, slot = "armor", level = 75, vocations = VOC_PALADIN_ONLY },
	{ id = 8887, slot = "armor", level = 75, vocations = VOC_PALADIN_ONLY },
	{ id = 8888, slot = "armor", level = 100, vocations = VOC_PALADIN_ONLY },
	{ id = 8889, slot = "armor", level = 85, vocations = VOC_KNIGHT_ONLY },
	{ id = 8890, slot = "armor", level = 100, vocations = VOC_SORCERER_ONLY },
	{ id = 8891, slot = "armor", vocations = VOC_PALADIN_ONLY },
	{ id = 8892, slot = "armor", vocations = VOC_SORCERER_DRUID },
	{ id = 9776, slot = "armor", level = 80, vocations = VOC_KNIGHT_ONLY },
	{ id = 11301, slot = "armor", level = 50, vocations = VOC_KNIGHT_PALADIN },
	{ ids = {11355, 11356}, slot = "armor", level = 60, vocations = VOC_SORCERER_DRUID },
	{ id = 12607, slot = "armor", level = 100, vocations = VOC_KNIGHT_PALADIN },
	{ id = 12642, slot = "armor", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 12643, slot = "armor", level = 100, vocations = VOC_SORCERER_DRUID },
	{ id = 15406, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 15407, slot = "armor", level = 150, vocations = VOC_PALADIN_ONLY },
	{ id = 15489, slot = "armor", level = 80, vocations = VOC_SORCERER_DRUID },
	{ id = 18399, slot = "armor", level = 150, vocations = VOC_SORCERER_DRUID },
	{ id = 18404, slot = "armor", level = 120, vocations = VOC_KNIGHT_PALADIN },
	{ id = 21706, slot = "armor" },
	{ id = 21725, slot = "armor", level = 130, vocations = VOC_SORCERER_DRUID },
	{ id = 23535, slot = "armor", level = 40, vocations = VOC_SORCERER_DRUID },
	{ id = 23537, slot = "armor", vocations = VOC_KNIGHT_PALADIN },
	{ id = 24741, slot = "armor", level = 50 },
	{ id = 25174, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25175, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25176, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25180, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25181, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25182, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25183, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25184, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25185, slot = "armor", level = 200, vocations = VOC_KNIGHT_ONLY },
	{ id = 25186, slot = "armor", level = 200, vocations = VOC_PALADIN_ONLY },
	{ id = 25188, slot = "armor", level = 200, vocations = VOC_PALADIN_ONLY },
	{ id = 25189, slot = "armor", level = 200, vocations = VOC_PALADIN_ONLY },
	{ id = 25190, slot = "armor", level = 200, vocations = VOC_DRUID_SORCERER },
	{ id = 25192, slot = "armor", level = 200, vocations = VOC_DRUID_SORCERER },
	{ id = 25193, slot = "armor", level = 200, vocations = VOC_DRUID_SORCERER },

	-- Legs
	{ id = 2470, slot = "legs", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2477, slot = "legs", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2488, slot = "legs", vocations = VOC_KNIGHT_PALADIN },
	{ id = 2504, slot = "legs" },
	{ id = 7885, slot = "legs", level = 40, vocations = VOC_SORCERER_DRUID },
	{ id = 7894, slot = "legs", level = 40, vocations = VOC_SORCERER_DRUID },
	{ id = 7895, slot = "legs", level = 40, vocations = VOC_SORCERER_DRUID },
	{ id = 7896, slot = "legs", level = 40, vocations = VOC_SORCERER_DRUID },
	{ id = 8923, slot = "legs", vocations = VOC_PALADIN_ONLY },
	{ id = 9777, slot = "legs", level = 80, vocations = VOC_PALADIN_ONLY },
	{ id = 11304, slot = "legs" },
	{ id = 15409, slot = "legs", level = 130, vocations = VOC_SORCERER_DRUID },
	{ id = 15412, slot = "legs", level = 185, vocations = VOC_KNIGHT_ONLY },
	{ id = 15490, slot = "legs", level = 75 },
	{ id = 18400, slot = "legs", level = 150, vocations = VOC_SORCERER_DRUID },
	{ id = 18405, slot = "legs", level = 150, vocations = VOC_PALADIN_ONLY },
	{ id = 21700, slot = "legs", vocations = VOC_SORCERER_DRUID },
	{ id = 23539, slot = "legs", level = 60 },

	-- Weapons (hand)
	{ id = 18390, slot = "hand", level = 65, vocations = VOC_SORCERER_ONLY },
	{ id = 18409, slot = "hand", level = 65, vocations = VOC_SORCERER_ONLY },
	{ id = 18411, slot = "hand", level = 65, vocations = VOC_DRUID_ONLY },
	{ id = 18412, slot = "hand", level = 65, vocations = VOC_DRUID_ONLY },
	{ id = 18451, slot = "hand", level = 120 },
	{ id = 18452, slot = "hand", level = 120 },
	{ id = 18465, slot = "hand", level = 120 },
	{ id = 22400, slot = "hand", level = 250, vocations = VOC_KNIGHT_ONLY },
	{ id = 22403, slot = "hand", level = 250, vocations = VOC_KNIGHT_ONLY },
	{ id = 22406, slot = "hand", level = 250, vocations = VOC_KNIGHT_ONLY },
	{ id = 22409, slot = "hand", level = 250, vocations = VOC_KNIGHT_ONLY },
	{ id = 22412, slot = "hand", level = 250, vocations = VOC_KNIGHT_ONLY },
	{ id = 22415, slot = "hand", level = 250, vocations = VOC_KNIGHT_ONLY },
	{ id = 22418, slot = "hand", level = 250, vocations = VOC_PALADIN_ONLY },
	{ id = 22421, slot = "hand", level = 250, vocations = VOC_PALADIN_ONLY },
	{ id = 23542, slot = "hand", level = 55 },
	{ id = 23543, slot = "hand", level = 25 },
	{ id = 23544, slot = "hand", level = 60 },
	{ id = 23545, slot = "hand", level = 45 },
	{ id = 23547, slot = "hand", level = 55 },
	{ id = 23548, slot = "hand", level = 25 },
	{ id = 23549, slot = "hand", level = 75, vocations = VOC_KNIGHT_ONLY },
	{ id = 23550, slot = "hand", level = 75, vocations = VOC_KNIGHT_ONLY },
	{ id = 23551, slot = "hand", level = 75, vocations = VOC_KNIGHT_ONLY },
	{ id = 23590, slot = "hand", level = 70 },

	-- Shields
	{ id = 2175, slot = "shield", vocations = VOC_SORCERER_DRUID },
	{ id = 8900, slot = "shield", level = 30, vocations = VOC_SORCERER_DRUID },
	{ id = 8901, slot = "shield", level = 40, vocations = VOC_SORCERER_DRUID },
	{ id = 8902, slot = "shield", level = 50, vocations = VOC_SORCERER_DRUID },
	{ id = 8903, slot = "shield", level = 60, vocations = VOC_SORCERER_DRUID },
	{ id = 8904, slot = "shield", level = 70, vocations = VOC_SORCERER_DRUID },
	{ id = 8905, slot = "shield", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8906, slot = "shield", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8907, slot = "shield", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8908, slot = "shield", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8909, slot = "shield", level = 100, vocations = VOC_KNIGHT_ONLY },
	{ id = 8918, slot = "shield", level = 80, vocations = VOC_SORCERER_DRUID },
	{ id = 12644, slot = "shield", level = 80, vocations = VOC_KNIGHT_ONLY },
	{ id = 12647, slot = "shield", level = 100, vocations = VOC_SORCERER_DRUID },
	{ id = 15411, slot = "shield", level = 120, vocations = VOC_SORCERER_DRUID },
	{ id = 15413, slot = "shield", level = 130, vocations = VOC_KNIGHT_ONLY },
	{ id = 16112, slot = "shield", level = 150, vocations = VOC_SORCERER_DRUID },
	{ id = 18401, slot = "shield", level = 130, vocations = VOC_SORCERER_DRUID },
	{ id = 18410, slot = "shield", level = 150, vocations = VOC_KNIGHT_ONLY },
	{ id = 22422, slot = "shield", level = 75, vocations = VOC_SORCERER_DRUID },
	{ id = 22423, slot = "shield", level = 120, vocations = VOC_SORCERER_DRUID },
	{ id = 22424, slot = "shield", level = 250, vocations = VOC_SORCERER_DRUID },
	{ id = 23771, slot = "shield", vocations = VOC_SORCERER_DRUID_PLAIN },
	{ id = 25411, slot = "shield", level = 150, vocations = VOC_SORCERER_DRUID },
	{ id = 25414, slot = "shield", level = 200 },

	-- Quivers
	{ id = 12425, slot = "shield", vocations = VOC_PALADIN_ONLY },
}

local deEquipItems = {
	-- Boots
	2195, 2640, 6132, 7886, 7891, 7892, 7893, 9932, 9933, {11117, 11118}, 11240, 11303,
	12646, 15410, 18406, 23540, 24637, 24742, 25429, 26132,
	-- Amulets
	2161, 2170, 2172, 2173, 2197, 2198, 2199, 2200, 2201, 7887, 7888, 7889, 7890, 8266,
	10218, 10219, 10220, 10221, 11374, 15403, 18402, 18407, 21691, 23541, 23554, 24851,
	25423, 25424, 26198, 26199, 26200,
	-- Rings (asymmetric: 10310 has a DeEquip entry with no matching Equip entry above)
	2164, 2202, 2203, 2204, 2205, 2206, 2210, 2211, 2212, 2215, 2216, 6301, 18528, 22516,
	26185, 26187, 26189, 10309, 10310, 10311, 10312, 10314,
	-- Helmets
	2323, 2343, 2474, 2502, 2664, 5461, 7459, 7900, 7901, 7902, 7903, 8820, 9778, 10016,
	10570, 11302, 11368, 12630, 12645, 13756, 15408, 18398, 18403, 23536, 24261, 25413, 26130,
	-- Armors
	2466, 2472, 2476, 2487, 2492, 2500, 2503, 2660, 7884, 7897, 7898, 7899, 8819, 8821,
	8865, 8866, 8867, 8868, 8869, 8870, 8871, 8872, 8877, 8878, 8879, 8880, 8881, 8882,
	8883, 8884, 8885, 8886, 8887, 8888, 8889, 8890, 8891, 8892, 9776, 11301, {11355, 11356},
	12607, 12642, 12643, 15406, 15407, 15489, 18399, 18404, 21706, 21725, 23535, 23537,
	24741, 25174, 25175, 25176, 25180, 25181, 25182, 25183, 25184, 25185, 25186, 25188,
	25189, 25190, 25192, 25193,
	-- Legs
	2470, 2477, 2488, 2504, 7885, 7894, 7895, 7896, 8923, 9777, 11304, 15409, 15412, 15490,
	18400, 18405, 21700, 23539,
	-- Weapons (hand)
	18390, 18409, 18411, 18412, 18451, 18452, 18465, 22400, 22403, 22406, 22409, 22412,
	22415, 22418, 22421, 23542, 23543, 23544, 23545, 23547, 23548, 23549, 23550, 23551, 23590,
	-- Shields
	2175, 8900, 8901, 8902, 8903, 8904, 8905, 8906, 8907, 8908, 8909, 8918, 12644, 12647,
	15411, 15413, 16112, 18401, 18410, 22422, 22423, 22424, 23771, 25411, 25414,
	-- Quivers
	12425,
}

-- deEquipItems reuses the slot from the matching equip entry instead of repeating it.
local slotById = {}
for _, entry in ipairs(equipRequirements) do
	for _, id in ipairs(entry.ids or { entry.id }) do
		slotById[id] = entry.slot
	end
end

-- 10310's equip side is registered in equip/claw_of_the_noxious_spawn.lua, so its slot is set here.
slotById[10310] = "ring"

local function applyVocations(ev, vocations)
	if not vocations then
		return
	end

	local lastShownIndex = 0
	for i, voc in ipairs(vocations) do
		if voc[2] ~= false then
			lastShownIndex = i
		end
	end

	for i, voc in ipairs(vocations) do
		local show = voc[2] ~= false
		ev:vocation(voc[1], show, i == lastShownIndex)
	end
end

for _, entry in ipairs(equipRequirements) do
	local ev = ItemEvent()
	ev:type("equip")
	ev:nativeFunction("onequipitem")
	for _, id in ipairs(entry.ids or { entry.id }) do
		ev:id(id)
	end
	ev:slot(entry.slot)
	if entry.level then
		ev:level(entry.level)
	end
	applyVocations(ev, entry.vocations)
	ev:register()
end

for _, idOrRange in ipairs(deEquipItems) do
	local ids = type(idOrRange) == "table" and idOrRange or { idOrRange }
	local ev = ItemEvent()
	ev:type("deequip")
	ev:nativeFunction("ondeequipitem")
	for _, id in ipairs(ids) do
		ev:id(id)
		ev:slot(slotById[id])
	end
	ev:register()
end
