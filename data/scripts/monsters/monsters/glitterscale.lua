local mtype = Game.createMonsterType("Glitterscale")
local monster = {}

monster.name = "Glitterscale"
monster.description = "Glitterscale"
monster.experience = 700
monster.race = "blood"
monster.maxHealth = 1000
monster.health = 1000
monster.speed = 180
monster.manaCost = 0
monster.corpse = 5973
monster.outfit = { lookType = 34 }
monster.runHealth = 300
monster.changeTarget = {
    interval = 2000,
    chance = 10,
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
        maxDamage = -120,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 8,
        minDamage = -90,
        maxDamage = -180,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 2000,
        chance = 12,
        range = 7,
        minDamage = -70,
        maxDamage = -120,
        radius = 4,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 18,
    armor = 25,
}
monster.elements = {
    {type = COMBAT_EARTHDAMAGE, percent = 80},
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "paralyze", condition = true},
    {type = "invisible", condition = true},
}

mtype:register(monster)
