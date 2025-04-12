// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_GAME_H
#define FS_GAME_H

#include <curl/curl.h>

#include "account.h"
#include "combat.h"
#include "groups.h"
#include "map.h"
#include "position.h"
#include "item.h"
#include "container.h"
#include "player.h"
#include "raids.h"
#include "npc.h"
#include "wildcardtree.h"
#include "quests.h"

#include <gtl/phmap.hpp>

class ServiceManager;
class Creature;
class Monster;
class Npc;
class CombatInfo;

enum stackPosType_t {
	STACKPOS_MOVE,
	STACKPOS_LOOK,
	STACKPOS_TOPDOWN_ITEM,
	STACKPOS_USEITEM,
	STACKPOS_USETARGET,
};

enum WorldType_t {
	WORLD_TYPE_NO_PVP = 1,
	WORLD_TYPE_PVP = 2,
	WORLD_TYPE_PVP_ENFORCED = 3,
};

enum GameState_t {
	GAME_STATE_STARTUP,
	GAME_STATE_INIT,
	GAME_STATE_NORMAL,
	GAME_STATE_CLOSED,
	GAME_STATE_SHUTDOWN,
	GAME_STATE_CLOSING,
	GAME_STATE_MAINTAIN,
};

enum DiscordMessageType {
	MESSAGE_NORMAL = 0,
	MESSAGE_ERROR,
	MESSAGE_LOG,
	MESSAGE_INFO
};

static constexpr int32_t PLAYER_NAME_LENGTH = 25;
static constexpr int32_t EVENT_LIGHTINTERVAL = 10000;
static constexpr int32_t EVENT_WORLDTIMEINTERVAL = 2500;
static constexpr int32_t EVENT_DECAYINTERVAL = 250;
static constexpr int32_t EVENT_DECAY_BUCKETS = 4;
static constexpr int32_t MOVE_CREATURE_INTERVAL = 1000;
static constexpr int32_t RANGE_MOVE_CREATURE_INTERVAL = 1500;
static constexpr int32_t RANGE_MOVE_ITEM_INTERVAL = 400;
static constexpr int32_t RANGE_USE_ITEM_INTERVAL = 400;
static constexpr int32_t RANGE_USE_ITEM_EX_INTERVAL = 400;
static constexpr int32_t RANGE_USE_WITH_CREATURE_INTERVAL = 400;
static constexpr int32_t RANGE_ROTATE_ITEM_INTERVAL = 400;
static constexpr int32_t RANGE_BROWSE_FIELD_INTERVAL = 400;
static constexpr int32_t RANGE_WRAP_ITEM_INTERVAL = 400;
static constexpr int32_t RANGE_REQUEST_TRADE_INTERVAL = 400;

/**
  * Main Game class.
  * This class is responsible to control everything that happens
  */

class Game
{
	public:
		Game();
		~Game();

		// non-copyable
		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;

		void start(ServiceManager* manager);

		void forceAddCondition(uint32_t creatureId, Condition* condition);
		void forceRemoveCondition(uint32_t creatureId, ConditionType_t type);

		bool loadMainMap(const std::string& filename);
		void loadMap(const std::string& path);

		/**
		  * Get the map size - info purpose only
		  * \param width width of the map
		  * \param height height of the map
		  */
		void getMapDimensions(uint32_t& width, uint32_t& height) const {
			width = map.width;
			height = map.height;
		}

		void setWorldType(WorldType_t type);
		WorldType_t getWorldType() const {
			return worldType;
		}

		CylinderPtr internalGetCylinder(const PlayerPtr& player, const Position& pos);
		ThingPtr internalGetThing(const PlayerPtr& player, const Position& pos, int32_t index,
		                        uint32_t spriteId, stackPosType_t type);
		static void internalGetPosition(const ItemPtr& item, Position& pos, uint8_t& stackpos);

		static std::string getTradeErrorDescription(ReturnValue ret, const ItemPtr& item);

