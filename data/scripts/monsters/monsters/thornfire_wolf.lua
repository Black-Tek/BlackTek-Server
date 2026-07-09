local mtype = Game.createMonsterType("Thornfire Wolf")
local monster = {}

monster.name = "Thornfire Wolf"
monster.description = "a thornfire wolf"
monster.experience = 200
monster.race = "energy"
monster.maxHealth = 600
monster.health = 600
monster.speed = 250
monster.manaCost = 0
monster.corpse = 13859
monster.outfit = { lookType = 414 }
monster.changeTarget = {
    interval = 4000,
    chance = 10,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
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
        maxDamage = -68,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 15,
        range = 1,
        minDamage = -35,
        maxDamage = -70,
        target = true,
        shootEffect = CONST_ANI_WHIRLWINDSWORD,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 15,
        range = 7,
        minDamage = -30,
        maxDamage = -70,
        radius = 2,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_EXPLOSIONHIT,
    },
    {
        name = "firefield",
        interval = 2000,
        chance = 10,
        range = 7,
        radius = 2,
        target = true,
    },
}
monster.defenses = {
    defense = 15,
    armor = 15,
    {
        name = "healing",
        interval = 1000,
        chance = 20,
        minDamage = 100,
        maxDamage = 220,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ICEDAMAGE, percent = 1},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "invisible", condition = true},
    {type = "paralyze", condition = true},
}
monster.loot = {
    {
        id = "gold coin",
        chance = 64000,
        maxCount = 53,
    },
    {
        id = "meat",
        chance = 34000,
    },
    {
        id = "wolf paw",
        chance = 7900,
    },
    {
        id = "flaming arrow",
        chance = 15000,
        maxCount = 8,
    },
    {
        id = "fiery heart",
        chance = 7900,
    },
}

mtype:register(monster)
