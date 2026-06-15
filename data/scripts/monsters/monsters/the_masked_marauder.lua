local mtype = Game.createMonsterType("The Masked Marauder")
local monster = {}

monster.name = "The Masked Marauder"
monster.description = "The Masked Marauder"
monster.experience = 3500
monster.race = "blood"
monster.maxHealth = 7320
monster.health = 7320
monster.speed = 250
monster.manaCost = 0
monster.corpse = 7349
monster.outfit = { lookType = 234 }
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
        maxDamage = -640,
    },
    {
        name = "physical",
        interval = 2000,
        chance = 40,
        minDamage = -38,
        maxDamage = -150,
        shootEffect = CONST_ANI_THROWINGKNIFE,
    },
}
monster.defenses = {
    defense = 35,
    armor = 25,
    {
        name = "healing",
        interval = 1000,
        chance = 50,
        minDamage = 75,
        maxDamage = 125,
        effect = CONST_ME_MAGIC_BLUE,
    },
}
monster.elements = {
    {type = COMBAT_HOLYDAMAGE, percent = -15},
    {type = COMBAT_ENERGYDAMAGE, percent = 30},
}
monster.immunities = {
    {type = "fire", combat = true, condition = true},
    {type = "outfit", condition = true},
    {type = "drunk", condition = true},
    {type = "invisible", condition = true},
}
monster.voices = {
    interval = 5000,
    chance = 10,
    {text = "Didn't you leave your house door open?", yell = false},
    {text = "Oops, your shoelaces are open!", yell = false},
    {text = "Look! It's Ferumbras behind you!", yell = false},
    {text = "Stop! I give up!", yell = false},
}

mtype:register(monster)