		// Struct to store damage info for a rewardboss
		struct PlayerScoreInfo {
			int32_t damageDone = 0;   // Damage dealt by the player to the boss
			int32_t damageTaken = 0;  // Damage taken by the player from the boss
			int32_t healingDone = 0;  // Healing done to players near boss
		};

		// Struct to store contribution info for a rewardboss
		struct RewardBossContributionInfo {
			gtl::node_hash_map<uint32_t, PlayerScoreInfo> playerScoreTable;  // Map of player GUIDs to PlayerScoreInfo
		};

		// Map to track the contributions of players to different rewardbosses
		gtl::node_hash_map<uint32_t, RewardBossContributionInfo> rewardBossTracking;

		void resetDamageTracking(uint32_t monsterId);  // Function to reset damage tracking for a specific monster

		/**
		  * Returns a creature based on the unique creature identifier
		  * \param id is the unique creature id to get a creature pointer to
		  * \returns A Creature pointer to the creature
		  */
		CreaturePtr getCreatureByID(uint32_t id);

		/**
		  * Returns a monster based on the unique creature identifier
		  * \param id is the unique monster id to get a monster pointer to
		  * \returns A Monster pointer to the monster
		  */
		MonsterPtr getMonsterByID(uint32_t id);

		/**
		  * Returns an npc based on the unique creature identifier
		  * \param id is the unique npc id to get a npc pointer to
		  * \returns A NPC pointer to the npc
		  */
		NpcPtr getNpcByID(uint32_t id);

		/**
		  * Returns a player based on the unique creature identifier
		  * \param id is the unique player id to get a player pointer to
		  * \returns A Pointer to the player
		  */
		PlayerPtr getPlayerByID(uint32_t id);

		/**
		  * Returns a creature based on a string name identifier
		  * \param s is the name identifier
		  * \returns A Pointer to the creature
		  */
		CreaturePtr getCreatureByName(const std::string& s);

		/**
		  * Returns a npc based on a string name identifier
		  * \param s is the name identifier
		  * \returns A Pointer to the npc
		  */
		NpcPtr getNpcByName(const std::string& s) const;

		/**
		  * Returns a player based on a string name identifier
		  * \param s is the name identifier
		  * \returns A Pointer to the player
		  */
		PlayerPtr getPlayerByName(const std::string& s);

		/**
		  * Returns a player based on guid
		  * \returns A Pointer to the player
		  */
		PlayerPtr getPlayerByGUID(const uint32_t& guid);

		/**
		  * Returns a player based on a string name identifier, with support for the "~" wildcard.
		  * \param s is the name identifier, with or without wildcard
		  * \return "RETURNVALUE_PLAYERWITHTHISNAMEISNOTONLINE" or "RETURNVALUE_NAMEISTOOAMBIGIOUS"
		  */
		ReturnValue getPlayerByNameWildcard(const std::string& s);

		/**
		  * Returns a player based on an account number identifier
		  * \param acc is the account identifier
		  * \returns A Pointer to the player
		  */
		PlayerPtr getPlayerByAccount(uint32_t acc);

		/**
		  * Place Creature on the map without sending out events to the surrounding.
		  * \param creature Creature to place on the map
		  * \param pos The position to place the creature
		  * \param extendedPos If true, the creature will in first-hand be placed 2 tiles away
		  * \param forced If true, placing the creature will not fail because of obstacles (creatures/items)
		  */
		bool internalPlaceCreature(CreaturePtr creature, const Position& pos, bool extendedPos = false, bool forced = false);

		/**
		  * Place Creature on the map.
		  * \param creature Creature to place on the map
		  * \param pos The position to place the creature
		  * \param extendedPos If true, the creature will in first-hand be placed 2 tiles away
		  * \param force If true, placing the creature will not fail because of obstacles (creatures/items)
		  * \param MagicEffect the magic effect that appears with creature when placed.
		  */
		bool placeCreature(CreaturePtr creature, const Position& pos, bool extendedPos = false, bool forced = false, MagicEffectClasses magicEffect = CONST_ME_TELEPORT);



