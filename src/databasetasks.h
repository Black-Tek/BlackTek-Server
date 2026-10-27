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

#ifndef FS_DATABASETASKS_H
#define FS_DATABASETASKS_H

#include <condition_variable>
#include <deque>
#include "thread_holder_base.h"
#include "database.h"
#include "enums.h"

struct DatabaseTask {
	DatabaseTask(std::string&& query, std::function<void(DBResult_ptr, bool)>&& callback, bool store) :
		query(std::move(query)), callback(std::move(callback)), store(store) {}

	std::string query;
	std::function<void(DBResult_ptr, bool)> callback;
	bool store;
};

class DatabaseTasks : public ThreadHolder<DatabaseTasks>
{
	public:
		DatabaseTasks() = default;
		void start();
		void flush();
		void shutdown();

		void addTask(std::string query, std::function<void(DBResult_ptr, bool)> callback = nullptr, bool store = false);

		void threadMain();
	private:
		void runTask(const DatabaseTask& task);

		Database db;
		std::thread thread;
		std::deque<DatabaseTask> tasks;
		std::mutex taskLock;
		std::condition_variable taskSignal;
};

extern DatabaseTasks g_databaseTasks;

#endif
