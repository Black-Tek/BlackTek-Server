// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "party.h"
#include "game.h"
#include "configmanager.h"
#include "events.h"

#include <fmt/format.h>

extern Game g_game;
extern ConfigManager g_config;
extern Events* g_events;

uint32_t _total_parties_created = 0;
std::unordered_map<uint32_t, PartyPtr> _game_parties;

Party::Party() : id(0)
{

}

Party::Party(const PlayerPtr& leader) :  leader(leader), id(++_total_parties_created)
{

}

PartyPtr Party::get(uint32_t id)
{
	auto it = _game_parties.find(id);

	if (it != _game_parties.end())
		return it->second;

	return nullptr;
}

PartyPtr Party::make(const PlayerPtr& player)
{
	auto new_party = std::make_shared<Party>(player);
	_game_parties.try_emplace(new_party->getId(), new_party);
	return new_party;
}

void Party::disband()
{
	const auto& self = shared_from_this();

	if (not g_events->eventPartyOnDisband(self))
	{
		return;
	}

	for (const auto& invitee : inviteList)
	{
		invitee->removePartyInvitation(self);
		leader->sendCreatureShield(invitee);
	}

	for (const auto& member : memberList)
	{
		member->setParty(0);
		member->sendClosePrivate(CHANNEL_PARTY);
		member->sendTextMessage(MESSAGE_INFO_DESCR, "Your party has been disbanded.");
	}

	for (const auto& member : memberList)
	{
		g_game.updatePlayerShield(member);

		for (const auto& otherMember : memberList)
		{
			otherMember->sendCreatureSkull(member);
		}

		member->sendCreatureSkull(leader);
		leader->sendCreatureSkull(member);
		g_game.updatePlayerHelpers(member);
	}

	
	leader->setParty(0);
	leader->sendClosePrivate(CHANNEL_PARTY);
	g_game.updatePlayerShield(leader);
	g_game.updatePlayerHelpers(leader);
	leader->sendCreatureSkull(leader);
	leader->sendTextMessage(MESSAGE_INFO_DESCR, "Your party has been disbanded.");
	leader = nullptr;
}

bool Party::leaveParty(const PlayerPtr& player, bool forceRemove /* = false */)
{
	if (not player or player->getPartyId() == 0)
		return false;

	bool canRemove = g_events->eventPartyOnLeave(shared_from_this(), player);

	if (not forceRemove and not canRemove)
		return false;

	bool missingLeader = false;
	if (leader == player)
	{
		if (not memberList.empty())
		{
			if (memberList.size() == 1 and inviteList.empty())
			{
				missingLeader = true;
			} 
			else
			{
				passPartyLeadership(memberList.front(), true);
			}
		} 
		else
		{
			missingLeader = true;
		}
	}

	//since we already passed the leadership, we remove the player from the list
	if (const auto& it = std::ranges::find(memberList, player); it != memberList.end())
		memberList.erase(it);

	player->setParty(0);
	player->sendClosePrivate(CHANNEL_PARTY);
	g_game.updatePlayerShield(player);
	g_game.updatePlayerHelpers(player);

	for (const auto& member : memberList)
	{
		member->sendCreatureSkull(player);
		player->sendPlayerPartyIcons(member);
		g_game.updatePlayerHelpers(member);
	}

	leader->sendCreatureSkull(player);
	player->sendCreatureSkull(player);
	player->sendPlayerPartyIcons(leader);

	player->sendTextMessage(MESSAGE_INFO_DESCR, "You have left the party.");

	updateSharedExperience();

	clearPlayerPoints(player);

	broadcastPartyMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has left the party.", player->getName()));

	if (missingLeader or empty())
		disband();

	return true;
}
bool Party::passPartyLeadership(const PlayerPtr& player, bool forceRemove /* = false*/)
{
	if (not player or not leader or leader == player or player->getPartyId() == 0) 
		return false;

	if (not g_events->eventPartyOnPassLeadership(shared_from_this(), player) and not forceRemove)
		return false;

	// Remove it before to broadcast the message correctly
	if (const auto it = std::ranges::find(memberList, player); it != memberList.end())
	{
		it->reset();
		memberList.erase(it);
	}


	broadcastPartyMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} is now the leader of the party.", player->getName()), true);

	PlayerPtr oldLeader = leader;
	leader = player;
	memberList.insert(memberList.begin(), oldLeader);

	updateSharedExperience();

	for (const auto& member : memberList)
	{
		member->sendCreatureShield(oldLeader);
		member->sendCreatureShield(leader);
	}

	for (const auto& invitee : inviteList)
	{
		invitee->sendCreatureShield(oldLeader);
		invitee->sendCreatureShield(leader);
	}

	leader->sendCreatureShield(oldLeader);
	leader->sendCreatureShield(leader);

	player->sendTextMessage(MESSAGE_INFO_DESCR, "You are now the leader of the party.");
	return true;
}

