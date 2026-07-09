#include "otpch.h"
#include "damagemodifier.h"
#include "combat.h"

namespace BlackTek
{
	gtl::flat_hash_map<uint64_t, std::string> modifier_monster_names;

	DamageModifier::~DamageModifier()
	{
		if (guid != 0)
			modifier_monster_names.erase(guid);
	}

	DamageModifier::DamageModifier(const DamageModifier& other)
		: guid(generateGUID()),
		  damage_type(other.damage_type),
		  to_damage_type(other.to_damage_type),
		  value(other.value),
		  filter_index(other.filter_index),
		  mod_stance(other.mod_stance),
		  mod_type(other.mod_type),
		  factor(other.factor),
		  chance(other.chance),
		  origin_type(other.origin_type),
		  creature_type(other.creature_type),
		  race_type(other.race_type),
		  true_leech(other.true_leech)
	{
		std::memcpy(name_buf, other.name_buf, sizeof(name_buf));

		if (filter_index & Flag::Named)
		{
			auto it = modifier_monster_names.find(other.guid);
			if (it != modifier_monster_names.end())
			{
				std::string name_copy = it->second;
				modifier_monster_names.try_emplace(guid, std::move(name_copy));
			}
			else if (name_buf[0] != '\0')
			{
				modifier_monster_names.try_emplace(guid, std::string_view(name_buf));
			}
		}
	}

	DamageModifier& DamageModifier::operator=(const DamageModifier& other)
	{
		if (this == &other)
			return *this;

		modifier_monster_names.erase(guid);
		damage_type    = other.damage_type;
		to_damage_type = other.to_damage_type;
		value          = other.value;
		filter_index   = other.filter_index;
		mod_stance     = other.mod_stance;
		mod_type       = other.mod_type;
		factor         = other.factor;
		chance         = other.chance;
		origin_type    = other.origin_type;
		creature_type  = other.creature_type;
		race_type      = other.race_type;
		true_leech     = other.true_leech;

		std::memcpy(name_buf, other.name_buf, sizeof(name_buf));

		if (filter_index & Flag::Named)
		{
			auto it = modifier_monster_names.find(other.guid);

			if (it != modifier_monster_names.end())
				modifier_monster_names.insert_or_assign(guid, it->second);
			else if (name_buf[0] != '\0')
				modifier_monster_names.insert_or_assign(guid, std::string_view(name_buf));
		}
		return *this;
	}

	DamageModifier::DamageModifier(DamageModifier&& other) noexcept
		: guid(other.guid),
		  damage_type(other.damage_type),
		  to_damage_type(other.to_damage_type),
		  value(other.value),
		  filter_index(other.filter_index),
		  mod_stance(other.mod_stance),
		  mod_type(other.mod_type),
		  factor(other.factor),
		  chance(other.chance),
		  origin_type(other.origin_type),
		  creature_type(other.creature_type),
		  race_type(other.race_type),
		  true_leech(other.true_leech)
	{
		std::memcpy(name_buf, other.name_buf, sizeof(name_buf));
		other.guid = 0;
	}

	DamageModifier& DamageModifier::operator=(DamageModifier&& other) noexcept
	{
		if (this == &other)
			return *this;

		modifier_monster_names.erase(guid);
		guid           = other.guid;
		other.guid     = 0;
		damage_type    = other.damage_type;
		to_damage_type = other.to_damage_type;
		value          = other.value;
		filter_index   = other.filter_index;
		mod_stance     = other.mod_stance;
		mod_type       = other.mod_type;
		factor         = other.factor;
		chance         = other.chance;
		origin_type    = other.origin_type;
		creature_type  = other.creature_type;
		race_type      = other.race_type;
		true_leech     = other.true_leech;
		std::memcpy(name_buf, other.name_buf, sizeof(name_buf));
		return *this;
	}

