#pragma once

#include <Windows.h>

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"

class ENGINE_API FMutex
{
public:
    FMutex();
    ~FMutex();

    NON_COPY_MOVEABLE(FMutex)

public:
    void Lock() const;
    [[nodiscard]] bool8 TryLock() const;
    void Unlock() const;

public:
    [[nodiscard]] HANDLE GetNativeHandle() const { return MutexHandle; }

private:
    HANDLE MutexHandle;
};
