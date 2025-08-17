#pragma once

#include <chrono>
#include <Windows.h>

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"
#include "Engine/Core/Utility/Function.hpp"

class FCriticalSection;

class ENGINE_API FConditionVariable
{
public:
    using FPredicateFunction = TFunction<bool8()>;

public:
    FConditionVariable();
    ~FConditionVariable();

    NON_COPY_MOVEABLE(FConditionVariable)

public:
    void NotifyOne();
    void NotifyAll();

public:
    void Wait(const FCriticalSection& CriticalSection);
    void Wait(const FCriticalSection& CriticalSection, const FPredicateFunction& Predicate);
    bool8 WaitFor(const FCriticalSection& CriticalSection, std::chrono::milliseconds Timeout);
    bool8 WaitFor(const FCriticalSection& CriticalSection, std::chrono::milliseconds Timeout, const FPredicateFunction& Predicate);

private:
    bool8 WaitForInternal(const FCriticalSection& CriticalSection, std::chrono::milliseconds Timeout);

private:
    CONDITION_VARIABLE ConditionVariable;
};
