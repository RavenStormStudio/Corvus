// RavenStorm Copyright @ 2025-2025

#pragma once

#include <memory>

#include "Memory.hpp"

template <typename T>
struct FDefaultDeleter
{
    constexpr FDefaultDeleter() noexcept = default;

    template <class TOther> requires std::is_convertible_v<T*, TOther*>
    constexpr FDefaultDeleter(const FDefaultDeleter<TOther>&) noexcept
    {
    }

    constexpr void operator()(T* Pointer) const noexcept
    {
        static_assert(0 < sizeof(T), "can't delete an incomplete type");
        FMemory::DestroyObject<T>(Pointer);
    }
};

template <typename T, typename TDeleter = FDefaultDeleter<T>>
using TUniquePtr = std::unique_ptr<T, TDeleter>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
TUniquePtr<T> MakeUnique(TArguments&&... Arguments)
{
    return TUniquePtr<T>(FMemory::New<T>(std::forward<TArguments>(Arguments)...));
}

template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
TSharedPtr<T> MakeShared(TArguments&&... Arguments)
{
    return TSharedPtr<T>(FMemory::New<T>(std::forward<TArguments>(Arguments)...), FDefaultDeleter<T>());
}
