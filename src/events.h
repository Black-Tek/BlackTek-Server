// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_EVENTS_H
#define FS_EVENTS_H

#include "luascript.h"
#include "const.h"
#include "creature.h"
#include "party.h"

class ItemType;
class Tile;
class Spell;
class DamageModifier;

enum class EventInfoId {
	// Creature
	CREATURE_ONHEAR,

	// Monster
	MONSTER_ONSPAWN
};

class Events
{
	struct EventsInfo {
		// Creature
		int32_t creatureOnChangeOutfit = -1;
		int32_t creatureOnAreaCombat = -1;
		int32_t creatureOnTargetCombat = -1;
		int32_t creatureOnHear = -1;
		int32_t creatureOnAttack = -1;
		int32_t creatureOnDefend = -1;

		// Party
		int32_t partyOnJoin = -1;
		int32_t partyOnLeave = -1;
		int32_t partyOnDisband = -1;
		int32_t partyOnShareExperience = -1;
		int32_t partyOnInvite = -1;
		int32_t partyOnRevokeInvitation = -1;
		int32_t partyOnPassLeadership = -1;

		// Player
		int32_t playerOnBrowseField = -1;
		int32_t playerOnLook = -1;
		int32_t playerOnLookInBattleList = -1;
		int32_t playerOnLookInTrade = -1;
		int32_t playerOnLookInShop = -1;
		int32_t playerOnMoveItem = -1;
		int32_t playerOnItemMoved = -1;
		int32_t playerOnMoveCreature = -1;
		int32_t playerOnReportRuleViolation = -1;
		int32_t playerOnReportBug = -1;
		int32_t playerOnTurn = -1;
		int32_t playerOnTradeRequest = -1;
		int32_t playerOnTradeAccept = -1;
		int32_t playerOnTradeCompleted = -1;
		int32_t playerOnGainExperience = -1;
		int32_t playerOnLoseExperience = -1;
		int32_t playerOnGainSkillTries = -1;
		int32_t playerOnWrapItem = -1;
		int32_t playerOnInventoryUpdate = -1;
		int32_t playerOnRotateItem = -1;
		int32_t playerOnSpellTry = -1;
		int32_t playerOnAugment = -1;
		int32_t playerOnRemoveAugment = -1;

		// Monster
		int32_t monsterOnDropLoot = -1;
		int32_t monsterOnSpawn = -1;

		// Item
		int32_t itemOnImbue = -1;
		int32_t itemOnRemoveImbue = -1;
		int32_t itemOnAttack = -1;
		int32_t itemOnDefend = -1;
		int32_t itemOnAugment = -1;
		int32_t itemOnRemoveAugment = -1;
		int32_t itemOnModifierAttack = -1;
		int32_t itemOnModifierDefend = -1;

	};

	public:
		Events();

		bool load();

		// Creature
		bool eventCreatureOnChangeOutfit(const CreaturePtr& creature, const Outfit_t& outfit);
		ReturnValue eventCreatureOnAreaCombat(const CreaturePtr& creature, const TilePtr& tile, bool aggressive);
		ReturnValue eventCreatureOnTargetCombat(const CreaturePtr& creature, const CreaturePtr& target);
		void eventCreatureOnHear(const CreaturePtr& creature, const CreaturePtr& speaker, const std::string& words, SpeakClasses type);
		void eventCreatureOnAttack(const CreaturePtr& attacker, const CreaturePtr& target, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);
		void eventCreatureOnDefend(const CreaturePtr& defender, const CreaturePtr& attacker, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);

		// Party
		bool eventPartyOnJoin(PartyPtr party, const PlayerPtr& player);
		bool eventPartyOnLeave(PartyPtr party, const PlayerPtr& player);
		bool eventPartyOnDisband(PartyPtr party);
		void eventPartyOnShareExperience(PartyPtr party, uint64_t& exp);
		bool eventPartyOnInvite(PartyPtr party, const PlayerPtr& player);
		bool eventPartyOnRevokeInvitation(PartyPtr party, const PlayerPtr& player);
		bool eventPartyOnPassLeadership(PartyPtr party, const PlayerPtr& player);

