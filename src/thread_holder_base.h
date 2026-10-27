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

#ifndef FS_THREAD_HOLDER_H
#define FS_THREAD_HOLDER_H

#include <thread>
#include <atomic>
#include "enums.h"

template <typename Derived>
class ThreadHolder
{
	public:
		ThreadHolder() {}
		void start() {
			setState(THREAD_STATE_RUNNING);
			thread = std::thread(&Derived::threadMain, static_cast<Derived*>(this));
		}

		void stop() {
			setState(THREAD_STATE_CLOSING);
		}

		void join() {
			if (thread.joinable()) {
				thread.join();
			}
		}
	protected:
		void setState(ThreadState newState) {
			threadState.store(newState, std::memory_order_relaxed);
		}

		ThreadState getState() const {
			return threadState.load(std::memory_order_relaxed);
		}
	private:
		std::atomic<ThreadState> threadState{THREAD_STATE_TERMINATED};
		std::thread thread;
};

#endif