		/**
		  * Remove Creature from the map.
		  * Removes the Creature from the map
		  * \param c Creature to remove
		  */
		bool removeCreature(CreaturePtr creature, bool isLogout = true);
		void executeDeath(uint32_t creatureId);

		void addCreatureCheck(const CreaturePtr& creature);
		static void removeCreatureCheck(const CreaturePtr& creature);

		size_t getPlayersOnline() const {
			return players.size();
		}

		size_t getMonstersOnline() const {
			return monsters.size();
		}

		size_t getNpcsOnline() const {
			return npcs.size();
		}

		uint32_t getPlayersRecord() const {
			return playersRecord;
		}

		LightInfo getWorldLightInfo() const {
			return {lightLevel, lightColor};
		}

		void setWorldLightInfo(LightInfo lightInfo) {
			lightLevel = lightInfo.level;
			lightColor = lightInfo.color;
			for (const auto& it : players) {
				it.second->sendWorldLight(lightInfo);
			}
		}

		void updateWorldLightLevel();

		ReturnValue internalMoveCreature(CreaturePtr creature, Direction direction, uint32_t flags = 0);
		ReturnValue internalMoveCreature(CreaturePtr creature, TilePtr toTile, uint32_t flags = 0);

		ReturnValue internalMoveItem(CylinderPtr fromCylinder, CylinderPtr toCylinder, int32_t index,
		                             ItemPtr item, uint32_t count, std::optional<std::reference_wrapper<ItemPtr>> _moveItem, uint32_t flags = 0, CreaturePtr actor = nullptr, ItemPtr tradeItem = nullptr, const Position* fromPos = nullptr, const Position* toPos = nullptr);
							// another spot to use a ref wrapper and possibly optional for ItemPtr* above

		ReturnValue internalAddItem(CylinderPtr toCylinder, ItemPtr item, int32_t index = INDEX_WHEREEVER,
		                            uint32_t flags = 0, bool test = false);
		ReturnValue internalAddItem(CylinderPtr toCylinder, ItemPtr item, int32_t index,
		                            uint32_t flags, bool test, uint32_t& remainderCount);
		ReturnValue internalRemoveItem(ItemPtr item, int32_t count = -1, bool test = false, uint32_t flags = 0);

		ReturnValue internalPlayerAddItem(const PlayerPtr& player, ItemPtr item, bool dropOnMap = true, slots_t slot = CONST_SLOT_WHEREEVER);

		/**
		  * Find an item of a certain type
		  * \param cylinder to search the item
		  * \param itemId is the item to remove
		  * \param subType is the extra type an item can have such as charges/fluidtype, default is -1
			* meaning it's not used
		  * \param depthSearch if true it will check child containers aswell
		  * \returns A pointer to the item to an item and nullptr if not found
		  */
		ItemPtr findItemOfType(const CylinderPtr& cylinder, uint16_t itemId,
		                     bool depthSearch = true, int32_t subType = -1) const;

		/**
		  * Remove/Add item(s) with a monetary value
		  * \param cylinder to remove the money from
		  * \param money is the amount to remove
		  * \param flags optional flags to modify the default behavior
		  * \returns true if the removal was successful
		  */
		bool removeMoney(CylinderPtr& cylinder, uint64_t money, uint32_t flags = 0);

		/**
		  * Add item(s) with monetary value
		  * \param cylinder which will receive money
		  * \param money the amount to give
		  * \param flags optional flags to modify default behavior
		  */
		void addMoney( CylinderPtr& cylinder, uint64_t money, uint32_t flags = 0);

