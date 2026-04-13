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
	inline void DamageModifier::setType(uint8_t modType)						{ mod_type = modType; }
	inline void DamageModifier::setStance(uint8_t stance)						{ mod_stance = stance; }
	inline void DamageModifier::setValue(uint16_t amount)						{ value = amount; }
	inline void DamageModifier::setChance(uint8_t chance)						{ this->chance = chance; }
	inline void DamageModifier::setFactor(uint8_t factor)						{ this->factor = factor; }
	inline void DamageModifier::setCombatType(uint16_t combatType)				{ damage_type = combatType; }
	inline void DamageModifier::setOriginType(uint8_t origin)					{ origin_type = origin; }
	inline void DamageModifier::setRaceType(uint8_t race)						{ race_type = race; }
	inline void DamageModifier::setCreatureType(uint8_t c_type)					{ creature_type = c_type; }
	inline void DamageModifier::setTransformDamageType(uint16_t damageType)		{ to_damage_type = damageType; }
	//inline void DamageModifier::setCreatureName(std::string_view creatureName)	{ m_creature_name = creatureName.data(); }
	inline const bool DamageModifier::isPercent() const							{ return factor == std::to_underlying(Factor::Percent); }
	inline const bool DamageModifier::isFlatValue() const						{ return factor == std::to_underlying(Factor::Flat); }


	inline const bool DamageModifier::appliesToDamage(const uint16_t damageType) const
	{
		return damage_type == COMBAT_NONE or damage_type == damageType;
	}

	inline const bool DamageModifier::appliesToOrigin(const uint8_t origin) const
	{
		bool applies = origin_type == 0 or origin_type == origin;
		applies = applies or origin_type >= BlackTek::Combat::Origin::Augment;
		return applies;
	}

	inline const bool DamageModifier::appliesToTarget(const uint8_t creatureType, const uint8_t race, const std::string_view creatureName) const
	{
		bool valid_target = (creatureType == creature_type or creature_type == CREATURETYPE_ATTACKABLE);
		valid_target = valid_target or ((creature_type == CREATURETYPE_SUMMON_ALL) and ( creatureType == CREATURETYPE_SUMMON_OWN or creatureType == CREATURETYPE_SUMMON_GUILD or creatureType == CREATURETYPE_SUMMON_HOSTILE or creatureType == CREATURETYPE_SUMMON_PARTY));
		valid_target = valid_target and (race_type == RACE_NONE or race_type == race);

		if (filter_index & Flag::Named)
			valid_target = valid_target and creatureName == ""; // todo: has monster name, must look up name by modifier id from a map

		return valid_target;
	}

	inline const bool DamageModifier::applies(uint16_t damageType, uint8_t creatureType, uint8_t origin, uint8_t race, const std::string_view creatureName) const
	{
		auto damage_counts = appliesToDamage(damageType);
		auto origin_counts = appliesToOrigin(origin);
		auto target_counts = appliesToTarget(creatureType, race, creatureName);
		return damage_counts and origin_counts and target_counts;
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

	//inline const bool DamageModifier::hasCreatureName() const					{ return not m_creature_name.empty() and m_creature_name != "none"; }
	//inline const std::string& DamageModifier::getMonsterName() const			{ return m_creature_name; }
	inline const uint16_t	DamageModifier::getConversionType() const			{ return to_damage_type; }
	inline const uint8_t	DamageModifier::getCreatureType() const				{ return creature_type; }
	inline const uint8_t	DamageModifier::getRaceType() const					{ return race_type; }

}