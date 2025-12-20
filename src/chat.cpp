// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "chat.h"
#include "game.h"
#include "pugicast.h"
#include "scheduler.h"

#include <fmt/format.h>

extern Chat* g_chat;
extern Game g_game;

bool PrivateChatChannel::isInvited(uint32_t guid) const
{
	if (guid == getOwner()) {
		return true;
	}
	return invites.contains(guid);
}

bool PrivateChatChannel::removeInvite(uint32_t guid)
{
	return invites.erase(guid) != 0;
}

void PrivateChatChannel::invitePlayer(const PlayerConstPtr& player, const PlayerPtr& invitePlayer)
{
	auto result = invites.emplace(invitePlayer->getGUID(), invitePlayer);
	if (!result.second) {
		return;
	}

	invitePlayer->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} invites you to {:s} private chat channel.", player->getName(), player->getSex() == PLAYERSEX_FEMALE ? "her" : "his"));

	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has been invited.", invitePlayer->getName()));

	for (const auto& it : users) {
		it.second->sendChannelEvent(id, invitePlayer->getName(), CHANNELEVENT_INVITE);
	}
}

void PrivateChatChannel::excludePlayer(const PlayerConstPtr& player, const PlayerPtr& excludePlayer)
{
	if (!removeInvite(excludePlayer->getGUID())) {
		return;
	}
	
	removeUser(excludePlayer);

	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has been excluded.", excludePlayer->getName()));

	excludePlayer->sendClosePrivate(id);

	for (const auto& val : users | std::views::values) {
		val->sendChannelEvent(id, excludePlayer->getName(), CHANNELEVENT_EXCLUDE);
	}
}

void PrivateChatChannel::closeChannel() const
{
	for (const auto& val : users | std::views::values) {
		val->sendClosePrivate(id);
	}
}

bool ChatChannel::addUser(const PlayerPtr& player)
{
	if (users.contains(player->getID())) {
		return false;
	}

	if (!executeOnJoinEvent(player)) {
		return false;
	}

	// TODO: Move to script when guild channels can be scripted
	if (id == CHANNEL_GUILD) {
		auto guild = player->getGuild();
		if (guild && !guild->getMotd().empty()) {
			g_scheduler.addEvent(createSchedulerTask(150, [playerID = player->getID()]() { g_game.sendGuildMotd(playerID); }));
		}
	}

	if (!publicChannel) {
		for (const auto& val : users | std::views::values) {
			val->sendChannelEvent(id, player->getName(), CHANNELEVENT_JOIN);
		}
	}

	users[player->getID()] = player;
	return true;
}

bool ChatChannel::removeUser(const PlayerConstPtr& player)
{
	auto iter = users.find(player->getID());
	if (iter == users.end()) {
		return false;
	}

	users.erase(iter);

	if (!publicChannel) {
		for (const auto& val : users | std::views::values) {
			val->sendChannelEvent(id, player->getName(), CHANNELEVENT_LEAVE);
		}
	}

	executeOnLeaveEvent(player);
	return true;
}

bool ChatChannel::hasUser(const PlayerConstPtr& player) const {
	return users.contains(player->getID());
}

void ChatChannel::sendToAll(const std::string& message, SpeakClasses type) const
{
	for (const auto& val : users | std::views::values) {
		val->sendChannelMessage("", message, type, id);
	}
}

bool ChatChannel::talk(const PlayerConstPtr& fromPlayer, SpeakClasses type, const std::string& text)
{
	if (!users.contains(fromPlayer->getID())) {
		return false;
	}

	for (const auto& val : users | std::views::values) {
		val->sendToChannel(fromPlayer, type, text, id);
	}
	return true;
}

bool ChatChannel::executeCanJoinEvent(const PlayerConstPtr& player) const
{
	if (canJoinEvent == -1) {
		return true;
	}

	//canJoin(player)
	LuaScriptInterface* scriptInterface = g_chat->getScriptInterface();
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - CanJoinChannelEvent::execute] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(canJoinEvent, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(canJoinEvent);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	return scriptInterface->callFunction(1);
}

