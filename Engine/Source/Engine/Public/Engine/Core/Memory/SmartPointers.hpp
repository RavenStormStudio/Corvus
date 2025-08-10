#pragma once

#include <memory>

#include "Memory.hpp"

template <typename T, typename TDeleter = std::default_delete<T>>
using TUniquePtr = std::unique_ptr<T, TDeleter>;

template <typename T>
using TSharedPtr = std::shared_ptr<T>;

template <typename T>
using TWeakPtr = std::weak_ptr<T>;

template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
TUniquePtr<T> MakeUnique(TArguments&&... Arguments)
{
    return TUniquePtr<T>(FMemory::NewObject<T>(std::forward<TArguments>(Arguments)...));
}

template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
TSharedPtr<T> MakeShared(TArguments&&... Arguments)
{
    return TSharedPtr<T>(FMemory::NewObject<T>(std::forward<TArguments>(Arguments)...));
}
