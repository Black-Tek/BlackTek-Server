// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include <boost/range/adaptor/reversed.hpp>

#include "protocolgame.h"

#include "outputmessage.h"

#include "player.h"

#include "accountmanager.h"

#include "configmanager.h"
#include "actions.h"
#include "game.h"
#include "iologindata.h"
#include "iomarket.h"
#include "ban.h"
#include "scheduler.h"

#include <fmt/format.h>
#include <gtl/btree.hpp>

extern ConfigManager g_config;
extern Actions actions;
extern CreatureEvents* g_creatureEvents;
extern Chat* g_chat;

using namespace BlackTek::Network;

namespace 
{

	std::deque<std::pair<int64_t, uint32_t>> waitList; // (timeout, player guid)
	auto priorityEnd = waitList.end();

	auto findClient(uint32_t guid) 
	{
		std::size_t slot = 1;
		for (auto it = waitList.begin(), end = waitList.end(); it != end; ++it, ++slot) 
		{
			if (it->second == guid) 
			{
				return std::make_pair(it, slot);
			}
		}

		return std::make_pair(waitList.end(), slot);
	}

	constexpr int64_t getWaitTime(std::size_t slot)
	{
		if (slot < 5) 
		{
			return 5;
		} 
		else if (slot < 10) 
		{
			return 10;
		} 
		else if (slot < 20) 
		{
			return 20;
		} 
		else if (slot < 50) 
		{
			return 60;
		} 
		else 
		{
			return 120;
		}
	}

	constexpr int64_t getTimeout(std::size_t slot)
	{
		// timeout is set to 15 seconds longer than expected retry attempt
		return getWaitTime(slot) + 15; // todo: turn magic number into config option
	}

	std::size_t clientLogin(const PlayerConstPtr& player)
	{
		if (player->hasFlag(PlayerFlag_CanAlwaysLogin) or player->getAccountType() >= ACCOUNT_TYPE_GAMEMASTER) 
		{
			return 0;
		}

		uint32_t maxPlayers = static_cast<uint32_t>(g_config.getNumber(ConfigManager::MAX_PLAYERS));

		if (maxPlayers == 0 or (waitList.empty() and g_game.getPlayersOnline() < maxPlayers)) 
		{
			return 0;
		}

		int64_t time = OTSYS_TIME();

		auto it = waitList.begin();
		while (it != waitList.end()) 
		{
			if ((it->first - time) <= 0) 
			{
				it = waitList.erase(it);
			}
			else 
			{
				++it;
			}
		}

		std::size_t slot;
		std::tie(it, slot) = findClient(player->getGUID());

		if (it != waitList.end()) 
		{
			// If server has capacity for this client, let him in even though his current slot might be higher than 0.
			if ((g_game.getPlayersOnline() + slot) <= maxPlayers) 
			{
				waitList.erase(it);
				return 0;
			}

			//let them wait a bit longer
			it->first = time + (getTimeout(slot) * 1000);
			return slot;
		}


		if (player->isPremium()) 
		{
			priorityEnd = waitList.emplace(priorityEnd, time + (getTimeout(slot + 1) * 1000), player->getGUID());
			return std::distance(waitList.begin(), priorityEnd);
		}
		waitList.emplace_back(time + (getTimeout(waitList.size() + 1) * 1000), player->getGUID());
		return waitList.size();
	}

}

void ProtocolGame::release()
{
	//dispatcher thread
	if (player and player->client == shared_from_this()) 
	{
		player->client.reset();
		player = nullptr;
	}

	OutputMessagePool::getInstance().removeProtocolFromAutosend(shared_from_this());
	Protocol::release();
}

void ProtocolGame::login(uint32_t characterId, uint32_t accountId, OperatingSystem_t operatingSystem)
{
	//dispatcher thread
	const auto& foundPlayer = g_game.getPlayerByGUID(characterId);
	const auto managerEnabled = g_config.getBoolean(ConfigManager::ENABLE_ACCOUNT_MANAGER);
	const auto isAccountManager = characterId == AccountManager::ID and managerEnabled;
	if (not foundPlayer or g_config.getBoolean(ConfigManager::ALLOW_CLONES) or isAccountManager)
	{
		player = Player::makePlayer(getThis());
		
		player->setID();
		player->setGUID(characterId);

		if (not IOLoginData::preloadPlayer(player))
		{
			disconnectClient("Your character could not be loaded.");
			return;
		}

		if (IOBan::isPlayerNamelocked(player->getGUID()))
		{
			disconnectClient("Your character has been namelocked.");
			return;
		}

		if (g_game.getGameState() == GAME_STATE_CLOSING and not player->hasFlag(PlayerFlag_CanAlwaysLogin))
		{
			disconnectClient("The game is just going down.\nPlease try again later.");
			return;
		}

		if (g_game.getGameState() == GAME_STATE_CLOSED and not player->hasFlag(PlayerFlag_CanAlwaysLogin))
		{
			disconnectClient("Server is currently closed.\nPlease try again later.");
			return;
		}

		if (g_config.getBoolean(ConfigManager::ONE_PLAYER_ON_ACCOUNT) 
			and characterId != AccountManager::ID 
			and player->getAccountType() < ACCOUNT_TYPE_GAMEMASTER 
			and g_game.getPlayerByAccount(player->getAccount()))
		{
			disconnectClient("You may only login with one character\nof your account at the same time.");
			return;
		}

		if (not player->hasFlag(PlayerFlag_CannotBeBanned))
		{
			BanInfo banInfo;
			if (IOBan::isAccountBanned(accountId, banInfo))
			{
				if (banInfo.reason.empty())
				{
					banInfo.reason = "(none)";
				}

				if (banInfo.expiresAt > 0)
				{
					disconnectClient(fmt::format("Your account has been banned until {:s} by {:s}.\n\nReason specified:\n{:s}", formatDateShort(banInfo.expiresAt), banInfo.bannedBy, banInfo.reason));
				}
				else
				{
					disconnectClient(fmt::format("Your account has been permanently banned by {:s}.\n\nReason specified:\n{:s}", banInfo.bannedBy, banInfo.reason));
				}
				return;
			}
		}

		if (std::size_t currentSlot = clientLogin(player))
		{
			uint8_t retryTime = getWaitTime(currentSlot);
			auto output = OutputMessagePool::getOutputMessage();
			output->add(ServerCode::LoginQueue);
			output->addString(fmt::format("Too many players online.\nYou are at place {:d} on the waiting list.", currentSlot));
			output->addByte(retryTime);
			send(std::move(output));
			disconnect();
			return;
		}

		if (not IOLoginData::loadPlayerById(player, player->getGUID()))
		{
			disconnectClient("Your character could not be loaded.");
			return;
		}

		player->setOperatingSystem(operatingSystem);

		// Todo : add back position spawn determined by config.lua
		if (isAccountManager)
		{
			player->accountNumber = accountId;
			auto x = static_cast<uint16_t>(g_config.getNumber(ConfigManager::ACCOUNT_MANAGER_POS_X));
			auto y = static_cast<uint16_t>(g_config.getNumber(ConfigManager::ACCOUNT_MANAGER_POS_Y));
			auto z = static_cast<uint8_t>(g_config.getNumber(ConfigManager::ACCOUNT_MANAGER_POS_Z));
			if (not g_game.placeCreature(player, Position{ x, y, z }))
			{
				if (not g_game.placeCreature(player, player->getTemplePosition(), false, true))
				{
					disconnectClient("Unable To Spawn Account Manager Please contact Admin!.");
					std::cout << "Account Manager Failed to spawn at location X = " << x << " Y = " << y << " Z = " << z << " \n";
					return;
				}
			}
		}
		else
		{
			if (not g_game.placeCreature(player, player->getLoginPosition()))
			{
				if (not g_game.placeCreature(player, player->getTemplePosition(), false, true))
				{
					disconnectClient("Temple position is wrong. Contact the administrator.");
					return;
				}
			}
		}

		if (operatingSystem >= CLIENTOS_OTCLIENT_LINUX)
		{
			player->registerCreatureEvent("ExtendedOpcode");
		}

		player->lastIP = player->getIP();
		player->lastLoginSaved = std::max<time_t>(time(nullptr), player->lastLoginSaved + 1);
		acceptPackets = true;
	} 
	else
	{
		if (eventConnect != 0 or not g_config.getBoolean(ConfigManager::REPLACE_KICK_ON_LOGIN))
		{
			//Already trying to connect
			disconnectClient("You are already logged in.");
			return;
		}

		if (foundPlayer->client)
		{
			foundPlayer->disconnect();
			foundPlayer->isConnecting = true;

			eventConnect = g_scheduler.addEvent(createSchedulerTask(1000, [=, thisPtr = getThis(), playerID = foundPlayer->getID()]()
			{
				thisPtr->connect(playerID, operatingSystem);
			}));
		} 
		else
		{
			connect(foundPlayer->getID(), operatingSystem);
		}
	}
	OutputMessagePool::getInstance().addProtocolToAutosend(shared_from_this());
}

void ProtocolGame::connect(uint32_t playerId, OperatingSystem_t operatingSystem)
{
	eventConnect = 0;
	const auto& foundPlayer = g_game.getPlayerByID(playerId);

	if (not foundPlayer or foundPlayer->client) 
	{
		disconnectClient("You are already logged in.");
		return;
	}

	if (isConnectionExpired()) 
	{
		//ProtocolGame::release() has been called at this point and the Connection object
		//no longer exists, so we return to prevent leakage of the Player.
		return;
	}

	player = foundPlayer;
	g_chat->removeUserFromAllChannels(player);
	player->clearModalWindows();
	player->setOperatingSystem(operatingSystem);
	player->isConnecting = false;

	player->client = getThis();
	sendAddCreature(player, player->getPosition(), 0);
	player->lastIP = player->getIP();
	player->lastLoginSaved = std::max<time_t>(time(nullptr), player->lastLoginSaved + 1);
	player->resetIdleTime();
	acceptPackets = true;
}

void ProtocolGame::logout(bool displayEffect, bool forced)
{
	//dispatcher thread
	if (not player)
	{
		return;
	}

	if (not player->isRemoved())
	{
		if (not forced)
		{
			if (not player->isAccessPlayer())
			{
				if (player->getTile()->hasFlag(TILESTATE_NOLOGOUT))
				{
					player->sendCancelMessage(RETURNVALUE_YOUCANNOTLOGOUTHERE);
					return;
				}

				if (not player->getTile()->hasFlag(TILESTATE_PROTECTIONZONE) and player->hasCondition(CONDITION_INFIGHT))
				{
					player->sendCancelMessage(RETURNVALUE_YOUMAYNOTLOGOUTDURINGAFIGHT);
					return;
				}
			}

			//scripting event - onLogout
			if (not g_creatureEvents->playerLogout(player))
			{
				//Let the script handle the error message
				return;
			}
		}

		if (displayEffect and player->getHealth() > 0 and not player->isInGhostMode())
		{
			g_game.addMagicEffect(player->getPosition(), CONST_ME_POFF);
		}
	}

	disconnect();

	g_game.removeCreature(player);
}

void ProtocolGame::onRecvFirstMessage(NetworkMessage& msg)
{
	if (g_game.getGameState() == GAME_STATE_SHUTDOWN)
	{
		disconnect();
		return;
	}

	OperatingSystem_t operatingSystem = static_cast<OperatingSystem_t>(msg.get<uint16_t>());
	version = msg.get<uint16_t>();

	msg.skipBytes(7); // U32 client version, U8 client type, U16 dat revision

	if (not Protocol::RSA_decrypt(msg))
	{
		disconnect();
		return;
	}

	xtea::key key;
	key[0] = msg.get<uint32_t>();
	key[1] = msg.get<uint32_t>();
	key[2] = msg.get<uint32_t>();
	key[3] = msg.get<uint32_t>();
	enableXTEAEncryption();
	setXTEAKey(std::move(key));

	if (operatingSystem >= CLIENTOS_OTCLIENT_LINUX)
	{
		NetworkMessage opcodeMessage;
		opcodeMessage.add(ServerCode::ExtendedOpcode);
		opcodeMessage.add(CommonCode::Zero); // uint8_t -- 1 byte width
		opcodeMessage.add<SpecialCode>(SpecialCode::Zero); // uint16_t -- 2 byte width
		writeToOutputBuffer(opcodeMessage);
	}

	msg.skipBytes(1); // gamemaster flag

	// acc name or email, password, token, timestamp divided by 30
	auto sessionArgs = explodeString(msg.getString(), "\n", 4);
	if (sessionArgs.size() != 4)
	{
		disconnect();
		return;
	}

	auto accountName = sessionArgs[0];
	auto password = sessionArgs[1];
	std::string_view token = sessionArgs[2];
	uint32_t tokenTime = 0;

	try
	{
		tokenTime = std::stoul(sessionArgs[3].data());
	}
	catch (const std::invalid_argument&) {
		disconnectClient("Malformed token packet.");
		return;
	}
	catch (const std::out_of_range&)
	{
		disconnectClient("Token time is too long.");
		return;
	}

	auto characterName = msg.getString();
	uint32_t timeStamp = msg.get<uint32_t>();
	uint8_t randNumber = msg.getByte();

	if (challengeTimestamp != timeStamp or challengeRandom != randNumber)
	{
		disconnect();
		return;
	}

	if (version < CLIENT_VERSION_MIN or version > CLIENT_VERSION_MAX)
	{
		disconnectClient(fmt::format("Only clients with protocol {:s} allowed!", CLIENT_VERSION_STR));
		return;
	}

	if (accountName.empty()
		and password.empty()
		and g_config.getBoolean(ConfigManager::ENABLE_ACCOUNT_MANAGER)
		and g_config.getBoolean(ConfigManager::ENABLE_NO_PASS_LOGIN))
	{
		accountName = g_config.getString(ConfigManager::ACCOUNT_MANAGER_AUTH);
		password = g_config.getString(ConfigManager::ACCOUNT_MANAGER_AUTH);
	} 

	if (g_game.getGameState() == GAME_STATE_STARTUP)
	{
		disconnectClient("Gameworld is starting up. Please wait.");
		return;
	}

	if (g_game.getGameState() == GAME_STATE_MAINTAIN)
	{
		disconnectClient("Gameworld is under maintenance. Please re-connect in a while.");
		return;
	}

	BanInfo banInfo;
	if (IOBan::isIpBanned(getIP(), banInfo))
	{
		if (banInfo.reason.empty())
		{
			banInfo.reason = "(none)";
		}

		disconnectClient(fmt::format("Your IP has been banned until {:s} by {:s}.\n\nReason specified:\n{:s}", formatDateShort(banInfo.expiresAt), banInfo.bannedBy, banInfo.reason));
		return;
	}

	auto [accountId, characterId] = IOLoginData::gameworldAuthentication(accountName, password, characterName, token, tokenTime);
	if (characterName == AccountManager::NAME)
	{
		if (accountId == 0)
		{
			std::tie(accountId, characterId) = IOLoginData::getAccountIdByAccountName(accountName, password, characterName);
		}
	}

	if (accountId == 0)
	{
		disconnectClient("Account name or password is not correct.");
		return;
	}

	g_dispatcher.addTask([=, thisPtr = getThis()]() { thisPtr->login(characterId, accountId, operatingSystem); });
}

