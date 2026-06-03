-- Combat table definitions for monster spells.
-- Spells with setCallback keep that call in the spell file; base props come from here.
-- hirintror_skill_reducer and glooth_fairy_skill_reducer create dynamic combat arrays
-- and are excluded — their spell files remain self-contained.

-- Pre-built custom areas (inline definitions from original spell files)
local _waveTArea             = createCombatArea({{1, 1, 1}, {0, 1, 0}, {0, 3, 0}})
local _explosionWaveArea     = createCombatArea({{1, 1, 1}, {1, 1, 1}, {0, 1, 0}, {0, 3, 0}})
local _lloydWave1Area        = createCombatArea({
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
})
local _lloydWave2Area        = createCombatArea({
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
})
local _ferumbrasSoulfireArea = createCombatArea({
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
})
local _ferumbrasElectrifyArea = createCombatArea({
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 3, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
})
local _energyPulseArea = createCombatArea({
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
})
local _remorselessWaveArea = createCombatArea({
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
})
local _sourceOfCorruptionArea = createCombatArea({{0,0,0,0,0},{0,1,3,1,0},{0,0,0,0,0}})
local _freedSoulArea = createCombatArea({
    {0, 0, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 3, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 0, 0},
})
local _frozenMinionWaveArea = createCombatArea({
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0},
})
local _aggressiveLavaBombArea = createCombatArea({
    {0, 1, 1, 1, 0},
    {1, 1, 1, 1, 1},
    {1, 1, 3, 1, 1},
    {1, 1, 1, 1, 1},
    {0, 1, 1, 1, 0},
})
local _emberBeastBeamArea = createCombatArea({{1},{1},{1},{1},{1},{3}})
local _emberBeastAreaArea = createCombatArea({{1,1,1},{1,3,1},{1,1,1}})

-- Pre-created conditions for combats that embed them
local _physicalExplosionCondition = Condition(Combat.DamageType.Physical) -- intentional: evaluates to 1
_physicalExplosionCondition:setParameter(CONDITION_PARAM_DELAYED, 1)
_physicalExplosionCondition:addDamage(3, 10000, -25)

local _ferumbrasSoulfireCondition = Condition(CONDITION_FIRE)
_ferumbrasSoulfireCondition:setParameter(CONDITION_PARAM_DELAYED, 1)
_ferumbrasSoulfireCondition:addDamage(50, 9000, -10)

local _pixieCondition = Condition(CONDITION_ATTRIBUTES)
_pixieCondition:setParameter(CONDITION_PARAM_TICKS, 6000)
_pixieCondition:setParameter(CONDITION_PARAM_SKILL_DISTANCEPERCENT, 30)
_pixieCondition:setParameter(CONDITION_PARAM_SKILL_SHIELDPERCENT, 30)
_pixieCondition:setParameter(CONDITION_PARAM_SKILL_MELEEPERCENT, 30)

local _hirintrorFreezeCondition = Condition(CONDITION_FREEZING)
_hirintrorFreezeCondition:setParameter(CONDITION_PARAM_DELAYED, 1)
_hirintrorFreezeCondition:addDamage(25, 8000, -8)

