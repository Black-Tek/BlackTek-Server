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

#include "scheduler.h"
#include <boost/asio/post.hpp>
#include <memory>

uint32_t Scheduler::addEvent(SchedulerTask* task)
{
	std::unique_ptr<SchedulerTask> ownedTask(task);
	if (getState() != THREAD_STATE_RUNNING)
		return 0;

	// check if the event has a valid id
	if (task->getEventId() == 0) {
		task->setEventId(++lastEventId);
	}

	const uint32_t eventId = task->getEventId();
	boost::asio::post(io_context, [this, task = std::move(ownedTask)]() mutable {
		if (getState() != THREAD_STATE_RUNNING)
			return;

		// insert the event id in the list of active events
		auto it = eventIdTimerMap.emplace(task->getEventId(), boost::asio::steady_timer{io_context});
		auto& timer = it.first->second;

		timer.expires_after(std::chrono::milliseconds(task->getDelay()));
		timer.async_wait([this, task = std::move(task)](const boost::system::error_code& error) mutable {
			eventIdTimerMap.erase(task->getEventId());

			if (error or getState() == THREAD_STATE_TERMINATED) {
				// the timer has been manually canceled(timer->cancel()) or Scheduler::shutdown has been called
				return;
			}

			g_dispatcher.addTask(task.release());
		});
	});

	return eventId;
}

void Scheduler::stopEvent(uint32_t eventId)
{
	if (eventId == 0) {
		return;
	}

	boost::asio::post(io_context, [this, eventId]() {
		// search the event id
		auto it = eventIdTimerMap.find(eventId);
		if (it != eventIdTimerMap.end()) {
			it->second.cancel();
		}
	});
}

void Scheduler::shutdown()
{
	setState(THREAD_STATE_TERMINATED);
	boost::asio::post(io_context, [this]() {
		// cancel all active timers
		for (auto& it : eventIdTimerMap) {
			it.second.cancel();
		}

		io_context.stop();
	});
}

SchedulerTask* createSchedulerTask(uint32_t delay, TaskFunc&& f)
{
	return new SchedulerTask(delay, std::move(f));
}

namespace BlackTek::Scheduling
{
	boost::asio::awaitable<void> RunDispatcherHeartbeat(Scheduler& scheduler, std::chrono::milliseconds interval, TaskFunc onTick)
	{
		boost::asio::steady_timer timer(scheduler.getIoContext());
		auto nextTick = std::chrono::steady_clock::now();

		while (scheduler.isRunning())
		{
			nextTick += interval;
			timer.expires_at(nextTick);

			boost::system::error_code ec;
			co_await timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec));

			if (ec)
			{
				co_return;
			}

			g_dispatcher.addTask(TaskFunc(onTick));
		}
	}
}