void ProtocolGame::onConnect()
{
	auto output = OutputMessagePool::getOutputMessage();
	static std::random_device rd;
	static std::ranlux24 generator(rd());
	static std::uniform_int_distribution<uint16_t> randNumber(0, 255);

	// Skip checksum
	output->skipBytes(sizeof(uint32_t));

	// Packet length & type
	output->add<uint16_t>(6);
	output->add(ServerCode::Challenge);

	// Add timestamp & random number
	challengeTimestamp = static_cast<uint32_t>(time(nullptr));
	output->add<uint32_t>(challengeTimestamp);

	challengeRandom = randNumber(generator);
	output->addByte(challengeRandom);

	// Go back and write checksum
	output->skipBytes(-12);
	output->add<uint32_t>(adlerChecksum(output->getOutputBuffer() + sizeof(uint32_t), 8));

	send(std::move(output));
}

void ProtocolGame::disconnectClient(const std::string& message) const
{
	auto output = OutputMessagePool::getOutputMessage();
	output->add(ServerCode::LoginOrPendingState);
	output->addString(message);
    send(std::move(output));
	disconnect();
}

void ProtocolGame::writeToOutputBuffer(const NetworkMessage& msg)
{
	auto out = getOutputBuffer(msg.getLength());
	out->append(msg);
}

void ProtocolGame::parsePacket(NetworkMessage& msg)
{
	if (not acceptPackets or g_game.getGameState() == GAME_STATE_SHUTDOWN or msg.getLength() == 0)
	{
		return;
	}

	ClientCode recvbyte = static_cast<ClientCode>(msg.getByte());

	if (not player)
	{
		if (static_cast<ClientCode>(recvbyte) == ClientCode::Exit)
		{
			disconnect();
		}

		return;
	}

	//a dead player can not performs actions
	if (player->isRemoved() or player->getHealth() <= 0)
	{
		if (static_cast<ClientCode>(recvbyte) == ClientCode::Exit)
		{
			disconnect();
			return;
		}

		if (static_cast<ClientCode>(recvbyte) != ClientCode::Logout)
		{
			return;
		}
	}

	auto player_id = player->getID();

	// Account Manager
	if (player->isAccountManager())
	{
		switch (recvbyte)
		{
			case ClientCode::Logout: addGameTask([thisPtr = getThis()]() { thisPtr->logout(true, false); });	break;
			case ClientCode::PingBack: addGameTask([player_id]() { g_game.playerReceivePingBack(player_id); }); break;
			case ClientCode::Ping: addGameTask([player_id]() { g_game.playerReceivePing(player_id); }); break;
			case ClientCode::TextWindow: parseTextWindow(msg); break;
			case ClientCode::ModalWindowAnswer: parseModalWindowAnswer(msg); break;
			default: addGameTask([player_id]() { g_game.doAccountManagerReset(player_id); });	break;
		}

		if (msg.isOverrun())
		{
			disconnect();
		}
		return;
	}


	switch (recvbyte)
	{
		case ClientCode::Logout: addGameTask([thisPtr = getThis()]() { thisPtr->logout(true, false); }); break;
		case ClientCode::PingBack: addGameTask([player_id]() { g_game.playerReceivePingBack(player_id); }); break;
		case ClientCode::Ping: addGameTask([player_id]() { g_game.playerReceivePing(player_id); }); break;
		case ClientCode::ExtendedOpcode: parseExtendedOpcode(msg); break; //otclient extended opcode
		case ClientCode::AutoWalk: parseAutoWalk(msg); break;
		case ClientCode::MoveNorth: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_NORTH); }); break;
		case ClientCode::MoveEast: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_EAST); }); break;
		case ClientCode::MoveSouth: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_SOUTH); }); break;
		case ClientCode::MoveWest: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_WEST); }); break;
		case ClientCode::StopAutoWalk: addGameTask([player_id]() { g_game.playerStopAutoWalk(player_id); }); break;
		case ClientCode::MoveNorthEast: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_NORTHEAST); }); break;
		case ClientCode::MoveSouthEast: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_SOUTHEAST); }); break;
		case ClientCode::MoveSouthWest: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_SOUTHWEST); }); break;
		case ClientCode::MoveNorthWest: addGameTask([player_id]() { g_game.playerMove(player_id, DIRECTION_NORTHWEST); }); break;
		case ClientCode::TurnNorth: addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [player_id]() { g_game.playerTurn(player_id, DIRECTION_NORTH); }); break;
		case ClientCode::TurnEast: addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [player_id]() { g_game.playerTurn(player_id, DIRECTION_EAST); }); break;
		case ClientCode::TurnSouth: addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [player_id]() { g_game.playerTurn(player_id, DIRECTION_SOUTH); }); break;
		case ClientCode::TurnWest: addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [player_id]() { g_game.playerTurn(player_id, DIRECTION_WEST); }); break;
		case ClientCode::EquipObject: parseEquipObject(msg); break;
		case ClientCode::Throw: parseThrow(msg); break;
		case ClientCode::LookInShop: parseLookInShop(msg); break;
		case ClientCode::Purchase: parsePlayerPurchase(msg); break;
		case ClientCode::Sale: parsePlayerSale(msg); break;
		case ClientCode::CloseShop: addGameTask([player_id]() { g_game.playerCloseShop(player_id); }); break;
		case ClientCode::RequestTrade: parseRequestTrade(msg); break;
		case ClientCode::LookInTrade: parseLookInTrade(msg); break;
		case ClientCode::AcceptTrade: addGameTask([player_id]() { g_game.playerAcceptTrade(player_id); }); break;
		case ClientCode::CloseTrade: addGameTask([player_id]() { g_game.playerCloseTrade(player_id); }); break;
		case ClientCode::UseItem: parseUseItem(msg); break;
		case ClientCode::UseItemEx: parseUseItemEx(msg); break;
		case ClientCode::UseWithCreature: parseUseWithCreature(msg); break;
		case ClientCode::RotateItem: parseRotateItem(msg); break;
		case ClientCode::CloseContainer: parseCloseContainer(msg); break;
		case ClientCode::UpArrowContainer: parseUpArrowContainer(msg); break;
		case ClientCode::TextWindow: parseTextWindow(msg); break;
		case ClientCode::HouseWindow: parseHouseWindow(msg); break;
		case ClientCode::WrapItem: parseWrapItem(msg); break;
		case ClientCode::LookAt: parseLookAt(msg); break;
		case ClientCode::LookInBattleList: parseLookInBattleList(msg); break;
		case ClientCode::JoinAggression: /* join aggression */ break;
		case ClientCode::Say: parseSay(msg); break;
		case ClientCode::RequestChannels: addGameTask([player_id]() { g_game.playerRequestChannels(player_id); }); break;
		case ClientCode::OpenChannel: parseOpenChannel(msg); break;
		case ClientCode::CloseChannel: parseCloseChannel(msg); break;
		case ClientCode::OpenPrivateChannel: parseOpenPrivateChannel(msg); break;
		case ClientCode::CloseNpcChannel: addGameTask([player_id]() { g_game.playerCloseNpcChannel(player_id); }); break;
		case ClientCode::FightMode: parseFightModes(msg); break;
		case ClientCode::Attack: parseAttack(msg); break;
		case ClientCode::Follow: parseFollow(msg); break;
		case ClientCode::InviteToParty: parseInviteToParty(msg); break;
		case ClientCode::JoinParty: parseJoinParty(msg); break;
		case ClientCode::RevokePartyInvite: parseRevokePartyInvite(msg); break;
		case ClientCode::PassPartyLeadership: parsePassPartyLeadership(msg); break;
		case ClientCode::LeaveParty: addGameTask([player_id]() { g_game.playerLeaveParty(player_id); }); break;
		case ClientCode::EnableSharedPartyExperience: parseEnableSharedPartyExperience(msg); break;
		case ClientCode::CreatePrivateChannel: addGameTask([player_id]() { g_game.playerCreatePrivateChannel(player_id); }); break;
		case ClientCode::ChannelInvite: parseChannelInvite(msg); break;
		case ClientCode::ChannelExclude: parseChannelExclude(msg); break;
		case ClientCode::CancelAttackAndFollow: addGameTask([player_id]() { g_game.playerCancelAttackAndFollow(player_id); }); break;
		case ClientCode::UpdateTile: /* update tile */ break;
		case ClientCode::UpdateContainer: parseUpdateContainer(msg); break;
		case ClientCode::BrowseField: parseBrowseField(msg); break;
		case ClientCode::SeekInContainer: parseSeekInContainer(msg); break;
		case ClientCode::RequestOutfit: addGameTask([player_id]() { g_game.playerRequestOutfit(player_id); }); break;
		case ClientCode::SetOutfit: parseSetOutfit(msg); break;
		case ClientCode::ToggleMount: parseToggleMount(msg); break;
		case ClientCode::AddVip: parseAddVip(msg); break;
		case ClientCode::RemoveVip: parseRemoveVip(msg); break;
		case ClientCode::EditVip: parseEditVip(msg); break;
		case ClientCode::BugReport: parseBugReport(msg); break;
		case ClientCode::ThankYou: /* thank you */ break;
		case ClientCode::DebugAssert: parseDebugAssert(msg); break;
		///new protocol byte maybe? ///case 0xEE: addGameTask([player_id]() { g_game.playerSay(player_id, 0, TALKTYPE_SAY, "", "hi"); }); break;
		case ClientCode::ShowQuestLog: addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [player_id]() { g_game.playerShowQuestLog(player_id); }); break;
		case ClientCode::QuestLine: parseQuestLine(msg); break;
		case ClientCode::RuleViolationReport: parseRuleViolationReport(msg); break;
		case ClientCode::GetObjectInfo: /* get object info */ break;
		case ClientCode::MarketLeave: parseMarketLeave(); break;
		case ClientCode::MarketBrowse: parseMarketBrowse(msg); break;
		case ClientCode::MarketCreateOffer: parseMarketCreateOffer(msg); break;
		case ClientCode::MarketCancelOffer: parseMarketCancelOffer(msg); break;
		case ClientCode::MarketAcceptOffer: parseMarketAcceptOffer(msg); break;
		case ClientCode::ModalWindowAnswer: parseModalWindowAnswer(msg); break;

		default:
			 //std::cout << "Player: " << player->getName() << " sent an unknown packet header: 0x" << std::hex << static_cast<uint16_t>(recvbyte) << std::dec << "!" << std::endl;
			break;
	}

	if (msg.isOverrun())
	{
		disconnect();
	}
}

void ProtocolGame::GetTileDescription(const TileConstPtr& tile, NetworkMessage& msg)
{
	msg.add<SpecialCode>(SpecialCode::Zero); //environmental effects

	int32_t count;
	if (const auto& ground = tile->getGround())
	{
		msg.addItem(ground);
		count = 1;
	} 
	else
	{
		count = 0;
	}

	const auto& items = tile->getItemList();
	if (items)
	{
		for (auto it = items->getBeginTopItem(), end = items->getEndTopItem(); it != end; ++it)
		{
			msg.addItem(*it);

			if (++count == 10)
			{
				break;
			}
		}
	}

	if (const auto& creatures = tile->getCreatures())
	{
		for (const auto& creature : boost::adaptors::reverse(*creatures))
		{
			if (not player->canSeeCreature(creature) or count >= 10)
			{
				continue;
			}

			bool known;
			uint32_t removedKnown;
			checkCreatureAsKnown(creature->getID(), known, removedKnown);
			AddCreature(msg, creature, known, removedKnown);
			++count;
		}
	}

	if (items and count < 10)
	{
		for (auto it = items->getBeginDownItem(), end = items->getEndDownItem(); it != end; ++it)
		{
			msg.addItem(*it);

			if (++count == 10)
			{
				return;
			}
		}
	}
}

void ProtocolGame::GetMapDescription(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, NetworkMessage& msg)
{
	int32_t skip = -1;
	int32_t startz, endz, zstep;

	if (z > 7)
	{
		startz = z - 2;
		endz = std::min<int32_t>(MAP_MAX_LAYERS - 1, z + 2);
		zstep = 1;
	} 
	else
	{
		startz = 7;
		endz = 0;
		zstep = -1;
	}

	for (int32_t nz = startz; nz != endz + zstep; nz += zstep)
	{
		GetFloorDescription(msg, x, y, nz, width, height, z - nz, skip);
	}

	if (skip >= 0)
	{
		msg.addByte(skip);
		msg.add(CommonCode::End);
	}
}