		/**
		  * Transform one item to another type/count
		  * \param item is the item to transform
		  * \param newId is the new itemid
		  * \param newCount is the new count value, use default value (-1) to not change it
		  * \returns true if the transformation was successful
		  */
		ItemPtr transformItem(const ItemPtr& item, uint16_t newId, int32_t newCount = -1);

		/**
		  * Teleports an object to another position
		  * \param thing is the object to teleport
		  * \param newPos is the new position
		  * \param pushMove force teleport if false
		  * \param flags optional flags to modify default behavior
		  * \returns true if the teleportation was successful
		  */
		ReturnValue internalTeleport(const ThingPtr& thing, const Position& newPos, bool pushMove = true, uint32_t flags = 0);

		/**
		  * Turn a creature to a different direction.
		  * \param creature Creature to change the direction
		  * \param dir Direction to turn to
		  */
		bool internalCreatureTurn(const CreaturePtr& creature, Direction dir);

		/**
		  * Creature wants to say something.
		  * \param creature Creature pointer
		  * \param type Type of message
		  * \param text The text to say
		  */
		bool internalCreatureSay(const CreaturePtr& creature, SpeakClasses type, const std::string& text,
		                         bool ghostMode, SpectatorVec* spectatorsPtr = nullptr, const Position* pos = nullptr, bool echo = false);

		void loadPlayersRecord();
		void checkPlayersRecord();

		void sendGuildMotd(uint32_t playerId);
		void kickPlayer(uint32_t playerId, bool displayEffect);
		void playerReportBug(uint32_t playerId, const std::string& message, const Position& position, uint8_t category);
		void playerDebugAssert(uint32_t playerId, const std::string& assertLine, const std::string& date, const std::string& description, const std::string& comment);
		void playerAnswerModalWindow(uint32_t playerId, uint32_t modalWindowId, uint8_t button, uint8_t choice);
		void playerReportRuleViolation(uint32_t playerId, const std::string& targetName, uint8_t reportType, uint8_t reportReason, const std::string& comment, const std::string& translation);

		bool internalStartTrade(const PlayerPtr& player, const PlayerPtr& tradePartner, const ItemPtr& tradeItem);
		void internalCloseTrade(const PlayerPtr& player, bool sendCancel = true);
		bool playerBroadcastMessage(const PlayerPtr& player, const std::string& text) const;
		void broadcastMessage(const std::string& text, MessageClasses type) const;

