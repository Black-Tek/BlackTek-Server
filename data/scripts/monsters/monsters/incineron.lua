local mtype = Game.createMonsterType("Incineron")
local monster = {}

monster.name = "Incineron"
monster.description = "Incineron"
monster.experience = 3500
monster.race = "fire"
monster.maxHealth = 7000
monster.health = 7000
monster.speed = 260
monster.manaCost = 0
monster.corpse = 6324
monster.outfit = { lookType = 243 }
monster.changeTarget = {
    interval = 2000,
    chance = 9,
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
        name = "fire",
        interval = 2000,
        chance = 35,
        minDamage = 0,
        maxDamage = -1000,
        length = 8,
        spread = 1,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 35,
        range = 7,
        minDamage = 0,
        maxDamage = -395,
        radius = 7,
        target = false,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 25,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
}
monster.immunities = {
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
    {type = "fire", combat = true, condition = true},
    {type = "ice", combat = true, condition = true},
}

mtype:register(monster)