void ProtocolGame::GetFloorDescription(NetworkMessage& msg, int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t offset, int32_t& skip)
{
	for (int32_t nx = 0; nx < width; nx++)
	{
		for (int32_t ny = 0; ny < height; ny++)
		{
			if (const auto& tile = g_game.map.getTile(x + nx + offset, y + ny + offset, z))
			{
				if (skip >= 0)
				{
					msg.addByte(skip);
					msg.add(CommonCode::End);
				}

				skip = 0;
				GetTileDescription(tile, msg);
			}
			else if (skip == 0xFE)
			{
				msg.add(CommonCode::End);
				msg.add(CommonCode::End);
				skip = -1;
			} 
			else
			{
				++skip;
			}
		}
	}
}

void ProtocolGame::checkCreatureAsKnown(uint32_t id, bool& known, uint32_t& removedKnown)
{
	const auto result = knownCreatureSet.insert(id);
	if (not result.second) 
	{
		known = true;
		return;
	}

	known = false;

	if (knownCreatureSet.size() > 1300)
	{
		// Look for a creature to remove
		for (auto it = knownCreatureSet.begin(), end = knownCreatureSet.end(); it != end; ++it)
		{
			const auto& creature = g_game.getCreatureByID(*it);
			if (not canSee(creature))
			{
				removedKnown = *it;
				knownCreatureSet.erase(it);
				return;
			}
		}

		// Bad situation. Let's just remove anyone.
		auto it = knownCreatureSet.begin();
		if (*it == id)
		{
			++it;
		}

		removedKnown = *it;
		knownCreatureSet.erase(it);
	} 
	else
	{
		removedKnown = 0;
	}
}

bool ProtocolGame::canSee(const CreatureConstPtr& creature) const
{
	if (not creature or not player or creature->isRemoved())
	{
		return false;
	}

	if (not player->canSeeCreature(creature))
	{
		return false;
	}

	return canSee(creature->getPosition());
}

bool ProtocolGame::canSee(const Position& pos) const
{
	return canSee(pos.x, pos.y, pos.z);
}

bool ProtocolGame::canSee(int32_t x, int32_t y, int32_t z) const
{
	if (not player)
	{
		return false;
	}

	const Position& myPos = player->getPosition();
	if (myPos.z <= 7)
	{
		//we are on ground level or above (7 -> 0)
		//view is from 7 -> 0
		if (z > 7)
		{
			return false;
		}
	} 
	else
	{ // if (myPos.z >= 8) {
		//we are underground (8 -> 15)
		//view is +/- 2 from the floor we stand on
		if (std::abs(myPos.getZ() - z) > 2)
		{
			return false;
		}
	}

	//negative offset means that the action taken place is on a lower floor than ourself
	int32_t offsetz = myPos.getZ() - z;
	if ((x >= myPos.getX() - Map::maxClientViewportX + offsetz)
		and (x <= myPos.getX() + (Map::maxClientViewportX + 1) + offsetz)
		and	(y >= myPos.getY() - Map::maxClientViewportY + offsetz)
		and (y <= myPos.getY() + (Map::maxClientViewportY + 1) + offsetz))
	{
		return true;
	}
	return false;
}

// Parse methods
void ProtocolGame::parseChannelInvite(NetworkMessage& msg)
{
	auto name = msg.getString();
	addGameTask([playerID = player->getID(), name = std::string{ name }]() { g_game.playerChannelInvite(playerID, name); });
}

void ProtocolGame::parseChannelExclude(NetworkMessage& msg)
{
	auto name = msg.getString();
	addGameTask([=, playerID = player->getID(), name = std::string{ name }]() { g_game.playerChannelExclude(playerID, name); });
}

void ProtocolGame::parseOpenChannel(NetworkMessage& msg)
{
	uint16_t channelID = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerOpenChannel(playerID, channelID); });
}

void ProtocolGame::parseCloseChannel(NetworkMessage& msg)
{
	uint16_t channelID = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerCloseChannel(playerID, channelID); });
}

void ProtocolGame::parseOpenPrivateChannel(NetworkMessage& msg)
{
	auto receiver = msg.getString();
	addGameTask([playerID = player->getID(), receiver = std::string{ receiver }]() { g_game.playerOpenPrivateChannel(playerID, receiver); });
}

void ProtocolGame::parseAutoWalk(NetworkMessage& msg)
{
	uint8_t numdirs = msg.getByte();
	if (numdirs == 0 or (msg.getBufferPosition() + numdirs) != (msg.getLength() + 8))
	{
		return;
	}

	msg.skipBytes(numdirs);
	std::vector<Direction> path;
	path.reserve(numdirs);

	for (uint8_t i = 0; i < numdirs; ++i)
	{
		uint8_t rawdir = msg.getPreviousByte();
		switch (rawdir)
		{
			case 1: path.push_back(DIRECTION_EAST); break;
			case 2: path.push_back(DIRECTION_NORTHEAST); break;
			case 3: path.push_back(DIRECTION_NORTH); break;
			case 4: path.push_back(DIRECTION_NORTHWEST); break;
			case 5: path.push_back(DIRECTION_WEST); break;
			case 6: path.push_back(DIRECTION_SOUTHWEST); break;
			case 7: path.push_back(DIRECTION_SOUTH); break;
			case 8: path.push_back(DIRECTION_SOUTHEAST); break;
			default: break;
		}
	}

	if (path.empty())
	{
		return;
	}

	addGameTask([playerID = player->getID(), path = std::move(path)]() { g_game.playerAutoWalk(playerID, path); });
}

void ProtocolGame::parseSetOutfit(NetworkMessage& msg)
{
	Outfit_t newOutfit;
	newOutfit.lookType = msg.get<uint16_t>();
	newOutfit.lookHead = msg.getByte();
	newOutfit.lookBody = msg.getByte();
	newOutfit.lookLegs = msg.getByte();
	newOutfit.lookFeet = msg.getByte();
	newOutfit.lookAddons = msg.getByte();
	newOutfit.lookMount = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerChangeOutfit(playerID, newOutfit); });
}

void ProtocolGame::parseToggleMount(NetworkMessage& msg)
{
	bool mount = msg.getByte() != 0;
	addGameTask([=, playerID = player->getID()]() { g_game.playerToggleMount(playerID, mount); });
}

void ProtocolGame::parseUseItem(NetworkMessage& msg)
{
	Position pos = msg.getPosition();
	uint16_t spriteId = msg.get<uint16_t>();
	uint8_t stackpos = msg.getByte();
	uint8_t index = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerUseItem(playerID, pos, stackpos, index, spriteId); });
}

void ProtocolGame::parseUseItemEx(NetworkMessage& msg)
{
	Position fromPos = msg.getPosition();
	uint16_t fromSpriteId = msg.get<uint16_t>();
	uint8_t fromStackPos = msg.getByte();
	Position toPos = msg.getPosition();
	uint16_t toSpriteId = msg.get<uint16_t>();
	uint8_t toStackPos = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]()
	{
		g_game.playerUseItemEx(playerID, fromPos, fromStackPos, fromSpriteId, toPos, toStackPos, toSpriteId);
	});
}

void ProtocolGame::parseUseWithCreature(NetworkMessage& msg)
{
	Position fromPos = msg.getPosition();
	uint16_t spriteId = msg.get<uint16_t>();
	uint8_t fromStackPos = msg.getByte();
	uint32_t creatureId = msg.get<uint32_t>();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]()
	{
		g_game.playerUseWithCreature(playerID, fromPos, fromStackPos, creatureId, spriteId);
	});
}

void ProtocolGame::parseCloseContainer(NetworkMessage& msg)
{
	uint8_t cid = msg.getByte();
	addGameTask([=, playerID = player->getID()]() { g_game.playerCloseContainer(playerID, cid); });
}

void ProtocolGame::parseUpArrowContainer(NetworkMessage& msg)
{
	uint8_t cid = msg.getByte();
	addGameTask([=, playerID = player->getID()]() { g_game.playerMoveUpContainer(playerID, cid); });
}

void ProtocolGame::parseUpdateContainer(NetworkMessage& msg)
{
	uint8_t cid = msg.getByte();
	addGameTask([=, playerID = player->getID()]() { g_game.playerUpdateContainer(playerID, cid); });
}

void ProtocolGame::parseThrow(NetworkMessage& msg)
{
	Position fromPos = msg.getPosition();
	uint16_t spriteId = msg.get<uint16_t>();
	uint8_t fromStackpos = msg.getByte();
	Position toPos = msg.getPosition();
	uint8_t count = msg.getByte();

	if (toPos != fromPos)
	{
		addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]()
		{
			g_game.playerMoveThing(playerID, fromPos, spriteId, fromStackpos, toPos, count);
		});
	}
}

void ProtocolGame::parseLookAt(NetworkMessage& msg)
{
	Position pos = msg.getPosition();
	msg.skipBytes(2); // spriteId
	uint8_t stackpos = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerLookAt(playerID, pos, stackpos); });
}

void ProtocolGame::parseLookInBattleList(NetworkMessage& msg)
{
	uint32_t creatureID = msg.get<uint32_t>();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerLookInBattleList(playerID, creatureID); });
}

void ProtocolGame::parseSay(NetworkMessage& msg)
{
	std::string_view receiver;
	uint16_t channelId;

	SpeakClasses type = static_cast<SpeakClasses>(msg.getByte());
	switch (type) {
		case TALKTYPE_PRIVATE_TO:
		case TALKTYPE_PRIVATE_RED_TO:
			receiver = msg.getString();
			channelId = 0;
			break;

		case TALKTYPE_CHANNEL_Y:
		case TALKTYPE_CHANNEL_R1:
			channelId = msg.get<uint16_t>();
			break;

		default:
			channelId = 0;
			break;
	}

	auto text = msg.getString();
	if (text.length() > 255)
	{
		return;
	}

	addGameTask([=, playerID = player->getID(), receiver = std::string{ receiver }, text = std::string{ text }]()
	{
		g_game.playerSay(playerID, channelId, type, receiver, text);
	});
}

void ProtocolGame::parseFightModes(NetworkMessage& msg)
{
	uint8_t rawFightMode = msg.getByte(); // 1 - offensive, 2 - balanced, 3 - defensive
	uint8_t rawChaseMode = msg.getByte(); // 0 - stand while fighting, 1 - chase opponent
	uint8_t rawSecureMode = msg.getByte(); // 0 - can't attack unmarked, 1 - can attack unmarked
	// uint8_t rawPvpMode = msg.getByte(); // pvp mode introduced in 10.0

	fightMode_t fightMode;
	if (rawFightMode == 1)
	{
		fightMode = FIGHTMODE_ATTACK;
	}
	else if (rawFightMode == 2)
	{
		fightMode = FIGHTMODE_BALANCED;
	}
	else
	{
		fightMode = FIGHTMODE_DEFENSE;
	}

	addGameTask([=, playerID = player->getID()]() { g_game.playerSetFightModes(playerID, fightMode, rawChaseMode != 0, rawSecureMode != 0); });
}

void ProtocolGame::parseAttack(NetworkMessage& msg)
{
	uint32_t creatureID = msg.get<uint32_t>();
	// msg.get<uint32_t>(); creatureID (same as above)
	addGameTask([=, playerID = player->getID()]() { g_game.playerSetAttackedCreature(playerID, creatureID); });
}

void ProtocolGame::parseFollow(NetworkMessage& msg)
{
	uint32_t creatureID = msg.get<uint32_t>();
	// msg.get<uint32_t>(); creatureID (same as above)
	addGameTask([=, playerID = player->getID()]() { g_game.playerFollowCreature(playerID, creatureID); });
}

void ProtocolGame::parseEquipObject(NetworkMessage& msg)
{
	uint16_t spriteID = msg.get<uint16_t>();
	// msg.get<uint8_t>();

	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerEquipItem(playerID, spriteID); });
}

void ProtocolGame::parseTextWindow(NetworkMessage& msg)
{
	uint32_t windowTextID = msg.get<uint32_t>();
	auto newText = msg.getString();
	if (not player->isAccountManager())
	{
		addGameTask([playerID = player->getID(), windowTextID, newText = std::string{ newText }]() { g_game.playerWriteItem(playerID, windowTextID, newText); });
	}
	else
	{
		if (player->getAccount() == 1)
		{
			addGameTask([windowTextID, playerID = player->getID(), newText = std::string{ newText }]() { g_game.onAccountManagerRecieveText(playerID, windowTextID, newText); });
		} 
		else
		{
			addGameTask([windowTextID, playerID = player->getID(), newText = std::string{ newText }]() { g_game.onPrivateAccountManagerRecieveText(playerID, windowTextID, newText); });
		}
	}
}

void ProtocolGame::parseHouseWindow(NetworkMessage& msg)
{
	uint8_t doorId = msg.getByte();
	uint32_t id = msg.get<uint32_t>();
	auto text = msg.getString();
	addGameTask([=, playerID = player->getID(), text = std::string{ text }]() { g_game.playerUpdateHouseWindow(playerID, doorId, id, text); });
}

void ProtocolGame::parseWrapItem(NetworkMessage& msg)
{
	Position pos = msg.getPosition();
	uint16_t spriteId = msg.get<uint16_t>();
	uint8_t stackpos = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerWrapItem(playerID, pos, stackpos, spriteId); });
}

void ProtocolGame::parseLookInShop(NetworkMessage& msg)
{
	uint16_t id = msg.get<uint16_t>();
	uint8_t count = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerLookInShop(playerID, id, count); });
}

void ProtocolGame::parsePlayerPurchase(NetworkMessage& msg)
{
	uint16_t id = msg.get<uint16_t>();
	uint8_t count = msg.getByte();
	uint8_t amount = msg.getByte();
	bool ignoreCap = msg.getByte() != 0;
	bool inBackpacks = msg.getByte() != 0;
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]()
	{
		g_game.playerPurchaseItem(playerID, id, count, amount, ignoreCap, inBackpacks);
	});
}

