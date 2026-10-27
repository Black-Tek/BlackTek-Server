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

#ifndef FS_FILELOADER_H
#define FS_FILELOADER_H

#include <limits>
#include <span>
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>

class PropStream;

namespace OTB {
using MappedFile = boost::iostreams::mapped_file_source;
using ContentIt  = MappedFile::iterator;
using Identifier = std::array<char, 4>;

struct Node
{
	using ChildrenVector = std::vector<Node>;

	ChildrenVector children;
	ContentIt      propsBegin;
	ContentIt      propsEnd;
	uint8_t           type;
	enum NodeChar: uint8_t
	{
		ESCAPE = 0xFD,
		START  = 0xFE,
		END    = 0xFF,
	};
};

struct LoadError : std::exception {
	const char* what() const noexcept override = 0;
};

struct InvalidOTBFormat final : LoadError {
	const char* what() const noexcept override {
		return "Invalid OTBM file format";
	}
};

class Loader {
	MappedFile     fileContents;
	Node              root;
	std::vector<char> propBuffer;
public:
	Loader(const std::string& fileName, const Identifier& acceptedIdentifier);
	bool getProps(const Node& node, PropStream& props);
	const Node& parseTree();

	const char* data() const noexcept { return fileContents.data(); }
};

} //namespace OTB

class PropStream
{
	public:
		void init(const char* a, size_t size) {
			p = a;
			end = a + size;
		}

		size_t size() const {
			return end - p;
		}

		template <typename T>
		bool read(T& ret) {
			if (size() < sizeof(T)) {
				return false;
			}

			memcpy(&ret, p, sizeof(T));
			p += sizeof(T);
			return true;
		}

		std::pair<std::string_view, bool> readString() {
			uint16_t strLen;
			if (!read<uint16_t>(strLen)) {
				return { "", false };
			}

			if (size() < strLen) {
				return { "", false };
			}

			std::string_view ret{ p, strLen };
			p += strLen;
			return { ret, true };
		}

		bool readBytes(std::span<std::byte> out) {
			if (size() < out.size()) {
				return false;
			}
			memcpy(out.data(), p, out.size());
			p += out.size();
			return true;
		}

		bool skip(size_t n) {
			if (size() < n) {
				return false;
			}

			p += n;
			return true;
		}

	private:
		const char* p = nullptr;
		const char* end = nullptr;
};

class PropWriteStream
{
	public:
		PropWriteStream() = default;

		// non-copyable
		PropWriteStream(const PropWriteStream&) = delete;
		PropWriteStream& operator=(const PropWriteStream&) = delete;

		std::string_view getStream() const {
			return { buffer.data(), buffer.size() }; 
		}

		void clear() {
			buffer.clear();
		}

		template <typename T>
		void write(T add) {
			char* addr = reinterpret_cast<char*>(&add);
			std::copy(addr, addr + sizeof(T), std::back_inserter(buffer));
		}

		void writeBytes(std::span<const std::byte> data) {
			const char* addr = reinterpret_cast<const char*>(data.data());
			std::copy(addr, addr + data.size(), std::back_inserter(buffer));
		}

		void writeString(const std::string& str) {
			size_t strLength = str.size();
			if (strLength > std::numeric_limits<uint16_t>::max()) {
				write<uint16_t>(0);
				return;
			}

			write(static_cast<uint16_t>(strLength));
			std::copy(str.begin(), str.end(), std::back_inserter(buffer));
		}

	private:
		std::vector<char> buffer;
};

#endif
