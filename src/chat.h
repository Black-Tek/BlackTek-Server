/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Upstream Copyright:
 *   Copyright (C) The Forgotten Server Authors
 *   Governed by the GNU General Public License v2.0 (GPL-2.0)
 *
 * Modifications & BlackTek Core Additions:
 *   Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * ============================================================================
 * DUAL LICENSING & TRANSITION NOTICE
 * ============================================================================
 *
 * This file contains original code from The Forgotten Server alongside substantial
 * modifications and new architecture by BlackTek.
 *
 * To facilitate the project's long-term goal of transitioning the codebase to a
 * fully permissive MIT License:
 *
 * 1. All original TFS source lines remain governed by GPL-2.0.
 * 2. All contributions, modifications, and new additions authored by BlackTek
 *    are dual-licensed under both the MIT License and GPL-2.0.
 * 3. Downstream users distributing this combined codebase must comply with
 *    the GPL-2.0 requirements as long as original upstream TFS code remains.
 *
 * Once an individual module or file has been completely refactored or rewritten
 * such that no original TFS code remains, the file will be relicensed solely
 * under the MIT License.
 *
 * See licensing/original codebase/GNU-GPL (GPL-2.0) and licensing/License (MIT)
 * in the project root for the full texts.
 */

#ifndef FS_CHAT_H
#define FS_CHAT_H

#include "const.h"
#include "luascript.h"

class Player;

using UsersMap = std::map<uint32_t, PlayerPtr>;
using InvitedMap = std::map<uint32_t, PlayerConstPtr>;

class ChatChannel
{
	public:
		ChatChannel() = default;
		ChatChannel(uint16_t channelId, std::string channelName):
			id{channelId}, name{std::move(channelName)} {}

		virtual ~ChatChannel() = default;

		bool addUser(const PlayerPtr& player);
		bool removeUser(const PlayerConstPtr& player);
		bool hasUser( const PlayerConstPtr& player) const;

		bool talk(const PlayerConstPtr& fromPlayer, SpeakClasses type, const std::string& text);
		void sendToAll(const std::string& message, SpeakClasses type) const;

		const std::string& getName() const {
			return name;
		}
	
		uint16_t getId() const {
			return id;
		}
	
		const UsersMap& getUsers() const {
			return users;
		}
	
		virtual const InvitedMap* getInvitedUsers() const {
			return nullptr;
		}

		virtual uint32_t getOwner() const {
			return 0;
		}

		bool isPublicChannel() const { return publicChannel; }

		bool executeOnJoinEvent(const PlayerConstPtr& player) const;
		bool executeCanJoinEvent(const PlayerConstPtr& player) const;
		bool executeOnLeaveEvent(const PlayerConstPtr& player) const;
		bool executeOnSpeakEvent(const PlayerConstPtr& player, SpeakClasses& type, const std::string& message) const;

	protected:
		UsersMap users;

		uint16_t id;

	private:
		std::string name;

		int32_t canJoinEvent = -1;
		int32_t onJoinEvent = -1;
		int32_t onLeaveEvent = -1;
		int32_t onSpeakEvent = -1;

		bool publicChannel = false;

	friend class Chat;
};

class PrivateChatChannel final : public ChatChannel
{
	public:
		PrivateChatChannel(uint16_t channelId, const std::string& channelName) : ChatChannel(channelId, channelName) {}

		uint32_t getOwner() const override {
			return owner;
		}
	
		void setOwner(uint32_t owner) {
			this->owner = owner;
		}

		bool isInvited(uint32_t guid) const;

		void invitePlayer(const PlayerConstPtr& player, const PlayerPtr& invitePlayer);
		void excludePlayer(const PlayerConstPtr& player, const PlayerPtr& excludePlayer);

		bool removeInvite(uint32_t guid);

		void closeChannel() const;

		const InvitedMap* getInvitedUsers() const override {
			return &invites;
		}

	private:
		InvitedMap invites;
		uint32_t owner = 0;
};

using ChannelList = std::list<ChatChannel*>;

class Chat
{
	public:
		Chat();

		// non-copyable
		Chat(const Chat&) = delete;
		Chat& operator=(const Chat&) = delete;

		bool load();

		ChatChannel* createChannel(const PlayerConstPtr& player, uint16_t channelId);
		bool deleteChannel(const PlayerConstPtr& player, uint16_t channelId);

		ChatChannel* addUserToChannel(const PlayerPtr& player, uint16_t channelId);
		bool removeUserFromChannel(const PlayerConstPtr& player, uint16_t channelId);
		void removeUserFromAllChannels(const PlayerConstPtr& player);

		bool talkToChannel(const PlayerConstPtr& player, SpeakClasses type, const std::string& text, uint16_t channelId);

		ChannelList getChannelList(const PlayerConstPtr& player);

		ChatChannel* getChannel(const PlayerConstPtr& player, uint16_t channelId);
		ChatChannel* getChannelById(uint16_t channelId);
		ChatChannel* getGuildChannelById(uint32_t guildId);
		PrivateChatChannel* getPrivateChannel(const PlayerConstPtr& player);

		LuaScriptInterface* getScriptInterface() {
			return &scriptInterface;
		}

	private:
		std::map<uint16_t, ChatChannel> normalChannels;
		std::map<uint16_t, PrivateChatChannel> privateChannels;
		std::map<uint32_t, ChatChannel> partyChannels;
		std::map<uint32_t, ChatChannel> guildChannels;

		LuaScriptInterface scriptInterface;

		PrivateChatChannel dummyPrivate;
};

#endif
