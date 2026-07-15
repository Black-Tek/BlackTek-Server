// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

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

		void threadMain() { io_context.run(); }
	private:
		std::atomic<uint32_t> lastEventId{0};
		gtl::node_hash_map<uint32_t, boost::asio::steady_timer> eventIdTimerMap;
		boost::asio::io_context io_context;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work{ io_context.get_executor() };
};

extern Scheduler g_scheduler;

#endif
