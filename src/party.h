// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_PARTY_H
#define FS_PARTY_H

#include "player.h"
#include "monsters.h"

class Player;
class Party;

using PlayerVector = std::vector<PlayerPtr>;

static constexpr int32_t EXPERIENCE_SHARE_RANGE = 30;
static constexpr int32_t EXPERIENCE_SHARE_FLOORS = 1;

enum SharedExpStatus_t : uint8_t {
	SHAREDEXP_OK,
	SHAREDEXP_TOOFARAWAY,
	SHAREDEXP_LEVELDIFFTOOLARGE,
	SHAREDEXP_MEMBERINACTIVE,
	SHAREDEXP_EMPTYPARTY
};

class Party
{
	public:
		explicit Party(const PlayerPtr& leader);

		PlayerPtr getLeader() const {
			return leader;
		}
	
		PlayerVector& getMembers() {
			return memberList;
		}
	
		const PlayerVector& getInvitees() const {
			return inviteList;
		}
	
		size_t getMemberCount() const {
			return memberList.size();
		}
	
		size_t getInvitationCount() const {
			return inviteList.size();
		}

		void disband();
		bool invitePlayer(const PlayerPtr& player);
		bool joinParty(const PlayerPtr& player);
		void revokeInvitation(const PlayerPtr& player);
		bool passPartyLeadership(const PlayerPtr& player, bool forceRemove = false);
		bool leaveParty(const PlayerPtr& player, bool forceRemove = false);

		bool removeInvite(const PlayerPtr& player, bool removeFromPlayer = true);

		bool isPlayerInvited(const PlayerConstPtr& player) const;
		void updateAllPartyIcons();
		void broadcastPartyMessage(MessageClasses msgClass, const std::string& msg, bool sendToInvitations = false) const;
	
		bool empty() const {
			return memberList.empty() && inviteList.empty();
		}
	
		bool canOpenCorpse(uint32_t ownerId) const;

		void shareExperience(uint64_t experience, const CreaturePtr& source = nullptr);
		bool setSharedExperience(const PlayerPtr& player, bool sharedExpActive);
	
		bool isSharedExperienceActive() const {
			return sharedExpActive;
		}
	
		bool isSharedExperienceEnabled() const {
			return sharedExpEnabled;
		}
	
		bool canUseSharedExperience(const PlayerConstPtr& player) const;
		SharedExpStatus_t getMemberSharedExperienceStatus(const PlayerConstPtr& player) const;
		void updateSharedExperience();

		void updatePlayerTicks(const PlayerPtr& player, uint32_t points);
		void clearPlayerPoints(const PlayerPtr& player);

	private:
		SharedExpStatus_t getSharedExperienceStatus() const;

		std::map<uint32_t, int64_t> ticksMap;

		PlayerVector memberList;
		PlayerVector inviteList;

		PlayerPtr leader;

		bool sharedExpActive = false;
		bool sharedExpEnabled = false;
};

#endif