bool ChatChannel::executeOnJoinEvent(const PlayerConstPtr& player) const
{
	if (onJoinEvent == -1) {
		return true;
	}

	//onJoin(player)
	LuaScriptInterface* scriptInterface = g_chat->getScriptInterface();
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - OnJoinChannelEvent::execute] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(onJoinEvent, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(onJoinEvent);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	return scriptInterface->callFunction(1);
}

bool ChatChannel::executeOnLeaveEvent(const PlayerConstPtr& player) const
{
	if (onLeaveEvent == -1) {
		return true;
	}

	//onLeave(player)
	LuaScriptInterface* scriptInterface = g_chat->getScriptInterface();
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - OnLeaveChannelEvent::execute] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(onLeaveEvent, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(onLeaveEvent);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	return scriptInterface->callFunction(1);
}

bool ChatChannel::executeOnSpeakEvent(const PlayerConstPtr& player, SpeakClasses& type, const std::string& message) const
{
	if (onSpeakEvent == -1) {
		return true;
	}

	//onSpeak(player, type, message)
	LuaScriptInterface* scriptInterface = g_chat->getScriptInterface();
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - OnSpeakChannelEvent::execute] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(onSpeakEvent, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();

	scriptInterface->pushFunction(onSpeakEvent);
	LuaScriptInterface::pushSharedPtr(L, player);
	LuaScriptInterface::setMetatable(L, -1, "Player");

	lua_pushinteger(L, type);
	LuaScriptInterface::pushString(L, message);

	bool result = false;
	int size0 = lua_gettop(L);
	int ret = scriptInterface->protectedCall(L, 3, 1);
	if (ret != 0) {
		LuaScriptInterface::reportError(nullptr, LuaScriptInterface::popString(L));
	} else if (lua_gettop(L) > 0) {
		if (lua_isboolean(L, -1)) {
			result = LuaScriptInterface::getBoolean(L, -1);
		} else if (lua_isnumber(L, -1)) {
			result = true;
			type = LuaScriptInterface::getNumber<SpeakClasses>(L, -1);
		}
		lua_pop(L, 1);
	}

	if ((lua_gettop(L) + 4) != size0) {
		LuaScriptInterface::reportError(nullptr, "Stack size changed!");
	}
	scriptInterface->resetScriptEnv();
	return result;
}

Chat::Chat():
	scriptInterface("Chat Interface"),
	dummyPrivate(CHANNEL_PRIVATE, "Private Chat Channel")
{
	scriptInterface.initState();
}

bool Chat::load()
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("data/chatchannels/chatchannels.xml");
	if (!result) {
		printXMLError("Error - Chat::load", "data/chatchannels/chatchannels.xml", result);
		return false;
	}

	for (auto channelNode : doc.child("channels").children()) {
		uint16_t channelId = pugi::cast<uint16_t>(channelNode.attribute("id").value());
		std::string channelName = channelNode.attribute("name").as_string();
		bool isPublic = channelNode.attribute("public").as_bool();
		pugi::xml_attribute scriptAttribute = channelNode.attribute("script");

		auto it = normalChannels.find(channelId);
		if (it != normalChannels.end()) {
			ChatChannel& channel = it->second;
			channel.publicChannel = isPublic;
			channel.name = channelName;

			if (scriptAttribute) {
				if (scriptInterface.loadFile("data/chatchannels/scripts/" + std::string(scriptAttribute.as_string())) == 0) {
					channel.onSpeakEvent = scriptInterface.getEvent("onSpeak");
					channel.canJoinEvent = scriptInterface.getEvent("canJoin");
					channel.onJoinEvent = scriptInterface.getEvent("onJoin");
					channel.onLeaveEvent = scriptInterface.getEvent("onLeave");
				} else {
					std::cout << "[Warning - Chat::load] Can not load script: " << scriptAttribute.as_string() << std::endl;
				}
			}

			UsersMap tempUserMap = std::move(channel.users);
			for (const auto& val : tempUserMap | std::views::values) {
				channel.addUser(val);
			}
			continue;
		}

		ChatChannel channel(channelId, channelName);
		channel.publicChannel = isPublic;

		if (scriptAttribute) {
			if (scriptInterface.loadFile("data/chatchannels/scripts/" + std::string(scriptAttribute.as_string())) == 0) {
				channel.onSpeakEvent = scriptInterface.getEvent("onSpeak");
				channel.canJoinEvent = scriptInterface.getEvent("canJoin");
				channel.onJoinEvent = scriptInterface.getEvent("onJoin");
				channel.onLeaveEvent = scriptInterface.getEvent("onLeave");
			} else {
				std::cout << "[Warning - Chat::load] Can not load script: " << scriptAttribute.as_string() << std::endl;
			}
		}

		normalChannels[channel.id] = channel;
	}
	return true;
}