		//Implementation of player invoked events
		void playerMoveThing(uint32_t playerId, const Position& fromPos, uint16_t spriteId, uint8_t fromStackPos,
		                     const Position& toPos, uint8_t count);
		void playerMoveCreatureByID(uint32_t playerId, uint32_t movingCreatureId, const Position& movingCreatureOrigPos, const Position& toPos);
		void playerMoveCreature(PlayerPtr& player, CreaturePtr& movingCreature, const Position& movingCreatureOrigPos, TilePtr& toTile);
		void playerMoveItemByPlayerID(uint32_t playerId, const Position& fromPos, uint16_t spriteId, uint8_t fromStackPos, const Position& toPos, uint8_t count);
		void playerMoveItem(const PlayerPtr& player, const Position& fromPos,
		                    uint16_t spriteId, uint8_t fromStackPos, const Position& toPos, uint8_t count, ItemPtr item, CylinderPtr toCylinder);
		void playerEquipItem(uint32_t playerId, uint16_t spriteId);
		void playerMove(uint32_t playerId, Direction direction);
		void playerCreatePrivateChannel(uint32_t playerId);
		void playerChannelInvite(uint32_t playerId, const std::string& name);
		void playerChannelExclude(uint32_t playerId, const std::string& name);
		void playerRequestChannels(uint32_t playerId);
		void playerOpenChannel(uint32_t playerId, uint16_t channelId);
		void playerCloseChannel(uint32_t playerId, uint16_t channelId);
		void playerOpenPrivateChannel(uint32_t playerId, std::string receiver);
		void playerCloseNpcChannel(uint32_t playerId);
		void playerReceivePing(uint32_t playerId);
		void playerReceivePingBack(uint32_t playerId);
		void playerAutoWalk(uint32_t playerId, const std::vector<Direction>& listDir);
		void playerStopAutoWalk(uint32_t playerId);
		void playerUseItemEx(uint32_t playerId, const Position& fromPos, uint8_t fromStackPos,
		                     uint16_t fromSpriteId, const Position& toPos, uint8_t toStackPos, uint16_t toSpriteId);
		void playerUseItem(uint32_t playerId, const Position& pos, uint8_t stackPos, uint8_t index, uint16_t spriteId);
		void playerUseWithCreature(uint32_t playerId, const Position& fromPos, uint8_t fromStackPos, uint32_t creatureId, uint16_t spriteId);
		void playerCloseContainer(uint32_t playerId, uint8_t cid);
		void playerMoveUpContainer(uint32_t playerId, uint8_t cid);
		void playerUpdateContainer(uint32_t playerId, uint8_t cid);
		void playerRotateItem(uint32_t playerId, const Position& pos, uint8_t stackPos, const uint16_t spriteId);
		void playerWriteItem(uint32_t playerId, uint32_t windowTextId, const std::string& text);
		void playerBrowseField(uint32_t playerId, const Position& pos);
		void playerSeekInContainer(uint32_t playerId, uint8_t containerId, uint16_t index);
		void playerUpdateHouseWindow(uint32_t playerId, uint8_t listId, uint32_t windowTextId, const std::string& text);
		void playerWrapItem(uint32_t playerId, const Position& position, uint8_t stackPos, const uint16_t spriteId);
		void playerRequestTrade(uint32_t playerId, const Position& pos, uint8_t stackPos,
		                        uint32_t tradePlayerId, uint16_t spriteId);
		void playerAcceptTrade(uint32_t playerId);
		void playerLookInTrade(uint32_t playerId, bool lookAtCounterOffer, uint8_t index);
		void playerPurchaseItem(uint32_t playerId, uint16_t spriteId, uint8_t count, uint8_t amount,
		                        bool ignoreCap = false, bool inBackpacks = false);
		void playerSellItem(uint32_t playerId, uint16_t spriteId, uint8_t count,
		                    uint8_t amount, bool ignoreEquipped = false);
		void playerCloseShop(uint32_t playerId);
		void playerLookInShop(uint32_t playerId, uint16_t spriteId, uint8_t count);
		void playerCloseTrade(uint32_t playerId);
		void playerSetAttackedCreature(uint32_t playerId, uint32_t creatureId);
		void playerFollowCreature(uint32_t playerId, uint32_t creatureId);
		void playerCancelAttackAndFollow(uint32_t playerId);
		void playerSetFightModes(uint32_t playerId, fightMode_t fightMode, bool chaseMode, bool secureMode);
		void playerLookAt(uint32_t playerId, const Position& pos, uint8_t stackPos);
		void playerLookInBattleList(uint32_t playerId, uint32_t creatureId);
		void playerRequestAddVip(uint32_t playerId, const std::string& name);
		void playerRequestRemoveVip(uint32_t playerId, uint32_t guid);
		void playerRequestEditVip(uint32_t playerId, uint32_t guid, const std::string& description, uint32_t icon, bool notify);
		void playerTurn(uint32_t playerId, Direction dir);
		void playerRequestOutfit(uint32_t playerId);
		void playerShowQuestLog(uint32_t playerId);
		void playerShowQuestLine(uint32_t playerId, uint16_t questId);
		void playerSay(uint32_t playerId, uint16_t channelId, SpeakClasses type,
		               const std::string& receiver, const std::string& text);
		ModalWindow CreatePrivateAccountManagerWindow(const uint32_t modalWindowId, const uint32_t optionId = 0);
		void HandlePrivateAccountManagerInput(const PlayerPtr& player, const uint32_t modalWindowId, const uint8_t button, const uint8_t choice);
		ModalWindow CreateAccountManagerWindow(const uint32_t modalWindowId);
		void playerChangeOutfit(uint32_t playerId, Outfit_t outfit);
		void playerInviteToParty(uint32_t playerId, uint32_t invitedId);
		void playerJoinParty(uint32_t playerId, uint32_t leaderId);
		void playerRevokePartyInvitation(uint32_t playerId, uint32_t invitedId);
		void playerPassPartyLeadership(uint32_t playerId, uint32_t newLeaderId);
		void playerLeaveParty(uint32_t playerId);
		void playerEnableSharedPartyExperience(uint32_t playerId, bool sharedExpActive);
		void playerToggleMount(uint32_t playerId, bool mount);
		void playerLeaveMarket(uint32_t playerId);
		void playerBrowseMarket(uint32_t playerId, uint16_t spriteId);
		void playerBrowseMarketOwnOffers(uint32_t playerId);
		void playerBrowseMarketOwnHistory(uint32_t playerId);
		void playerCreateMarketOffer(uint32_t playerId, uint8_t type, uint16_t spriteId, uint16_t amount, uint32_t price, bool anonymous);
		void playerCancelMarketOffer(uint32_t playerId, uint32_t timestamp, uint16_t counter);
		void playerAcceptMarketOffer(uint32_t playerId, uint32_t timestamp, uint16_t counter, uint16_t amount);
		void playerCancelMove(uint32_t playerId);

