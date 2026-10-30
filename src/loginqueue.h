#ifndef BLACKTEK_LOGINQUEUE_H
#define BLACKTEK_LOGINQUEUE_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iterator>

namespace BlackTek::Net
{
	class LoginQueue
	{
	public:
		[[nodiscard]] static constexpr int64_t GetWaitTime(std::size_t slot) noexcept
		{
			if (slot < 5)
				return 5;
			if (slot < 10)
				return 10;
			if (slot < 20)
				return 20;
			if (slot < 50)
				return 60;
			return 120;
		}

		std::size_t Login(uint32_t guid, bool premium, uint32_t online, uint32_t maximum, int64_t now)
		{
			if (maximum == 0)
				return 0;

			std::erase_if(entries, [now](const Entry& entry) { return entry.expires_at <= now; });
			if (entries.empty() and online < maximum)
				return 0;

			auto existing = std::find_if(entries.begin(), entries.end(), [guid](const Entry& entry) { return entry.guid == guid; });
			if (existing != entries.end())
			{
				const auto slot = static_cast<std::size_t>(std::distance(entries.begin(), existing)) + 1;
				if (online < maximum and slot <= maximum - online)
				{
					entries.erase(existing);
					return 0;
				}
				existing->expires_at = now + (GetWaitTime(slot) + 15) * 1000;
				return slot;
			}

			const auto insertion = premium
				? std::find_if(entries.begin(), entries.end(), [](const Entry& entry) { return not entry.premium; })
				: entries.end();
			const auto slot = static_cast<std::size_t>(std::distance(entries.begin(), insertion)) + 1;
			entries.insert(insertion, Entry{now + (GetWaitTime(slot) + 15) * 1000, guid, premium});
			return slot;
		}

	private:
		struct Entry
		{
			int64_t expires_at;
			uint32_t guid;
			bool premium;
		};

		std::deque<Entry> entries;
	};
}

#endif
