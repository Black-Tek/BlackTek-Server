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
#include "fileloader.h"

namespace OTB {

constexpr Identifier wildcard = {{'\0', '\0', '\0', '\0'}};

Loader::Loader(const std::string& fileName, const Identifier& acceptedIdentifier):
	fileContents(fileName)
{
	constexpr auto minimalSize = sizeof(Identifier) + sizeof(Node::START) + sizeof(Node::type) + sizeof(Node::END);
	if (fileContents.size() <= minimalSize) {
		throw InvalidOTBFormat{};
	}

	Identifier fileIdentifier;
	std::copy(fileContents.begin(), fileContents.begin() + fileIdentifier.size(), fileIdentifier.begin());
	if (fileIdentifier != acceptedIdentifier && fileIdentifier != wildcard) {
		throw InvalidOTBFormat{};
	}
}

using NodeStack = std::stack<Node*, std::vector<Node*>>;
static Node& getCurrentNode(const NodeStack& nodeStack) {
	if (nodeStack.empty()) {
		throw InvalidOTBFormat{};
	}
	return *nodeStack.top();
}

const Node& Loader::parseTree()
{
	auto it = fileContents.begin() + sizeof(Identifier);
	if (static_cast<uint8_t>(*it) != Node::START) {
		throw InvalidOTBFormat{};
	}
	root.type = *(++it);
	root.propsBegin = ++it;
	NodeStack parseStack;
	parseStack.push(&root);

	for (; it != fileContents.end(); ++it) {
		switch(static_cast<uint8_t>(*it)) {
			case Node::START: {
				auto& currentNode = getCurrentNode(parseStack);
				if (currentNode.children.empty()) {
					currentNode.propsEnd = it;
				}
				currentNode.children.emplace_back();
				auto& child = currentNode.children.back();
				if (++it == fileContents.end()) {
					throw InvalidOTBFormat{};
				}
				child.type = *it;
				child.propsBegin = it + sizeof(Node::type);
				parseStack.push(&child);
				break;
			}
			case Node::END: {
				auto& currentNode = getCurrentNode(parseStack);
				if (currentNode.children.empty()) {
					currentNode.propsEnd = it;
				}
				parseStack.pop();
				break;
			}
			case Node::ESCAPE: {
				if (++it == fileContents.end()) {
					throw InvalidOTBFormat{};
				}
				break;
			}
			default: {
				break;
			}
		}
	}
	if (!parseStack.empty()) {
		throw InvalidOTBFormat{};
	}

	return root;
}

bool Loader::getProps(const Node& node, PropStream& props)
{
    const auto* src = node.propsBegin;
    const auto* srcEnd = node.propsEnd;
    const ptrdiff_t size = srcEnd - src;

    if (size <= 0) [[unlikely]]
    {
        return false;
    }

    if (static_cast<ptrdiff_t>(propBuffer.capacity()) < size)
    {
        propBuffer.reserve(static_cast<size_t>(size));
    }

    propBuffer.clear();
    auto* dst = propBuffer.data();

    while (src < srcEnd)
    {
        if (*src == static_cast<char>(Node::ESCAPE)) [[unlikely]]
        {
            ++src;

            if (src < srcEnd)
            {
                *dst++ = *src++;
            }
        }
        else
        {
            *dst++ = *src++;
        }
    }

    props.init(propBuffer.data(), static_cast<size_t>(dst - propBuffer.data()));
    return true;
}

} //namespace OTB
