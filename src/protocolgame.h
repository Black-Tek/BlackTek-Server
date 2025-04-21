// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_PROTOCOLGAME_H
#define FS_PROTOCOLGAME_H

#include "protocol.h"
#include "chat.h"
#include "creature.h"
#include "tasks.h"

class NetworkMessage;
class Player;
class Game;
class House;
class Container;
class Tile;
class Connection;
class Quest;
class ProtocolGame;
using ProtocolGame_ptr = std::shared_ptr<ProtocolGame>;

extern Game g_game;

struct TextMessage
{
	MessageClasses type = MESSAGE_STATUS_DEFAULT;
	std::string text;
	Position position;
	uint16_t channelId;
	struct {
		int32_t value = 0;
		TextColor_t color;
	} primary, secondary;

	TextMessage() = default;
	TextMessage(MessageClasses type, std::string text) : type(type), text(std::move(text)) {}
};

class ProtocolGame final : public Protocol
{
	public:
		// static protocol information
		enum {server_sends_first = true};
		enum {protocol_identifier = 0}; // Not required as we send first
		enum {use_checksum = true};
		static const char* protocol_name() {
			return "gameworld protocol";
		}
		// todo: use reference for connection
		explicit ProtocolGame(Connection_ptr connection) : Protocol(connection) {}

		void login(uint32_t characterId, uint32_t accountId, OperatingSystem_t operatingSystem);
		void logout(bool displayEffect, bool forced);

		uint16_t getVersion() const {
			return version;
		}

	private:
		ProtocolGame_ptr getThis() {
			return std::static_pointer_cast<ProtocolGame>(shared_from_this());
		}
		void connect(uint32_t playerId, OperatingSystem_t operatingSystem);
		void disconnectClient(const std::string& message) const;
		void writeToOutputBuffer(const NetworkMessage& msg);

		void release() override;

		void checkCreatureAsKnown(uint32_t id, bool& known, uint32_t& removedKnown);

		bool canSee(int32_t x, int32_t y, int32_t z) const;
		bool canSee(const CreatureConstPtr&) const;
		bool canSee(const Position& pos) const;

		// we have all the parse methods
		void parsePacket(NetworkMessage& msg) override;
		void onRecvFirstMessage(NetworkMessage& msg) override;
		void onConnect() override;

		//Parse methods
		void parseAutoWalk(NetworkMessage& msg);
		void parseSetOutfit(NetworkMessage& msg);
		void parseSay(NetworkMessage& msg);
		void parseLookAt(NetworkMessage& msg);
		void parseLookInBattleList(NetworkMessage& msg);
		void parseFightModes(NetworkMessage& msg);
		void parseAttack(NetworkMessage& msg);
		void parseFollow(NetworkMessage& msg);
		void parseEquipObject(NetworkMessage& msg);

		void parseBugReport(NetworkMessage& msg);
		void parseDebugAssert(NetworkMessage& msg);
		void parseRuleViolationReport(NetworkMessage& msg);

		void parseThrow(NetworkMessage& msg);
		void parseUseItemEx(NetworkMessage& msg);
		void parseUseWithCreature(NetworkMessage& msg);
		void parseUseItem(NetworkMessage& msg);
		void parseCloseContainer(NetworkMessage& msg);
		void parseUpArrowContainer(NetworkMessage& msg);
		void parseUpdateContainer(NetworkMessage& msg);
		void parseTextWindow(NetworkMessage& msg);
		void parseHouseWindow(NetworkMessage& msg);
		void parseWrapItem(NetworkMessage& msg);

		void parseLookInShop(NetworkMessage& msg);
		void parsePlayerPurchase(NetworkMessage& msg);
		void parsePlayerSale(NetworkMessage& msg);

		void parseQuestLine(NetworkMessage& msg);

		void parseInviteToParty(NetworkMessage& msg);
		void parseJoinParty(NetworkMessage& msg);
		void parseRevokePartyInvite(NetworkMessage& msg);
		void parsePassPartyLeadership(NetworkMessage& msg);
		void parseEnableSharedPartyExperience(NetworkMessage& msg);

		void parseToggleMount(NetworkMessage& msg);

		void parseModalWindowAnswer(NetworkMessage& msg);

		void parseBrowseField(NetworkMessage& msg);
		void parseSeekInContainer(NetworkMessage& msg);

		//trade methods
		void parseRequestTrade(NetworkMessage& msg);
		void parseLookInTrade(NetworkMessage& msg);

		//market methods
		void parseMarketLeave();
		void parseMarketBrowse(NetworkMessage& msg);
		void parseMarketCreateOffer(NetworkMessage& msg);
		void parseMarketCancelOffer(NetworkMessage& msg);
		void parseMarketAcceptOffer(NetworkMessage& msg);

		//VIP methods
		void parseAddVip(NetworkMessage& msg);
		void parseRemoveVip(NetworkMessage& msg);
		void parseEditVip(NetworkMessage& msg);

		void parseRotateItem(NetworkMessage& msg);

