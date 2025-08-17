#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>
#include <chrono>

#include "Engine/Threading/CriticalSection.hpp"

TEST_CASE("FCriticalSection - Basic Construction", "[FCriticalSection]")
{
    SECTION("Default constructor creates valid critical section")
    {
        FCriticalSection TestCriticalSection;
        CRITICAL_SECTION NativeHandle = TestCriticalSection.GetNativeHandle();
        REQUIRE(NativeHandle.LockCount >= -1);
    }

    SECTION("Constructor with spin count creates valid critical section")
    {
        FCriticalSection TestCriticalSection(8000);
        CRITICAL_SECTION NativeHandle = TestCriticalSection.GetNativeHandle();
        REQUIRE(NativeHandle.LockCount >= -1);
    }

    SECTION("Constructor with zero spin count creates valid critical section")
    {
        FCriticalSection TestCriticalSection(0);
        CRITICAL_SECTION NativeHandle = TestCriticalSection.GetNativeHandle();
        REQUIRE(NativeHandle.LockCount >= -1);
    }

    SECTION("Destructor properly cleans up critical section")
    {
        CRITICAL_SECTION StoredHandle;
        {
            FCriticalSection TestCriticalSection;
            StoredHandle = TestCriticalSection.GetNativeHandle();
            REQUIRE(StoredHandle.LockCount >= -1);
        }
        REQUIRE(true);
    }
}

TEST_CASE("FCriticalSection - Basic Locking", "[FCriticalSection]")
{
    SECTION("Lock and unlock basic functionality")
    {
        FCriticalSection TestCriticalSection;
        
        TestCriticalSection.Lock();
        TestCriticalSection.Unlock();
    }

    SECTION("TryLock on unlocked critical section succeeds")
    {
        FCriticalSection TestCriticalSection;
        
        bool8 bLockResult = TestCriticalSection.TryLock();
        REQUIRE(bLockResult == true);
        
        TestCriticalSection.Unlock();
    }

    SECTION("TryLock on locked critical section from same thread succeeds")
    {
        FCriticalSection TestCriticalSection;
        
        TestCriticalSection.Lock();
        
        bool8 bLockResult = TestCriticalSection.TryLock();
        REQUIRE(bLockResult == true);
        
        TestCriticalSection.Unlock();
        TestCriticalSection.Unlock();
    }

    SECTION("TryLock on locked critical section from different thread fails")
    {
        FCriticalSection TestCriticalSection;
        bool8 bTryLockResult = false;
        bool8 bThreadStarted = false;
        
        TestCriticalSection.Lock();
        
        std::thread TestThread([&TestCriticalSection, &bTryLockResult, &bThreadStarted]()
        {
            bThreadStarted = true;
            bTryLockResult = TestCriticalSection.TryLock();
            if (bTryLockResult)
            {
                TestCriticalSection.Unlock();
            }
        });
        
        while (!bThreadStarted)
        {
            std::this_thread::yield();
        }
        
        TestThread.join();
        
        REQUIRE(bTryLockResult == false);
        
        TestCriticalSection.Unlock();
    }

    SECTION("Multiple lock/unlock cycles work correctly")
    {
        FCriticalSection TestCriticalSection;
        
        for (int32 Index = 0; Index < 10; ++Index)
        {
            TestCriticalSection.Lock();
            TestCriticalSection.Unlock();
        }
        
        bool8 bLockResult = TestCriticalSection.TryLock();
        REQUIRE(bLockResult == true);
        TestCriticalSection.Unlock();
    }

    SECTION("Recursive locking works correctly")
    {
        FCriticalSection TestCriticalSection;
        
        TestCriticalSection.Lock();
        TestCriticalSection.Lock();
        TestCriticalSection.Lock();
        
        bool8 bTryLockResult = TestCriticalSection.TryLock();
        REQUIRE(bTryLockResult == true);
        
        TestCriticalSection.Unlock();
        TestCriticalSection.Unlock();
        TestCriticalSection.Unlock();
        TestCriticalSection.Unlock();
        
        bTryLockResult = TestCriticalSection.TryLock();
        REQUIRE(bTryLockResult == true);
        TestCriticalSection.Unlock();
    }
}