		// Player
		bool eventPlayerOnBrowseField(const PlayerPtr& player, const Position& position);
		void eventPlayerOnLook(const PlayerPtr& player, const Position& position, const ThingPtr& thing, uint8_t stackpos, int32_t lookDistance);
		void eventPlayerOnLookInBattleList(const PlayerPtr& player, const CreaturePtr& creature, int32_t lookDistance);
		void eventPlayerOnLookInTrade(const PlayerPtr& player, const PlayerPtr& partner, const ItemPtr& item, int32_t lookDistance);
		bool eventPlayerOnLookInShop(const PlayerPtr& player, const ItemType* itemType, uint8_t count, const std::string& description);
		ReturnValue eventPlayerOnMoveItem(const PlayerPtr& player, const ItemPtr& item, uint16_t count, const Position& fromPosition, const Position& toPosition, const CylinderPtr& fromCylinder, const CylinderPtr& toCylinder);
		void eventPlayerOnItemMoved(const PlayerPtr& player, const ItemPtr& item, uint16_t count, const Position& fromPosition, const Position& toPosition, const CylinderPtr& fromCylinder, const CylinderPtr& toCylinder);
		bool eventPlayerOnMoveCreature(const PlayerPtr& player, const CreaturePtr& creature, const Position& fromPosition, const Position& toPosition);
		void eventPlayerOnReportRuleViolation(const PlayerPtr& player, const std::string& targetName, uint8_t reportType, uint8_t reportReason, const std::string& comment, const std::string& translation);
		bool eventPlayerOnReportBug(const PlayerPtr& player, const std::string& message, const Position& position, uint8_t category);
		bool eventPlayerOnTurn(const PlayerPtr& player, Direction direction);
		bool eventPlayerOnTradeRequest(const PlayerPtr& player, const PlayerPtr& target, const ItemPtr& item);
		bool eventPlayerOnTradeAccept(const PlayerPtr& player, const PlayerPtr& target, const ItemPtr& item, const ItemPtr& targetItem);
		void eventPlayerOnTradeCompleted(const PlayerPtr& player, const PlayerPtr& target, const ItemPtr& item, const ItemPtr& targetItem, bool isSuccess);
		void eventPlayerOnGainExperience(const PlayerPtr& player, const CreaturePtr& source, uint64_t& exp, uint64_t rawExp);
		void eventPlayerOnLoseExperience(const PlayerPtr& player, uint64_t& exp);
		void eventPlayerOnGainSkillTries(const PlayerPtr& player, skills_t skill, uint64_t& tries);
		void eventPlayerOnWrapItem(const PlayerPtr& player, const ItemPtr& item);
		void eventPlayerOnInventoryUpdate(const PlayerPtr& player, const ItemPtr& item, slots_t slot, bool equip);
		void eventPlayerOnRotateItem(const PlayerPtr& player, const ItemPtr& item);
		bool eventPlayerOnSpellTry(const PlayerPtr& player, const Spell* spell, SpellType_t spellType);
		void eventPlayerOnAugment(const PlayerPtr& player, std::shared_ptr<Augment> augment);
		void eventPlayerOnRemoveAugment(const PlayerPtr& player, std::shared_ptr<Augment> augment);

		// Monster
		void eventMonsterOnDropLoot(const MonsterPtr& monster, const ContainerPtr& corpse);
		bool eventMonsterOnSpawn(const MonsterPtr& monster, const Position& position, bool startup, bool artificial);

		// Item
		bool eventItemOnImbue(const ItemPtr& item, const std::shared_ptr<Imbuement>& imbuement, bool created = true);
		void eventItemOnRemoveImbue(const ItemPtr& item, ImbuementType imbueType, bool decayed = false);

		void eventItemOnAttack(const ItemPtr& item, const PlayerPtr& itemHolder, const CreaturePtr& defender, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);
		void eventItemOnDefend(const ItemPtr& item, const PlayerPtr& itemHolder, const CreaturePtr& attacker, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);
		void eventItemOnAugment(const ItemPtr& item, std::shared_ptr<Augment> augment);
		void eventItemOnRemoveAugment(const ItemPtr& item, std::shared_ptr<Augment> augment);

		void eventItemOnModifierAttack(const ItemPtr& item, const PlayerPtr& itemHolder, const CreaturePtr& defender, const std::shared_ptr<DamageModifier>& modifier, CombatDamage& damage);
    	void eventItemOnModifierDefend(const ItemPtr& item, const PlayerPtr& itemHolder, const CreaturePtr& attacker, const std::shared_ptr<DamageModifier>& modifier, CombatDamage& damage);

		constexpr auto getScriptId(EventInfoId eventInfoId) const {
			switch (eventInfoId)
			{
			case EventInfoId::CREATURE_ONHEAR:
				return info.creatureOnHear;
			case EventInfoId::MONSTER_ONSPAWN:
				return info.monsterOnSpawn;
			default:
				return -1;
			}
		};

	private:
		LuaScriptInterface scriptInterface;
		EventsInfo info;
};

#endif
