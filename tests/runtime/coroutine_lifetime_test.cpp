#include "otpch.h"
#include "game.h"
#include "zones.h"

#include <gtest/gtest.h>

namespace BlackTek::Runtime::Tests
{
    struct QueueSleep
    {
        TimerQueue& queue;
        bool valid;
        bool await_ready() const noexcept { return false; }
        template <typename Promise>
        void await_suspend(std::coroutine_handle<Promise> handle) const
        {
            queue.add(TimerQueue::Clock::now(), handle, [valid = valid] { return valid; }, std::is_same_v<Promise, CoroTask::promise_type>);
        }
        void await_resume() const noexcept {}
    };

    static CoroTask HoldGenericFrame(TimerQueue& queue, std::shared_ptr<int> token, bool valid)
    {
        co_await QueueSleep{queue, valid};
    }

    static Zones::SpawnCoroTask HoldBorrowedFrame(Zones::Zone& zone, TimerQueue& queue, std::shared_ptr<int> token)
    {
        co_await QueueSleep{queue, false};
    }

    TEST(TimerQueue, InvalidGenericFrameReleasesParameters)
    {
        TimerQueue queue;
        auto token = std::make_shared<int>(1);
        std::weak_ptr<int> weakToken = token;
        HoldGenericFrame(queue, token, false);
        token.reset();
        queue.tick();
        EXPECT_TRUE(weakToken.expired());
    }

    TEST(TimerQueue, DestructionReleasesPendingGenericFrames)
    {
        std::weak_ptr<int> weakToken;
        {
            TimerQueue queue;
            auto token = std::make_shared<int>(1);
            weakToken = token;
            HoldGenericFrame(queue, token, true);
        }
        EXPECT_TRUE(weakToken.expired());
    }

    TEST(TimerQueue, InvalidBorrowedFrameRemainsOwnedByZone)
    {
        TimerQueue queue;
        Zones::ZonePool pool;
        const auto handle = pool.Emplace();
        auto token = std::make_shared<int>(1);
        std::weak_ptr<int> weakToken = token;
        HoldBorrowedFrame(*pool.TryGetMutable(handle), queue, token);
        token.reset();
        queue.tick();
        EXPECT_FALSE(weakToken.expired());
        pool.Clear();
        EXPECT_TRUE(weakToken.expired());
    }

    TEST(TimerQueue, ValidGenericFrameCompletesOnce)
    {
        TimerQueue queue;
        auto token = std::make_shared<int>(1);
        std::weak_ptr<int> weakToken = token;
        HoldGenericFrame(queue, token, true);
        token.reset();
        queue.tick();
        EXPECT_TRUE(weakToken.expired());
        EXPECT_TRUE(queue.queue.empty());
    }
}