bool Party::joinParty(const PlayerPtr& player)
{
	if (not player or player->getPartyId() != 0)	
        return false;

	auto max_party_size = g_config.getNumber(ConfigManager::MAXIMUM_PARTY_SIZE);

	if (memberList.size() >= max_party_size)
		return false;

	if (not g_events->eventPartyOnJoin(shared_from_this(), player))
		return false;

	const auto it = std::find(inviteList.begin(), inviteList.end(), player);

	if (it == inviteList.end())
		return false;

	inviteList.erase(it);

	broadcastPartyMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has joined the party.", player->getName()));

	player->setParty(id);

	g_game.updatePlayerShield(player);

	for (const auto& member : memberList)
	{
		member->sendCreatureSkull(player);
		player->sendPlayerPartyIcons(member);
	}

	player->sendCreatureSkull(player);
	leader->sendCreatureSkull(player);
	player->sendPlayerPartyIcons(leader);

	memberList.push_back(player);

	g_game.updatePlayerHelpers(player);

	player->removePartyInvitation(shared_from_this());
	updateSharedExperience();

	const std::string& leaderName = leader->getName();
	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("You have joined {:s}'{:s} party. Open the party channel to communicate with your companions.", leaderName, leaderName.back() == 's' ? "" : "s"));
	return true;
}

bool Party::removeInvite(const PlayerPtr& player, bool removeFromPlayer/* = true*/)
{
	const auto it = std::find(inviteList.begin(), inviteList.end(), player);

	if (it == inviteList.end())
	{
		return false;
	}

	inviteList.erase(it);
	leader->sendCreatureShield(player);
	player->sendCreatureShield(leader);

	if (removeFromPlayer)
	{
		player->removePartyInvitation(shared_from_this());
	}

	if (empty())
	{
		disband();
	} 
	else
	{
		for (const auto& member : memberList)
		{
			g_game.updatePlayerHelpers(member);
		}
		g_game.updatePlayerHelpers(leader);
	}

	return true;
}

void Party::revokeInvitation(const PlayerPtr& player)
{
	if (not g_events->eventPartyOnRevokeInvitation(shared_from_this(), player))
	{
		return;
	}
	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has revoked {:s} invitation.", leader->getName(), leader->getSex() == PLAYERSEX_FEMALE ? "her" : "his"));
	leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("Invitation for {:s} has been revoked.", player->getName()));
	removeInvite(player);
}

bool Party::invitePlayer(const PlayerPtr& player)
{
	if (not player)
        return false;

	if (auto party = player->getParty();
		party and party->id != 0)
	{
		leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} is already in a party.", player->getName()));
	}

	if (isPlayerInvited(player))
		return false;

	auto max_invites = g_config.getNumber(ConfigManager::MAXIMUM_INVITE_COUNT);

	if (inviteList.size() >= max_invites)
	{
		leader->sendTextMessage(MESSAGE_INFO_DESCR, "Your cannot invite more players!");
		return false;
	}

	if (empty())
	{
		leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has been invited. Open the party channel to communicate with your members.", player->getName()));
		g_game.updatePlayerShield(leader);
		leader->sendCreatureSkull(leader);
	} 
	else 
	{
		leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has been invited.", player->getName()));
	}

	inviteList.push_back(player);


	for (const auto& member : memberList)
	{
		g_game.updatePlayerHelpers(member);
	}

	g_game.updatePlayerHelpers(leader);

	leader->sendCreatureShield(player);
	player->sendCreatureShield(leader);

	player->addPartyInvitation(shared_from_this());

	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has invited you to {:s} party.", leader->getName(), leader->getSex() == PLAYERSEX_FEMALE ? "her" : "his"));
	return true;
}

bool Party::isPlayerInvited(const PlayerConstPtr& player) const
{
	if (not player)
		return false;

	return std::ranges::find(inviteList, player) != inviteList.end();
}

void Party::updateAllPartyIcons()
{
	for (const auto& member : memberList)
	{
		for (const auto& otherMember : memberList)
		{
			member->sendCreatureShield(otherMember);
		}

		member->sendCreatureShield(leader);
		leader->sendCreatureShield(member);
	}
	leader->sendCreatureShield(leader);
}

void Party::broadcastPartyMessage(MessageClasses msgClass, const std::string& msg, bool sendToInvitations /*= false*/) const
{
	for (const auto& member : memberList)
	{
		member->sendTextMessage(msgClass, msg);
	}

	leader->sendTextMessage(msgClass, msg);

	if (sendToInvitations)
	{
		for (const auto& invitee : inviteList)
		{
			invitee->sendTextMessage(msgClass, msg);
		}
	}
}

void Party::updateSharedExperience()
{
	if (sharedExpActive)
	{
		bool result = getSharedExperienceStatus() == SHAREDEXP_OK;
		if (result != sharedExpEnabled)
		{
			sharedExpEnabled = result;
			updateAllPartyIcons();
		}
	}
}

namespace { // todo : why is this needed?

