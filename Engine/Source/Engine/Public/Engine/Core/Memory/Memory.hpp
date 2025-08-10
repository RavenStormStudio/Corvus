#pragma once

#include "Engine/Core/CoreTypes.hpp"

#include <type_traits>
#include <memory>

class ENGINE_API FMemory
{
public:
    static void* Allocate(size64 Size, uint8 Alignment = 8);
    static void* Reallocate(void* OriginalPointer, size64 Size, uint8 Alignment = 8);
    static void Free(void* Pointer, uint8 Alignment = 8);

    static size64 GetPointerSize(void* Pointer, uint8 Alignment = 8);
    
    template<typename T, typename... TArguments>
    static T* NewObject(TArguments&&... Arguments)
    {
        T* Pointer = static_cast<T*>(Allocate(sizeof(T)));
        return std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...);
    }

    template<typename T, typename... TArguments>
    static T* NewObjectAligned(uint8 Alignment, TArguments&&... Arguments)
    {
        T* Pointer = static_cast<T*>(Allocate(sizeof(T), Alignment));
        return std::construct_at<T, TArguments...>(Pointer, std::forward<TArguments>(Arguments)...);
    }

    template<typename T>
    static void DestroyObject(T* Pointer)
    {
        std::destroy_at<T>(Pointer);
        Free(static_cast<void*>(Pointer));
    }

    template<typename T>
    static void DestroyObjectAligned(T* Pointer, uint8 Alignment)
    {
        std::destroy_at<T>(Pointer);
        Free(static_cast<void*>(Pointer), Alignment);
    }
private:
};