		void parsePlayerExtendedOpcode(uint32_t playerId, uint8_t opcode, const std::string& buffer);

		std::vector<ItemPtr> getMarketItemList(uint16_t wareId, uint16_t sufficientCount, const PlayerConstPtr& player);

		void cleanup();
		void shutdown();

		bool canThrowObjectTo(const Position& fromPos, const Position& toPos, bool checkLineOfSight = true, bool sameFloor = false,
		                      int32_t rangex = Map::maxClientViewportX, int32_t rangey = Map::maxClientViewportY);
		bool isSightClear(const Position& fromPos, const Position& toPos, bool sameFloor = false);

		void changeSpeed(const CreaturePtr& creature, int32_t varSpeedDelta);
		void internalCreatureChangeOutfit(const CreaturePtr& creature, const Outfit_t& outfit);
		void internalCreatureChangeVisible(const CreaturePtr& creature, bool visible);
		void changeLight(const CreatureConstPtr& creature);
		void updateCreatureSkull(const CreatureConstPtr& creature);
		void updatePlayerShield(const PlayerPtr& player);
		void updatePlayerHelpers(const PlayerConstPtr& player);
		void updateCreatureType(const CreaturePtr& creature);
		void updateCreatureWalkthrough(const CreatureConstPtr& creature);
		void notifySpectators(const CreatureConstPtr& creature);

		GameState_t getGameState() const;
		void setGameState(GameState_t newState);
		void saveGameState();

		//Events
		void checkCreatureWalk(uint32_t creatureId);
		void updateCreatureWalk(uint32_t creatureId);
		void checkCreatureAttack(uint32_t creatureId);
		void checkCreatures(size_t index);
		void checkLight();

		bool combatBlockHit(CombatDamage& damage, const CreaturePtr& attacker, const CreaturePtr& target, bool checkDefense, bool checkArmor, bool field, bool ignoreResistances = false);

		void combatGetTypeInfo(CombatType_t combatType, const CreaturePtr& target, TextColor_t& color, uint8_t& effect);

		bool combatChangeHealth(const CreaturePtr& attacker, const CreaturePtr& target, CombatDamage& damage);
		bool combatChangeMana(const CreaturePtr& attacker, const CreaturePtr& target, CombatDamage& damage);

