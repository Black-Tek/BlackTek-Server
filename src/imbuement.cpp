#include "imbuement.h"

bool Imbuement::isSkill() const {
    switch (this->imbuetype){

        case ImbuementType::IMBUEMENT_TYPE_FIST_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_AXE_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_SWORD_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_CLUB_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_DISTANCE_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_FISHING_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_SHIELD_SKILL :
        case ImbuementType::IMBUEMENT_TYPE_MAGIC_LEVEL :
            return true;

        default:
            return false;
    }
}


bool Imbuement::isSpecialSkill() const {
	switch (this->imbuetype){

		case ImbuementType::IMBUEMENT_TYPE_LIFE_LEECH :
		case ImbuementType::IMBUEMENT_TYPE_MANA_LEECH :
		case ImbuementType::IMBUEMENT_TYPE_CRITICAL_CHANCE :
		case ImbuementType::IMBUEMENT_TYPE_CRITICAL_AMOUNT:
			return true;

		default:
			return false;
	}
}


bool Imbuement::isDamage() const {
	switch (this->imbuetype){

		case ImbuementType::IMBUEMENT_TYPE_FIRE_DAMAGE :
		case ImbuementType::IMBUEMENT_TYPE_EARTH_DAMAGE :
		case ImbuementType::IMBUEMENT_TYPE_ICE_DAMAGE :
		case ImbuementType::IMBUEMENT_TYPE_ENERGY_DAMAGE :
		case ImbuementType::IMBUEMENT_TYPE_DEATH_DAMAGE :
		case ImbuementType::IMBUEMENT_TYPE_HOLY_DAMAGE :
			return true;

		default:
			return false;
	}
}


bool Imbuement::isResist() const {
	switch (this->imbuetype){

		case ImbuementType::IMBUEMENT_TYPE_FIRE_RESIST :
		case ImbuementType::IMBUEMENT_TYPE_EARTH_RESIST :
		case ImbuementType::IMBUEMENT_TYPE_ICE_DAMAGE :
		case ImbuementType::IMBUEMENT_TYPE_ENERGY_RESIST :
		case ImbuementType::IMBUEMENT_TYPE_DEATH_RESIST :
		case ImbuementType::IMBUEMENT_TYPE_HOLY_RESIST :
			return true;

		default:
			return false;
	}
}

bool Imbuement::isStat() const {
	switch (this->imbuetype){

		case ImbuementType::IMBUEMENT_TYPE_SPEED_BOOST :
		case ImbuementType::IMBUEMENT_TYPE_CAPACITY_BOOST :
			return true;

		default:
			return false;
	}
}

bool Imbuement::isEquipDecay() const {
	return this->decaytype == ImbuementDecayType::IMBUEMENT_DECAY_EQUIPPED;
}

bool Imbuement::isInfightDecay() const {
	return this->decaytype == ImbuementDecayType::IMBUEMENT_DECAY_INFIGHT;
}