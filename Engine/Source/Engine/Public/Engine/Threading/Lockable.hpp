#pragma once

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"

#include <type_traits>

template <typename T>
concept CBaseLockable = requires(T& Lockable)
{
    Lockable.Lock();
    Lockable.Unlock();
};

template <typename T>
concept CLockable = CBaseLockable<T> && requires(T& Lockable)
{
    { Lockable.TryLock() } -> std::convertible_to<bool8>;
};

template <CLockable TLockable>
struct TScopedLock final
{
    explicit TScopedLock(TLockable& InLockable)
        : Lockable(InLockable)
    {
        Lockable.Lock();
    }

    ~TScopedLock()
    {
        Lockable.Unlock();
    }

    NON_COPY_MOVEABLE(TScopedLock)

private:
    TLockable& Lockable;
};
