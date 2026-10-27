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

#ifndef FS_TASKS_H
#define FS_TASKS_H

#include <condition_variable>
#include "thread_holder_base.h"
#include "enums.h"

using TaskFunc = std::function<void(void)>;
const int DISPATCHER_TASK_EXPIRATION = 2000;
const auto TASK_TIME_ZERO = std::chrono::steady_clock::time_point(std::chrono::milliseconds(0));

class Task
{
	public:
		// DO NOT allocate this class on the stack
		explicit Task(TaskFunc&& f) : func(std::move(f)) {}
		Task(uint32_t ms, TaskFunc&& f) :
			expiration(std::chrono::steady_clock::now() + std::chrono::milliseconds(ms)), func(std::move(f)) {}

		virtual ~Task() = default;
		void operator()() const
		{
			func();
		}

		void setDontExpire() {
			expiration = TASK_TIME_ZERO;
		}

		bool hasExpired() const {
			if (expiration == TASK_TIME_ZERO) {
				return false;
			}
			return expiration < std::chrono::steady_clock::now();
		}

	protected:
		std::chrono::steady_clock::time_point expiration = TASK_TIME_ZERO;

	private:
		// Expiration has another meaning for scheduler tasks,
		// then it is the time the task should be added to the
		// dispatcher
		TaskFunc func;
};

Task* createTask(TaskFunc&& f);
Task* createTask(uint32_t expiration, TaskFunc&& f);

class Dispatcher : public ThreadHolder<Dispatcher> {
	public:
		void addTask(Task* task);

		void addTask(TaskFunc&& f) { addTask(new Task(std::move(f))); }

		void addTask(uint32_t expiration, TaskFunc&& f) { addTask(new Task(expiration, std::move(f))); }

		void shutdown();

		uint64_t getDispatcherCycle() const {
			return dispatcherCycle;
		}

		void threadMain();

	private:
		std::mutex taskLock;
		std::condition_variable taskSignal;

		std::vector<Task*> taskList;
		uint64_t dispatcherCycle = 0;
};

extern Dispatcher g_dispatcher;
extern Dispatcher g_utility_boss;

#endif
