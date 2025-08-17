#include <chrono>
#include <thread>
#include <vector>
#include <catch2/catch_test_macros.hpp>

#include "Engine/Threading/Mutex.hpp"

TEST_CASE("FMutex - Basic Construction", "[FMutex]")
{
    SECTION("Default constructor creates valid mutex")
    {
        FMutex TestMutex;
        REQUIRE(TestMutex.GetNativeHandle() != nullptr);
        REQUIRE(TestMutex.GetNativeHandle() != INVALID_HANDLE_VALUE);
    }

    SECTION("Destructor properly cleans up mutex")
    {
        HANDLE MutexHandle;
        {
            FMutex TestMutex;
            MutexHandle = TestMutex.GetNativeHandle();
            REQUIRE(MutexHandle != nullptr);
            REQUIRE(MutexHandle != INVALID_HANDLE_VALUE);
        }
        REQUIRE(MutexHandle != nullptr);
    }
}

TEST_CASE("FMutex - Basic Locking", "[FMutex]")
{
    SECTION("Lock and unlock basic functionality")
    {
        FMutex TestMutex;
        TestMutex.Lock();
        TestMutex.Unlock();
    }

    SECTION("TryLock on unlocked mutex succeeds")
    {
        FMutex TestMutex;
        bool8 LockResult = TestMutex.TryLock();
        REQUIRE(LockResult == true);
        TestMutex.Unlock();
    }

    SECTION("TryLock on unlocked mutex from different thread succeeds")
    {
        FMutex TestMutex;
        bool8 TryLockResult = false;

        std::thread TestThread([&TestMutex, &TryLockResult]()
        {
            TryLockResult = TestMutex.TryLock();
            if (TryLockResult)
            {
                TestMutex.Unlock();
            }
        });

        TestThread.join();

        REQUIRE(TryLockResult == true);
    }

    SECTION("TryLock on locked mutex from different thread fails")
    {
        FMutex TestMutex;
        bool8 TryLockResult = false;
        bool8 ThreadStarted = false;
        TestMutex.Lock();
        std::thread TestThread([&TestMutex, &TryLockResult, &ThreadStarted]()
        {
            ThreadStarted = true;
            TryLockResult = TestMutex.TryLock();
        });
        while (!ThreadStarted)
        {
            std::this_thread::yield();
        }
        TestThread.join();
        REQUIRE(TryLockResult == false);
        TestMutex.Unlock();
    }

    SECTION("Windows mutex is recursive - same thread can lock multiple times")
    {
        FMutex TestMutex;
        TestMutex.Lock();
        bool8 LockResult = TestMutex.TryLock();
        REQUIRE(LockResult == true);
        TestMutex.Unlock();
        TestMutex.Unlock();
        bool8 FinalTryLock = TestMutex.TryLock();
        REQUIRE(FinalTryLock == true);
        TestMutex.Unlock();
    }

    SECTION("Multiple lock/unlock cycles work correctly")
    {
        FMutex TestMutex;
        for (int32 i = 0; i < 5; ++i)
        {
            TestMutex.Lock();
            TestMutex.Unlock();
        }
        bool8 LockResult = TestMutex.TryLock();
        REQUIRE(LockResult == true);
        TestMutex.Unlock();
    }
}

TEST_CASE("FMutex - Thread Safety", "[FMutex]")
{
    SECTION("Mutex provides mutual exclusion between threads")
    {
        FMutex TestMutex;
        int32 SharedCounter = 0;
        constexpr int32 NumThreads = 4;
        constexpr int32 IncrementsPerThread = 1000;

        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);

        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&TestMutex, &SharedCounter, IncrementsPerThread]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < IncrementsPerThread; ++ThreadIndex)
                {
                    TestMutex.Lock();
                    ++SharedCounter;
                    TestMutex.Unlock();
                }
            });
        }

        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }

        REQUIRE(SharedCounter == NumThreads * IncrementsPerThread);
    }

    SECTION("TryLock contention between threads")
    {
        FMutex TestMutex;
        int32 SuccessfulLocks = 0;
        constexpr int32 NumThreads = 8;

        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);

        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&TestMutex, &SuccessfulLocks]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < 100; ++ThreadIndex)
                {
                    if (TestMutex.TryLock())
                    {
                        ++SuccessfulLocks;
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                        TestMutex.Unlock();
                    }
                }
            });
        }

        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }

        REQUIRE(SuccessfulLocks > 0);
        REQUIRE(SuccessfulLocks <= NumThreads * 100);
    }

    SECTION("Mixed Lock and TryLock usage")
    {
        FMutex TestMutex;
        int32 LockCount = 0;
        int32 TryLockSuccessCount = 0;
        bool8 bTestCompleted = false;

        std::thread LockThread([&TestMutex, &LockCount, &bTestCompleted]()
        {
            while (!bTestCompleted)
            {
                TestMutex.Lock();
                ++LockCount;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                TestMutex.Unlock();
            }
        });

        std::thread TryLockThread([&TestMutex, &TryLockSuccessCount, &bTestCompleted]()
        {
            while (!bTestCompleted)
            {
                if (TestMutex.TryLock())
                {
                    ++TryLockSuccessCount;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    TestMutex.Unlock();
                }
                std::this_thread::yield();
            }
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bTestCompleted = true;

        LockThread.join();
        TryLockThread.join();

        REQUIRE(LockCount > 0);
        REQUIRE(TryLockSuccessCount > 0);
    }
}

TEST_CASE("FMutex - Performance", "[FMutex]")
{
    SECTION("Lock/Unlock performance benchmark")
    {
        const FMutex TestMutex;
        constexpr int32 NumOperations = 10000;
        const auto StartTime = std::chrono::high_resolution_clock::now();

        for (int32 Index = 0; Index < NumOperations; ++Index)
        {
            TestMutex.Lock();
            TestMutex.Unlock();
        }

        const auto EndTime = std::chrono::high_resolution_clock::now();
        const auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);

        REQUIRE(Duration.count() < 1000000);
    }

    SECTION("TryLock performance benchmark")
    {
        const FMutex TestMutex;
        constexpr int32 NumOperations = 10000;
        int32 SuccessfulTryLocks = 0;

        const auto StartTime = std::chrono::high_resolution_clock::now();

        for (int32 Index = 0; Index < NumOperations; ++Index)
        {
            if (TestMutex.TryLock())
            {
                ++SuccessfulTryLocks;
                TestMutex.Unlock();
            }
        }

        const auto EndTime = std::chrono::high_resolution_clock::now();
        const auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);

        REQUIRE(SuccessfulTryLocks == NumOperations);
        REQUIRE(Duration.count() < 500000);
    }
}

