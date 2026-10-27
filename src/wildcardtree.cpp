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

#include <stack>

#include "wildcardtree.h"

WildcardTreeNode* WildcardTreeNode::getChild(char ch)
{
	auto it = children.find(ch);
	if (it == children.end()) {
		return nullptr;
	}
	return &it->second;
}

const WildcardTreeNode* WildcardTreeNode::getChild(char ch) const
{
	auto it = children.find(ch);
	if (it == children.end()) {
		return nullptr;
	}
	return &it->second;
}

WildcardTreeNode* WildcardTreeNode::addChild(char ch, bool breakpoint)
{
	WildcardTreeNode* child = getChild(ch);
	if (child) {
		if (breakpoint && !child->breakpoint) {
			child->breakpoint = true;
		}
	} else {
		auto pair = children.emplace(std::piecewise_construct,
				std::forward_as_tuple(ch), std::forward_as_tuple(breakpoint));
		child = &pair.first->second;
	}
	return child;
}

void WildcardTreeNode::insert(const std::string& str)
{
	WildcardTreeNode* cur = this;

	size_t length = str.length() - 1;
	for (size_t pos = 0; pos < length; ++pos) {
		cur = cur->addChild(str[pos], false);
	}

	cur->addChild(str[length], true);
}

void WildcardTreeNode::remove(const std::string& str)
{
	WildcardTreeNode* cur = this;

	std::stack<WildcardTreeNode*> path;
	path.push(cur);
	size_t len = str.length();
	for (size_t pos = 0; pos < len; ++pos) {
		cur = cur->getChild(str[pos]);
		if (!cur) {
			return;
		}
		path.push(cur);
	}

	cur->breakpoint = false;

	do {
		cur = path.top();
		path.pop();

		if (!cur->children.empty() || cur->breakpoint || path.empty()) {
			break;
		}

		cur = path.top();

		auto it = cur->children.find(str[--len]);
		if (it != cur->children.end()) {
			cur->children.erase(it);
		}
	} while (true);
}

ReturnValue WildcardTreeNode::findOne(const std::string& query, std::string& result) const
{
	const WildcardTreeNode* cur = this;
	for (char pos : query) {
		cur = cur->getChild(pos);
		if (!cur) {
			return RETURNVALUE_PLAYERWITHTHISNAMEISNOTONLINE;
		}
	}

	result = query;

	do {
		size_t size = cur->children.size();
		if (size == 0) {
			return RETURNVALUE_NOERROR;
		} else if (size > 1 || cur->breakpoint) {
			return RETURNVALUE_NAMEISTOOAMBIGUOUS;
		}

		auto it = cur->children.begin();
		result += it->first;
		cur = &it->second;
	} while (true);
}
