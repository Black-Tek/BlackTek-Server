local mtype = Game.createMonsterType("Drasilla")
local monster = {}

monster.name = "Drasilla"
monster.description = "Drasilla"
monster.experience = 700
monster.race = "blood"
monster.maxHealth = 1260
monster.health = 1260
monster.speed = 170
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 34 }
monster.runHealth = 100
monster.targetDistance = 1
monster.staticAttackChance = 100
monster.flags = {
    summonable = false,
    attackable = true,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canWalkOnEnergy = false,
    canWalkOnPoison = false,
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
        interval = 6000,
        chance = 60,
        minDamage = -100,
        maxDamage = -180,
        length = 8,
        spread = 3,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "fire",
        interval = 3000,
        chance = 50,
        range = 10,
        minDamage = -70,
        maxDamage = -115,
        radius = 5,
        target = true,
        effect = CONST_ME_FIREAREA,
    },
}
monster.defenses = {
    defense = 33,
    armor = 32,
    {
        name = "healing",
        interval = 6000,
        chance = 65,
        minDamage = 20,
        maxDamage = 50,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 20},
    {type = COMBAT_ICEDAMAGE, percent = -10},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 2000,
    chance = 5,
    {text = "FCHHHHHHHH!", yell = false},
    {text = "GROOOOAAAAAAAAR!", yell = false},
    {text = "DIRTY LITTLE HUMANS", yell = false},
    {text = "YOU CAN'T KEEP ME HERE FOREVER", yell = false},
}

mtype:register(monster)