ChatChannel* Chat::createChannel(const PlayerConstPtr& player, uint16_t channelId)
{
	if (getChannel(player, channelId)) {
		return nullptr;
	}

	switch (channelId) {
		case CHANNEL_GUILD: {
			auto guild = player->getGuild();
			if (guild) {
				auto ret = guildChannels.emplace(std::make_pair(guild->getId(), ChatChannel(channelId, guild->getName())));
				return &ret.first->second;
			}
			break;
		}

		case CHANNEL_PARTY: {
			auto party = player->getParty();
			if (party) {
				auto ret = partyChannels.emplace(std::make_pair(party->getId(), ChatChannel(channelId, "Party")));
				return &ret.first->second;
			}
			break;
		}

		case CHANNEL_PRIVATE: {
			//only 1 private channel for each premium player
			if (!player->isPremium() || getPrivateChannel(player)) {
				return nullptr;
			}

			//find a free private channel slot
			for (uint16_t i = 100; i < 10000; ++i) {
				auto ret = privateChannels.emplace(std::make_pair(i, PrivateChatChannel(i, player->getName() + "'s Channel")));
				if (ret.second) { //second is a bool that indicates that a new channel has been placed in the map
					auto& newChannel = (*ret.first).second;
					newChannel.setOwner(player->getGUID());
					return &newChannel;
				}
			}
			break;
		}

		default:
			break;
	}
	return nullptr;
}

bool Chat::deleteChannel(const PlayerConstPtr& player, uint16_t channelId)
{
	switch (channelId) {
		case CHANNEL_GUILD: {
			auto guild = player->getGuild();
			if (!guild) {
				return false;
			}

			const auto it = guildChannels.find(guild->getId());
			if (it == guildChannels.end()) {
				return false;
			}

			guildChannels.erase(it);
			break;
		}

		case CHANNEL_PARTY: {
			auto party = player->getParty();
			if (!party) {
				return false;
			}

			const auto it = partyChannels.find(party->getId());
			if (it == partyChannels.end()) {
				return false;
			}

			partyChannels.erase(it);
			break;
		}

		default: {
			const auto it = privateChannels.find(channelId);
			if (it == privateChannels.end()) {
				return false;
			}

			it->second.closeChannel();

			privateChannels.erase(it);
			break;
		}
	}
	return true;
}

ChatChannel* Chat::addUserToChannel(const PlayerPtr& player, uint16_t channelId)
{
	ChatChannel* channel = getChannel(player, channelId);
	if (channel && channel->addUser(player)) {
		return channel;
	}
	return nullptr;
}

bool Chat::removeUserFromChannel(const PlayerConstPtr& player, uint16_t channelId)
{
	ChatChannel* channel = getChannel(player, channelId);
	if (!channel || !channel->removeUser(player)) {
		return false;
	}

	if (channel->getOwner() == player->getGUID()) {
		deleteChannel(player, channelId);
	}
	return true;
}

void Chat::removeUserFromAllChannels(const PlayerConstPtr& player)
{
	for (auto& val : normalChannels | std::views::values) {
		val.removeUser(player);
	}

	for (auto& val : partyChannels | std::views::values) {
		val.removeUser(player);
	}

	for (auto& val : guildChannels | std::views::values) {
		val.removeUser(player);
	}

	auto it = privateChannels.begin();
	while (it != privateChannels.end()) {
		PrivateChatChannel* channel = &it->second;
		channel->removeInvite(player->getGUID());
		channel->removeUser(player);
		if (channel->getOwner() == player->getGUID()) {
			channel->closeChannel();
			it = privateChannels.erase(it);
		} else {
			++it;
		}
	}
}