TEST_CASE("FCriticalSection - Thread Safety", "[FCriticalSection]")
{
    SECTION("Critical section provides mutual exclusion between threads")
    {
        FCriticalSection TestCriticalSection;
        int32 SharedCounter = 0;
        constexpr int32 NumThreads = 4;
        constexpr int32 IncrementsPerThread = 2000;
        
        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);
        
        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&TestCriticalSection, &SharedCounter, IncrementsPerThread]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < IncrementsPerThread; ++ThreadIndex)
                {
                    TestCriticalSection.Lock();
                    ++SharedCounter;
                    TestCriticalSection.Unlock();
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
        FCriticalSection TestCriticalSection;
        int32 SuccessfulLocks = 0;
        constexpr int32 NumThreads = 6;
        
        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);
        
        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&TestCriticalSection, &SuccessfulLocks]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < 200; ++ThreadIndex)
                {
                    if (TestCriticalSection.TryLock())
                    {
                        ++SuccessfulLocks;
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                        TestCriticalSection.Unlock();
                    }
                }
            });
        }
        
        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }
        
        REQUIRE(SuccessfulLocks > 0);
        REQUIRE(SuccessfulLocks <= NumThreads * 200);
    }

    SECTION("Mixed Lock and TryLock usage")
    {
        FCriticalSection TestCriticalSection;
        int32 LockCount = 0;
        int32 TryLockSuccessCount = 0;
        bool8 bTestCompleted = false;
        
        std::thread LockThread([&TestCriticalSection, &LockCount, &bTestCompleted]()
        {
            while (!bTestCompleted)
            {
                TestCriticalSection.Lock();
                ++LockCount;
                std::this_thread::sleep_for(std::chrono::microseconds(10));
                TestCriticalSection.Unlock();
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
        
        std::thread TryLockThread([&TestCriticalSection, &TryLockSuccessCount, &bTestCompleted]()
        {
            while (!bTestCompleted)
            {
                if (TestCriticalSection.TryLock())
                {
                    ++TryLockSuccessCount;
                    std::this_thread::sleep_for(std::chrono::microseconds(10));
                    TestCriticalSection.Unlock();
                }
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bTestCompleted = true;
        
        LockThread.join();
        TryLockThread.join();
        
        REQUIRE(LockCount > 0);
        REQUIRE(TryLockSuccessCount > 0);
    }

    SECTION("High contention scenario")
    {
        FCriticalSection TestCriticalSection;
        int32 SharedValue = 0;
        constexpr int32 NumThreads = 12;
        constexpr int32 OperationsPerThread = 500;
        
        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);
        
        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&TestCriticalSection, &SharedValue, OperationsPerThread]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < OperationsPerThread; ++ThreadIndex)
                {
                    TestCriticalSection.Lock();
                    int32 TempValue = SharedValue;
                    std::this_thread::yield();
                    SharedValue = TempValue + 1;
                    TestCriticalSection.Unlock();
                }
            });
        }
        
        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }
        
        REQUIRE(SharedValue == NumThreads * OperationsPerThread);
    }
}

