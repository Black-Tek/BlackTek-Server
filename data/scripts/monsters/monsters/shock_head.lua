local mtype = Game.createMonsterType("Shock Head")
local monster = {}

monster.name = "Shock Head"
monster.description = "a shock head"
monster.experience = 2300
monster.race = "blood"
monster.maxHealth = 4200
monster.health = 4200
monster.speed = 250
monster.manaCost = 0
monster.corpse = 22392
monster.outfit = { lookType = 579 }
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
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -798,
    },
    {
        name = "death",
        interval = 2000,
        chance = 15,
        minDamage = -200,
        maxDamage = -300,
        length = 5,
        spread = 2,
        target = true,
        effect = CONST_ME_BLACKSMOKE,
    },
    {
        name = "speed",
        interval = 2000,
        chance = 15,
        length = 8,
        spread = 3,
        target = true,
        speed = -800,
        duration = 7500,
        effect = CONST_ME_PURPLEENERGY,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 20,
        minDamage = 0,
        maxDamage = -350,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_EARTH,
        effect = CONST_ME_STONES,
    },
    {
        name = "shock head skill reducer 1",
        interval = 2000,
        chance = 5,
        range = 5,
    },
    {
        name = "shock head skill reducer 2",
        interval = 2000,
        chance = 5,
    },
}
monster.defenses = {
    defense = 30,
    armor = 30,
    {
        name = "healing",
        interval = 2000,
        chance = 15,
        minDamage = 250,
        maxDamage = 350,
        effect = CONST_ME_INSECTS,
    },
}
monster.elements = {
    {type = COMBAT_PHYSICALDAMAGE, percent = -5},
    {type = COMBAT_DEATHDAMAGE, percent = 30},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_ICEDAMAGE, percent = 5},
}
monster.immunities = {
    {type = "earth", combat = true, condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "<brrzz>", yell = false},
    {text = "Thun... Thun... THUNDER!", yell = false},
}
monster.loot = {
    {
        id = "small sapphire",
        chance = 5000,
    },
    {
        id = "gold coin",
        chance = 43000,
        maxCount = 100,
    },
    {
        id = "gold coin",
        chance = 50000,
        maxCount = 87,
    },
    {
        id = "royal helmet",
        chance = 270,
    },
    {
        id = "goosebump leather",
        chance = 10000,
    },
}

mtype:register(monster)
