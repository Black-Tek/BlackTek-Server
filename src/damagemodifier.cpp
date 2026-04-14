#include "otpch.h"
#include "damagemodifier.h"
#include "combat.h"

namespace BlackTek
{
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
	inline void DamageModifier::setType(uint8_t modType)						
	{
		mod_type = modType;
		auto attack_mod = (mod_stance == std::to_underlying(Stance::Attack));
		
		if (attack_mod)
		{
			auto conversion = (modType == std::to_underlying(AttackType::Conversion));

			if (conversion)
				filter_index |= Flag::Converted;
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
	inline void DamageModifier::setValue(uint16_t amount)						{ value = amount; }
	inline void DamageModifier::setChance(uint8_t amount)						{ chance = amount; }
	inline void DamageModifier::setFactor(uint8_t value)						{ factor = value; }
	inline const bool DamageModifier::isPercent() const							{ return factor == std::to_underlying(Factor::Percent); }
	inline const bool DamageModifier::isFlatValue() const						{ return factor == std::to_underlying(Factor::Flat); }
	// Mental Note: I just realized while writing this function, that an additional benefit to "out of band"ing these strings (sparse data) is that
	// when going to set the data, because we store it in a container, we are not changing the object, we can actually use const XD
	inline void DamageModifier::setCreatureName(std::string_view creatureName) const noexcept { modifier_monster_names.try_emplace(guid, creatureName); }

	inline void DamageModifier::setCombatType(uint16_t combatType)				
	{
		if (combatType == damage_type)
			return;

		auto original = damage_type;
		damage_type = combatType;

		if (combatType != 0)
			filter_index |= Flag::Damage;

		// So here is how it works &= adds the flag
		// and ~ clears a flag, so adding a cleared flag to the mask
		// apparently is how you clear the flag from the mask.
		filter_index &= ~original;
	}

	inline void DamageModifier::setOriginType(uint8_t origin)
	{
		if (origin == origin_type)
			return;

		auto original = origin_type;
		origin_type = origin;

		if (origin != 0)
			filter_index |= Flag::Origin;

		filter_index &= ~original;
	}

	inline void DamageModifier::setRaceType(uint8_t race)
	{
		if (race == race_type)
			return;

		auto original = race_type;
		race_type = race;

		if (race != 0)
			filter_index |= Flag::Race;

		filter_index &= ~original;
	}

	inline void DamageModifier::setCreatureType(uint8_t c_type)
	{
		if (c_type == creature_type)
			return;

		auto original = creature_type;
		creature_type = c_type;

		if (c_type != CREATURETYPE_ATTACKABLE)
			filter_index |= Flag::Creature;

		filter_index &= ~original;
	}

	inline void DamageModifier::setTransformDamageType(uint16_t damageType)
	{
		// if we set it to a conversion type, we already add to our filter appropriately
		// if the user forgot to set it to a conversion type, well then it shouldn't be flagged as one either
		to_damage_type = damageType;
	}

	inline const bool DamageModifier::appliesToDamage(const uint16_t damageType) const
	{
		return damage_type == std::to_underlying(Combat::DamageType::Unknown) or damage_type == damageType;
	}

	inline const bool DamageModifier::appliesToOrigin(const uint8_t origin) const
	{
		bool applies = origin_type == 0 or origin_type == origin;
		applies = applies or origin_type >= std::to_underlying(Combat::Origin::Augment);
		return applies;
	}

	inline const bool DamageModifier::appliesToTarget(const uint8_t creatureType, const uint8_t race, const std::string_view creatureName) const
	{
		// todo: Create a new embedded enum for these creature types and call it TargetType, use that for combat and here and revert back to the old
		// standardized "creaturetype" data/values passed along to the client for tha purpose, while using the TargetType for combat 
		bool valid_target = (creatureType == creature_type or creature_type == CREATURETYPE_ATTACKABLE);
		valid_target = valid_target or ((creature_type == CREATURETYPE_SUMMON_ALL) and ( creatureType == CREATURETYPE_SUMMON_OWN or creatureType == CREATURETYPE_SUMMON_GUILD or creatureType == CREATURETYPE_SUMMON_HOSTILE or creatureType == CREATURETYPE_SUMMON_PARTY));
		valid_target = valid_target and (race_type == RACE_NONE or race_type == race);

		if (filter_index & Flag::Named)
		{
			auto it = modifier_monster_names.find(guid);
			if (it != modifier_monster_names.end())
				valid_target = valid_target and creatureName == it->second;
		}
		return valid_target;
	}

	inline const bool DamageModifier::applies(uint16_t damageType, uint8_t creatureType, uint8_t origin, uint8_t race, const std::string_view creatureName) const
	{
		auto damage_counts = appliesToDamage(damageType);
		auto origin_counts = appliesToOrigin(origin);
		auto target_counts = appliesToTarget(creatureType, race, creatureName);
		return damage_counts and origin_counts and target_counts;
	}

	inline const bool DamageModifier::hasCreatureName() const
	{ 
		return modifier_monster_names.find(guid) != modifier_monster_names.end();
	}

	inline const std::string& DamageModifier::getMonsterName() const
	{
		auto it = modifier_monster_names.find(guid);
		if (it != modifier_monster_names.end())
			return it->second;
		return "none";
	}

	inline const uint64_t	DamageModifier::getGUID() const						{ return guid; }
	inline const uint8_t	DamageModifier::getStance() const					{ return mod_stance; }
	inline const uint8_t	DamageModifier::getType() const						{ return mod_type; }
	inline const uint16_t	DamageModifier::getValue() const					{ return value; }
	inline const uint8_t	DamageModifier::getChance() const					{ return chance; }
	inline const uint16_t	DamageModifier::getDamageType() const				{ return damage_type; }
	inline const uint8_t	DamageModifier::getOriginType() const				{ return origin_type; }
	inline const bool		DamageModifier::isAttackStance() const				{ return mod_stance == std::to_underlying(Stance::Attack); }
	inline const bool		DamageModifier::isDefenseStance() const				{ return mod_stance == std::to_underlying(Stance::Defense); }
	inline const uint16_t	DamageModifier::getConversionType() const			{ return to_damage_type; }
	inline const uint8_t	DamageModifier::getCreatureType() const				{ return creature_type; }
	inline const uint8_t	DamageModifier::getRaceType() const					{ return race_type; }
}