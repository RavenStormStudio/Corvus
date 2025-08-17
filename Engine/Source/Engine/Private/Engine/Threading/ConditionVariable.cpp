#include "Engine/Threading/ConditionVariable.hpp"

#include "Engine/Threading/CriticalSection.hpp"

FConditionVariable::FConditionVariable()
{
    InitializeConditionVariable(&ConditionVariable);
}

FConditionVariable::~FConditionVariable() = default;

void FConditionVariable::NotifyOne()
{
    WakeConditionVariable(&ConditionVariable);
}

void FConditionVariable::NotifyAll()
{
    WakeAllConditionVariable(&ConditionVariable);
}

void FConditionVariable::Wait(const FCriticalSection& CriticalSection)
{
    WaitForInternal(CriticalSection, std::chrono::milliseconds(INFINITE));
}

void FConditionVariable::Wait(const FCriticalSection& CriticalSection, const FPredicateFunction& Predicate)
{
    while (!Predicate())
    {
        WaitForInternal(CriticalSection, std::chrono::milliseconds(INFINITE));
    }
}

bool8 FConditionVariable::WaitFor(const FCriticalSection& CriticalSection, const std::chrono::milliseconds Timeout)
{
    return WaitForInternal(CriticalSection, Timeout);
}

bool8 FConditionVariable::WaitFor(const FCriticalSection& CriticalSection, const std::chrono::milliseconds Timeout, const FPredicateFunction& Predicate)
{
    const auto EndTime = std::chrono::steady_clock::now() + Timeout;
    while (!Predicate())
    {
        const auto Now = std::chrono::steady_clock::now();
        if (Now >= EndTime)
        {
            return false;
        }
        const auto RemainingTime = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - Now);
        if (!WaitForInternal(CriticalSection, RemainingTime))
        {
            return false;
        }
    }
    return true;
}

bool8 FConditionVariable::WaitForInternal(const FCriticalSection& CriticalSection, const std::chrono::milliseconds Timeout)
{
    CRITICAL_SECTION NativeCriticalSection = CriticalSection.GetNativeHandle();
    return SleepConditionVariableCS(&ConditionVariable, &NativeCriticalSection, static_cast<DWORD>(Timeout.count()));
}
