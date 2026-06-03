local mtype = Game.createMonsterType("The Obliverator")
local monster = {}

monster.name = "The Obliverator"
monster.description = "The Obliverator"
monster.experience = 6000
monster.race = "fire"
monster.maxHealth = 9020
monster.health = 9020
monster.speed = 280
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 35 }
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
        maxDamage = -700,
    },
    {
        name = "fire",
        interval = 1000,
        chance = 20,
        range = 5,
        minDamage = -100,
        maxDamage = -250,
        radius = 7,
        target = true,
        shootEffect = CONST_ANI_FIRE,
        effect = CONST_ME_FIREAREA,
    },
    {
        name = "energy",
        interval = 3000,
        chance = 30,
        minDamage = -200,
        maxDamage = -500,
        length = 8,
        effect = CONST_ME_ENERGYHIT,
    },
}
monster.defenses = {
    defense = 45,
    armor = 40,
    {
        name = "healing",
        interval = 4000,
        chance = 5,
        minDamage = 50,
        maxDamage = 200,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 50},
    {type = COMBAT_DEATHDAMAGE, percent = 35},
    {type = COMBAT_HOLYDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.maxSummons = 3
monster.summons = {
    {name = "fire elemental", interval = 2000, chance = 50, max = 0},
}
monster.voices = {
    interval = 1000,
    chance = 10,
    {text = "NO ONE WILL BEAT ME!", yell = false},
}

mtype:register(monster)