		//animation help functions
		void addCreatureHealth(const CreatureConstPtr& target);
		static void addCreatureHealth(const SpectatorVec& spectators, const CreatureConstPtr& target);
		void addMagicEffect(const Position& pos, uint8_t effect);
		static void addMagicEffect(const SpectatorVec& spectators, const Position& pos, uint8_t effect);
		void addDistanceEffect(const Position& fromPos, const Position& toPos, uint8_t effect);
		static void addDistanceEffect(const SpectatorVec& spectators, const Position& fromPos, const Position& toPos, uint8_t effect);

		void setAccountStorageValue(const uint32_t accountId, const uint32_t key, const int32_t value);
		int32_t getAccountStorageValue(const uint32_t accountId, const uint32_t key) const;
		void loadAccountStorageValues();
		bool saveAccountStorageValues() const;

		void startDecay(const ItemPtr& item);

		int16_t getWorldTime() const { return worldTime; }
		void updateWorldTime();

		void loadMotdNum();
		void saveMotdNum() const;
		const std::string& getMotdHash() const { return motdHash; }
		uint32_t getMotdNum() const { return motdNum; }
		void incrementMotdNum() { motdNum++; }

		void sendOfflineTrainingDialog(const PlayerPtr& player) const;

		const gtl::node_hash_map<uint32_t, PlayerPtr>& getPlayers() const { return players; }
		const std::map<uint32_t, NpcPtr>& getNpcs() const { return npcs; }
		const std::map<uint32_t, MonsterPtr>& getMonsters() const { return monsters; }

		void addPlayer(PlayerPtr player);
		void removePlayer(const PlayerPtr& player);

		void addNpc(const NpcPtr& npc);
		void removeNpc(const NpcPtr& npc);

		void addMonster(MonsterPtr monster);
		void removeMonster(const MonsterPtr& monster);

		Guild* getGuild(uint32_t id) const;
		void addGuild(Guild* guild);
		void removeGuild(uint32_t guildId);

		gtl::node_hash_map<TilePtr, ContainerPtr> browseFields;

		void internalRemoveItems(const std::vector<ItemPtr>& itemList, uint32_t amount, bool stackable);

		BedItemPtr getBedBySleeper(uint32_t guid) const;
		void setBedSleeper(const BedItemPtr& bed, uint32_t guid);
		void removeBedSleeper(uint32_t guid);

		ItemPtr getUniqueItem(uint16_t uniqueId);
		bool addUniqueItem(uint16_t uniqueId, const ItemPtr& item);
		void removeUniqueItem(uint16_t uniqueId);

		bool reload(ReloadTypes_t reloadType);

		Groups groups;
		Map map;
		Mounts mounts;
		Raids raids;
		Quests quests;

		std::forward_list<ItemPtr> toDecayItems;

		std::unordered_set<TilePtr> getTilesToClean() const {
			return tilesToClean;
		}
	
		bool isTileInCleanList(const TilePtr& tile) const { 
			return tilesToClean.contains(tile); 
		}
	
		void addTileToClean(TilePtr tile) {
			tilesToClean.emplace(tile);
		}
	
		void removeTileToClean(const TilePtr& tile) {
			tilesToClean.erase(tile);
		}
	
		void clearTilesToClean() {
			tilesToClean.clear();
		}

		void addCharacterOption(CharacterOption& option)
		{
			character_options.emplace_back(option);
		}

		CharacterOption getOptionById(const uint32_t id)
		{
			for (const auto& option : character_options) 
			{
				if (option.id == id)
				{
					return option;
				}
			}
			// log here
			[[unlikely]]
			return CharacterOption();
		}

		void onAccountManagerRecieveText(uint32_t player_id, uint32_t window_id, const std::string& text);
		void onAccountManagerInput(const PlayerPtr& player, const uint32_t modalWindowId, const uint8_t button, const uint8_t choice);
		void onPrivateAccountManagerInput(const PlayerPtr& player, const uint32_t modalWindowId, const uint8_t button, const uint8_t choice);
		void doAccountManagerLogin(const PlayerPtr& player);
		void doAccountManagerReset(uint32_t player_id) {
			const auto player = getPlayerByID(player_id);
			if (!player)
			{
				return;
			}
			player->sendCancelWalk(); // this is needed
			doAccountManagerLogin(player);
		}

