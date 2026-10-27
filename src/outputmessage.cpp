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

#include "otpch.h"

#include "outputmessage.h"
#include "protocol.h"
#include "lockfree.h"
#include "scheduler.h"

extern Scheduler g_scheduler;

namespace {

const uint16_t NETWORKMESSAGE_FREE_LIST_CAPACITY = 1024;

} // namespace

NetworkMessagePtr NetworkMessagePool::getNetworkMessage()
{
	return std::allocate_shared<NetworkMessage>(LockfreePoolingAllocator<void, NETWORKMESSAGE_FREE_LIST_CAPACITY>());
}

namespace {

const uint16_t OUTPUTMESSAGE_FREE_LIST_CAPACITY = 2048;
const std::chrono::milliseconds OUTPUTMESSAGE_AUTOSEND_DELAY {10};

void sendAll(const std::vector<Protocol_ptr>& bufferedProtocols);

void scheduleSendAll(const std::vector<Protocol_ptr>& bufferedProtocols)
{
	static auto nextTick = std::chrono::steady_clock::now();
	g_scheduler.addEvent(createSchedulerTask(BlackTek::NextResyncDelay(nextTick, static_cast<int32_t>(OUTPUTMESSAGE_AUTOSEND_DELAY.count())), [&]() { sendAll(bufferedProtocols); }));
}

void sendAll(const std::vector<Protocol_ptr>& bufferedProtocols)
{
	//dispatcher thread
	for (auto& protocol : bufferedProtocols) {
		auto& msg = protocol->getCurrentBuffer();
		if (msg) {
			protocol->send(std::move(msg));
		}
	}

	if (!bufferedProtocols.empty()) {
		scheduleSendAll(bufferedProtocols);
	}
}

}

void OutputMessagePool::addProtocolToAutosend(Protocol_ptr protocol)
{
	//dispatcher thread
	if (bufferedProtocols.empty()) {
		scheduleSendAll(bufferedProtocols);
	}
	bufferedProtocols.emplace_back(protocol);
}

void OutputMessagePool::removeProtocolFromAutosend(const Protocol_ptr& protocol)
{
	//dispatcher thread
	auto it = std::find(bufferedProtocols.begin(), bufferedProtocols.end(), protocol);
	if (it != bufferedProtocols.end()) {
		std::swap(*it, bufferedProtocols.back());
		bufferedProtocols.pop_back();
	}
}

OutputMessage_ptr OutputMessagePool::getOutputMessage()
{
	// LockfreePoolingAllocator<void,...> will leave (void* allocate) ill-formed because
	// of sizeof(T), so this guarantees that you get clean memory every time.
	return std::allocate_shared<OutputMessage>(LockfreePoolingAllocator<void, OUTPUTMESSAGE_FREE_LIST_CAPACITY>());
}