		//Channel tabs
		void parseChannelInvite(NetworkMessage& msg);
		void parseChannelExclude(NetworkMessage& msg);
		void parseOpenChannel(NetworkMessage& msg);
		void parseOpenPrivateChannel(NetworkMessage& msg);
		void parseCloseChannel(NetworkMessage& msg);

		//Send functions
		void sendChannelMessage(const std::string& author, const std::string& text, SpeakClasses type, uint16_t channel);
		void sendChannelEvent(uint16_t channelId, const std::string& playerName, ChannelEvent_t channelEvent);
		void sendClosePrivate(uint16_t channelId);
		void sendCreatePrivateChannel(uint16_t channelId, const std::string& channelName);
		void sendChannelsDialog();
		void sendChannel(uint16_t channelId, const std::string& channelName, const UsersMap* channelUsers, const InvitedMap* invitedUsers);
		void sendOpenPrivateChannel(const std::string& receiver);
		void sendToChannel(const CreatureConstPtr& creature, SpeakClasses type, const std::string& text, uint16_t channelId);
		void sendPrivateMessage(const PlayerConstPtr& speaker, SpeakClasses type, const std::string& text);
		void sendIcons(uint16_t icons);
		void sendFYIBox(const std::string& message);

		void sendDistanceShoot(const Position& from, const Position& to, uint8_t type);
		void sendMagicEffect(const Position& pos, uint8_t type);
		void sendCreatureHealth(const CreatureConstPtr& creature);
		void sendSkills();
		void sendPing();
		void sendPingBack();
		void sendCreatureTurn(const CreatureConstPtr& creature, uint32_t stackPos);
		void sendCreatureSay(const CreatureConstPtr& creature, SpeakClasses type, const std::string& text, const Position* pos = nullptr);

		void sendQuestLog();
		void sendQuestLine(const Quest* quest);

		void sendCancelWalk();
		void sendChangeSpeed(const CreatureConstPtr& creature, uint32_t speed);
		void sendCancelTarget();
		void sendCreatureOutfit(const CreatureConstPtr& creature, const Outfit_t& outfit);
		void sendStats();
		void sendBasicData();
		void sendTextMessage(const TextMessage& message);
		void sendReLoginWindow(uint8_t unfairFightReduction);

		void sendTutorial(uint8_t tutorialId);
		void sendAddMarker(const Position& pos, uint8_t markType, const std::string& desc);

		void sendCreatureWalkthrough(const CreatureConstPtr& creature, bool walkthrough);
		void sendCreatureShield(const CreatureConstPtr& creature);
		void sendCreatureSkull(const CreatureConstPtr& creature);
		void sendCreatureType(uint32_t creatureId, uint8_t creatureType);
		void sendCreatureHelpers(uint32_t creatureId, uint16_t helpers);

		void sendShop(const NpcPtr& npc, const ShopInfoList& itemList);
		void sendCloseShop();
		void sendSaleItemList(const std::list<ShopInfo>& shop);
		void sendMarketEnter();
		void sendMarketLeave();
		void sendMarketBrowseItem(uint16_t itemId, const MarketOfferList& buyOffers, const MarketOfferList& sellOffers);
		void sendMarketAcceptOffer(const MarketOfferEx& offer);
		void sendMarketBrowseOwnOffers(const MarketOfferList& buyOffers, const MarketOfferList& sellOffers);
		void sendMarketCancelOffer(const MarketOfferEx& offer);
		void sendMarketBrowseOwnHistory(const HistoryMarketOfferList& buyOffers, const HistoryMarketOfferList& sellOffers);
		void sendMarketDetail(uint16_t itemId);
		void sendTradeItemRequest(const std::string& traderName, const ItemPtr& item, bool ack);
		void sendCloseTrade();

		void sendTextWindow(uint32_t windowTextId, const ItemPtr& item, uint16_t maxlen, bool canWrite);
		void sendTextWindow(uint32_t windowTextId, uint32_t itemId, const std::string& text);
		void sendHouseWindow(uint32_t windowTextId, const std::string& text);
		void sendAccountManagerTextBox(uint32_t windowTextId, const std::string& text);
		void sendOutfitWindow();

		void sendUpdatedVIPStatus(uint32_t guid, VipStatus_t newStatus);
		void sendVIP(uint32_t guid, const std::string& name, const std::string& description, uint32_t icon, bool notify, VipStatus_t status);
		void sendVIPEntries();

		void sendPendingStateEntered();
		void sendEnterWorld();

		void sendFightModes();

		void sendCreatureLight(const CreatureConstPtr& creature);
		void sendWorldLight(LightInfo lightInfo);

		void sendCreatureSquare(const CreatureConstPtr& creature, SquareColor_t color);

		void sendSpellCooldown(uint8_t spellId, uint32_t time);
		void sendSpellGroupCooldown(SpellGroup_t groupId, uint32_t time);

		//tiles
		void sendMapDescription(const Position& pos);

