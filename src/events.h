// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_EVENTS_H
#define FS_EVENTS_H

#include "luascript.h"
#include "const.h"
#include "creature.h"

class Party;
class ItemType;
class Tile;
class Spell;

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

	};

	public:
		Events();

		bool load();

		// Creature
		bool eventCreatureOnChangeOutfit(Creature* creature, const Outfit_t& outfit);
		ReturnValue eventCreatureOnAreaCombat(Creature* creature, Tile* tile, bool aggressive);
		ReturnValue eventCreatureOnTargetCombat(Creature* creature, Creature* target);
		void eventCreatureOnHear(Creature* creature, Creature* speaker, const std::string& words, SpeakClasses type);
		void eventCreatureOnAttack(Creature* attacker, Creature* target, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);
		void eventCreatureOnDefend(Creature* defender, Creature* attacker, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);

		// Party
		bool eventPartyOnJoin(Party* party, Player* player);
		bool eventPartyOnLeave(Party* party, Player* player);
		bool eventPartyOnDisband(Party* party);
		void eventPartyOnShareExperience(Party* party, uint64_t& exp);
		bool eventPartyOnInvite(Party* party, Player* player);
		bool eventPartyOnRevokeInvitation(Party* party, Player* player);
		bool eventPartyOnPassLeadership(Party* party, Player* player);

		// Player
		bool eventPlayerOnBrowseField(Player* player, const Position& position);
		void eventPlayerOnLook(Player* player, const Position& position, Thing* thing, uint8_t stackpos, int32_t lookDistance);
		void eventPlayerOnLookInBattleList(Player* player, Creature* creature, int32_t lookDistance);
		void eventPlayerOnLookInTrade(Player* player, Player* partner, Item* item, int32_t lookDistance);
		bool eventPlayerOnLookInShop(Player* player, const ItemType* itemType, uint8_t count, const std::string& description);
		ReturnValue eventPlayerOnMoveItem(Player* player, Item* item, uint16_t count, const Position& fromPosition, const Position& toPosition, Cylinder* fromCylinder, Cylinder* toCylinder);
		void eventPlayerOnItemMoved(Player* player, Item* item, uint16_t count, const Position& fromPosition, const Position& toPosition, Cylinder* fromCylinder, Cylinder* toCylinder);
		bool eventPlayerOnMoveCreature(Player* player, Creature* creature, const Position& fromPosition, const Position& toPosition);
		void eventPlayerOnReportRuleViolation(Player* player, const std::string& targetName, uint8_t reportType, uint8_t reportReason, const std::string& comment, const std::string& translation);
		bool eventPlayerOnReportBug(Player* player, const std::string& message, const Position& position, uint8_t category);
		bool eventPlayerOnTurn(Player* player, Direction direction);
		bool eventPlayerOnTradeRequest(Player* player, Player* target, Item* item);
		bool eventPlayerOnTradeAccept(Player* player, Player* target, Item* item, Item* targetItem);
		void eventPlayerOnTradeCompleted(Player* player, Player* target, Item* item, Item* targetItem, bool isSuccess);
		void eventPlayerOnGainExperience(Player* player, Creature* source, uint64_t& exp, uint64_t rawExp);
		void eventPlayerOnLoseExperience(Player* player, uint64_t& exp);
		void eventPlayerOnGainSkillTries(Player* player, skills_t skill, uint64_t& tries);
		void eventPlayerOnWrapItem(Player* player, Item* item);
		void eventPlayerOnInventoryUpdate(Player* player, Item* item, slots_t slot, bool equip);
		void eventPlayerOnRotateItem(Player* player, Item* item);
		bool eventPlayerOnSpellTry(Player* player, const Spell* spell, SpellType_t spellType);
		void eventPlayerOnAugment(Player* player, std::shared_ptr<Augment> augment);
		void eventPlayerOnRemoveAugment(Player* player, std::shared_ptr<Augment> augment);

		// Monster
		void eventMonsterOnDropLoot(Monster* monster, Container* corpse);
		bool eventMonsterOnSpawn(Monster* monster, const Position& position, bool startup, bool artificial);

		// Item
		bool eventItemOnImbue(Item* item, std::shared_ptr<Imbuement> imbuement, bool created = true);
		void eventItemOnRemoveImbue(Item* item, ImbuementType imbueType, bool decayed = false);

		void eventItemOnAttack(Item* item, Player* itemHolder, Creature* defender, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);
		void eventItemOnDefend(Item* item, Player* itemHolder, Creature* attacker, BlockType_t blockType, CombatType_t combatType, CombatOrigin origin, bool criticalDamage = false, bool leechedDamage = false);
		void eventItemOnAugment(Item* item, std::shared_ptr<Augment> augment);
		void eventItemOnRemoveAugment(Item* item, std::shared_ptr<Augment> augment);
		

		int32_t getScriptId(EventInfoId eventInfoId) {
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
