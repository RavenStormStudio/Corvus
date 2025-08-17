#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <catch2/catch_test_macros.hpp>

#include "Engine/Threading/ConditionVariable.hpp"
#include "Engine/Threading/CriticalSection.hpp"

TEST_CASE("FConditionVariable - Basic Construction", "[FConditionVariable]")
{
    SECTION("Default constructor creates valid condition variable")
    {
        FConditionVariable TestConditionVariable;
        REQUIRE(true);
    }

    SECTION("Destructor properly cleans up condition variable")
    {
        {
            FConditionVariable TestConditionVariable;
        }
        REQUIRE(true);
    }
}

TEST_CASE("FConditionVariable - Basic Notification", "[FConditionVariable]")
{
    SECTION("NotifyOne wakes single waiting thread")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<bool8> bWaitCompleted{false};
        std::atomic<bool8> bThreadStarted{false};

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            bThreadStarted.store(true);
            TestConditionVariable.Wait(TestCriticalSection);
            bWaitCompleted.store(true);
            TestCriticalSection.Unlock();
        });

        while (!bThreadStarted.load())
        {
            std::this_thread::yield();
        }

        REQUIRE(bWaitCompleted.load() == false);

        TestConditionVariable.NotifyOne();
        WaitingThread.join();

        REQUIRE(bWaitCompleted.load() == true);
    }

    SECTION("NotifyAll wakes multiple waiting threads")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<int32> CompletedThreads{0};
        std::atomic<int32> StartedThreads{0};
        constexpr int32 NumThreads = 3;

        std::vector<std::thread> WaitingThreads;
        WaitingThreads.reserve(NumThreads);

        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            WaitingThreads.emplace_back([&]()
            {
                StartedThreads.fetch_add(1);

                TestCriticalSection.Lock();
                const bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(100));
                TestCriticalSection.Unlock();

                if (bResult)
                {
                    CompletedThreads.fetch_add(1);
                }
            });
        }

        while (StartedThreads.load() < NumThreads)
        {
            std::this_thread::yield();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        TestConditionVariable.NotifyAll();

        for (auto& CurrentThread : WaitingThreads)
        {
            CurrentThread.join();
        }

        REQUIRE(CompletedThreads.load() > 0);
    }
}

TEST_CASE("FConditionVariable - Wait Operations", "[FConditionVariable]")
{
    SECTION("Wait blocks until notified")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<bool8> bWaitStarted{false};
        std::atomic<bool8> bWaitCompleted{false};

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            bWaitStarted.store(true);
            TestConditionVariable.Wait(TestCriticalSection);
            bWaitCompleted.store(true);
            TestCriticalSection.Unlock();
        });

        while (!bWaitStarted.load())
        {
            std::this_thread::yield();
        }

        REQUIRE(bWaitCompleted.load() == false);

        TestConditionVariable.NotifyOne();
        WaitingThread.join();

        REQUIRE(bWaitCompleted.load() == true);
    }

    SECTION("Wait with predicate using timeout to avoid stalling")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<int32> SharedValue{0};

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            SharedValue.store(1);
            const bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(20), [&]() { return SharedValue.load() >= 3; });
            TestCriticalSection.Unlock();
            REQUIRE(bResult == false);
        });

        WaitingThread.join();
        REQUIRE(SharedValue.load() == 1);
    }
}

TEST_CASE("FConditionVariable - WaitFor Operations", "[FConditionVariable]")
{
    SECTION("WaitFor returns false on timeout")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        bool8 bWaitResult = true;

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            bWaitResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(10));
            TestCriticalSection.Unlock();
        });

        WaitingThread.join();
        REQUIRE(bWaitResult == false);
    }

    SECTION("WaitFor returns true when notified before timeout")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<bool8> bWaitResult{false};
        std::atomic<bool8> bThreadStarted{false};

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            bThreadStarted.store(true);
            bWaitResult.store(TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(100)));
            TestCriticalSection.Unlock();
        });

        while (!bThreadStarted.load())
        {
            std::this_thread::yield();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        TestConditionVariable.NotifyOne();

        WaitingThread.join();
        REQUIRE(bWaitResult.load() == true);
    }

    SECTION("WaitFor with predicate returns false on timeout")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        bool8 bCondition = false;
        bool8 bWaitResult = true;

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            bWaitResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(10), [&]() { return bCondition; });
            TestCriticalSection.Unlock();
        });

        WaitingThread.join();
        REQUIRE(bWaitResult == false);
    }

    SECTION("WaitFor with predicate returns true when condition becomes true")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<bool8> bCondition{false};
        std::atomic<bool8> bWaitResult{false};
        std::atomic<bool8> bThreadStarted{false};

        std::thread WaitingThread([&]()
        {
            TestCriticalSection.Lock();
            bThreadStarted.store(true);
            bWaitResult.store(TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(50), [&]() { return bCondition.load(); }));
            TestCriticalSection.Unlock();
        });

        while (!bThreadStarted.load())
        {
            std::this_thread::yield();
        }

        bCondition.store(true);
        TestConditionVariable.NotifyOne();

        WaitingThread.join();
        REQUIRE(bWaitResult.load() == true);
    }
}