		void sendAddTileItem(const Position& pos, uint32_t stackpos, const ItemConstPtr& item);
		void sendUpdateTileItem(const Position& pos, uint32_t stackpos, const ItemConstPtr& item);
		void sendRemoveTileThing(const Position& pos, uint32_t stackpos);
		void sendUpdateTileCreature(const Position& pos, uint32_t stackpos, const CreatureConstPtr& creature);
		void sendRemoveTileCreature(const CreatureConstPtr& creature, const Position& pos, uint32_t stackpos);
		void sendUpdateTile(const TileConstPtr& tile, const Position& pos);

		void sendAddCreature(const CreatureConstPtr& creature, const Position& pos, int32_t stackpos, MagicEffectClasses magicEffect = CONST_ME_NONE);
		void sendMoveCreature(const CreatureConstPtr& creature, const Position& newPos, int32_t newStackPos,
		                      const Position& oldPos, int32_t oldStackPos, bool teleport);

		//containers
		void sendAddContainerItem(uint8_t cid, uint16_t slot, const ItemConstPtr& item);
		void sendUpdateContainerItem(uint8_t cid, uint16_t slot, const ItemConstPtr& item);
		void sendRemoveContainerItem(uint8_t cid, uint16_t slot, const ItemConstPtr& lastItem);

		void sendContainer(uint8_t cid, const ContainerConstPtr& container, bool hasParent, uint16_t firstIndex);
		void sendCloseContainer(uint8_t cid);

		//inventory
		void sendInventoryItem(slots_t slot, const ItemConstPtr& item);
		void sendItems();

		//messages
		void sendModalWindow(const ModalWindow& modalWindow);

		//Help functions

		// translate a tile to client-readable format
		void GetTileDescription(const TileConstPtr& tile, NetworkMessage& msg);

		// translate a floor to client-readable format
		void GetFloorDescription(NetworkMessage& msg, int32_t x, int32_t y, int32_t z,
		                         int32_t width, int32_t height, int32_t offset, int32_t& skip);

		// translate a map area to client-readable format
		void GetMapDescription(int32_t x, int32_t y, int32_t z,
		                       int32_t width, int32_t height, NetworkMessage& msg);

		void AddCreature(NetworkMessage& msg, const CreatureConstPtr& creature, bool known, uint32_t remove);
		void AddPlayerStats(NetworkMessage& msg) const;
		void AddOutfit(NetworkMessage& msg, const Outfit_t& outfit);
		void AddPlayerSkills(NetworkMessage& msg) const;
		void AddWorldLight(NetworkMessage& msg, LightInfo lightInfo) const;
		void AddCreatureLight(NetworkMessage& msg, const CreatureConstPtr& creature) const;

		//tiles
		static void RemoveTileThing(NetworkMessage& msg, const Position& pos, uint32_t stackpos);
		static void RemoveTileCreature(NetworkMessage& msg, const CreatureConstPtr& creature, const Position& pos, uint32_t stackpos);

		void MoveUpCreature(NetworkMessage& msg, const CreatureConstPtr& creature, const Position& newPos, const Position& oldPos);
		void MoveDownCreature(NetworkMessage& msg, const CreatureConstPtr& creature, const Position& newPos, const Position& oldPos);

		//shop
		void AddShopItem(NetworkMessage& msg, const ShopInfo& item);

		//otclient
		void parseExtendedOpcode(NetworkMessage& msg);


		// account manager
		std::string getTempAccountName()
		{
			return account_name;
		}

		void setTempAccountName(std::string& name)
		{
			account_name = name;
		}

		std::string getTempPassword()
		{
			return account_password;
		}

		void setTempPassword(std::string& password)
		{
			account_password = password;
		}

		Position getTempPosition()
		{
			return spawn_position;
		}

		void setTempPosition(Position spawn)
		{
			spawn_position = spawn;
		}

		uint32_t getTempTownId()
		{
			return town_id;
		}

		void setTempTownId(uint32_t town)
		{
			town_id = town;
		}

		uint32_t getTempCharacterChoice()
		{
			return character_choice;
		}

		void setTempCharacterChoice(uint32_t choice)
		{
			character_choice = choice;
		}

		uint32_t getTempVocation()
		{
			return vocation_id;
		}

		void setTempVocation(uint32_t vocation)
		{
			vocation_id = vocation;
		}

		friend class Player;

		// Helpers so we don't need to bind every time
		template <typename Callable>
		void addGameTask(Callable&& function) {
			g_dispatcher.addTask(createTask(std::forward<Callable>(function)));
		}

		template <typename Callable>
		void addGameTaskTimed(uint32_t delay, Callable&& function) {
			g_dispatcher.addTask(createTask(delay, std::forward<Callable>(function)));
		}

		std::unordered_set<uint32_t> knownCreatureSet;
		PlayerPtr player = nullptr;
		std::string account_name{};
		std::string account_password{};
		std::string character_name{};
		Position spawn_position{};
		uint32_t town_id = 0;
		uint32_t character_choice = 0;
		uint32_t vocation_id = 0;
		uint32_t eventConnect = 0;
		uint32_t challengeTimestamp = 0;
		uint16_t version = CLIENT_VERSION_MIN;

		uint8_t challengeRandom = 0;

		bool debugAssertSent = false;
		bool acceptPackets = false;
};

#endif