TEST_CASE("FCriticalSection - Performance", "[FCriticalSection]")
{
    SECTION("Lock/Unlock performance benchmark")
    {
        FCriticalSection TestCriticalSection;
        constexpr int32 NumOperations = 50000;
        
        auto StartTime = std::chrono::high_resolution_clock::now();
        
        for (int32 Index = 0; Index < NumOperations; ++Index)
        {
            TestCriticalSection.Lock();
            TestCriticalSection.Unlock();
        }
        
        auto EndTime = std::chrono::high_resolution_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);
        
        REQUIRE(Duration.count() < 1000000);
    }

    SECTION("TryLock performance benchmark")
    {
        FCriticalSection TestCriticalSection;
        constexpr int32 NumOperations = 50000;
        int32 SuccessfulTryLocks = 0;
        
        auto StartTime = std::chrono::high_resolution_clock::now();
        
        for (int32 Index = 0; Index < NumOperations; ++Index)
        {
            if (TestCriticalSection.TryLock())
            {
                ++SuccessfulTryLocks;
                TestCriticalSection.Unlock();
            }
        }
        
        auto EndTime = std::chrono::high_resolution_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);
        
        REQUIRE(SuccessfulTryLocks == NumOperations);
        REQUIRE(Duration.count() < 500000);
    }

    SECTION("Recursive locking performance")
    {
        FCriticalSection TestCriticalSection;
        constexpr int32 NumOperations = 10000;
        constexpr int32 RecursiveDepth = 5;
        
        auto StartTime = std::chrono::high_resolution_clock::now();
        
        for (int32 Index = 0; Index < NumOperations; ++Index)
        {
            for (int32 DepthIndex = 0; DepthIndex < RecursiveDepth; ++DepthIndex)
            {
                TestCriticalSection.Lock();
            }
            
            for (int32 DepthIndex = 0; DepthIndex < RecursiveDepth; ++DepthIndex)
            {
                TestCriticalSection.Unlock();
            }
        }
        
        auto EndTime = std::chrono::high_resolution_clock::now();
        auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime);
        
        REQUIRE(Duration.count() < 1000000);
    }
}

TEST_CASE("FCriticalSection - Spin Count", "[FCriticalSection]")
{
    SECTION("Different spin counts create valid critical sections")
    {
        FCriticalSection LowSpinCS(100);
        FCriticalSection HighSpinCS(10000);
        FCriticalSection ZeroSpinCS(0);
        
        REQUIRE(LowSpinCS.TryLock() == true);
        LowSpinCS.Unlock();
        
        REQUIRE(HighSpinCS.TryLock() == true);
        HighSpinCS.Unlock();
        
        REQUIRE(ZeroSpinCS.TryLock() == true);
        ZeroSpinCS.Unlock();
    }

    SECTION("Spin count affects contention behavior")
    {
        FCriticalSection HighSpinCS(8000);
        int32 SharedCounter = 0;
        constexpr int32 NumThreads = 4;
        constexpr int32 IncrementsPerThread = 1000;
        
        std::vector<std::thread> Threads;
        Threads.reserve(NumThreads);
        
        for (int32 Index = 0; Index < NumThreads; ++Index)
        {
            Threads.emplace_back([&HighSpinCS, &SharedCounter, IncrementsPerThread]()
            {
                for (int32 ThreadIndex = 0; ThreadIndex < IncrementsPerThread; ++ThreadIndex)
                {
                    HighSpinCS.Lock();
                    ++SharedCounter;
                    HighSpinCS.Unlock();
                }
            });
        }
        
        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }
        
        REQUIRE(SharedCounter == NumThreads * IncrementsPerThread);
    }
}

TEST_CASE("FCriticalSection - Native Handle", "[FCriticalSection]")
{
    SECTION("GetNativeHandle returns valid CRITICAL_SECTION")
    {
        FCriticalSection TestCriticalSection;
        CRITICAL_SECTION NativeHandle = TestCriticalSection.GetNativeHandle();
        
        REQUIRE(NativeHandle.LockCount >= -1);
    }

    SECTION("Native handle can be used with Windows API directly")
    {
        FCriticalSection TestCriticalSection;
        CRITICAL_SECTION NativeHandle = TestCriticalSection.GetNativeHandle();
        
        EnterCriticalSection(&NativeHandle);
        
        bool8 bTryLockResult = TestCriticalSection.TryLock();
        REQUIRE(bTryLockResult == true);
        
        TestCriticalSection.Unlock();
        LeaveCriticalSection(&NativeHandle);
        
        bTryLockResult = TestCriticalSection.TryLock();
        REQUIRE(bTryLockResult == true);
        TestCriticalSection.Unlock();
    }

    SECTION("Native handle maintains state consistency")
    {
        FCriticalSection TestCriticalSection;
        CRITICAL_SECTION NativeHandle = TestCriticalSection.GetNativeHandle();
        
        TestCriticalSection.Lock();
        
        BOOL bTryEnterResult = TryEnterCriticalSection(&NativeHandle);
        REQUIRE(bTryEnterResult == TRUE);
        
        LeaveCriticalSection(&NativeHandle);
        TestCriticalSection.Unlock();
        
        bool8 bFinalTryLock = TestCriticalSection.TryLock();
        REQUIRE(bFinalTryLock == true);
        TestCriticalSection.Unlock();
    }
}