MonsterCombats = {

    -- Elemental damage-over-time spells (use getTargets + addDamageCondition in spell file)
    GhastlyDragonCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
    },

    DjinnElectrify = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGY,
    },

    DrakenAbominationCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
    },

    LancerBeetleCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
    },

    BlightwalkerCurse = {
        damageType   = Combat.DamageType.Death,
        impactEffect = CONST_ME_SMALLCLOUDS,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    ChokingFearDrown = {
        damageType   = Combat.DamageType.Drown,
        impactEffect = CONST_ME_BUBBLES,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    CliffStriderElectrify = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYHIT,
    },

    DeathBlobCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
    },

    DjinnCancelInvisibility = {
        area = createCombatArea(AREA_CIRCLE3X3),
    },

    EnergyElementalElectrify = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_BLOCKHIT,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    HellspawnSoulfire = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_HITBYFIRE,
        distanceEffect = CONST_ANI_FIRE,
    },

    HellfireFighterSoulfire = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_FIREATTACK,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    LavaGolemSoulfire = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_EXPLOSIONHIT,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    LizardMagistratusCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
    },

    MagmaCrawlerSoulfire = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_EXPLOSIONHIT,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    MassiveEnergyElementalElectrify = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_BLOCKHIT,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    MassiveFireElementalSoulfire = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_MAGIC_RED,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    MonsterSoulfire = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREAREA,
        distanceEffect = CONST_ANI_FIRE,
    },

    MutatedBatCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
        area           = createCombatArea(AREA_SQUAREWAVE7),
    },

    PhanstamDrown = {
        damageType   = Combat.DamageType.Drown,
        impactEffect = CONST_ME_MAGIC_BLUE,
        area         = createCombatArea(AREA_SQUAREWAVE7),
    },

    QuaraConstrictorElectrify = {
        damageType     = Combat.DamageType.Energy,
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGY,
    },

    QuaraConstrictorFreeze = {
        damageType   = Combat.DamageType.Ice,
        impactEffect = CONST_ME_GREEN_RINGS,
        area         = createCombatArea(AREA_SQUARE1X1),
    },

    SeaSerpentDrown = {
        damageType   = Combat.DamageType.Drown,
        impactEffect = CONST_ME_WATERSPLASH,
        area         = createCombatArea(AREA_SQUARE1X1),
    },

    SouleaterDrown = {
        damageType   = Combat.DamageType.Drown,
        impactEffect = CONST_ME_MORTAREA,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    SpectreDrown = {
        damageType   = Combat.DamageType.Drown,
        impactEffect = CONST_ME_MAGIC_GREEN,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    UndeadDragonCurse = {
        damageType     = Combat.DamageType.Death,
        impactEffect   = CONST_ME_SMALLCLOUDS,
        distanceEffect = CONST_ANI_DEATH,
        area           = createCombatArea(AREA_SQUAREWAVE7),
    },

    VulcongraSoulfire = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = createCombatArea(AREA_SQUARE1X1),
    },

    WarGolemElectrify = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYHIT,
    },

    YoungSeaSerpentDrown = {
        damageType   = Combat.DamageType.Drown,
        impactEffect = CONST_ME_WATERSPLASH,
        area         = createCombatArea(AREA_SQUARE1X1),
    },

    -- Custom-area attacks
    WaveT = {
        damageType   = Combat.DamageType.Earth,
        impactEffect = CONST_ME_GREEN_RINGS,
        area         = _waveTArea,
    },

    ExplosionWave = {
        damageType   = Combat.DamageType.Physical,
        impactEffect = CONST_ME_EXPLOSIONHIT,
        area         = _explosionWaveArea,
    },

    LloydWave1 = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_PURPLECHAIN,
        area         = _lloydWave1Area,
    },

    LloydWave2 = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_PURPLECHAIN,
        area         = _lloydWave2Area,
    },

    EnergyPulseExplosion = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYAREA,
        area         = _energyPulseArea,
    },

    FerumbrasSoulfire = {
        damageType     = Combat.DamageType.Fire,
        impactEffect   = CONST_ME_FIREAREA,
        distanceEffect = CONST_ANI_FIRE,
        area           = _ferumbrasSoulfireArea,
        condition      = _ferumbrasSoulfireCondition,
    },

    FerumbrasElectrify = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_ENERGYHIT,
        area         = _ferumbrasElectrifyArea,
    },

    -- Spells with setCallback — base props only; callback wired in spell file
    RemorselessWave = {
        damageType   = Combat.DamageType.Death,
        impactEffect = CONST_ME_BLACKSMOKE,
        area         = _remorselessWaveArea,
    },

    SourceOfCorruptionWave = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_PURPLEENERGY,
        area         = _sourceOfCorruptionArea,
    },

    FreedSoulSpell = {
        damageType   = Combat.DamageType.Energy,
        impactEffect = CONST_ME_PURPLEENERGY,
        area         = _freedSoulArea,
    },

    FrozenMinionWave = {
        impactEffect = CONST_ME_POFF,
        aggressive   = false,
        area         = _frozenMinionWaveArea,
    },

    FrozenMinionBeam = {
        impactEffect = CONST_ME_POFF,
        aggressive   = false,
        area         = createCombatArea(AREA_BEAM7),
    },

    FrozenMinionHeal = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    IcicleHeal = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    HealMonsters = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    HealMonsters9x9 = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        aggressive   = false,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    AggressiveLavaBomb = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_HITBYFIRE,
        area         = _aggressiveLavaBombArea,
    },

    EmberBeastBeam = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_EXPLOSIONHIT,
        area         = _emberBeastBeamArea,
    },

    EmberBeastArea = {
        damageType   = Combat.DamageType.Fire,
        impactEffect = CONST_ME_EXPLOSIONHIT,
        area         = _emberBeastAreaArea,
    },

    -- Combat with embedded condition
    PhysicalExplosion = {
        damageType   = Combat.DamageType.Physical,
        impactEffect = 6,
        area         = createCombatArea(AREA_SQUARE1X1),
        condition    = _physicalExplosionCondition,
    },

    -- Summon challenge (setCallback in spell file)
    SummonChallenge = {
        impactEffect = CONST_ME_MAGIC_BLUE,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    -- Hirintror freeze (embedded condition)
    HirintrorFreeze = {
        impactEffect = CONST_ME_ICETORNADO,
        area         = createCombatArea({
            {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
            {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
            {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1},
            {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
            {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
            {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
            {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        }),
        condition    = _hirintrorFreezeCondition,
    },

    -- Pixie (embedded CONDITION_ATTRIBUTES)
    PixieSkillReducer = {
        impactEffect = CONST_ME_FAEEXPLOSION,
        area         = createCombatArea(AREA_CIRCLE2X2),
        condition    = _pixieCondition,
    },

    -- Skill reducers — only visual/area props; parameter tables stay in spell files
    BarbarianBrutetamerSkillReducer = {
        distanceEffect = CONST_ANI_SNOWBALL,
        impactEffect   = CONST_ME_POFF,
    },

    BetrayedWraithSkillReducer = {
        impactEffect = CONST_ME_YELLOW_RINGS,
        area         = createCombatArea(AREA_SQUAREWAVE5),
    },

    CliffStriderSkillReducer = {
        impactEffect = CONST_ME_MAGIC_RED,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    DarkTorturerSkillReducer = {
        impactEffect = CONST_ME_SOUND_PURPLE,
        area         = createCombatArea(AREA_SQUAREWAVE6),
    },

    DeeplingSpellsingerSkillReducer = {
        impactEffect   = CONST_ME_STUN,
        distanceEffect = CONST_ANI_EXPLOSION,
        area           = createCombatArea(AREA_BEAM1),
    },

    DemonOutcastSkillReducer = {
        distanceEffect = CONST_ANI_FLASHARROW,
        area           = createCombatArea(AREA_BEAM1),
    },

    DiabolicImpSkillReducer = {
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_BEAM1),
    },

    DipthrahSkillReducer = {
        impactEffect = CONST_ME_HOLYAREA,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    EnslavedDwarfSkillReducer1 = {
        impactEffect = CONST_ME_MAGIC_RED,
        area         = createCombatArea(AREA_CIRCLE2X2),
    },

    EnslavedDwarfSkillReducer2 = {
        impactEffect = CONST_ME_HITAREA,
        area         = createCombatArea(AREA_CROSS1X1),
    },

    FeversleepSkillReducer = {
        impactEffect = CONST_ME_STUN,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    ForestFurySkillReducer = {
        impactEffect   = CONST_ME_MAGIC_BLUE,
        distanceEffect = CONST_ANI_LARGEROCK,
        area           = createCombatArea(AREA_CIRCLE2X2),
    },

    FurySkillReducer = {
        impactEffect = CONST_ME_SOUND_YELLOW,
        area         = createCombatArea(AREA_CIRCLE3X3),
    },

    IceGolemSkillReducer = {
        impactEffect = CONST_ME_HITAREA,
        area         = createCombatArea(AREA_SQUARE1X1),
    },

    PirateCorsairSkillReducer = {
        impactEffect = CONST_ME_SOUND_PURPLE,
        area         = createCombatArea(AREA_BEAM1),
    },

    ShockHeadSkillReducer1 = {
        impactEffect   = CONST_ME_GROUNDSHAKER,
        distanceEffect = CONST_ANI_EXPLOSION,
        area           = createCombatArea(AREA_CIRCLE2X2),
    },

    ShockHeadSkillReducer2 = {
        impactEffect = CONST_ME_STUN,
        area         = createCombatArea(AREA_CIRCLE6X6),
    },

    SilencerSkillReducer = {
        impactEffect   = CONST_ME_ENERGYHIT,
        distanceEffect = CONST_ANI_ENERGY,
        area           = createCombatArea(AREA_CIRCLE2X2),
    },

    StamporSkillReducer = {
        impactEffect   = CONST_ME_SMALLPLANTS,
        distanceEffect = CONST_ANI_SMALLEARTH,
        area           = createCombatArea(AREA_BEAM1),
    },

    WarlockSkillReducer = {
        impactEffect   = CONST_ME_ICEAREA,
        distanceEffect = CONST_ANI_ICE,
        area           = createCombatArea(AREA_BEAM1),
    },

    WarGolemSkillReducer = {
        impactEffect = CONST_ME_STUN,
        area         = createCombatArea(AREA_BEAM8),
    },

    WerewolfSkillReducer = {
        impactEffect = CONST_ME_DRAWBLOOD,
        area         = createCombatArea(AREA_BEAM1),
    },
}