void ProtocolGame::parsePlayerSale(NetworkMessage& msg)
{
	uint16_t id = msg.get<uint16_t>();
	uint8_t count = msg.getByte();
	uint8_t amount = msg.getByte();
	bool ignoreEquipped = msg.getByte() != 0;
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerSellItem(playerID, id, count, amount, ignoreEquipped); });
}

void ProtocolGame::parseRequestTrade(NetworkMessage& msg)
{
	Position pos = msg.getPosition();
	uint16_t spriteId = msg.get<uint16_t>();
	uint8_t stackpos = msg.getByte();
	uint32_t playerId = msg.get<uint32_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerRequestTrade(playerID, pos, stackpos, playerId, spriteId); });
}

void ProtocolGame::parseLookInTrade(NetworkMessage& msg)
{
	bool counterOffer = (static_cast<ClientCode>(msg.getByte()) == ClientCode::CounterOffer);
	uint8_t index = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerLookInTrade(playerID, counterOffer, index); });
}

void ProtocolGame::parseAddVip(NetworkMessage& msg)
{
	auto name = msg.getString();
	addGameTask([playerID = player->getID(), name = std::string{ name }]() { g_game.playerRequestAddVip(playerID, name); });
}

void ProtocolGame::parseRemoveVip(NetworkMessage& msg)
{
	uint32_t guid = msg.get<uint32_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerRequestRemoveVip(playerID, guid); });
}

void ProtocolGame::parseEditVip(NetworkMessage& msg)
{
	uint32_t guid = msg.get<uint32_t>();
	auto description = msg.getString();
	uint32_t icon = std::min<uint32_t>(10, msg.get<uint32_t>()); // 10 is max icon in 9.63
	bool notify = msg.getByte() != 0;
	addGameTask([=, playerID = player->getID(), description = std::string{ description }]() { g_game.playerRequestEditVip(playerID, guid, description, icon, notify); });
}

void ProtocolGame::parseRotateItem(NetworkMessage& msg)
{
	Position pos = msg.getPosition();
	uint16_t spriteId = msg.get<uint16_t>();
	uint8_t stackpos = msg.getByte();
	addGameTaskTimed(DISPATCHER_TASK_EXPIRATION, [=, playerID = player->getID()]() { g_game.playerRotateItem(playerID, pos, stackpos, spriteId); });
}

void ProtocolGame::parseRuleViolationReport(NetworkMessage& msg)
{
	uint8_t reportType = msg.getByte();
	uint8_t reportReason = msg.getByte();
	auto targetName = msg.getString();
	auto comment = msg.getString();
	std::string_view translation;
	if (reportType == REPORT_TYPE_NAME)
	{
		translation = msg.getString();
	}
	else if (reportType == REPORT_TYPE_STATEMENT)
	{
		translation = msg.getString();
		msg.get<uint32_t>(); // statement id, used to get whatever player have said, we don't log that.
	}

	addGameTask([=, playerID = player->getID(), targetName = std::string{ targetName }, comment = std::string{ comment }, translation = std::string{ translation }]()
	{
		g_game.playerReportRuleViolation(playerID, targetName, reportType, reportReason, comment, translation);
	});
}

void ProtocolGame::parseBugReport(NetworkMessage& msg)
{
	uint8_t category = msg.getByte();
	auto message = msg.getString();

	Position position;
	if (category == BUG_CATEGORY_MAP) {
		position = msg.getPosition();
	}

	addGameTask([=, playerID = player->getID(), message = std::string{ message }]() { g_game.playerReportBug(playerID, message, position, category); });
}

void ProtocolGame::parseDebugAssert(NetworkMessage& msg)
{
	if (debugAssertSent)
	{
		return;
	}

	debugAssertSent = true;

	auto assertLine = msg.getString();
	auto date = msg.getString();
	auto description = msg.getString();
	auto comment = msg.getString();
	addGameTask([playerID = player->getID(), assertLine = std::string{ assertLine }, date = std::string{ date }, description = std::string{ description }, comment = std::string{ comment }]()
	{
		g_game.playerDebugAssert(playerID, assertLine, date, description, comment);
	});
}

void ProtocolGame::parseInviteToParty(NetworkMessage& msg)
{
	uint32_t targetID = msg.get<uint32_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerInviteToParty(playerID, targetID); });
}

void ProtocolGame::parseJoinParty(NetworkMessage& msg)
{
	uint32_t targetID = msg.get<uint32_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerJoinParty(playerID, targetID); });
}

void ProtocolGame::parseRevokePartyInvite(NetworkMessage& msg)
{
	uint32_t targetID = msg.get<uint32_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerRevokePartyInvitation(playerID, targetID); });
}

void ProtocolGame::parsePassPartyLeadership(NetworkMessage& msg)
{
	uint32_t targetID = msg.get<uint32_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerPassPartyLeadership(playerID, targetID); });
}

void ProtocolGame::parseEnableSharedPartyExperience(NetworkMessage& msg)
{
	bool sharedExpActive = msg.getByte() == 1;
	addGameTask([=, playerID = player->getID()]() { g_game.playerEnableSharedPartyExperience(playerID, sharedExpActive); });
}

void ProtocolGame::parseQuestLine(NetworkMessage& msg)
{
	uint16_t questID = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerShowQuestLine(playerID, questID); });
}

void ProtocolGame::parseMarketLeave()
{
	addGameTask([playerID = player->getID()]() { g_game.playerLeaveMarket(playerID); });
}

void ProtocolGame::parseMarketBrowse(NetworkMessage& msg)
{
	uint16_t browseId = msg.get<uint16_t>();
	if (browseId == MARKETREQUEST_OWN_OFFERS)
	{
		g_dispatcher.addTask([playerID = player->getID()]() { g_game.playerBrowseMarketOwnOffers(playerID); });
	}
	else if (browseId == MARKETREQUEST_OWN_HISTORY)
	{
		g_dispatcher.addTask([playerID = player->getID()]() { g_game.playerBrowseMarketOwnHistory(playerID); });
	}
	else
	{
		g_dispatcher.addTask([=, playerID = player->getID()]() { g_game.playerBrowseMarket(playerID, browseId); });
	}
}

void ProtocolGame::parseMarketCreateOffer(NetworkMessage& msg)
{
	uint8_t type = msg.getByte();
	uint16_t spriteId = msg.get<uint16_t>();
	uint16_t amount = msg.get<uint16_t>();
	uint32_t price = msg.get<uint32_t>();
	bool anonymous = (msg.getByte() != 0);
	addGameTask([=, playerID = player->getID()]() { g_game.playerCreateMarketOffer(playerID, type, spriteId, amount, price, anonymous); });
}

void ProtocolGame::parseMarketCancelOffer(NetworkMessage& msg)
{
	uint32_t timestamp = msg.get<uint32_t>();
	uint16_t counter = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerCancelMarketOffer(playerID, timestamp, counter); });
}

void ProtocolGame::parseMarketAcceptOffer(NetworkMessage& msg)
{
	uint32_t timestamp = msg.get<uint32_t>();
	uint16_t counter = msg.get<uint16_t>();
	uint16_t amount = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerAcceptMarketOffer(playerID, timestamp, counter, amount); });
}

void ProtocolGame::parseModalWindowAnswer(NetworkMessage& msg)
{
	uint32_t id = msg.get<uint32_t>();
	uint8_t button = msg.getByte();
	uint8_t choice = msg.getByte();
	addGameTask([=, playerID = player->getID()]() { g_game.playerAnswerModalWindow(playerID, id, button, choice); });
}

void ProtocolGame::parseBrowseField(NetworkMessage& msg)
{
	Position pos = msg.getPosition();
	addGameTask([=, playerID = player->getID()]() { g_game.playerBrowseField(playerID, pos); });
}

void ProtocolGame::parseSeekInContainer(NetworkMessage& msg)
{
	uint8_t containerId = msg.getByte();
	uint16_t index = msg.get<uint16_t>();
	addGameTask([=, playerID = player->getID()]() { g_game.playerSeekInContainer(playerID, containerId, index); });
}

