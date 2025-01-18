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

Party::Party(const PlayerPtr& leader) : leader(leader)
{
	leader->setParty(this);
}

void Party::disband()
{
	if (!g_events->eventPartyOnDisband(this)) {
		return;
	}

	PlayerPtr currentLeader = leader;
	leader = nullptr;

	currentLeader->setParty(nullptr);
	currentLeader->sendClosePrivate(CHANNEL_PARTY);
	g_game.updatePlayerShield(currentLeader);
	g_game.updatePlayerHelpers(currentLeader);
	currentLeader->sendCreatureSkull(currentLeader);
	currentLeader->sendTextMessage(MESSAGE_INFO_DESCR, "Your party has been disbanded.");

	for (const auto& invitee : inviteList) {
		invitee->removePartyInvitation(this);
		currentLeader->sendCreatureShield(invitee);
	}
	inviteList.clear();

	for (const auto& member : memberList) {
		member->setParty(nullptr);
		member->sendClosePrivate(CHANNEL_PARTY);
		member->sendTextMessage(MESSAGE_INFO_DESCR, "Your party has been disbanded.");
	}

	for (const auto& member : memberList) {
		g_game.updatePlayerShield(member);

		for (const auto& otherMember : memberList) {
			otherMember->sendCreatureSkull(member);
		}

		member->sendCreatureSkull(currentLeader);
		currentLeader->sendCreatureSkull(member);
		g_game.updatePlayerHelpers(member);
	}
	memberList.clear();
	delete this;
}

bool Party::leaveParty(const PlayerPtr& player, bool forceRemove /* = false */)
{
	if (!player) {
		return false;
	}

	if (player->getParty() != this && leader != player) {
		return false;
	}

	bool canRemove = g_events->eventPartyOnLeave(this, player);
	if (!forceRemove && !canRemove) {
		return false;
	}

	bool missingLeader = false;
	if (leader == player) {
		if (!memberList.empty()) {
			if (memberList.size() == 1 && inviteList.empty()) {
				missingLeader = true;
			} else {
				passPartyLeadership(memberList.front(), true);
			}
		} else {
			missingLeader = true;
		}
	}

	//since we already passed the leadership, we remove the player from the list
	if (const auto& it = std::ranges::find(memberList, player); it != memberList.end()) {
		memberList.erase(it);
	}

	player->setParty(nullptr);
	player->sendClosePrivate(CHANNEL_PARTY);
	g_game.updatePlayerShield(player);
	g_game.updatePlayerHelpers(player);

	for (const auto& member : memberList) {
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

	if (missingLeader || empty()) {
		disband();
	}

	return true;
}
bool Party::passPartyLeadership(const PlayerPtr& player, bool forceRemove /* = false*/)
{
	if (!player || leader == player || player->getParty() != this) {
		return false;
	}

	if (!g_events->eventPartyOnPassLeadership(this, player) && !forceRemove) {
		return false;
	}

	//Remove it before to broadcast the message correctly
	if (const auto it = std::ranges::find(memberList, player); it != memberList.end()) {
		memberList.erase(it);
	}

	broadcastPartyMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} is now the leader of the party.", player->getName()), true);

	PlayerPtr oldLeader = leader;
	leader = player;

	memberList.insert(memberList.begin(), oldLeader);

	updateSharedExperience();

	for (const auto& member : memberList) {
		member->sendCreatureShield(oldLeader);
		member->sendCreatureShield(leader);
	}

	for (const auto& invitee : inviteList) {
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
	if (!g_events->eventPartyOnJoin(this, player)) {
		return false;
	}

	const auto it = std::find(inviteList.begin(), inviteList.end(), player);
	if (it == inviteList.end()) {
		return false;
	}

	inviteList.erase(it);

	broadcastPartyMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has joined the party.", player->getName()));

	player->setParty(this);

	g_game.updatePlayerShield(player);

	for (const auto& member : memberList) {
		member->sendCreatureSkull(player);
		player->sendPlayerPartyIcons(member);
	}

	player->sendCreatureSkull(player);
	leader->sendCreatureSkull(player);
	player->sendPlayerPartyIcons(leader);

	memberList.push_back(player);

	g_game.updatePlayerHelpers(player);

	player->removePartyInvitation(this);
	updateSharedExperience();

	const std::string& leaderName = leader->getName();
	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("You have joined {:s}'{:s} party. Open the party channel to communicate with your companions.", leaderName, leaderName.back() == 's' ? "" : "s"));
	return true;
}

bool Party::removeInvite(const PlayerPtr& player, bool removeFromPlayer/* = true*/)
{
	const auto it = std::find(inviteList.begin(), inviteList.end(), player);
	if (it == inviteList.end()) {
		return false;
	}

	inviteList.erase(it);

	leader->sendCreatureShield(player);
	player->sendCreatureShield(leader);

	if (removeFromPlayer) {
		player->removePartyInvitation(this);
	}

	if (empty()) {
		disband();
	} else {
		for (const auto& member : memberList) {
			g_game.updatePlayerHelpers(member);
		}

		g_game.updatePlayerHelpers(leader);
	}

	return true;
}

void Party::revokeInvitation(const PlayerPtr& player)
{
	if (!g_events->eventPartyOnRevokeInvitation(this, player)) {
		return;
	}
	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has revoked {:s} invitation.", leader->getName(), leader->getSex() == PLAYERSEX_FEMALE ? "her" : "his"));
	leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("Invitation for {:s} has been revoked.", player->getName()));
	removeInvite(player);
}