		CURL* curl;

	private:
		bool playerSaySpell(const PlayerPtr& player, SpeakClasses type, const std::string& text);
		void playerWhisper(const PlayerPtr& player, const std::string& text);
		bool playerYell(const PlayerPtr& player, const std::string& text);
		bool playerSpeakTo(const PlayerPtr& player, SpeakClasses type, const std::string& receiver, const std::string& text);
		void playerSpeakToNpc(const PlayerPtr& player, const std::string& text);

		void checkDecay();
		void internalDecayItem(const ItemPtr& item);

		gtl::node_hash_map<uint32_t, PlayerPtr> players;
		gtl::node_hash_map<std::string, PlayerPtr> mappedPlayerNames;
		gtl::node_hash_map<uint32_t, PlayerPtr> mappedPlayerGuids;
		gtl::node_hash_map<uint32_t, Guild*> guilds;
		std::vector<TilePtr> loaded_tiles;
		std::vector<ItemPtr> loaded_tile_items;
		std::vector<CharacterOption> character_options;
		gtl::node_hash_map<uint16_t, ItemPtr> uniqueItems;
		gtl::node_hash_map<uint32_t, gtl::flat_hash_map<uint32_t, int32_t>> accountStorageMap;

		std::list<ItemPtr> decayItems[EVENT_DECAY_BUCKETS];
		std::list<CreaturePtr> checkCreatureLists[EVENT_CREATURECOUNT];
	
		size_t lastBucket = 0;

		WildcardTreeNode wildcardTree { false };

		std::map<uint32_t, NpcPtr> npcs;
		std::map<uint32_t, MonsterPtr> monsters;

		//list of items that are in trading state, mapped to the player
		std::map<ItemPtr, uint32_t> tradeItems;

		std::map<uint32_t, BedItemPtr> bedSleepersMap;

		std::unordered_set<TilePtr> tilesToClean;

		ModalWindow offlineTrainingWindow { std::numeric_limits<uint32_t>::max(), "Choose a Skill", "Please choose a skill:" };

		static constexpr uint8_t LIGHT_DAY = 250;
		static constexpr uint8_t LIGHT_NIGHT = 40;
		// 1h realtime   = 1day worldtime
		// 2.5s realtime = 1min worldtime
		// worldTime is calculated in minutes
		static constexpr int16_t GAME_SUNRISE = 360;
		static constexpr int16_t GAME_DAYTIME = 480;
		static constexpr int16_t GAME_SUNSET = 1080;
		static constexpr int16_t GAME_NIGHTTIME = 1200;
		static constexpr float LIGHT_CHANGE_SUNRISE = static_cast<int>(float(float(LIGHT_DAY - LIGHT_NIGHT) / float(GAME_DAYTIME - GAME_SUNRISE)) * 100) / 100.0f;
		static constexpr float LIGHT_CHANGE_SUNSET = static_cast<int>(float(float(LIGHT_DAY - LIGHT_NIGHT) / float(GAME_NIGHTTIME - GAME_SUNSET)) * 100) / 100.0f;

		uint8_t lightLevel = LIGHT_DAY;
		uint8_t lightColor = 215;
		int16_t worldTime = 0;

		GameState_t gameState = GAME_STATE_NORMAL;
		WorldType_t worldType = WORLD_TYPE_PVP;

		ServiceManager* serviceManager = nullptr;

		void updatePlayersRecord() const;
		uint32_t playersRecord = 0;

		std::string motdHash;
		uint32_t motdNum = 0;

		uint32_t lastStageLevel = 0;
		bool stagesEnabled = false;
		bool useLastStageLevel = false;
};

#endif