// Send methods
void ProtocolGame::sendOpenPrivateChannel(const std::string& receiver)
{
	NetworkMessage msg;
	msg.add(ServerCode::OpenPrivateChannel);
	msg.addString(receiver);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendChannelEvent(uint16_t channelId, const std::string& playerName, ChannelEvent_t channelEvent)
{
	NetworkMessage msg;
	msg.add(ServerCode::ChannelEvent);
	msg.add<uint16_t>(channelId);
	msg.addString(playerName);
	msg.addByte(channelEvent);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureOutfit(const CreatureConstPtr& creature, const Outfit_t& outfit)
{
	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::CreatureOutfit);
	msg.add<uint32_t>(creature->getID());
	AddOutfit(msg, outfit);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureLight(const CreatureConstPtr& creature)
{
	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	AddCreatureLight(msg, creature);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendWorldLight(LightInfo lightInfo)
{
	NetworkMessage msg;
	AddWorldLight(msg, lightInfo);
	writeToOutputBuffer(msg);
}

// Todo: should be using this for walkthrough in pz, and gm walkthrough (check and make sure it's used)
void ProtocolGame::sendCreatureWalkthrough(const CreatureConstPtr& creature, bool walkthrough)
{
	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::CreatureWalkthrough);
	msg.add<uint32_t>(creature->getID());
	msg.add(walkthrough ? CommonCode::False : CommonCode::True);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureShield(const CreatureConstPtr& creature)
{
	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::CreatureShield);
	msg.add<uint32_t>(creature->getID());
	msg.addByte(player->getPartyShield(creature->getPlayer()));
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureSkull(const CreatureConstPtr& creature)
{
	if (g_game.getWorldType() != WORLD_TYPE_PVP)
	{
		return;
	}

	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::CreatureSkull);
	msg.add<uint32_t>(creature->getID());
	msg.addByte(player->getSkullClient(creature));
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureType(uint32_t creatureId, uint8_t creatureType)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureType);
	msg.add<uint32_t>(creatureId);
	msg.addByte(creatureType);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureHelpers(uint32_t creatureId, uint16_t helpers)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureHelpers);
	msg.add<uint32_t>(creatureId);
	msg.add<uint16_t>(helpers);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureSquare(const CreatureConstPtr& creature, SquareColor_t color)
{
	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::CreatureSquare);
	msg.add<uint32_t>(creature->getID());
	msg.add(CommonCode::True); // todo: figure out what this code is supposed to be, assumed a true/false fill in?
	msg.addByte(color);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendTutorial(uint8_t tutorialId)
{
	NetworkMessage msg;
	msg.add(ServerCode::Tutorial);
	msg.addByte(tutorialId);
	writeToOutputBuffer(msg);
}

// Todo: there are plenty of secondary values passed along with many of these network messages which we don't have
// documented by way of enums or anything, like the below example, marktype, now, perhaps those are actual enums somewhere
// whether or not they are, the point is, all of these secondary values need to have a well defined range that
// any programmer using the code can reference, so the todo is to come back and find them all, and define them all
void ProtocolGame::sendAddMarker(const Position& pos, uint8_t markType, const std::string& desc)
{
	NetworkMessage msg;
	msg.add(ServerCode::MapMarker);
	msg.addPosition(pos);
	msg.addByte(markType);
	msg.addString(desc);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendReLoginWindow(uint8_t unfairFightReduction)
{
	NetworkMessage msg;
	msg.add(ServerCode::Death);
	msg.add(CommonCode::Zero);
	msg.addByte(unfairFightReduction);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendStats()
{
	NetworkMessage msg;
	AddPlayerStats(msg);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendBasicData()
{
	NetworkMessage msg;
	msg.add(ServerCode::BasicData);

	if (player->isPremium())
	{
		msg.add(CommonCode::True);
		msg.add<uint32_t>(g_config.getBoolean(ConfigManager::FREE_PREMIUM) ? 0 : player->premiumEndsAt);
	} 
	else
	{
		msg.add(CommonCode::Zero);
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::Zero));
	}

	msg.addByte(player->getVocation()->getClientId());
	msg.add<uint16_t>(255); // number of known spells

	// todo: figure out why the hell we send every last spell id
	for (uint8_t spellId = 0; spellId < 255; spellId++)
	{
		msg.addByte(spellId);
	}
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendTextMessage(const TextMessage& message)
{
	NetworkMessage msg;
	msg.add(ServerCode::TextMessage);
	msg.addByte(message.type);
	switch (message.type)
	{
		case MESSAGE_DAMAGE_DEALT:
		case MESSAGE_DAMAGE_RECEIVED:
		case MESSAGE_DAMAGE_OTHERS:
		{
			msg.addPosition(message.position);
			msg.add<uint32_t>(message.primary.value);
			msg.addByte(message.primary.color);
			msg.add<uint32_t>(message.secondary.value);
			msg.addByte(message.secondary.color);
			break;
		}
		case MESSAGE_HEALED:
		case MESSAGE_HEALED_OTHERS:
		case MESSAGE_EXPERIENCE:
		case MESSAGE_EXPERIENCE_OTHERS:
		{
			msg.addPosition(message.position);
			msg.add<uint32_t>(message.primary.value);
			msg.addByte(message.primary.color);
			break;
		}
		case MESSAGE_GUILD:
		case MESSAGE_PARTY_MANAGEMENT:
		case MESSAGE_PARTY:
		{
			msg.add<uint16_t>(message.channelId);
			break;
		}
		default: break;
	}
	msg.addString(message.text);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendClosePrivate(uint16_t channelId)
{
	NetworkMessage msg;
	msg.add(ServerCode::ClosePrivateChannel);
	msg.add<uint16_t>(channelId);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatePrivateChannel(uint16_t channelId, const std::string& channelName)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatePrivateChannel);
	msg.add<uint16_t>(channelId);
	msg.addString(channelName);
	msg.add<SpecialCode>(SpecialCode::True);
	msg.addString(player->getName());
	msg.add<SpecialCode>(SpecialCode::Zero);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendChannelsDialog()
{
	NetworkMessage msg;
	msg.add(ServerCode::ChannelsDialog);

	const ChannelList& list = g_chat->getChannelList(player);
	msg.addByte(list.size());
	for (ChatChannel* channel : list)
	{
		msg.add<uint16_t>(channel->getId());
		msg.addString(channel->getName());
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendChannel(uint16_t channelId, const std::string& channelName, const UsersMap* channelUsers, const InvitedMap* invitedUsers)
{
	NetworkMessage msg;
	msg.add(ServerCode::SendChannel);

	msg.add<uint16_t>(channelId);
	msg.addString(channelName);

	if (channelUsers)
	{
		msg.add<uint16_t>(channelUsers->size());
		for (const auto& it : *channelUsers)
		{
			msg.addString(it.second->getName());
		}
	} 
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (invitedUsers)
	{
		msg.add<uint16_t>(invitedUsers->size());
		for (const auto& it : *invitedUsers)
		{
			msg.addString(it.second->getName());
		}
	} 
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendChannelMessage(const std::string& author, const std::string& text, SpeakClasses type, uint16_t channel)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureSay);
	msg.add<uint32_t>(0); // statement guid (unused by clients)
	msg.addString(author);
	msg.add<SpecialCode>(SpecialCode::Zero);
	msg.addByte(type);
	msg.add<uint16_t>(channel);
	msg.addString(text);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendIcons(uint16_t icons)
{
	NetworkMessage msg;
	msg.add(ServerCode::Icons);
	msg.add<uint16_t>(icons);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendContainer(uint8_t cid, const ContainerConstPtr& container, bool hasParent, uint16_t firstIndex)
{
	NetworkMessage msg;
	msg.add(ServerCode::Container);

	msg.addByte(cid);

	if (container->getID() == ITEM_BROWSEFIELD)
	{
		msg.addItem(ITEM_BAG, 1);
		msg.addString("Browse Field");
	}
	else
	{
		msg.addItem(container);
		msg.addString(container->getName());
	}

	msg.addByte(container->capacity());

	msg.add(hasParent ? CommonCode::True : CommonCode::Zero);

	msg.add(container->isUnlocked() ? CommonCode::True : CommonCode::Zero); // Drag and drop
	msg.add(container->hasPagination() ? CommonCode::True : CommonCode::Zero); // Pagination

	uint32_t containerSize = container->size();
	msg.add<uint16_t>(containerSize);
	msg.add<uint16_t>(firstIndex);
	if (firstIndex < containerSize)
	{
		uint8_t itemsToSend = std::min<uint32_t>(std::min<uint32_t>(container->capacity(), containerSize - firstIndex), std::numeric_limits<uint8_t>::max());

		msg.addByte(itemsToSend);
		for (auto it = container->getItemList().begin() + firstIndex, end = it + itemsToSend; it != end; ++it)
		{
			msg.addItem(*it);
		}
	}
	else
	{
		msg.add(CommonCode::Zero);
	}
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendShop(const NpcPtr& npc, const ShopInfoList& itemList)
{
	NetworkMessage msg;
	msg.add(ServerCode::NpcShop);
	msg.addString(npc->getName());

	uint16_t itemsToSend = std::min<size_t>(itemList.size(), std::numeric_limits<uint16_t>::max());
	msg.add<uint16_t>(itemsToSend);

	uint16_t i = 0;
	for (auto it = itemList.begin(); i < itemsToSend; ++it, ++i)
	{
		AddShopItem(msg, *it);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCloseShop()
{
	NetworkMessage msg;
	msg.add(ServerCode::CloseNpcShop);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendSaleItemList(const std::list<ShopInfo>& shop)
{
	NetworkMessage msg;
	msg.add(ServerCode::SaleItemList);
	msg.add<uint64_t>(player->getMoney() + player->getBankBalance());

	std::map<uint16_t, uint32_t> saleMap;

	if (shop.size() <= 5)
	{
		// For very small shops it's not worth it to create the complete map
		for (const ShopInfo& shopInfo : shop)
		{
			if (shopInfo.sellPrice == 0)
			{
				continue;
			}

			int8_t subtype = -1;

			const ItemType& itemType = Item::items[shopInfo.itemId];
			if (itemType.hasSubType() and !itemType.stackable)
			{
				subtype = (shopInfo.subType == 0 ? -1 : shopInfo.subType);
			}

			uint32_t count = player->getItemTypeCount(shopInfo.itemId, subtype);
			if (count > 0)
			{
				saleMap[shopInfo.itemId] = count;
			}
		}
	}
	else
	{
		// Large shop, it's better to get a cached map of all item counts and use it
		// We need a temporary map since the finished map should only contain items
		// available in the shop
		gtl::btree_map<uint32_t, uint32_t> tempSaleMap;
		player->getAllItemTypeCount(tempSaleMap);

		// We must still check manually for the special items that require subtype matches
		// (That is, fluids such as potions etc., actually these items are very few since
		// health potions now use their own ID)
		for (const ShopInfo& shopInfo : shop)
		{
			if (shopInfo.sellPrice == 0)
			{
				continue;
			}

			int8_t subtype = -1;

			const ItemType& itemType = Item::items[shopInfo.itemId];
			if (itemType.hasSubType() and !itemType.stackable)
			{
				subtype = (shopInfo.subType == 0 ? -1 : shopInfo.subType);
			}

			if (subtype != -1)
			{
				uint32_t count;
				if (itemType.isFluidContainer() or itemType.isSplash())
				{
					count = player->getItemTypeCount(shopInfo.itemId, subtype); // This shop item requires extra checks
				}
				else
				{
					count = subtype;
				}

				if (count > 0)
				{
					saleMap[shopInfo.itemId] = count;
				}
			}
			else
			{
				gtl::btree_map<uint32_t, uint32_t>::const_iterator findIt = tempSaleMap.find(shopInfo.itemId);
				if (findIt != tempSaleMap.end() and findIt->second > 0)
				{
					saleMap[shopInfo.itemId] = findIt->second;
				}
			}
		}
	}

	uint8_t itemsToSend = std::min<size_t>(saleMap.size(), std::numeric_limits<uint8_t>::max());
	msg.addByte(itemsToSend);

	uint8_t i = 0;
	for (std::map<uint16_t, uint32_t>::const_iterator it = saleMap.begin(); i < itemsToSend; ++it, ++i)
	{
		msg.addItemId(it->first);
		msg.addByte(std::min<uint32_t>(it->second, std::numeric_limits<uint8_t>::max()));
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketEnter()
{
	NetworkMessage msg;
	msg.add(ServerCode::MarketEnter);

	msg.add<uint64_t>(player->getBankBalance());
	msg.addByte(std::min<uint32_t>(IOMarket::getPlayerOfferCount(player->getGUID()), std::numeric_limits<uint8_t>::max()));

	player->setInMarket(true);

	std::map<uint16_t, uint32_t> depotItems;
	std::forward_list<ContainerPtr> containerList{ player->getInbox() };

	for (const auto& chest : player->depotChests)
	{
		if (not chest.second->empty())
		{
			containerList.push_front(chest.second);
		}
	}

	do
	{
		ContainerPtr container = containerList.front();
		containerList.pop_front();

		if (not container)
		{
			continue;
		}

		for (const auto& item : container->getItemList())
		{
			const auto& c = item->getContainer();
			if (c and not c->empty())
			{
				containerList.push_front(c);
				continue;
			}

			const ItemType& itemType = Item::items[item->getID()];
			if (itemType.wareId == 0)
			{
				continue;
			}

			if (c and (not itemType.isContainer() or c->capacity() != itemType.maxItems))
			{
				continue;
			}

			if (not item->hasMarketAttributes())
			{
				continue;
			}

			depotItems[itemType.wareId] += Item::countByType(item, -1);
		}
	} while (not containerList.empty());

	uint16_t itemsToSend = std::min<size_t>(depotItems.size(), std::numeric_limits<uint16_t>::max());
	msg.add<uint16_t>(itemsToSend);

	uint16_t i = 0;
	for (std::map<uint16_t, uint32_t>::const_iterator it = depotItems.begin(); i < itemsToSend; ++it, ++i)
	{
		msg.add<uint16_t>(it->first);
		
		msg.add<uint16_t>(std::min<uint32_t>(std::numeric_limits<uint16_t>::max(), it->second));
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketLeave()
{
	NetworkMessage msg;
	msg.add(ServerCode::MarketLeave);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketBrowseItem(uint16_t itemId, const MarketOfferList& buyOffers, const MarketOfferList& sellOffers)
{
	NetworkMessage msg;
	msg.reset();
	msg.add(ServerCode::MarketAction);
	msg.addItemId(itemId);

	msg.add<uint32_t>(buyOffers.size());

	for (const MarketOffer& offer : buyOffers)
	{
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
		msg.addString(offer.playerName);
	}

	msg.add<uint32_t>(sellOffers.size());

	for (const MarketOffer& offer : sellOffers)
	{
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
		msg.addString(offer.playerName);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketAcceptOffer(const MarketOfferEx& offer)
{
	NetworkMessage msg;
	msg.add(ServerCode::MarketAction);
	msg.addItemId(offer.itemId);

	if (offer.type == MARKETACTION_BUY)
	{
		msg.add<uint32_t>(1);
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
		msg.addString(offer.playerName);
		msg.add<uint32_t>(0);
	}
	else
	{
		msg.add<uint32_t>(0);
		msg.add<uint32_t>(1);
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
		msg.addString(offer.playerName);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketBrowseOwnOffers(const MarketOfferList& buyOffers, const MarketOfferList& sellOffers)
{
	NetworkMessage msg;
	msg.add(ServerCode::MarketAction);
	msg.add<uint16_t>(MARKETREQUEST_OWN_OFFERS);

	msg.add<uint32_t>(buyOffers.size());
	for (const MarketOffer& offer : buyOffers)
	{
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.addItemId(offer.itemId);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
	}

	msg.add<uint32_t>(sellOffers.size());
	for (const MarketOffer& offer : sellOffers)
	{
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.addItemId(offer.itemId);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketCancelOffer(const MarketOfferEx& offer)
{
	NetworkMessage msg;
	msg.add(ServerCode::MarketAction);
	msg.add<uint16_t>(MARKETREQUEST_OWN_OFFERS);

	if (offer.type == MARKETACTION_BUY)
	{
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::True));
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.addItemId(offer.itemId);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::Zero));
	}
	else
	{
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::Zero));
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::True));
		msg.add<uint32_t>(offer.timestamp);
		msg.add<uint16_t>(offer.counter);
		msg.addItemId(offer.itemId);
		msg.add<uint16_t>(offer.amount);
		msg.add<uint32_t>(offer.price);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketBrowseOwnHistory(const HistoryMarketOfferList& buyOffers, const HistoryMarketOfferList& sellOffers)
{
	uint32_t i = 0;
	std::map<uint32_t, uint16_t> counterMap;
	uint32_t buyOffersToSend = std::min<uint32_t>(buyOffers.size(), 810 + std::max<int32_t>(0, 810 - sellOffers.size()));
	uint32_t sellOffersToSend = std::min<uint32_t>(sellOffers.size(), 810 + std::max<int32_t>(0, 810 - buyOffers.size()));

	NetworkMessage msg;
	msg.add(ServerCode::MarketAction);
	msg.add<uint16_t>(MARKETREQUEST_OWN_HISTORY);

	msg.add<uint32_t>(buyOffersToSend);
	for (auto it = buyOffers.begin(); i < buyOffersToSend; ++it, ++i)
	{
		msg.add<uint32_t>(it->timestamp);
		msg.add<uint16_t>(counterMap[it->timestamp]++);
		msg.addItemId(it->itemId);
		msg.add<uint16_t>(it->amount);
		msg.add<uint32_t>(it->price);
		msg.addByte(it->state);
	}

	counterMap.clear();
	i = 0;

	msg.add<uint32_t>(sellOffersToSend);
	for (auto it = sellOffers.begin(); i < sellOffersToSend; ++it, ++i)
	{
		msg.add<uint32_t>(it->timestamp);
		msg.add<uint16_t>(counterMap[it->timestamp]++);
		msg.addItemId(it->itemId);
		msg.add<uint16_t>(it->amount);
		msg.add<uint32_t>(it->price);
		msg.addByte(it->state);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMarketDetail(uint16_t itemId)
{
	NetworkMessage msg;
	msg.add(ServerCode::MarketDetail);
	msg.addItemId(itemId);

	const ItemType& it = Item::items[itemId];
	if (it.armor != 0)
	{
		msg.addString(std::to_string(it.armor));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.attack != 0)
	{
		// TODO: chance to hit, range
		// example:
		// "attack +x, chance to hit +y%, z fields"
		if (it.abilities and it.abilities->elementType != COMBAT_NONE and it.abilities->elementDamage != 0) {
			msg.addString(fmt::format("{:d} physical +{:d} {:s}", it.attack, it.abilities->elementDamage, getCombatName(it.abilities->elementType)));
		}
		else
		{
			msg.addString(std::to_string(it.attack));
		}
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.isContainer())
	{
		msg.addString(std::to_string(it.maxItems));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.defense != 0)
	{
		if (it.extraDefense != 0)
		{
			msg.addString(fmt::format("{:d} {:+d}", it.defense, it.extraDefense));
		}
		else
		{
			msg.addString(std::to_string(it.defense));
		}
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (not it.description.empty())
	{
		const std::string& descr = it.description;
		if (descr.back() == '.')
		{
			msg.addString(std::string(descr, 0, descr.length() - 1));
		}
		else
		{
			msg.addString(descr);
		}
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.decayTime != 0)
	{
		msg.addString(fmt::format("{:d} seconds", it.decayTime));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.minReqLevel != 0)
	{
		msg.addString(std::to_string(it.minReqLevel));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.minReqMagicLevel != 0)
	{
		msg.addString(std::to_string(it.minReqMagicLevel));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	msg.addString(it.vocationString);

	msg.addString(it.runeSpellName);

	if (it.abilities)
	{
		std::ostringstream ss;
		bool separator = false;

		for (uint8_t i = SKILL_FIRST; i <= SKILL_LAST; i++)
		{
			if (not it.abilities->skills[i])
			{
				continue;
			}

			if (separator)
			{
				ss << ", ";
			} 
			else
			{
				separator = true;
			}

			ss << getSkillName(i) << ' ' << std::showpos << it.abilities->skills[i] << std::noshowpos;
		}

		if (it.abilities->stats[STAT_MAGICPOINTS] != 0)
		{
			if (separator)
			{
				ss << ", ";
			}
			else
			{
				separator = true;
			}

			ss << "magic level " << std::showpos << it.abilities->stats[STAT_MAGICPOINTS] << std::noshowpos;
		}

		if (it.abilities->speed != 0)
		{
			if (separator)
			{
				ss << ", ";
			}

			ss << "speed " << std::showpos << (it.abilities->speed >> 1) << std::noshowpos;
		}

		msg.addString(ss.str());
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	if (it.charges != 0)
	{
		msg.addString(std::to_string(it.charges));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	std::string weaponName = getWeaponName(it.weaponType);

	if (it.slotPosition & SLOTP_TWO_HAND)
	{
		if (not weaponName.empty())
		{
			weaponName += ", two-handed";
		}
		else
		{
			weaponName = "two-handed";
		}
	}

	msg.addString(weaponName);

	if (it.weight != 0)
	{
		std::ostringstream ss;
		if (it.weight < 10)
		{
			ss << "0.0" << it.weight;
		} 
		else if (it.weight < 100)
		{
			ss << "0." << it.weight;
		}
		else
		{
			std::string weightString = std::to_string(it.weight);
			weightString.insert(weightString.end() - 2, '.');
			ss << weightString;
		}
		ss << " oz";
		msg.addString(ss.str());
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	MarketStatistics* statistics = IOMarket::getInstance().getPurchaseStatistics(itemId);
	if (statistics)
	{
		msg.add(CommonCode::True);
		msg.add<uint32_t>(statistics->numTransactions);
		msg.add<uint32_t>(std::min<uint64_t>(std::numeric_limits<uint32_t>::max(), statistics->totalPrice));
		msg.add<uint32_t>(statistics->highestPrice);
		msg.add<uint32_t>(statistics->lowestPrice);
	}
	else
	{
		msg.add(CommonCode::Zero);
	}

	statistics = IOMarket::getInstance().getSaleStatistics(itemId);
	if (statistics)
	{
		msg.add(CommonCode::True);
		msg.add<uint32_t>(statistics->numTransactions);
		msg.add<uint32_t>(std::min<uint64_t>(std::numeric_limits<uint32_t>::max(), statistics->totalPrice));
		msg.add<uint32_t>(statistics->highestPrice);
		msg.add<uint32_t>(statistics->lowestPrice);
	}
	else
	{
		msg.add(CommonCode::Zero);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendQuestLog()
{
	NetworkMessage msg;
	msg.add(ServerCode::QuestLog);
	msg.add<uint16_t>(g_game.quests.getQuestsCount(player));

	for (const Quest& quest : g_game.quests.getQuests())
	{
		if (quest.isStarted(player))
		{
			msg.add<uint16_t>(quest.getID());
			msg.addString(quest.getName());
			msg.addByte(quest.isCompleted(player));
		}
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendQuestLine(const Quest* quest)
{
	NetworkMessage msg;
	msg.add(ServerCode::QuestLine);
	msg.add<uint16_t>(quest->getID());
	msg.addByte(quest->getMissionsCount(player));

	for (const Mission& mission : quest->getMissions())
	{
		if (mission.isStarted(player))
		{
			msg.addString(mission.getName(player));
			msg.addString(mission.getDescription(player));
		}
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendTradeItemRequest(const std::string& traderName, const ItemPtr& item, bool ack)
{
	if (not item) {
		std::cout << "Error: item is null!" << std::endl;
		return;
	}

	NetworkMessage msg;
	msg.add(ack ? ServerCode::TradeItemRequest : ServerCode::TradeAcknowledged);
	msg.addString(traderName);

	if (auto tradeContainer = item->getContainer())
	{

		std::vector<ContainerPtr> containerStack;
		std::vector<ItemPtr> itemList;

		itemList.push_back(item);

		if (not tradeContainer->getItemList().empty())
		{
			containerStack.push_back(tradeContainer);
		}

		while (not containerStack.empty())
		{
			auto container = containerStack.back();
			containerStack.pop_back();

			if (not container)
			{
				continue;
			}

			for (auto& containerItem : container->getItemList())
			{
				if (not containerItem)
				{
					continue;
				}

				if (auto tmpContainer = containerItem->getContainer())
				{
					containerStack.push_back(tmpContainer);
				}

				itemList.push_back(containerItem);
			}
		}

		msg.addByte(static_cast<uint8_t>(itemList.size()));

		for (const auto& listItem : itemList)
		{
			if (listItem)
			{
				msg.addItem(listItem);
			}
		}
	}
	else
	{
		msg.add(CommonCode::True);
		msg.addItem(item);
	}

	writeToOutputBuffer(msg);
}



void ProtocolGame::sendCloseTrade()
{
	NetworkMessage msg;
	msg.add(ServerCode::CloseTrade);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCloseContainer(uint8_t cid)
{
	NetworkMessage msg;
	msg.add(ServerCode::CloseContainer);
	msg.addByte(cid);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureTurn(const CreatureConstPtr& creature, uint32_t stackPos)
{
	if (not canSee(creature))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::UpdateTileThing);
	if (stackPos >= 10)
	{
		msg.add<SpecialCode>(SpecialCode::End);
		msg.add<uint32_t>(creature->getID());
	}
	else
	{
		msg.addPosition(creature->getPosition());
		msg.addByte(stackPos);
	}

	msg.add<SpecialCode>(SpecialCode::CreatureTurn);
	msg.add<uint32_t>(creature->getID());
	msg.addByte(creature->getDirection());
	msg.add(player->canWalkthroughEx(creature) ? CommonCode::Zero : CommonCode::True);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureSay(const CreatureConstPtr& creature, SpeakClasses type, const std::string& text, const Position* pos/* = nullptr*/)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureSay);

	static uint32_t statementId = 0;
	msg.add<uint32_t>(++statementId);

	msg.addString(creature->getName());

	//Add level only for players
	if (const auto& speaker = creature->getPlayer())
	{
		msg.add<uint16_t>(speaker->getLevel());
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	msg.addByte(type);

	if (pos)
	{
		msg.addPosition(*pos);
	}
	else
	{
		msg.addPosition(creature->getPosition());
	}

	msg.addString(text);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendToChannel(const CreatureConstPtr& creature, SpeakClasses type, const std::string& text, uint16_t channelId)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureSay);

	static uint32_t statementId = 0;
	msg.add<uint32_t>(++statementId);
	if (not creature)
	{
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::Zero));
	}
	else
	{
		msg.addString(creature->getName());
		//Add level only for players
		if (const auto& speaker = creature->getPlayer())
		{
			msg.add<uint16_t>(speaker->getLevel());
		}
		else
		{
			msg.add<SpecialCode>(SpecialCode::Zero);
		}
	}

	msg.addByte(type);
	msg.add<uint16_t>(channelId);
	msg.addString(text);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendPrivateMessage(const PlayerConstPtr& speaker, SpeakClasses type, const std::string& text)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureSay);
	static uint32_t statementId = 0;
	msg.add<uint32_t>(++statementId);
	if (speaker)
	{
		msg.addString(speaker->getName());
		msg.add<uint16_t>(speaker->getLevel());
	}
	else
	{
		msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::Zero));
	}
	msg.addByte(type);
	msg.addString(text);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCancelTarget()
{
	NetworkMessage msg;
	msg.add(ServerCode::CancelTarget);
	msg.add<uint32_t>(static_cast<uint32_t>(CommonCode::Zero)); // interesting, why do we need to send this? What other possibilities are there?
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendChangeSpeed(const CreatureConstPtr& creature, uint32_t speed)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureSpeed);
	msg.add<uint32_t>(creature->getID());
	msg.add<uint16_t>(creature->getBaseSpeed() / 2);
	msg.add<uint16_t>(speed / 2);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCancelWalk()
{
	NetworkMessage msg;
	msg.add(ServerCode::CancelWalk);
	msg.addByte(player->getDirection());
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendSkills()
{
	NetworkMessage msg;
	AddPlayerSkills(msg);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendPing()
{
	NetworkMessage msg;
	msg.add(ServerCode::Ping);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendPingBack()
{
	NetworkMessage msg;
	msg.add(ServerCode::PingBack);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendDistanceShoot(const Position& from, const Position& to, uint8_t type)
{
	NetworkMessage msg;
	msg.add(ServerCode::DistanceShoot);
	msg.addPosition(from);
	msg.addPosition(to);
	msg.addByte(type);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendMagicEffect(const Position& pos, uint8_t type)
{
	if (not canSee(pos)) {
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::MagicEffect);
	msg.addPosition(pos);
	msg.addByte(type);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendCreatureHealth(const CreatureConstPtr& creature)
{
	NetworkMessage msg;
	msg.add(ServerCode::CreatureHealth);
	msg.add<uint32_t>(creature->getID());

	if (creature->isHealthHidden())
	{
		msg.add(CommonCode::Zero);
	}
	else
	{
		msg.addByte(std::ceil((static_cast<double>(creature->getHealth()) / std::max<int32_t>(creature->getMaxHealth(), 1)) * 100));
	}
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendFYIBox(const std::string& message)
{
	NetworkMessage msg;
	msg.add(ServerCode::FyiBox);
	msg.addString(message);
	writeToOutputBuffer(msg);
}

//tile
void ProtocolGame::sendMapDescription(const Position& pos)
{
	NetworkMessage msg;
	msg.add(ServerCode::MapDescription);
	msg.addPosition(player->getPosition());
	GetMapDescription(pos.x - Map::maxClientViewportX, pos.y - Map::maxClientViewportY, pos.z, (Map::maxClientViewportX * 2) + 2, (Map::maxClientViewportY * 2) + 2, msg);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendAddTileItem(const Position& pos, uint32_t stackpos, const ItemConstPtr& item)
{
	if (not canSee(pos))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::AddTileThing);
	msg.addPosition(pos);
	msg.addByte(stackpos);
	msg.addItem(item);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendUpdateTileItem(const Position& pos, uint32_t stackpos, const ItemConstPtr& item)
{
	if (not canSee(pos))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::UpdateTileThing);
	msg.addPosition(pos);
	msg.addByte(stackpos);
	msg.addItem(item);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendRemoveTileThing(const Position& pos, uint32_t stackpos)
{
	if (not canSee(pos))
	{
		return;
	}

	NetworkMessage msg;
	RemoveTileThing(msg, pos, stackpos);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendUpdateTileCreature(const Position& pos, uint32_t stackpos, const CreatureConstPtr& creature)
{
	if (not canSee(pos))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::UpdateTileThing);
	msg.addPosition(pos);
	msg.addByte(stackpos);

	bool known;
	uint32_t removedKnown;
	checkCreatureAsKnown(creature->getID(), known, removedKnown);
	AddCreature(msg, creature, false, removedKnown);

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendRemoveTileCreature(const CreatureConstPtr& creature, const Position& pos, uint32_t stackpos)
{
    NetworkMessage msg;

	if (stackpos < 10) // todo : change all of these 10 magic numbers into their own config constant, with possibly a hard max?
	{
		if (not canSee(pos))
		{
			return;
		}

		RemoveTileThing(msg, pos, stackpos);
		writeToOutputBuffer(msg);
		return;
	}

	msg.add(ServerCode::RemoveTileThing);
	msg.add<SpecialCode>(SpecialCode::End);
	msg.add<uint32_t>(creature->getID());
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendUpdateTile(const TileConstPtr& tile, const Position& pos)
{
	if (not canSee(pos))
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::UpdateTile);
	msg.addPosition(pos);

	if (tile)
	{
		GetTileDescription(tile, msg);
		msg.add(CommonCode::Zero);
		msg.add(CommonCode::End);
	}
	else
	{
		msg.add(CommonCode::True);
		msg.add(CommonCode::End);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendPendingStateEntered()
{
	NetworkMessage msg;
	msg.add(ServerCode::PendingStateEntered);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendEnterWorld()
{
	NetworkMessage msg;
	msg.add(ServerCode::EnterWorld);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendFightModes()
{
	NetworkMessage msg;
	msg.add(ServerCode::FightMode);
	msg.addByte(player->fightMode);
	msg.addByte(player->chaseMode);
	msg.addByte(player->secureMode);
	msg.addByte(PVP_MODE_DOVE);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendAddCreature(const CreatureConstPtr& creature, const Position& pos, int32_t stackpos, MagicEffectClasses magicEffect/*= CONST_ME_NONE*/)
{
	if (not canSee(pos))
	{
		return;
	}

	if (creature != player)
	{
		// stack pos is always real index now, so it can exceed the limit
		// if stack pos exceeds the limit, we need to refresh the tile instead
		// 1. this is a rare case, and is only triggered by forcing summon in a position
		// 2. since no stackpos will be send to the client about that creature, removing
		//    it must be done with its id if its stackpos remains >= 10. this is done to
		//    add creatures to battle list instead of rendering on screen
		if (stackpos >= 10)
		{
			// @todo: should we avoid this check?
			if (const auto& tile = creature->getTile())
			{
				sendUpdateTile(tile, pos);
			}
		}
		else
		{
			// if stackpos is -1, the client will automatically detect it
			NetworkMessage msg;
			msg.add(ServerCode::AddTileThing);
			msg.addPosition(pos);
			msg.addByte(stackpos);

			bool known;
			uint32_t removedKnown;
			checkCreatureAsKnown(creature->getID(), known, removedKnown);
			AddCreature(msg, creature, known, removedKnown);
			writeToOutputBuffer(msg);
		}

		if (magicEffect != CONST_ME_NONE)
		{
			sendMagicEffect(pos, magicEffect);
		}
		return;
	}

	// For sure a player at this point

	NetworkMessage msg;
	msg.add(ServerCode::LoginSuccess);

	msg.add<uint32_t>(player->getID());
	msg.add<uint16_t>(50); // beat duration.. why does client need this? I suspect we can manipulate this for our own benefits

	msg.addDouble(Creature::speedA, 3);
	msg.addDouble(Creature::speedB, 3);
	msg.addDouble(Creature::speedC, 3);

	// can report bugs?
	if (player->getAccountType() >= ACCOUNT_TYPE_TUTOR)
	{
		msg.add(CommonCode::True);
	}
	else
	{
		msg.add(CommonCode::False);
	}

	msg.add(CommonCode::Zero); // can change pvp framing option
	msg.add(CommonCode::Zero); // expert mode button enabled

	msg.add<uint16_t>(0x00); // URL (string) to ingame store images
	msg.add<uint16_t>(25); // premium coin package size -- this should probably be a config option

	writeToOutputBuffer(msg);

	sendPendingStateEntered();
	sendEnterWorld();
	sendMapDescription(pos);

	if (magicEffect != CONST_ME_NONE)
	{
		sendMagicEffect(pos, magicEffect);
	}

	for (int i = CONST_SLOT_FIRST; i <= CONST_SLOT_LAST; ++i)
	{
		sendInventoryItem(static_cast<slots_t>(i), player->getInventoryItem(static_cast<slots_t>(i)));
	}

	sendInventoryItem(CONST_SLOT_STORE_INBOX, player->getStoreInbox()->getItem());

	const bool open = g_config.getBoolean(ConfigManager::AUTO_OPEN_CONTAINERS);
    if (open)
		player->autoOpenContainers();

	sendStats();
	sendSkills();

	//gameworld light-settings
	sendWorldLight(g_game.getWorldLightInfo());

	//player light level
	sendCreatureLight(creature);

	sendVIPEntries();

	sendBasicData();
	player->sendIcons();
}

void ProtocolGame::sendMoveCreature(const CreatureConstPtr& creature, const Position& newPos, int32_t newStackPos, const Position& oldPos, int32_t oldStackPos, bool teleport)
{
	if (creature == player)
	{
		if (teleport)
		{
			sendRemoveTileCreature(creature, oldPos, oldStackPos);
			sendMapDescription(newPos);
		}
		else
		{
			NetworkMessage msg;
			if (oldPos.z == 7 and newPos.z >= 8)
			{
				RemoveTileCreature(msg, creature, oldPos, oldStackPos);
			}
			else
			{
				msg.add(ServerCode::MoveCreature);
				if (oldStackPos < 10)
				{
					msg.addPosition(oldPos);
					msg.addByte(oldStackPos);
				}
				else
				{
					msg.add<SpecialCode>(SpecialCode::End);
					msg.add<uint32_t>(creature->getID());
				}
				msg.addPosition(newPos);
			}

			if (newPos.z > oldPos.z)
			{
				MoveDownCreature(msg, creature, newPos, oldPos);
			}
			else if (newPos.z < oldPos.z)
			{
				MoveUpCreature(msg, creature, newPos, oldPos);
			}

			if (oldPos.y > newPos.y)
			{ // north, for old x
				msg.add(ServerCode::MoveNorth);
				GetMapDescription(oldPos.x - Map::maxClientViewportX, newPos.y - Map::maxClientViewportY, newPos.z, (Map::maxClientViewportX * 2) + 2, 1, msg);
			}
			else if (oldPos.y < newPos.y)
			{ // south, for old x
				msg.add(ServerCode::MoveSouth);
				GetMapDescription(oldPos.x - Map::maxClientViewportX, newPos.y + (Map::maxClientViewportY + 1), newPos.z, (Map::maxClientViewportX * 2) + 2, 1, msg);
			}

			if (oldPos.x < newPos.x)
			{ // east, [with new y]
				msg.add(ServerCode::MoveEast);
				GetMapDescription(newPos.x + (Map::maxClientViewportX + 1), newPos.y - Map::maxClientViewportY, newPos.z, 1, (Map::maxClientViewportY * 2) + 2, msg);
			}
			else if (oldPos.x > newPos.x)
			{ // west, [with new y]
				msg.add(ServerCode::MoveWest);
				GetMapDescription(newPos.x - Map::maxClientViewportX, newPos.y - Map::maxClientViewportY, newPos.z, 1, (Map::maxClientViewportY * 2) + 2, msg);
			}
			writeToOutputBuffer(msg);
		}
	}
	else if (canSee(oldPos) and canSee(creature->getPosition()))
	{
		if (teleport or (oldPos.z == 7 and newPos.z >= 8))
		{
			sendRemoveTileCreature(creature, oldPos, oldStackPos);
			sendAddCreature(creature, newPos, newStackPos);
		}
		else
		{
			NetworkMessage msg;
			msg.add(ServerCode::MoveCreature);
			if (oldStackPos < 10)
			{
				msg.addPosition(oldPos);
				msg.addByte(oldStackPos);
			}
			else
			{
				msg.add<SpecialCode>(SpecialCode::End);
				msg.add<uint32_t>(creature->getID());
			}
			msg.addPosition(creature->getPosition());
			writeToOutputBuffer(msg);
		}
	}
	else if (canSee(oldPos))
	{
		sendRemoveTileCreature(creature, oldPos, oldStackPos);
	}
	else if (canSee(creature->getPosition()))
	{
		sendAddCreature(creature, newPos, newStackPos);
	}
}

void ProtocolGame::sendInventoryItem(slots_t slot, const ItemConstPtr& item)
{
	NetworkMessage msg;
	if (item)
	{
		msg.add(ServerCode::InventoryItem);
		msg.addByte(slot);
		msg.addItem(item);
	}
	else
	{
		msg.add(ServerCode::EmptyInventory);
		msg.addByte(slot);
	}
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendItems()
{
	NetworkMessage msg;
	msg.add(ServerCode::SendItems);

	const std::vector<uint16_t>& inventory = Item::items.getInventory();
	msg.add<uint16_t>(inventory.size() + 11);
	// todo figure out what the above magic number means and give it a constant
	// and investigate if this is the same loop twice or whats going on here...
	for (uint16_t i = 1; i <= 11; i++)
	{
		msg.add<uint16_t>(i);
		msg.add(CommonCode::Zero); //always 0
		msg.add<uint16_t>(static_cast<uint16_t>(CommonCode::True)); // always 1
	}

	for (auto itemTypeID : inventory)
	{
		msg.add<uint16_t>(itemTypeID);
		msg.add(CommonCode::Zero); //always 0
		msg.add<uint16_t>(1);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendAddContainerItem(uint8_t cid, uint16_t slot, const ItemConstPtr& item)
{
	NetworkMessage msg;
	msg.add(ServerCode::AddContainerItem);
	msg.addByte(cid);
	msg.add<uint16_t>(slot);
	msg.addItem(item);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendUpdateContainerItem(uint8_t cid, uint16_t slot, const ItemConstPtr& item)
{
	NetworkMessage msg;
	msg.add(ServerCode::UpdateContainerItem);
	msg.addByte(cid);
	msg.add<uint16_t>(slot);
	msg.addItem(item);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendRemoveContainerItem(uint8_t cid, uint16_t slot, const ItemConstPtr& lastItem)
{
	NetworkMessage msg;
	msg.add(ServerCode::RemoveContainerItem);
	msg.addByte(cid);
	msg.add<uint16_t>(slot);
	if (lastItem)
	{
		msg.addItem(lastItem);
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendTextWindow(uint32_t windowTextId, const ItemPtr& item, uint16_t maxlen, bool canWrite)
{
	NetworkMessage msg;
	msg.add(ServerCode::TextWindow);
	msg.add<uint32_t>(windowTextId);
	msg.addItem(item);

	if (canWrite)
	{
		msg.add<uint16_t>(maxlen);
		msg.addString(item->getText());
	}
	else
	{
		const std::string& text = item->getText();
		msg.add<uint16_t>(text.size());
		msg.addString(text);
	}

	const std::string& writer = item->getWriter();
	if (not writer.empty())
	{
		msg.addString(writer);
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	time_t writtenDate = item->getDate();
	if (writtenDate != 0)
	{
		msg.addString(formatDateShort(writtenDate));
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendTextWindow(uint32_t windowTextId, uint32_t itemId, const std::string& text)
{
	NetworkMessage msg;
	msg.add(ServerCode::TextWindow);
	msg.add<uint32_t>(windowTextId);
	msg.addItem(itemId, 1);
	msg.add<uint16_t>(text.size());
	msg.addString(text);
	msg.add<SpecialCode>(SpecialCode::Zero);
	msg.add<SpecialCode>(SpecialCode::Zero);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendHouseWindow(uint32_t windowTextId, const std::string& text)
{
	NetworkMessage msg;
	msg.add(ServerCode::HouseWindow);
	msg.add(CommonCode::Zero);
	msg.add<uint32_t>(windowTextId);
	msg.addString(text);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendAccountManagerTextBox(uint32_t windowTextId, const std::string& text)
{
	NetworkMessage msg;
	msg.add(ServerCode::TextWindow);
	msg.add<uint32_t>(windowTextId);
	msg.addItem(ITEM_LETTER, 1);
	msg.add<uint16_t>(18); // max string length aka max chars
	msg.addString(text);
	msg.add<SpecialCode>(SpecialCode::Zero);
	msg.add<SpecialCode>(SpecialCode::Zero);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendOutfitWindow()
{
	const auto& outfits = Outfits::getInstance().getOutfits(player->getSex());
	if (outfits.size() == 0)
	{
		return;
	}

	NetworkMessage msg;
	msg.add(ServerCode::OutfitWindow);

	Outfit_t currentOutfit = player->getDefaultOutfit();
	if (currentOutfit.lookType == 0)
	{
		Outfit_t newOutfit;
		newOutfit.lookType = outfits.front().lookType;
		currentOutfit = newOutfit;
	}

	Mount* currentMount = g_game.mounts.getMountByID(player->getCurrentMount());
	if (currentMount)
	{
		currentOutfit.lookMount = currentMount->clientId;
	}

	AddOutfit(msg, currentOutfit);

	std::vector<ProtocolOutfit> protocolOutfits;
	if (player->isAccessPlayer())
	{
		protocolOutfits.emplace_back("Gamemaster", 75, 0);
	}

	for (const Outfit& outfit : outfits)
	{
		uint8_t addons;
		if (not player->getOutfitAddons(outfit, addons))
		{
			continue;
		}

		protocolOutfits.emplace_back(outfit.name, outfit.lookType, addons);
		if (protocolOutfits.size() == std::numeric_limits<uint8_t>::max())
		{ // Game client currently doesn't allow more than 255 outfits
			break;
		}
	}

	msg.addByte(protocolOutfits.size());
	for (const ProtocolOutfit& outfit : protocolOutfits)
	{
		msg.add<uint16_t>(outfit.lookType);
		msg.addString(outfit.name);
		msg.addByte(outfit.addons);
	}

	std::vector<const Mount*> mounts;
	for (const Mount& mount : g_game.mounts.getMounts())
	{
		if (player->hasMount(&mount))
		{
			mounts.push_back(&mount);
		}
	}

	msg.addByte(mounts.size());
	for (const Mount* mount : mounts)
	{
		msg.add<uint16_t>(mount->clientId);
		msg.addString(mount->name);
	}

	writeToOutputBuffer(msg);
}

void ProtocolGame::sendUpdatedVIPStatus(uint32_t guid, VipStatus_t newStatus)
{
	NetworkMessage msg;
	msg.add(ServerCode::VipStatus);
	msg.add<uint32_t>(guid);
	msg.addByte(newStatus);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendVIP(uint32_t guid, const std::string& name, const std::string& description, uint32_t icon, bool notify, VipStatus_t status)
{
	NetworkMessage msg;
	msg.add(ServerCode::VipEntry);
	msg.add<uint32_t>(guid);
	msg.addString(name);
	msg.addString(description);
	msg.add<uint32_t>(std::min<uint32_t>(10, icon));
	msg.add(notify ? CommonCode::True : CommonCode::False);
	msg.addByte(status);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendVIPEntries()
{
	const std::forward_list<VIPEntry>& vipEntries = IOLoginData::getVIPEntries(player->getAccount());

	for (const VIPEntry& entry : vipEntries)
	{
		VipStatus_t vipStatus = VIPSTATUS_ONLINE;

		const auto& vipPlayer = g_game.getPlayerByGUID(entry.guid);

		if (not vipPlayer or not player->canSeeCreature(vipPlayer))
		{
			vipStatus = VIPSTATUS_OFFLINE;
		}

		sendVIP(entry.guid, entry.name, entry.description, entry.icon, entry.notify, vipStatus);
	}
}

void ProtocolGame::sendSpellCooldown(uint8_t spellId, uint32_t time)
{
	NetworkMessage msg;
	msg.add(ServerCode::SpellCooldown);
	msg.addByte(spellId);
	msg.add<uint32_t>(time);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendSpellGroupCooldown(SpellGroup_t groupId, uint32_t time)
{
	NetworkMessage msg;
	msg.add(ServerCode::SpellGroupCooldown);
	msg.addByte(groupId);
	msg.add<uint32_t>(time);
	writeToOutputBuffer(msg);
}

void ProtocolGame::sendModalWindow(const ModalWindow& modalWindow)
{
	NetworkMessage msg;
	msg.add(ServerCode::ModalWindow);

	msg.add<uint32_t>(modalWindow.id);
	msg.addString(modalWindow.title);
	msg.addString(modalWindow.message);

	msg.addByte(modalWindow.buttons.size());
	for (const auto& it : modalWindow.buttons)
	{
		msg.addString(it.first);
		msg.addByte(it.second);
	}

	msg.addByte(modalWindow.choices.size());
	for (const auto& it : modalWindow.choices)
	{
		msg.addString(it.first);
		msg.addByte(it.second);
	}

	msg.addByte(modalWindow.defaultEscapeButton);
	msg.addByte(modalWindow.defaultEnterButton);
	msg.add(modalWindow.priority ? CommonCode::True : CommonCode::False);

	writeToOutputBuffer(msg);
}

////////////// Add CommonCode messages
void ProtocolGame::AddCreature(NetworkMessage& msg, const CreatureConstPtr& creature, bool known, uint32_t remove)
{
	CreatureType_t creatureType = creature->getType();

	const auto& otherPlayer = creature->getPlayer();

	if (known)
	{
		msg.add<SpecialCode>(SpecialCode::AddKnownCreature);
		msg.add<uint32_t>(creature->getID());
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::AddCreature);
		msg.add<uint32_t>(remove);
		msg.add<uint32_t>(creature->getID());
		msg.addByte(creatureType);
		msg.addString(creature->getName());
	}

	if (creature->isHealthHidden())
	{
		msg.add(CommonCode::Zero);
	}
	else
	{
		// I'm a bit confused how we are sending current healh as a uint8_t. I need to come back and investigate this
		// I suspect this must just account for a "percent", hence the use of the double below, but it could just be for precision.
		msg.addByte(std::ceil((static_cast<double>(creature->getHealth()) / std::max<int32_t>(creature->getMaxHealth(), 1)) * 100));
	}

	msg.addByte(creature->getDirection());

	if (not creature->isInGhostMode() and not creature->isInvisible())
	{
		AddOutfit(msg, creature->getCurrentOutfit());
	}
	else
	{
		static Outfit_t outfit;
		AddOutfit(msg, outfit);
	}

	LightInfo lightInfo = creature->getCreatureLight();
	auto maxLight = static_cast<uint8_t>(255);
	msg.addByte(player->isAccessPlayer() ? maxLight : lightInfo.level);
	msg.addByte(lightInfo.color);

	msg.add<uint16_t>(creature->getStepSpeed() / 2);

	msg.addByte(player->getSkullClient(creature));
	msg.addByte(player->getPartyShield(otherPlayer));

	if (not known)
	{
		msg.addByte(player->getGuildEmblem(otherPlayer));
	}

	if (creatureType == CREATURETYPE_MONSTER)
	{
		if (const auto& master = creature->getMaster())
		{
			if (const auto& masterPlayer = master->getPlayer())
			{
				if (masterPlayer == player)
				{
					creatureType = CREATURETYPE_SUMMON_OWN;
				}
				else
				{
					creatureType = CREATURETYPE_SUMMON_HOSTILE;
				}
			}
		}
	}

	msg.addByte(creatureType); // Type (for summons)
	msg.addByte(creature->getSpeechBubble());
	msg.add(CommonCode::End); // MARK_UNMARKED

	if (otherPlayer)
	{
		msg.add<uint16_t>(otherPlayer->getHelpers());
	}
	else
	{
		msg.add<SpecialCode>(SpecialCode::Zero);
	}

	msg.add(player->canWalkthroughEx(creature) ? CommonCode::False : CommonCode::True);
}

void ProtocolGame::AddPlayerStats(NetworkMessage& msg) const
{
	msg.add(ServerCode::PlayerStats);

	msg.add<uint16_t>(std::min<int32_t>(player->getHealth(), std::numeric_limits<uint16_t>::max()));
	msg.add<uint16_t>(std::min<int32_t>(player->getMaxHealth(), std::numeric_limits<uint16_t>::max()));

	msg.add<uint32_t>(player->getFreeCapacity());
	msg.add<uint32_t>(player->getCapacity());

	msg.add<uint64_t>(player->getExperience());

	msg.add<uint16_t>(player->getLevel());
	msg.addByte(player->getLevelPercent());

	// todo : convert these to ServerSpecial Codes
	msg.add<uint16_t>(100); // base xp gain rate
	msg.add<uint16_t>(0); // xp voucher
	msg.add<uint16_t>(0); // low level bonus
	msg.add<uint16_t>(0); // xp boost
	msg.add<uint16_t>(100); // stamina multiplier (100 = x1.0)

	msg.add<uint16_t>(std::min<int32_t>(player->getMana(), std::numeric_limits<uint16_t>::max()));
	msg.add<uint16_t>(std::min<int32_t>(player->getMaxMana(), std::numeric_limits<uint16_t>::max()));

	msg.addByte(std::min<uint32_t>(player->getMagicLevel(), std::numeric_limits<uint8_t>::max()));
	msg.addByte(std::min<uint32_t>(player->getBaseMagicLevel(), std::numeric_limits<uint8_t>::max()));
	msg.addByte(player->getMagicLevelPercent());

	msg.addByte(player->getSoul());

	msg.add<uint16_t>(player->getStaminaMinutes());

	msg.add<uint16_t>(player->getBaseSpeed() / 2);

	Condition* condition = player->getCondition(CONDITION_REGENERATION, CONDITIONID_DEFAULT);
	msg.add<uint16_t>(condition ? condition->getTicks() / 1000 : 0);

	msg.add<uint16_t>(player->getOfflineTrainingTime() / 60 / 1000);

	msg.add<SpecialCode>(SpecialCode::Zero); // xp boost time (seconds)
	msg.add(CommonCode::Zero); // enables exp boost in the store
}

void ProtocolGame::AddPlayerSkills(NetworkMessage& msg) const
{
	msg.add(ServerCode::PlayerSkills);

	for (uint8_t i = SKILL_FIRST; i <= SKILL_LAST; ++i)
	{
		msg.add<uint16_t>(std::min<int32_t>(player->getSkillLevel(i), std::numeric_limits<uint16_t>::max()));
		msg.add<uint16_t>(player->getBaseSkill(i));
		msg.addByte(player->getSkillPercent(i));
	}

	for (uint8_t i = SPECIALSKILL_FIRST; i <= SPECIALSKILL_LAST; ++i)
	{
		msg.add<uint16_t>(std::min<uint16_t>(player->varSpecialSkills[i], 100));
		msg.add<SpecialCode>(SpecialCode::Zero);
	}
}

void ProtocolGame::AddOutfit(NetworkMessage& msg, const Outfit_t& outfit)
{
	msg.add<uint16_t>(outfit.lookType);

	if (outfit.lookType != 0)
	{
		msg.addByte(outfit.lookHead);
		msg.addByte(outfit.lookBody);
		msg.addByte(outfit.lookLegs);
		msg.addByte(outfit.lookFeet);
		msg.addByte(outfit.lookAddons);
	}
	else
	{
		msg.addItemId(outfit.lookTypeEx);
	}

	msg.add<uint16_t>(outfit.lookMount);
}

void ProtocolGame::AddWorldLight(NetworkMessage& msg, LightInfo lightInfo) const
{
	msg.add(ServerCode::WorldLight);
	auto maxLight = static_cast<uint8_t>(255);
	msg.addByte((player->isAccessPlayer() ? maxLight : lightInfo.level));
	msg.addByte(lightInfo.color);
}

void ProtocolGame::AddCreatureLight(NetworkMessage& msg, const CreatureConstPtr& creature) const
{
	LightInfo lightInfo = creature->getCreatureLight();

	msg.add(ServerCode::CreatureLight);
	msg.add<uint32_t>(creature->getID());
	auto maxLight = static_cast<uint8_t>(255);
	msg.addByte((player->isAccessPlayer() ? maxLight : lightInfo.level));
	msg.addByte(lightInfo.color);
}

//tile
void ProtocolGame::RemoveTileThing(NetworkMessage& msg, const Position& pos, uint32_t stackpos)
{
	if (stackpos >= 10)
	{
		return;
	}

	msg.add(ServerCode::RemoveTileThing);
	msg.addPosition(pos);
	msg.addByte(stackpos);
}

void ProtocolGame::RemoveTileCreature(NetworkMessage& msg, const CreatureConstPtr& creature, const Position& pos, uint32_t stackpos)
{
	if (stackpos < 10)
	{
		RemoveTileThing(msg, pos, stackpos);
	}
	
	msg.add(ServerCode::RemoveTileThing);
	msg.add<SpecialCode>(SpecialCode::End);
	msg.add<uint32_t>(creature->getID());
}

// Todo : The following two methods use far too many magic numbers, eliminate the magic numbers
// and possibly extract the math into it's own function/method with more clarity
void ProtocolGame::MoveUpCreature(NetworkMessage& msg, const CreatureConstPtr& creature, const Position& newPos, const Position& oldPos)
{
	if (creature != player)
	{
		return;
	}

	// floor change up
	msg.add(ServerCode::FloorChangeUp);

	// going to surface
	if (newPos.z == 7)
	{
		int32_t skip = -1;

		// floor 7 and 6 already set
		for (int i = 5; i >= 0; --i)
		{
			GetFloorDescription(msg, oldPos.x - Map::maxClientViewportX, oldPos.y - Map::maxClientViewportY, i, (Map::maxClientViewportX * 2) + 2, (Map::maxClientViewportY * 2) + 2, 8 - i, skip);
		}
		if (skip >= 0)
		{
			msg.addByte(skip);
			msg.add(CommonCode::End);
		}
	}
	// underground, going one floor up (still underground)
	else if (newPos.z > 7)
	{
		int32_t skip = -1;
		GetFloorDescription(msg, oldPos.x - Map::maxClientViewportX, oldPos.y - Map::maxClientViewportY, oldPos.getZ() - 3, (Map::maxClientViewportX * 2) + 2, (Map::maxClientViewportY * 2) + 2, 3, skip);

		if (skip >= 0)
		{
			msg.addByte(skip);
			msg.add(CommonCode::End);
		}
	}

	// moving up a floor up makes us out of sync
	// west
	msg.add(ServerCode::MoveWest);
	GetMapDescription(oldPos.x - Map::maxClientViewportX, oldPos.y - (Map::maxClientViewportY - 1), newPos.z, 1, (Map::maxClientViewportY * 2) + 2, msg);

	// north
	msg.add(ServerCode::MoveNorth);
	GetMapDescription(oldPos.x - Map::maxClientViewportX, oldPos.y - Map::maxClientViewportY, newPos.z, (Map::maxClientViewportX * 2) + 2, 1, msg);
}

void ProtocolGame::MoveDownCreature(NetworkMessage& msg, const CreatureConstPtr& creature, const Position& newPos, const Position& oldPos)
{
	if (creature != player)
	{
		return;
	}

	// floor change down
	msg.add(ServerCode::FloorChangeDown);

	// going from surface to underground
	if (newPos.z == 8)
	{
		int32_t skip = -1;

		for (int i = 0; i < 3; ++i)
		{
			GetFloorDescription(msg, oldPos.x - Map::maxClientViewportX, oldPos.y - Map::maxClientViewportY, newPos.z + i, (Map::maxClientViewportX * 2) + 2, (Map::maxClientViewportY * 2) + 2, -i - 1, skip);
		}
		if (skip >= 0)
		{
			msg.addByte(skip);
			msg.add(CommonCode::Zero);
		}
	}
	// going further down
	else if (newPos.z > oldPos.z and newPos.z > 8 and newPos.z < 14)
	{
		int32_t skip = -1;
		GetFloorDescription(msg, oldPos.x - Map::maxClientViewportX, oldPos.y - Map::maxClientViewportY, newPos.z + 2, (Map::maxClientViewportX * 2) + 2, (Map::maxClientViewportY * 2) + 2, -3, skip);

		if (skip >= 0)
		{
			msg.addByte(skip);
			msg.add(CommonCode::Zero);
		}
	}

	// moving down a floor makes us out of sync
	// east
	msg.add(ServerCode::MoveEast);
	GetMapDescription(oldPos.x + (Map::maxClientViewportX + 1), oldPos.y - (Map::maxClientViewportY + 1), newPos.z, 1, (Map::maxClientViewportY * 2) + 2, msg);

	// south
	msg.add(ServerCode::MoveSouth);
	GetMapDescription(oldPos.x - Map::maxClientViewportX, oldPos.y + (Map::maxClientViewportY + 1), newPos.z, (Map::maxClientViewportX * 2) + 2, 1, msg);
}

void ProtocolGame::AddShopItem(NetworkMessage& msg, const ShopInfo& item)
{
	const ItemType& it = Item::items[item.itemId];
	msg.add<uint16_t>(it.getID());

	if (it.isSplash() or it.isFluidContainer())
	{
		msg.addByte(serverFluidToClient(item.subType));
	}
	else
	{
		msg.add(CommonCode::Zero);
	}

	msg.addString(item.realName);
	msg.add<uint32_t>(it.weight);
	msg.add<uint32_t>(item.buyPrice);
	msg.add<uint32_t>(item.sellPrice);
}

void ProtocolGame::parseExtendedOpcode(NetworkMessage& msg)
{
	uint8_t opcode = msg.getByte();
	auto buffer = msg.getString();

	// process additional opcodes via lua script event
	addGameTask([=, playerID = player->getID(), buffer = std::string{ buffer }]() { g_game.parsePlayerExtendedOpcode(playerID, opcode, buffer); });
}
