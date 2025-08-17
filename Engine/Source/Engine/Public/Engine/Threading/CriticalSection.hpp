#pragma once
#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"
#include "Engine/Core/Crash/Assertion.hpp"

class ENGINE_API FCriticalSection final
{
public:
    explicit FCriticalSection(uint32 InSpinCount = 4000) noexcept;
    ~FCriticalSection() noexcept;

    NON_COPY_MOVEABLE(FCriticalSection)

public:
    void Lock() noexcept;
    [[nodiscard]] bool8 TryLock() noexcept;
    void Unlock() noexcept;

public:
    [[nodiscard]] CRITICAL_SECTION GetNativeHandle() const { return CriticalSection; }

private:
    CRITICAL_SECTION CriticalSection;
};