bool Party::invitePlayer(const PlayerPtr& player)
{
	if (isPlayerInvited(player)) {
		return false;
	}

	if (empty()) {
		leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has been invited. Open the party channel to communicate with your members.", player->getName()));
		g_game.updatePlayerShield(leader);
		leader->sendCreatureSkull(leader);
	} else {
		leader->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has been invited.", player->getName()));
	}

	inviteList.push_back(player);

	for (const auto& member : memberList) {
		g_game.updatePlayerHelpers(member);
	}
	g_game.updatePlayerHelpers(leader);

	leader->sendCreatureShield(player);
	player->sendCreatureShield(leader);

	player->addPartyInvitation(this);

	player->sendTextMessage(MESSAGE_INFO_DESCR, fmt::format("{:s} has invited you to {:s} party.", leader->getName(), leader->getSex() == PLAYERSEX_FEMALE ? "her" : "his"));
	return true;
}

bool Party::isPlayerInvited(const PlayerConstPtr& player) const
{
	return std::ranges::find(inviteList, player) != inviteList.end();
}

void Party::updateAllPartyIcons()
{
	for (const auto& member : memberList) {
		for (const auto& otherMember : memberList) {
			member->sendCreatureShield(otherMember);
		}

		member->sendCreatureShield(leader);
		leader->sendCreatureShield(member);
	}
	leader->sendCreatureShield(leader);
}

void Party::broadcastPartyMessage(MessageClasses msgClass, const std::string& msg, bool sendToInvitations /*= false*/) const
{
	for (const auto& member : memberList) {
		member->sendTextMessage(msgClass, msg);
	}

	leader->sendTextMessage(msgClass, msg);

	if (sendToInvitations) {
		for (const auto& invitee : inviteList) {
			invitee->sendTextMessage(msgClass, msg);
		}
	}
}

void Party::updateSharedExperience()
{
	if (sharedExpActive) {
		bool result = getSharedExperienceStatus() == SHAREDEXP_OK;
		if (result != sharedExpEnabled) {
			sharedExpEnabled = result;
			updateAllPartyIcons();
		}
	}
}

namespace {

const char* getSharedExpReturnMessage(SharedExpStatus_t value)
{
	switch (value) {
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
	if (!player || leader != player) {
		return false;
	}

	if (this->sharedExpActive == sharedExpActive) {
		return true;
	}

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
	g_events->eventPartyOnShareExperience(this, shareExperience);

	for (const auto& member : memberList) {
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
	if (memberList.empty()) {
		return SHAREDEXP_EMPTYPARTY;
	}

	uint32_t highestLevel = leader->getLevel();
	for (const auto& member : memberList) {
		if (member->getLevel() > highestLevel) {
			highestLevel = member->getLevel();
		}
	}

	uint32_t minLevel = static_cast<uint32_t>(std::ceil((static_cast<float>(highestLevel) * 2) / 3));
	if (player->getLevel() < minLevel) {
		return SHAREDEXP_LEVELDIFFTOOLARGE;
	}

	if (!Position::areInRange<EXPERIENCE_SHARE_RANGE, EXPERIENCE_SHARE_RANGE, EXPERIENCE_SHARE_FLOORS>(leader->getPosition(), player->getPosition())) {
		return SHAREDEXP_TOOFARAWAY;
	}

	if (!player->hasFlag(PlayerFlag_NotGainInFight)) {
		//check if the player has healed/attacked anything recently
		const auto it = ticksMap.find(player->getID());
		if (it == ticksMap.end()) {
			return SHAREDEXP_MEMBERINACTIVE;
		}

		uint64_t timeDiff = OTSYS_TIME() - it->second;
		if (timeDiff > static_cast<uint64_t>(g_config.getNumber(ConfigManager::PZ_LOCKED))) {
			return SHAREDEXP_MEMBERINACTIVE;
		}
	}
	return SHAREDEXP_OK;
}

SharedExpStatus_t Party::getSharedExperienceStatus() const
{
	SharedExpStatus_t leaderStatus = getMemberSharedExperienceStatus(leader);
	if (leaderStatus != SHAREDEXP_OK) {
		return leaderStatus;
	}

	for (const auto& member : memberList) {
		SharedExpStatus_t memberStatus = getMemberSharedExperienceStatus(member);
		if (memberStatus != SHAREDEXP_OK) {
			return memberStatus;
		}
	}
	return SHAREDEXP_OK;
}

void Party::updatePlayerTicks(const PlayerPtr& player, const uint32_t points)
{
	if (points != 0 && !player->hasFlag(PlayerFlag_NotGainInFight)) {
		ticksMap[player->getID()] = OTSYS_TIME();
		updateSharedExperience();
	}
}

void Party::clearPlayerPoints(const PlayerPtr& player)
{
	if (const auto it = ticksMap.find(player->getID()); it != ticksMap.end()) {
		ticksMap.erase(it);
		updateSharedExperience();
	}
}

bool Party::canOpenCorpse(const uint32_t ownerId) const
{
	if (const auto& player = g_game.getPlayerByID(ownerId)) {
		return leader->getID() == ownerId || player->getParty() == this;
	}
	return false;
}