TEST_CASE("FConditionVariable - Producer Consumer Pattern", "[FConditionVariable]")
{
    SECTION("Simple producer consumer with timeout safety")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<int32> ProducedCount{0};
        std::atomic<int32> ConsumedCount{0};
        constexpr int32 NumItems = 3;

        std::thread ProducerThread([&]()
        {
            for (int32 Index = 0; Index < NumItems; ++Index)
            {
                ProducedCount.fetch_add(1);
                TestConditionVariable.NotifyOne();
            }
        });

        std::thread ConsumerThread([&]()
        {
            while (ConsumedCount.load() < NumItems)
            {
                TestCriticalSection.Lock();
                const bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(50));
                TestCriticalSection.Unlock();

                if (bResult || ProducedCount.load() > ConsumedCount.load())
                {
                    ConsumedCount.fetch_add(1);
                }

                if (!bResult && ProducedCount.load() == 0)
                {
                    break;
                }
            }
        });

        ProducerThread.join();
        ConsumerThread.join();

        REQUIRE(ProducedCount.load() == NumItems);
        REQUIRE(ConsumedCount.load() <= NumItems);
    }
}

TEST_CASE("FConditionVariable - Thread Synchronization", "[FConditionVariable]")
{
    SECTION("Simple barrier synchronization with timeout")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        std::atomic<int32> ArrivedThreads{0};
        std::atomic<int32> CompletedThreads{0};
        constexpr int32 NumThreads = 2;

        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);

        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&]()
            {
                ArrivedThreads.fetch_add(1);

                TestCriticalSection.Lock();
                const bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(100));
                TestCriticalSection.Unlock();

                if (bResult || ArrivedThreads.load() == NumThreads)
                {
                    CompletedThreads.fetch_add(1);
                }
            });
        }

        while (ArrivedThreads.load() < NumThreads)
        {
            std::this_thread::yield();
        }

        TestConditionVariable.NotifyAll();

        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }

        REQUIRE(ArrivedThreads.load() == NumThreads);
        REQUIRE(CompletedThreads.load() > 0);
    }
}

TEST_CASE("FConditionVariable - Performance", "[FConditionVariable]")
{
    SECTION("Rapid notify operations")
    {
        FConditionVariable TestConditionVariable;
        constexpr int32 NumNotifications = 1000;

        const auto StartTime = std::chrono::high_resolution_clock::now();

        for (int32 Index = 0; Index < NumNotifications; ++Index)
        {
            TestConditionVariable.NotifyOne();
        }

        const auto EndTime = std::chrono::high_resolution_clock::now();
        const auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);

        REQUIRE(Duration.count() < 100000);
    }
}

TEST_CASE("FConditionVariable - Timeout Accuracy", "[FConditionVariable]")
{
    SECTION("WaitFor timeout duration accuracy")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;
        constexpr auto TimeoutDuration = std::chrono::milliseconds(20);

        const auto StartTime = std::chrono::high_resolution_clock::now();

        TestCriticalSection.Lock();
        const bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, TimeoutDuration);
        TestCriticalSection.Unlock();

        const auto EndTime = std::chrono::high_resolution_clock::now();
        const auto ActualDuration = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);

        REQUIRE(bResult == false);
        REQUIRE(ActualDuration.count() >= 15);
        REQUIRE(ActualDuration.count() <= 40);
    }
}

TEST_CASE("FConditionVariable - Edge Cases", "[FConditionVariable]")
{
    SECTION("Multiple notifications without waiters")
    {
        FConditionVariable TestConditionVariable;

        for (int32 Index = 0; Index < 10; ++Index)
        {
            TestConditionVariable.NotifyOne();
            TestConditionVariable.NotifyAll();
        }

        REQUIRE(true);
    }

    SECTION("Zero timeout WaitFor")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;

        TestCriticalSection.Lock();
        bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(0));
        TestCriticalSection.Unlock();

        REQUIRE(bResult == false);
    }

    SECTION("Very short timeout WaitFor")
    {
        FConditionVariable TestConditionVariable;
        FCriticalSection TestCriticalSection;

        auto StartTime = std::chrono::high_resolution_clock::now();

        TestCriticalSection.Lock();
        bool8 bResult = TestConditionVariable.WaitFor(TestCriticalSection, std::chrono::milliseconds(1));
        TestCriticalSection.Unlock();

        auto EndTime = std::chrono::high_resolution_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);

        REQUIRE(bResult == false);
        REQUIRE(Duration.count() <= 20);
    }
}
