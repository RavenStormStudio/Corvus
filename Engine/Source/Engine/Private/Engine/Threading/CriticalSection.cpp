#include "Engine/Threading/CriticalSection.hpp"

FCriticalSection::FCriticalSection(const uint32 InSpinCount) noexcept
{
    if (InSpinCount > 0)
    {
        InitializeCriticalSectionAndSpinCount(&CriticalSection, InSpinCount);
    }
    else
    {
        InitializeCriticalSection(&CriticalSection);
    }
}

FCriticalSection::~FCriticalSection() noexcept
{
    DeleteCriticalSection(&CriticalSection);
}

void FCriticalSection::Lock() noexcept
{
    EnterCriticalSection(&CriticalSection);
}

bool8 FCriticalSection::TryLock() noexcept
{
    return TryEnterCriticalSection(&CriticalSection);
}

void FCriticalSection::Unlock() noexcept
{
    LeaveCriticalSection(&CriticalSection);
}
