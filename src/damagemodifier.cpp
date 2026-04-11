#include "otpch.h"
#include "damagemodifier.h"

void DamageModifier::setTransformDamageType(CombatType_t damageType) { to_damage_type = damageType; }

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
