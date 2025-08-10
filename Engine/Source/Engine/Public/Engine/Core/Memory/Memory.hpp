#pragma once

#include "Engine/Core/CoreTypes.hpp"

#include <memory>
#include <type_traits>

class ENGINE_API FMemory
{
public:
    static void* Allocate(size64 Size, uint8 Alignment = 8);
    static void* Reallocate(void* OriginalPointer, size64 Size, uint8 Alignment = 8);
    static void Free(void* Pointer, uint8 Alignment = 8);

    static size64 GetPointerSize(void* Pointer, uint8 Alignment = 8);

    template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
    static T* NewObject(TArguments&&... Arguments)
    {
        T* Pointer = static_cast<T*>(Allocate(sizeof(T)));
        return std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...);
    }

    template <typename T, typename... TArguments> requires std::is_constructible_v<T, TArguments...>
    static T* NewObjectAligned(const uint8 Alignment, TArguments&&... Arguments)
    {
        T* Pointer = static_cast<T*>(Allocate(sizeof(T), Alignment));
        return std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...);
    }

    template <typename T>
    static void DestroyObject(T* Pointer)
    {
        std::destroy_at<T>(Pointer);
        Free(static_cast<void*>(Pointer));
    }

    template <typename T>
    static void DestroyObjectAligned(T* Pointer, const uint8 Alignment)
    {
        std::destroy_at<T>(Pointer);
        Free(static_cast<void*>(Pointer), Alignment);
    }

private:
};

// ReSharper disable CppClangTidyReadabilityRedundantDeclaration

void* operator new(size64 Size);
void* operator new[](size64 Size);

void operator delete(void* Pointer) noexcept;
void operator delete[](void* Pointer) noexcept;

// ReSharper enable CppClangTidyReadabilityRedundantDeclaration