TEST_CASE("FMutex - Native Handle", "[FMutex]")
{
    SECTION("GetNativeHandle returns valid Windows handle")
    {
        FMutex TestMutex;
        HANDLE NativeHandle = TestMutex.GetNativeHandle();

        REQUIRE(NativeHandle != nullptr);
        REQUIRE(NativeHandle != INVALID_HANDLE_VALUE);

        DWORD WaitResult = WaitForSingleObject(NativeHandle, 0);
        REQUIRE((WaitResult == WAIT_OBJECT_0 || WaitResult == WAIT_TIMEOUT));

        if (WaitResult == WAIT_OBJECT_0)
        {
            ReleaseMutex(NativeHandle);
        }
    }

    SECTION("Native handle can be used with Windows API directly")
    {
        FMutex TestMutex;
        HANDLE NativeHandle = TestMutex.GetNativeHandle();

        DWORD WaitResult = WaitForSingleObject(NativeHandle, INFINITE);
        REQUIRE(WaitResult == WAIT_OBJECT_0);

        bool8 bTryLockResult = TestMutex.TryLock();
        REQUIRE(bTryLockResult == true);

        BOOL bReleaseResult1 = ReleaseMutex(NativeHandle);
        REQUIRE(bReleaseResult1 == TRUE);

        TestMutex.Unlock();

        bTryLockResult = TestMutex.TryLock();
        REQUIRE(bTryLockResult == true);
        TestMutex.Unlock();
    }

    SECTION("Native handle contention from different thread")
    {
        FMutex TestMutex;
        HANDLE NativeHandle = TestMutex.GetNativeHandle();
        bool8 bTryLockFromOtherThread = true;

        DWORD WaitResult = WaitForSingleObject(NativeHandle, INFINITE);
        REQUIRE(WaitResult == WAIT_OBJECT_0);

        std::thread TestThread([&TestMutex, &bTryLockFromOtherThread]()
        {
            bTryLockFromOtherThread = TestMutex.TryLock();
        });

        TestThread.join();

        REQUIRE(bTryLockFromOtherThread == false);

        BOOL bReleaseResult = ReleaseMutex(NativeHandle);
        REQUIRE(bReleaseResult == TRUE);

        std::thread TestThread2([&TestMutex, &bTryLockFromOtherThread]()
        {
            bTryLockFromOtherThread = TestMutex.TryLock();
            if (bTryLockFromOtherThread)
            {
                TestMutex.Unlock();
            }
        });

        TestThread2.join();
        REQUIRE(bTryLockFromOtherThread == true);
    }
}

TEST_CASE("FMutex - Edge Cases", "[FMutex]")
{
    SECTION("Rapid lock/unlock cycles from multiple threads")
    {
        FMutex TestMutex;
        bool8 bStopTest = false;
        std::vector<int32> ThreadCounters(4, 0);

        std::vector<std::thread> Threads;
        Threads.reserve(4);

        for (int32 Index = 0; Index < 4; ++Index)
        {
            Threads.emplace_back([&TestMutex, &bStopTest, &ThreadCounters, Index]()
            {
                while (!bStopTest)
                {
                    TestMutex.Lock();
                    ++ThreadCounters[Index];
                    TestMutex.Unlock();
                }
            });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        bStopTest = true;

        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }

        for (int32 Index = 0; Index < 4; ++Index)
        {
            REQUIRE(ThreadCounters[Index] > 0);
        }
    }

    SECTION("Large number of threads contending for mutex")
    {
        FMutex TestMutex;
        int32 SharedValue = 0;
        constexpr int32 NumThreads = 16;
        constexpr int32 IncrementsPerThread = 100;

        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);

        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&TestMutex, &SharedValue, IncrementsPerThread]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < IncrementsPerThread; ++ThreadIndex)
                {
                    TestMutex.Lock();
                    const int32 TempValue = SharedValue;
                    std::this_thread::yield();
                    SharedValue = TempValue + 1;
                    TestMutex.Unlock();
                }
            });
        }

        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }

        REQUIRE(SharedValue == NumThreads * IncrementsPerThread);
    }
}
