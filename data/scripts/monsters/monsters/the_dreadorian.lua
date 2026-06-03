local mtype = Game.createMonsterType("The Dreadorian")
local monster = {}

monster.name = "The Dreadorian"
monster.description = "The Dreadorian"
monster.experience = 4000
monster.race = "blood"
monster.maxHealth = 9000
monster.health = 9000
monster.speed = 250
monster.manaCost = 0
monster.corpse = 6328
monster.outfit = { lookType = 234 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
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
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -370,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
    {
        name = "healing",
        interval = 2000,
        chance = 50,
        minDamage = 100,
        maxDamage = 150,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 90},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
    {type = COMBAT_DEATHDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_HOLYDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
}

mtype:register(monster)
