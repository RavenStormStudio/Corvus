#include "Engine/Threading/Mutex.hpp"

#include "Engine/Core/Crash/Assertion.hpp"

FMutex::FMutex()
{
    MutexHandle = CreateMutex(nullptr, false, nullptr);
    VERIFY(MutexHandle != nullptr, "Failed to create mutex");
}

FMutex::~FMutex()
{
    CloseHandle(MutexHandle);
}

void FMutex::Lock() const
{
    WaitForSingleObject(MutexHandle, INFINITE);
}

bool8 FMutex::TryLock() const
{
    return WaitForSingleObject(MutexHandle, 0) == WAIT_OBJECT_0;
}

void FMutex::Unlock() const
{
    VERIFY(ReleaseMutex(MutexHandle));
}
