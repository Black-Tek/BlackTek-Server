#include "otpch.h"
#include "scheduler.h"
#include "tasks.h"

#include <gtest/gtest.h>

namespace BlackTek::Scheduling::Tests
{
    template <typename T>
    class ManualRunner final : public T
    {
    public:
        void Run() noexcept { this->setState(THREAD_STATE_RUNNING); }
    };

    TEST(TaskLifetime, DispatcherReleasesQueuedTasksOnDestruction)
    {
        std::weak_ptr<int> token;
        {
            ManualRunner<Dispatcher> dispatcher;
            dispatcher.Run();
            const auto owner = std::make_shared<int>(1);
            token = owner;
            dispatcher.addTask([owner]() {});
        }
        EXPECT_TRUE(token.expired());
    }

    TEST(TaskLifetime, SchedulerReleasesUnstartedEventsOnDestruction)
    {
        std::weak_ptr<int> token;
        {
            ManualRunner<Scheduler> scheduler;
            scheduler.Run();
            const auto owner = std::make_shared<int>(1);
            token = owner;
            scheduler.addEvent(createSchedulerTask(60'000, [owner]() {}));
        }
        EXPECT_TRUE(token.expired());
    }

    TEST(TaskLifetime, SchedulerReleasesCancelledHandlersAfterShutdown)
    {
        std::weak_ptr<int> token;
        {
            ManualRunner<Scheduler> scheduler;
            scheduler.Run();
            const auto owner = std::make_shared<int>(1);
            token = owner;
            scheduler.addEvent(createSchedulerTask(60'000, [owner]() {}));
            scheduler.getIoContext().poll();
            scheduler.shutdown();
            scheduler.getIoContext().poll();
        }
        EXPECT_TRUE(token.expired());
    }
}
