local mtype = Game.createMonsterType("Spider Queen")
local monster = {}

monster.name = "Spider Queen"
monster.description = "a spider queen"
monster.experience = 120
monster.race = "venom"
monster.maxHealth = 10000
monster.health = 10000
monster.speed = 280
monster.manaCost = 0
monster.corpse = 6060
monster.outfit = { lookType = 219 }
monster.changeTarget = {
    interval = 5000,
    chance = 8,
}
monster.targetDistance = 1
monster.staticAttackChance = 90
monster.flags = {
    summonable = false,
    attackable = false,
    hostile = true,
    illusionable = false,
    convinceable = false,
    pushable = false,
    canPushItems = true,
    canPushCreatures = false,
    boss = true,
}
monster.attacks = {
    {
        name = "melee",
        interval = 2000,
        minDamage = 0,
        maxDamage = -1,
    },
    {
        name = "outfit",
        interval = 2000,
        chance = 100,
        range = 1,
        duration = 5000,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_ENERGYDAMAGE, percent = 10},
    {type = COMBAT_ICEDAMAGE, percent = -10},
    {type = COMBAT_FIREDAMAGE, percent = -15},
}
monster.immunities = {
    {type = "physical", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
    {type = "earth", combat = true, condition = true},
}

mtype:register(monster)