bool Chat::talkToChannel(const PlayerConstPtr& player, SpeakClasses type, const std::string& text, uint16_t channelId)
{
	const auto channel = getChannel(player, channelId);
	if (!channel) {
		return false;
	}

	if (channelId == CHANNEL_GUILD) {
		GuildRank_ptr rank = player->getGuildRank();
		if (rank && rank->level > 1) {
			type = TALKTYPE_CHANNEL_O;
		} else if (type != TALKTYPE_CHANNEL_Y) {
			type = TALKTYPE_CHANNEL_Y;
		}
	} else if (channelId == CHANNEL_PRIVATE || channelId == CHANNEL_PARTY) {
		type = TALKTYPE_CHANNEL_Y;
	}

	if (!channel->executeOnSpeakEvent(player, type, text)) {
		return false;
	}

	return channel->talk(player, type, text);
}

ChannelList Chat::getChannelList(const PlayerConstPtr& player)
{
	ChannelList list;
	if (player->getGuild()) {
		if (auto channel = getChannel(player, CHANNEL_GUILD)) {
			list.push_back(channel);
		} else {
			channel = createChannel(player, CHANNEL_GUILD);
			if (channel) {
				list.push_back(channel);
			}
		}
	}

	if (player->getParty()) {
		if (auto channel = getChannel(player, CHANNEL_PARTY)) {
			list.push_back(channel);
		} else {
			channel = createChannel(player, CHANNEL_PARTY);
			if (channel) {
				list.push_back(channel);
			}
		}
	}

	for (const auto& key : normalChannels | std::views::keys) {
		if (auto channel = getChannel(player, key)) {
			list.push_back(channel);
		}
	}

	bool hasPrivate = false;
	for (auto& val : privateChannels | std::views::values) {
		if (const auto channel = &val) {
			uint32_t guid = player->getGUID();
			if (channel->isInvited(guid)) {
				list.push_back(channel);
			}

			if (channel->getOwner() == guid) {
				hasPrivate = true;
			}
		}
	}

	if (!hasPrivate && player->isPremium()) {
		list.push_front(&dummyPrivate);
	}
	return list;
}

ChatChannel* Chat::getChannel(const PlayerConstPtr& player, uint16_t channelId)
{
	switch (channelId) {
		case CHANNEL_GUILD: {
			if (auto guild = player->getGuild()) {
				const auto it = guildChannels.find(guild->getId());
				if (it != guildChannels.end()) {
					return &it->second;
				}
			}
			break;
		}

		case CHANNEL_PARTY: {
			if (auto party = player->getParty()) {
				const auto it = partyChannels.find(party->getId());
				if (it != partyChannels.end()) {
					return &it->second;
				}
			}
			break;
		}

		default: {
			const auto it = normalChannels.find(channelId);
			if (it != normalChannels.end()) {
				ChatChannel& channel = it->second;
				if (!channel.executeCanJoinEvent(player)) {
					return nullptr;
				}
				return &channel;
			} else {
				auto it2 = privateChannels.find(channelId);
				if (it2 != privateChannels.end() && it2->second.isInvited(player->getGUID())) {
					return &it2->second;
				}
			}
			break;
		}
	}
	return nullptr;
}

ChatChannel* Chat::getGuildChannelById(uint32_t guildId)
{
	auto it = guildChannels.find(guildId);
	if (it == guildChannels.end()) {
		return nullptr;
	}
	return &it->second;
}

ChatChannel* Chat::getChannelById(uint16_t channelId)
{
	auto it = normalChannels.find(channelId);
	if (it == normalChannels.end()) {
		return nullptr;
	}
	return &it->second;
}

PrivateChatChannel* Chat::getPrivateChannel(const PlayerConstPtr& player)
{
	for (auto& it : privateChannels) {
		if (it.second.getOwner() == player->getGUID()) {
			return &it.second;
		}
	}
	return nullptr;
}
