local mtype = Game.createMonsterType("Lost Basher")
local monster = {}

monster.name = "Lost Basher"
monster.description = "a lost basher"
monster.experience = 1800
monster.race = "blood"
monster.maxHealth = 2600
monster.health = 2600
monster.speed = 250
monster.manaCost = 0
monster.corpse = 19963
monster.outfit = { lookType = 538 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 80
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = true,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
    canWalkOnEnergy = false,
    canWalkOnFire = false,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -351,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = 0,
        maxDamage = -220,
        radius = 3,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 15,
        radius = 4,
        target = true,
        duration = 6000,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
        effect = CONST_ME_SOUND_RED,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        radius = 2,
        target = false,
        speed = -650,
        duration = 15000,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 10,
        minDamage = 250,
        maxDamage = 500,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 1},
    {type = COMBAT_ICEDAMAGE, percent = 1},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Yhouuuu!", yell = false},
}
monster.loot = {
    {
        id = "piggy bank",
        chance = 4450,
    },
    {
        id = "gold coin",
        chance = 60000,
        maxCount = 100,
    },
    {
        id = "platinum coin",
        chance = 70000,
        maxCount = 2,
    },
    {
        id = "dwarven ring",
        chance = 2560,
    },
    {
        id = "fire axe",
        chance = 310,
    },
    {
        id = "war axe",
        chance = 120,
    },
    {
        id = "knight legs",
        chance = 310,
    },
    {
        id = "black shield",
        chance = 3710,
    },
    {
        id = "brown mushroom",
        chance = 15170,
        maxCount = 2,
    },
    {
        id = "iron ore",
        chance = 5280,
    },
    {
        id = "chaos mace",
        chance = 160,
    },
    {
        id = "spiked squelcher",
        chance = 420,
    },
    {
        id = "great mana potion",
        chance = 11240,
    },
    {
        id = "fiery knight axe",
        chance = 1200,
    },
    {
        id = "terra boots",
        chance = 780,
    },
    {
        id = "ultimate health potion",
        chance = 10250,
    },
    {
        id = "small topaz",
        chance = 10200,
    },
    {
        id = 13757,
        chance = 21130,
    },
    {
        id = "blue crystal shard",
        chance = 840,
    },
    {
        id = "lost basher's spike",
        chance = 17260,
    },
    {
        id = "lost basher's spike",
        chance = 14380,
    },
    {
        id = "bloody dwarven beard",
        chance = 1730,
    },
    {
        id = "pair of iron fists",
        chance = 1410,
    },
    {
        id = "buckle",
        chance = 10930,
    },
    {
        id = "bonecarving knife",
        chance = 7320,
    },
    {
        id = "leather harness",
        chance = 12500,
    },
    {
        id = "mad froth",
        chance = 11870,
    },
    {
        id = "basalt fetish",
        chance = 8800,
    },
    {
        id = "basalt figurine",
        chance = 9470,
    },
}

mtype:register(monster)
