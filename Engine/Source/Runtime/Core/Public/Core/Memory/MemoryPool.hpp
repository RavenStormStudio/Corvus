// RavenStorm Copyright @ 2025-2025

#pragma once

#include <memory>
#include <type_traits>

#include "Memory.hpp"

enum class CORE_API EMemoryPoolResizePolicy : uint8
{
    Linear,
    Exponential,
};

template <typename T, EMemoryPoolResizePolicy TPolicy = EMemoryPoolResizePolicy::Exponential, size64 TLinearIncrement = 16>
class TMemoryPool
{
private:
    union FMemoryPoolChunk
    {
        std::aligned_storage_t<sizeof(T), alignof(T)> Element;
        FMemoryPoolChunk* NextChunk;
    };

    struct FMemoryBlock
    {
        FMemoryPoolChunk* Memory;
        size64 Size;
        FMemoryBlock* NextBlock;

        FMemoryBlock(const size64 InSize)
            : Memory(static_cast<FMemoryPoolChunk*>(FMemory::Allocate(sizeof(FMemoryPoolChunk) * InSize, alignof(FMemoryPoolChunk)))), Size(InSize), NextBlock(nullptr)
        {
        }

        ~FMemoryBlock()
        {
            if (Memory)
            {
                FMemory::Free(Memory, alignof(FMemoryPoolChunk));
            }
        }
    };

public:
    TMemoryPool() = default;

    TMemoryPool(const TMemoryPool&) = delete;
    TMemoryPool& operator=(const TMemoryPool&) = delete;

    TMemoryPool(TMemoryPool&& Other) noexcept
        : Size(Other.Size), NumAllocatedElements(Other.NumAllocatedElements), FreeList(Other.FreeList), FirstMemoryBlock(Other.FirstMemoryBlock)
    {
        Other.Size = 0;
        Other.NumAllocatedElements = 0;
        Other.FreeList = nullptr;
        Other.FirstMemoryBlock = nullptr;
    }

    TMemoryPool& operator=(TMemoryPool&& Other) noexcept
    {
        if (this != &Other)
        {
            CleanupMemoryBlocks();
            Size = Other.Size;
            NumAllocatedElements = Other.NumAllocatedElements;
            FreeList = Other.FreeList;
            FirstMemoryBlock = Other.FirstMemoryBlock;
            Other.Size = 0;
            Other.NumAllocatedElements = 0;
            Other.FreeList = nullptr;
            Other.FirstMemoryBlock = nullptr;
        }
        return *this;
    }

    ~TMemoryPool()
    {
        CleanupMemoryBlocks();
    }

public:
    void Resize(const size64 NewSize)
    {
        if (NewSize <= Size)
        {
            return;
        }
        const size64 AdditionalChunks = NewSize - Size;
        FMemoryBlock* NewBlock = FMemory::New<FMemoryBlock>(AdditionalChunks);
        FMemoryPoolChunk* NewChunks = NewBlock->Memory;
        for (size64 Index = 0; Index < AdditionalChunks; ++Index)
        {
            if (Index == AdditionalChunks - 1)
            {
                NewChunks[Index].NextChunk = FreeList;
            }
            else
            {
                NewChunks[Index].NextChunk = &NewChunks[Index + 1];
            }
        }
        FreeList = &NewChunks[0];
        Size = NewSize;
        NewBlock->NextBlock = FirstMemoryBlock;
        FirstMemoryBlock = NewBlock;
    }

    template <typename... TArguments> requires std::is_constructible_v<T, TArguments...>
    [[nodiscard]] T* Allocate(TArguments&&... Arguments)
    {
        if (!FreeList)
        {
            const size64 GrowthSize = CalculateGrowthSize();
            Resize(Size + GrowthSize);
        }
        if (FreeList)
        {
            FMemoryPoolChunk* Chunk = FreeList;
            FreeList = Chunk->NextChunk;
            T* Object = std::construct_at<T>(reinterpret_cast<T*>(&Chunk->Element), std::forward<TArguments>(Arguments)...);
            ++NumAllocatedElements;
            return Object;
        }
        return nullptr;
    }

    void Free(T* Object)
    {
        if (!Object)
        {
            return;
        }
        std::destroy_at(Object);
        FMemoryPoolChunk* Chunk = reinterpret_cast<FMemoryPoolChunk*>(Object);
        Chunk->NextChunk = FreeList;
        FreeList = Chunk;
        --NumAllocatedElements;
    }

    [[nodiscard]] size64 GetSize() const
    {
        return Size;
    }

    [[nodiscard]] size64 GetNumAllocatedElements() const
    {
        return NumAllocatedElements;
    }

    [[nodiscard]] size64 GetNumFreeElements() const
    {
        return Size - NumAllocatedElements;
    }

private:
    size64 CalculateGrowthSize() const
    {
        if constexpr (TPolicy == EMemoryPoolResizePolicy::Linear)
        {
            return TLinearIncrement;
        }
        else
        {
            return Size > 0 ? Size : 16;
        }
    }

    void CleanupMemoryBlocks()
    {
        FMemoryBlock* CurrentBlock = FirstMemoryBlock;
        while (CurrentBlock)
        {
            FMemoryBlock* NextBlock = CurrentBlock->NextBlock;
            FMemory::DestroyObject(CurrentBlock, alignof(FMemoryBlock));
            CurrentBlock = NextBlock;
        }
        FirstMemoryBlock = nullptr;
        FreeList = nullptr;
        Size = 0;
        NumAllocatedElements = 0;
    }

private:
    size64 Size = 0;
    size64 NumAllocatedElements = 0;

    FMemoryPoolChunk* FreeList = nullptr;
    FMemoryBlock* FirstMemoryBlock = nullptr;
};
