local mtype = Game.createMonsterType("Lethal Lissy")
local monster = {}

monster.name = "Lethal Lissy"
monster.description = "Lethal Lissy"
monster.experience = 500
monster.race = "blood"
monster.maxHealth = 1450
monster.health = 1450
monster.speed = 240
monster.manaCost = 0
monster.corpse = 20438
monster.outfit = {
    lookType = 155,
    lookHead = 77,
    lookBody = 0,
    lookLegs = 76,
    lookFeet = 132,
    lookAddons = 3,
}
monster.changeTarget = {
    interval = 60000,
    chance = 0,
}
monster.targetDistance = 1
monster.staticAttackChance = 50
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -250,
    },
}
monster.defenses = {
    defense = 50,
    armor = 35,
    {
        name = "healing",
        interval = 6000,
        chance = 65,
        minDamage = 200,
        maxDamage = 250,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.immunities = {
    {type = "invisible", condition = true},
}
monster.maxSummons = 4
monster.summons = {
    {name = "Pirate Cutthroat", interval = 2000, chance = 50, max = 0},
}
monster.loot = {
    {
        id = "small diamond",
        chance = 100000,
    },
    {
        id = "gold coin",
        chance = 100000,
        maxCount = 40,
    },
    {
        id = "skull of Ratha",
        chance = 100000,
    },
    {
        id = "double axe",
        chance = 1500,
    },
    {
        id = "plate armor",
        chance = 4000,
    },
    {
        id = "knight armor",
        chance = 1200,
    },
    {
        id = 6100,
        chance = 100000,
    },
}

mtype:register(monster)
