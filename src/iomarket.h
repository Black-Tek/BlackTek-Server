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

#ifndef FS_IOMARKET_H
#define FS_IOMARKET_H

#include "enums.h"
#include "database.h"

class IOMarket
{
	public:
		static IOMarket& getInstance() {
			static IOMarket instance;
			return instance;
		}

		static MarketOfferList getActiveOffers(MarketAction_t action, uint16_t itemId);
		static MarketOfferList getOwnOffers(MarketAction_t action, uint32_t playerId);
		static HistoryMarketOfferList getOwnHistory(MarketAction_t action, uint32_t playerId);

		static void processExpiredOffers(const DBResult_ptr& result, bool);
		static void checkExpiredOffers();

		static uint32_t getPlayerOfferCount(uint32_t playerId);
		static MarketOfferEx getOfferByCounter(uint32_t timestamp, uint16_t counter);

		static void createOffer(uint32_t playerId, MarketAction_t action, uint32_t itemId, uint16_t amount, uint32_t price, bool anonymous);
		static void acceptOffer(uint32_t offerId, uint16_t amount);
		static void deleteOffer(uint32_t offerId);

		static void appendHistory(uint32_t playerId, MarketAction_t type, uint16_t itemId, uint16_t amount, uint32_t price, time_t timestamp, MarketOfferState_t state);
		static bool moveOfferToHistory(uint32_t offerId, MarketOfferState_t state);

		void updateStatistics();

		MarketStatistics* getPurchaseStatistics(uint16_t itemId);
		MarketStatistics* getSaleStatistics(uint16_t itemId);

	private:
		IOMarket() = default;

		std::map<uint16_t, MarketStatistics> purchaseStatistics;
		std::map<uint16_t, MarketStatistics> saleStatistics;
};

#endif
