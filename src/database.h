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

#ifndef FS_DATABASE_H
#define FS_DATABASE_H

#include "pugicast.h"
#include "console.h"

#include <mysql/mysql.h>
#include <span>

class DBResult;
using DBResult_ptr = std::shared_ptr<DBResult>;

class Database
{
	public:
		Database() = default;
		~Database();

		// non-copyable
		Database(const Database&) = delete;
		Database& operator=(const Database&) = delete;

		/**
		 * Singleton implementation.
		 *
		 * @return database connection handler singleton
		 */
		static Database& getInstance()
		{
			static Database instance;
			return instance;
		}

		/**
		 * Connects to the database
		 *
		 * @return true on successful connection, false on error
		 */
		bool connect();

		/**
		 * Executes command.
		 *
		 * Executes query which doesn't generates results (eg. INSERT, UPDATE, DELETE...).
		 *
		 * @param query command
		 * @return true on success, false on error
		 */
		bool executeQuery(const std::string& query);

		/**
		 * Queries database.
		 *
		 * Executes query which generates results (mostly SELECT).
		 *
		 * @return results object (nullptr on error)
		 */
		DBResult_ptr storeQuery(const std::string& query);

		/**
		 * Escapes string for query.
		 *
		 * Prepares string to fit SQL queries including quoting it.
		 *
		 * @param s string to be escaped
		 * @return quoted string
		 */
		std::string escapeString(std::string_view s) const { return escapeBlob(s.data(), s.length()); }

		/**
		 * Escapes binary stream for query.
		 *
		 * Prepares binary stream to fit SQL queries.
		 *
		 * @param s binary stream
		 * @param length stream length
		 * @return quoted string
		 */
		std::string escapeBlob(const char* s, uint32_t length) const;
		std::string escapeBlob(std::span<const std::byte> data) const {
			return escapeBlob(reinterpret_cast<const char*>(data.data()), static_cast<uint32_t>(data.size()));
		}

		/**
		 * Retrieve id of last inserted row
		 *
		 * @return id on success, 0 if last query did not result on any rows with auto_increment keys
		 */
		uint64_t getLastInsertId() const {
			return static_cast<uint64_t>(mysql_insert_id(handle));
		}

		/**
		 * Get database engine version
		 *
		 * @return the database engine version
		 */
		static const char* getClientVersion() {
			return mysql_get_client_info();
		}

		uint64_t getMaxPacketSize() const {
			return maxPacketSize;
		}

	private:
		/**
		 * Transaction related methods.
		 *
		 * Methods for starting, committing and rolling back transaction. Each of the returns boolean value.
		 *
		 * @return true on success, false on error
		 */
		bool beginTransaction();
		bool rollback();
		bool commit();

		MYSQL* handle = nullptr;
		std::recursive_mutex databaseLock;
		uint64_t maxPacketSize = 1048576;

	friend class DBTransaction;
};

class DBResult
{
	public:
		explicit DBResult(MYSQL_RES* res);
		~DBResult();

		// non-copyable
		DBResult(const DBResult&) = delete;
		DBResult& operator=(const DBResult&) = delete;

		template<typename T>
		T getNumber(std::string_view column) const
		{
			auto it = listNames.find(column);
			if (it == listNames.end()) {
				BlackTek::Console::Database::Error("DBResult::getNumber: column '{}' does not exist in the result set", column);
				return {};
			}

			if (row == nullptr or row[it->second] == nullptr) {
				return {};
			}

			return pugi::cast<T>(row[it->second]);
		}

		std::string_view getString(std::string_view column) const;

		bool hasNext() const;
		bool next();

	private:
		MYSQL_RES* handle;
		MYSQL_ROW row;

		std::map<std::string_view, size_t> listNames;

	friend class Database;
};

/**
 * INSERT statement.
 */
class DBInsert
{
	public:
		explicit DBInsert(std::string query);
		bool addRow(const std::string& row);
		bool addRow(std::ostringstream& row);
		bool execute();

	private:
		std::string query;
		std::string values;
		size_t length;
};

class DBTransaction
{
	public:
		constexpr DBTransaction() = default;

		~DBTransaction() {
			if (state == STATE_START) {
				Database::getInstance().rollback();
			}
		}

		// non-copyable
		DBTransaction(const DBTransaction&) = delete;
		DBTransaction& operator=(const DBTransaction&) = delete;

		bool begin() {
			if (state != STATE_NO_START or not Database::getInstance().beginTransaction())
				return false;

			state = STATE_START;
			return true;
		}

		bool commit() {
			if (state != STATE_START) {
				return false;
			}

			state = STATE_COMMIT;
			return Database::getInstance().commit();
		}

	private:
		enum TransactionStates_t {
			STATE_NO_START,
			STATE_START,
			STATE_COMMIT,
		};

		TransactionStates_t state = STATE_NO_START;
};

#endif