	void DamageModifier::increaseValue(uint16_t amount)
	{
		if ((value + amount) <= std::numeric_limits<uint16_t>::max())
		{
			value += amount;
		} 
		else
		{
			value = std::numeric_limits<uint16_t>::max();
			std::cout << "[WARNING] Amount exceded numeric limits for uint16_t. m_value set to limit." << "\n";
		}
	}

	void DamageModifier::decreaseValue(uint16_t amount)
	{
		if (value >= amount)
		{
			value -= amount;
		}
		else
		{
			value = 0;
			std::cout << "[WARNING] Amount is greater than value. value set to zero. " << "\n";
		}
	}

	/// Inline Methods' Definitions
void DamageModifier::setType(uint8_t modType)						
	{
		mod_type = modType;
		auto attack_mod = (mod_stance == std::to_underlying(Stance::Attack));
		
		if (attack_mod)
		{
			auto conversion = (modType == std::to_underlying(AttackType::Conversion));

			if (conversion)
				filter_index |= Flag::Converted;

			const auto is_healing = (modType == std::to_underlying(AttackType::Regeneration)
				or modType == std::to_underlying(AttackType::Attunement)
				or modType == std::to_underlying(AttackType::Vigor)
				or modType == std::to_underlying(AttackType::Transcendence));

			if (is_healing)
				filter_index |= Flag::HealBoost;
		}
		else
		{
			auto reformed = (modType == std::to_underlying(DefenseType::Reform));

			if (reformed)
				filter_index |= Flag::Reformed;
		}
	}

// I think we need to prevent users from changing the stance on a modifier so for now, we comment this one out..
//inline void DamageModifier::setStance(uint8_t stance)						{ mod_stance = stance; }
void DamageModifier::setValue(uint16_t amount)						{ value = amount; }
void DamageModifier::setChance(uint8_t amount)						{ chance = amount; }
void DamageModifier::setFactor(uint8_t value)						{ factor = value; }
const bool DamageModifier::isPercent() const noexcept				{ return factor == std::to_underlying(Factor::Percent); }
const bool DamageModifier::isFlatValue() const noexcept				{ return factor == std::to_underlying(Factor::Flat); }
void DamageModifier::setCreatureName(std::string_view creatureName) const noexcept
{
	modifier_monster_names.insert_or_assign(guid, std::string(creatureName));
	const size_t len = std::min(creatureName.size(), sizeof(name_buf) - 1);
	std::memcpy(name_buf, creatureName.data(), len);
	name_buf[len] = '\0';
}

void DamageModifier::setCombatType(uint16_t combatType)
	{
		if (combatType == damage_type)
			return;

		damage_type = combatType;

		if (combatType != 0)
			filter_index |= Flag::Damage;
		else
			filter_index &= ~static_cast<uint16_t>(Flag::Damage);
	}

void DamageModifier::setOriginType(uint8_t origin)
	{
		if (origin == origin_type)
			return;

		origin_type = origin;

		if (origin != 0)
			filter_index |= Flag::Origin;
		else
			filter_index &= ~static_cast<uint16_t>(Flag::Origin);
	}

void DamageModifier::setRaceType(uint8_t race)
	{
		if (race == race_type)
			return;

		race_type = race;

		if (race != 0)
			filter_index |= Flag::Race;
		else
			filter_index &= ~static_cast<uint16_t>(Flag::Race);
	}

void DamageModifier::setCreatureType(uint8_t c_type)
	{
		if (c_type == creature_type)
			return;

		creature_type = c_type;

		if (c_type != CREATURETYPE_ATTACKABLE)
			filter_index |= Flag::Creature;
		else
			filter_index &= ~static_cast<uint16_t>(Flag::Creature);
	}

void DamageModifier::setTransformDamageType(uint16_t damageType)
	{
		// if we set it to a conversion type, we already add to our filter appropriately
		// if the user forgot to set it to a conversion type, well then it shouldn't be flagged as one either
		to_damage_type = damageType;
	}

const bool DamageModifier::appliesToDamage(const uint16_t damageType) const noexcept
	{
		if (filter_index & DamageModifier::Flag::Damage)
			return damage_type == std::to_underlying(Combat::DamageType::Unknown) or damage_type == damageType;
		return true;
	}

const bool DamageModifier::appliesToOrigin(const uint8_t origin) const noexcept
	{
		if (filter_index & DamageModifier::Flag::Origin)
			return origin_type == 0 or origin_type == origin;
		return true;
	}

const bool DamageModifier::appliesToTargetType(const uint8_t creatureType) const
	{
		if (filter_index & DamageModifier::Flag::Creature)
		{
			auto applies = creature_type == std::to_underlying(CreatureType_t::CREATURETYPE_ATTACKABLE)
				or creature_type == creatureType
				or ((creature_type == CREATURETYPE_SUMMON_ALL) 
					and (creatureType == CREATURETYPE_SUMMON_OWN or creatureType == CREATURETYPE_SUMMON_GUILD or creatureType == CREATURETYPE_SUMMON_HOSTILE or creatureType == CREATURETYPE_SUMMON_PARTY));
			return applies;
		}
		return true;
	}

const bool DamageModifier::appliesToRaceType(const uint8_t race) const
	{
		if (filter_index & DamageModifier::Flag::Race)
			return race_type == std::to_underlying(RACE_NONE) or race_type == race;
		return true;
	}

const bool DamageModifier::appliesByName(const std::string_view creatureName) const
	{
		if (filter_index & DamageModifier::Flag::Named)
		{
			if (name_buf[0] != '\0' and std::string_view(name_buf) != creatureName)
				return false;

			auto it = modifier_monster_names.find(guid);
			if (it != modifier_monster_names.end())
				return creatureName == it->second;

			return false;
		}
		return true;
	}

const bool DamageModifier::applies(uint16_t damageType, uint8_t creatureType, uint8_t origin, uint8_t race, const std::string_view creatureName) const noexcept
	{
		const auto damage_counts	= appliesToDamage(damageType);
		const auto origin_counts	= appliesToOrigin(origin);
		const auto target_counts	= appliesToTargetType(creatureType);
		const auto race_counts		= appliesToRaceType(race);
		const auto name_counts		= appliesByName(creatureName);

		return damage_counts and origin_counts and target_counts and race_counts and name_counts;
	}

const bool DamageModifier::hasCreatureName() const noexcept
	{ 
		return modifier_monster_names.find(guid) != modifier_monster_names.end();
	}

const std::string& DamageModifier::getMonsterName() const noexcept
	{
		static const std::string none = "none";
		auto it = modifier_monster_names.find(guid);
		if (it != modifier_monster_names.end())
			return it->second;
		return none;
	}

const uint64_t	DamageModifier::getGUID() const	noexcept			{ return guid; }
const uint8_t	DamageModifier::getStance() const noexcept			{ return mod_stance; }
const uint8_t	DamageModifier::getType() const noexcept			{ return mod_type; }
const uint16_t	DamageModifier::getValue() const noexcept			{ return value; }
const uint8_t	DamageModifier::getChance() const noexcept			{ return chance; }
const uint16_t	DamageModifier::getDamageType() const noexcept		{ return damage_type; }
const uint8_t	DamageModifier::getOriginType() const noexcept		{ return origin_type; }
const bool		DamageModifier::isAttackStance() const noexcept		{ return mod_stance == std::to_underlying(Stance::Attack); }
const bool		DamageModifier::isDefenseStance() const noexcept	{ return mod_stance == std::to_underlying(Stance::Defense); }
const uint16_t	DamageModifier::getConversionType() const noexcept	{ return to_damage_type; }
const uint16_t	DamageModifier::getFilterIndex() const noexcept		{ return filter_index;}
const uint8_t	DamageModifier::getCreatureType() const noexcept	{ return creature_type; }
const uint8_t	DamageModifier::getRaceType() const	noexcept		{ return race_type; }
}