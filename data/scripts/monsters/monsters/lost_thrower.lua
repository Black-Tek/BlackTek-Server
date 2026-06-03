local mtype = Game.createMonsterType("Lost Thrower")
local monster = {}

monster.name = "Lost Thrower"
monster.description = "a lost thrower"
monster.experience = 1200
monster.race = "blood"
monster.maxHealth = 1700
monster.health = 1700
monster.speed = 220
monster.manaCost = 0
monster.corpse = 19998
monster.outfit = { lookType = 539 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 4
monster.staticAttackChance = 90
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
        maxDamage = -301,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        range = 7,
        minDamage = 0,
        maxDamage = -250,
        radius = 1,
        target = true,
        shootEffect = CONST_ANI_THROWINGSTAR,
        effect = CONST_ME_EXPLOSIONAREA,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 5,
        range = 7,
        radius = 2,
        target = false,
        effect = CONST_ME_MAGIC_RED,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 10,
        range = 7,
        minDamage = -150,
        maxDamage = -300,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDCLUB,
        effect = CONST_ME_STUN,
    },
    {
        name = "drunk",
        interval = 2000,
        chance = 10,
        radius = 3,
        target = true,
        duration = 6000,
        shootEffect = CONST_ANI_WHIRLWINDAXE,
        effect = CONST_ME_EXPLOSIONAREA,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 5,
        minDamage = 100,
        maxDamage = 500,
        effect = CONST_ME_MAGIC_BLUE,
    },
    {
        name = "invisible",
        interval = 2000,
        chance = 10,
        duration = 5000,
        effect = CONST_ME_TELEPORT,
    },
}
monster.elements = {
    {type = COMBAT_DEATHDAMAGE, percent = 5},
    {type = COMBAT_ICEDAMAGE, percent = 5},
    {type = COMBAT_ENERGYDAMAGE, percent = 15},
    {type = COMBAT_FIREDAMAGE, percent = 40},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 37,
    },
    {
        id = "platinum coin",
        chance = 70000,
        maxCount = 2,
    },
    {
        id = "brown mushroom",
        chance = 14500,
        maxCount = 2,
    },
    {
        id = "iron ore",
        chance = 9470,
    },
    {
        id = "great mana potion",
        chance = 13470,
    },
    {
        id = "great health potion",
        chance = 14730,
    },
    {
        id = 13757,
        chance = 12180,
    },
    {
        id = "lost basher's spike",
        chance = 11410,
    },
    {
        id = "buckle",
        chance = 850,
    },
    {
        id = "holy ash",
        chance = 15670,
    },
    {
        id = "broken throwing axe",
        chance = 1390,
    },
    {
        id = "helmet of the lost",
        chance = 13560,
    },
    {
        id = "lost bracers",
        chance = 14860,
    },
    {
        id = "mad froth",
        chance = 11810,
    },
    {
        id = "basalt fetish",
        chance = 6150,
    },
    {
        id = "basalt figurine",
        chance = 7900,
    },
}

mtype:register(monster)
