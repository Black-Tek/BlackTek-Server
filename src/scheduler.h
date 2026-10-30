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

#ifndef FS_SCHEDULER_H
#define FS_SCHEDULER_H

#include "tasks.h"
#include "thread_holder_base.h"

#include <gtl/phmap.hpp>
#include <algorithm>
#include <chrono>

static constexpr int32_t SCHEDULER_MINTICKS = 50;

namespace BlackTek
{
	inline uint32_t NextResyncDelay(std::chrono::steady_clock::time_point& nextTick, int32_t intervalMs)
	{
		using Clock = std::chrono::steady_clock;
		nextTick += std::chrono::milliseconds(intervalMs);

		const auto now = Clock::now();

		if (nextTick <= now)
			nextTick = now + std::chrono::milliseconds(intervalMs);

		return static_cast<uint32_t>(std::max<int64_t>(0, std::chrono::duration_cast<std::chrono::milliseconds>(nextTick - now).count()));
	}
}

class SchedulerTask : public Task
{
	public:
		void setEventId(uint32_t id) {
			eventId = id;
		}
	
		uint32_t getEventId() const {
			return eventId;
		}

		uint32_t getDelay() const {
			return delay;
		}
	
	private:
		SchedulerTask(uint32_t delay, TaskFunc&& f) : Task(std::move(f)), delay(delay) {}

		uint32_t eventId = 0;
		uint32_t delay = 0;

		friend SchedulerTask* createSchedulerTask(uint32_t, TaskFunc&&);
};

SchedulerTask* createSchedulerTask(uint32_t delay, TaskFunc&& f);

class Scheduler : public ThreadHolder<Scheduler>
{
	public:
		uint32_t addEvent(SchedulerTask* task);
		void stopEvent(uint32_t eventId);

		void shutdown();

		[[nodiscard]] bool isRunning() const noexcept { return getState() == THREAD_STATE_RUNNING; }
		[[nodiscard]] boost::asio::io_context& getIoContext() noexcept { return io_context; }

		void threadMain() { io_context.run(); }
	private:
		std::atomic<uint32_t> lastEventId{0};
		boost::asio::io_context io_context;
		gtl::node_hash_map<uint32_t, boost::asio::steady_timer> eventIdTimerMap;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work{ io_context.get_executor() };
};

extern Scheduler g_scheduler;

namespace BlackTek::Scheduling
{
	boost::asio::awaitable<void> RunDispatcherHeartbeat(Scheduler& scheduler, std::chrono::milliseconds interval, TaskFunc onTick);
}

#endif