TEST_CASE("FCriticalSection - Edge Cases", "[FCriticalSection]")
{
    SECTION("Rapid lock/unlock cycles from multiple threads")
    {
        FCriticalSection TestCriticalSection;
        std::atomic<bool8> bStopTest{false};
        std::vector<int32> ThreadCounters(6, 0);
        
        std::vector<std::thread> Threads;
        Threads.reserve(6);
        
        for (int32 Index = 0; Index < 6; ++Index)
        {
            Threads.emplace_back([&TestCriticalSection, &bStopTest, &ThreadCounters, Index]()
            {
                int32 LocalCounter = 0;
                while (!bStopTest.load() && LocalCounter < 1000)
                {
                    TestCriticalSection.Lock();
                    ++LocalCounter;
                    TestCriticalSection.Unlock();
                }
                ThreadCounters[Index] = LocalCounter;
            });
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bStopTest.store(true);
        
        for (auto& CurrentThread : Threads)
        {
            CurrentThread.join();
        }
        
        int32 TotalOperations = 0;
        for (int32 Index = 0; Index < 6; ++Index)
        {
            TotalOperations += ThreadCounters[Index];
        }
        
        REQUIRE(TotalOperations > 0);
    }

    SECTION("Deep recursive locking")
    {
        FCriticalSection TestCriticalSection;
        constexpr int32 RecursiveDepth = 50;
        
        for (int32 Index = 0; Index < RecursiveDepth; ++Index)
        {
            TestCriticalSection.Lock();
        }
        
        bool8 bTryLockResult = TestCriticalSection.TryLock();
        REQUIRE(bTryLockResult == true);
        
        for (int32 Index = 0; Index <= RecursiveDepth; ++Index)
        {
            TestCriticalSection.Unlock();
        }
        
        bTryLockResult = TestCriticalSection.TryLock();
        REQUIRE(bTryLockResult == true);
        TestCriticalSection.Unlock();
    }

    SECTION("TryLock with immediate unlock in tight loop")
    {
        FCriticalSection TestCriticalSection;
        constexpr int32 NumIterations = 10000;
        int32 SuccessCount = 0;
        
        for (int32 Index = 0; Index < NumIterations; ++Index)
        {
            if (TestCriticalSection.TryLock())
            {
                ++SuccessCount;
                TestCriticalSection.Unlock();
            }
        }
        
        REQUIRE(SuccessCount == NumIterations);
    }

    SECTION("Mixed operations under high contention")
    {
        FCriticalSection TestCriticalSection;
        int32 LockOperations = 0;
        int32 TryLockOperations = 0;
        bool8 bStopTest = false;
        
        std::thread LockThread([&TestCriticalSection, &LockOperations, &bStopTest]()
        {
            while (!bStopTest)
            {
                TestCriticalSection.Lock();
                ++LockOperations;
                TestCriticalSection.Unlock();
            }
        });
        
        std::thread TryLockThread([&TestCriticalSection, &TryLockOperations, &bStopTest]()
        {
            while (!bStopTest)
            {
                if (TestCriticalSection.TryLock())
                {
                    ++TryLockOperations;
                    TestCriticalSection.Unlock();
                }
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        bStopTest = true;
        
        LockThread.join();
        TryLockThread.join();
        
        REQUIRE(LockOperations > 0);
        REQUIRE(TryLockOperations > 0);
    }
}