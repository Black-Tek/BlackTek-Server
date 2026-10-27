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

#include "script.h"
#include "configmanager.h"

extern LuaEnvironment g_luaEnvironment;
extern ConfigManager g_config;

Scripts::Scripts() :
	scriptInterface("Scripts Interface")
{
	scriptInterface.initState();
}

Scripts::~Scripts()
{
	scriptInterface.reInitState();
}

bool Scripts::loadScripts(const std::string& folderName, bool isLib, bool reload)
{
	namespace fs = std::filesystem;

	const auto dir = fs::current_path() / "data" / folderName;
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		return true;
	}

	fs::recursive_directory_iterator endit;
	std::vector<fs::path> v;
	std::string disable = ("#");
	for(fs::recursive_directory_iterator it(dir); it != endit; ++it) {
		auto fn = it->path().parent_path().filename();
		if ((fn == "lib" && !isLib) || fn == "events") {
			continue;
		}
		if(fs::is_regular_file(*it) && it->path().extension() == ".lua") {
			size_t found = it->path().filename().string().find(disable);
			if (found != std::string::npos) {
				if (g_config.GetBoolean(ConfigManager::SCRIPTS_CONSOLE_LOGS)) {
					std::cout << "> " << it->path().filename().string() << " [disabled]" << std::endl;
				}
				continue;
			}
			v.push_back(it->path());
		}
	}
	sort(v.begin(), v.end());
	std::string redir;
	for (auto it = v.begin(); it != v.end(); ++it) {
		const std::string scriptFile = it->string();
		if (!isLib) {
			if (redir.empty() || redir != it->parent_path().string()) {
				auto p = fs::path(it->relative_path());
				if (g_config.GetBoolean(ConfigManager::SCRIPTS_CONSOLE_LOGS)) {
					std::cout << ">> [" << p.parent_path().filename() << "]" << std::endl;
				}
				redir = it->parent_path().string();
			}
		}

		if(scriptInterface.loadFile(scriptFile) == -1) {
			std::cout << "> " << it->filename().string() << " [error]" << std::endl;
			std::cout << "^ " << scriptInterface.getLastLuaError() << std::endl;
			continue;
		}

		if (g_config.GetBoolean(ConfigManager::SCRIPTS_CONSOLE_LOGS)) {
			if (!reload) {
				std::cout << "> " << it->filename().string() << " [loaded]" << std::endl;
			} else {
				std::cout << "> " << it->filename().string() << " [reloaded]" << std::endl;
			}
		}
	}

	return true;
}