	const char* getSharedExpReturnMessage(SharedExpStatus_t value)
	{
		switch (value) 
		{
			case SHAREDEXP_OK:
				return "Shared Experience is now active.";
			case SHAREDEXP_TOOFARAWAY:
				return "Shared Experience has been activated, but some members of your party are too far away.";
			case SHAREDEXP_LEVELDIFFTOOLARGE:
				return "Shared Experience has been activated, but the level spread of your party is too wide.";
			case SHAREDEXP_MEMBERINACTIVE:
				return "Shared Experience has been activated, but some members of your party are inactive.";
			case SHAREDEXP_EMPTYPARTY:
				return "Shared Experience has been activated, but you are alone in your party.";
			default:
				return "An error occured. Unable to activate shared experience.";
		}
	}

}

bool Party::setSharedExperience(const PlayerPtr& player, bool sharedExpActive)
{
	if (not player or leader != player or player->getPartyId() == 0) 
		return false;

	if (this->sharedExpActive == sharedExpActive)
		return true;

	this->sharedExpActive = sharedExpActive;

	if (sharedExpActive) {
		const SharedExpStatus_t sharedExpStatus = getSharedExperienceStatus();
		this->sharedExpEnabled = sharedExpStatus == SHAREDEXP_OK;
		leader->sendTextMessage(MESSAGE_INFO_DESCR, getSharedExpReturnMessage(sharedExpStatus));
	} else {
		leader->sendTextMessage(MESSAGE_INFO_DESCR, "Shared Experience has been deactivated.");
	}

	updateAllPartyIcons();
	return true;
}

void Party::shareExperience(const uint64_t experience, const CreaturePtr& source/* = nullptr*/)
{
	uint64_t shareExperience = experience;
	g_events->eventPartyOnShareExperience(shared_from_this(), shareExperience);

	for (const auto& member : memberList)
	{
		member->onGainSharedExperience(shareExperience, source);
	}
	leader->onGainSharedExperience(shareExperience, source);
}

bool Party::canUseSharedExperience(const PlayerConstPtr& player) const
{
	return getMemberSharedExperienceStatus(player) == SHAREDEXP_OK;
}

SharedExpStatus_t Party::getMemberSharedExperienceStatus(const PlayerConstPtr& player) const
{
	if (memberList.empty())
		return SHAREDEXP_EMPTYPARTY;

	uint32_t highestLevel = leader->getLevel();
	for (const auto& member : memberList)
	{
		if (member->getLevel() > highestLevel)
		{
			highestLevel = member->getLevel();
		}
	}

	uint32_t minLevel = static_cast<uint32_t>(std::ceil((static_cast<float>(highestLevel) * 2) / 3));

	if (player->getLevel() < minLevel)
		return SHAREDEXP_LEVELDIFFTOOLARGE;

	const auto share_range = g_config.getNumber(ConfigManager::PARTY_EXP_SHARE_RANGE);
	const auto floor_range = g_config.getNumber(ConfigManager::PARTY_EXP_SHARE_FLOORS);

	if (not Position::areInRange(leader->getPosition(), player->getPosition(), share_range, share_range, floor_range))
		return SHAREDEXP_TOOFARAWAY;

	if (not player->hasFlag(PlayerFlag_NotGainInFight))
	{
		//check if the player has healed/attacked anything recently
		const auto it = ticksMap.find(player->getID());
		if (it == ticksMap.end())
		{
			return SHAREDEXP_MEMBERINACTIVE;
		}

		uint64_t timeDiff = OTSYS_TIME() - it->second;
		if (timeDiff > static_cast<uint64_t>(g_config.getNumber(ConfigManager::PZ_LOCKED)))
		{
			return SHAREDEXP_MEMBERINACTIVE;
		}
	}
	return SHAREDEXP_OK;
}

SharedExpStatus_t Party::getSharedExperienceStatus() const
{
	SharedExpStatus_t leaderStatus = getMemberSharedExperienceStatus(leader);
	if (leaderStatus != SHAREDEXP_OK)
	{
		return leaderStatus;
	}

	for (const auto& member : memberList)
	{
		SharedExpStatus_t memberStatus = getMemberSharedExperienceStatus(member);
		if (memberStatus != SHAREDEXP_OK)
		{
			return memberStatus;
		}
	}
	return SHAREDEXP_OK;
}

void Party::updatePlayerTicks(const PlayerPtr& player, const uint32_t points)
{
	if (not player or player->getPartyId() == 0)
        return;

	if (points != 0 and not player->hasFlag(PlayerFlag_NotGainInFight))
	{
		ticksMap[player->getID()] = OTSYS_TIME();
		updateSharedExperience();
	}
}

void Party::clearPlayerPoints(const PlayerPtr& player)
{
	if (not player or player->getPartyId() == 0)
        return;

	if (const auto it = ticksMap.find(player->getID()); it != ticksMap.end())
	{
		ticksMap.erase(it);
		updateSharedExperience();
	}
}

bool Party::canOpenCorpse(const uint32_t ownerId) const
{

	if (const auto& player = g_game.getPlayerByID(ownerId))
	{
		return leader->getID() == ownerId or player->getPartyId() == id;
	}
	return false;
}
