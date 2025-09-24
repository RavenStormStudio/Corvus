// RavenStorm Copyright @ 2025-2025

#pragma once

#include <memory>
#include <type_traits>

class CORE_API FMemory
{
public:
    [[nodiscard]] static void* Allocate(size64 Size, uint8 Alignment = 8);
    [[nodiscard]] static void* Reallocate(void* OldPointer, size64 Size, uint8 Alignment = 8);
    static void Free(void* OldPointer, uint8 Alignment = 8);

    static void Copy(const void* SourcePointer, void* TargetPointer, size64 Size);

    [[nodiscard]] static size64 GetAllocationSize(const void* Pointer);

public:
    template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
    [[nodiscard]] static T* New(TArguments&&... Arguments)
    {
        T* Pointer = static_cast<T*>(Allocate(sizeof(T)));
        return std::construct_at<T>(Pointer, std::forward<TArguments>(Arguments)...);
    }

    template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
    [[nodiscard]] static T* NewAligned(const uint8 Alignment, TArguments&&... Arguments)
    {
        T* Pointer = static_cast<T*>(Allocate(sizeof(T), Alignment));
        return std::construct_at<T>(Pointer, std::forward<TArguments>(Arguments)...);
    }

    template <typename T>
    static void DestroyObject(T* Pointer, const uint8 Alignment = 8)
    {
        std::destroy_at<T>(Pointer);
        Free(static_cast<void*>(Pointer), Alignment);
    }
};
