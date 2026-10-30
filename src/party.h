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

#ifndef FS_PARTY_H
#define FS_PARTY_H

#include "declarations.h"
#include "monsters.h"

using PartyMembers = std::vector<PlayerPtr>;
using PartyInvitees = std::vector<PlayerPtr>;

enum SharedExpStatus_t : uint8_t 
{
	SHAREDEXP_OK,
	SHAREDEXP_TOOFARAWAY,
	SHAREDEXP_LEVELDIFFTOOLARGE,
	SHAREDEXP_MEMBERINACTIVE,
	SHAREDEXP_EMPTYPARTY
};

class Party : public std::enable_shared_from_this<Party>
{
	public:
		explicit Party();
		explicit Party(const PlayerPtr& leader);

		uint32_t getId() const
		{
			return id;
		}

		PlayerPtr getLeader() const 
		{
			if (leader)
				return leader;
			return nullptr;
		}
	
		PartyMembers& getMembers() 
		{
			return memberList;
		}
	
		const PartyInvitees& getInvitees() const 
		{
			return inviteList;
		}
	
		size_t getMemberCount() const 
		{
			return memberList.size();
		}
	
		size_t getInvitationCount() const 
		{
			return inviteList.size();
		}

		static std::shared_ptr<Party> get(uint32_t id);
		static std::shared_ptr<Party> make(const PlayerPtr& player);

		void disband();
		bool discardEmpty();
		bool invitePlayer(const PlayerPtr& player);
		bool joinParty(const PlayerPtr player);
		void revokeInvitation(const PlayerPtr& player);
		bool passPartyLeadership(const PlayerPtr player, bool forceRemove = false);
		bool leaveParty(const PlayerPtr player, bool forceRemove = false);

		bool removeInvite(const PlayerPtr player, bool removeFromPlayer = true);

		bool isPlayerInvited(const PlayerConstPtr& player) const;
		void updateAllPartyIcons();
		void broadcastPartyMessage(MessageClasses msgClass, const std::string& msg, bool sendToInvitations = false) const;
	
		bool empty() const 
		{
			return memberList.empty() and inviteList.empty();
		}
	
		bool canOpenCorpse(uint32_t ownerId) const;

		void shareExperience(uint64_t experience, const CreaturePtr& source = nullptr);
		bool setSharedExperience(const PlayerPtr& player, bool sharedExpActive);
	
		bool isSharedExperienceActive() const 
		{
			return sharedExpActive;
		}
	
		bool isSharedExperienceEnabled() const 
		{
			return sharedExpEnabled;
		}
	
		bool canUseSharedExperience(const PlayerConstPtr& player) const;
		SharedExpStatus_t getMemberSharedExperienceStatus(const PlayerConstPtr& player) const;
		void updateSharedExperience();

		void updatePlayerTicks(const PlayerPtr& player, uint32_t points);
		void clearPlayerPoints(const PlayerPtr& player);

	private:
		SharedExpStatus_t getSharedExperienceStatus() const;

		PartyMembers memberList;
		PartyInvitees inviteList;
		std::map<uint32_t, int64_t> ticksMap;
		PlayerPtr leader;
		uint32_t id = 0;
		bool sharedExpActive = false;
		bool sharedExpEnabled = false;
		bool disbanding = false;
};

using PartyPtr = std::shared_ptr<Party>;

#endif
