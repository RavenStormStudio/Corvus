// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/CoreConcepts.hpp"
#include "Core/Memory/Memory.hpp"

#include <cassert>
#include <type_traits>

template <typename TElement> requires std::is_object_v<TElement> && !std::is_abstract_v<TElement>
class TArray
{
private:
    static constexpr size64 DefaultCapacity = 4;
    static constexpr size64 GrowthFactor = 2;

public:
    constexpr TArray() = default;

    explicit TArray(const size64 InCount) requires std::is_trivially_constructible_v<TElement>
    {
        if (InCount > 0)
        {
            Reserve(InCount);
            for (size64 Index = 0; Index < InCount; ++Index)
            {
                EmplaceBack();
            }
        }
    }

    TArray(const size64 InCount, const TElement& InValue)
    {
        if (InCount > 0)
        {
            Reserve(InCount);
            for (size64 Index = 0; Index < InCount; ++Index)
            {
                EmplaceBack(InValue);
            }
        }
    }

    TArray(std::initializer_list<TElement> InInitializerList)
    {
        if (InInitializerList.size() > 0)
        {
            Reserve(InInitializerList.size());
            for (const auto& Element : InInitializerList)
            {
                EmplaceBack(Element);
            }
        }
    }

    TArray(const TArray& Other)
    {
        if (Other.Size > 0)
        {
            Reserve(Other.Size);
            for (size64 Index = 0; Index < Other.Size; ++Index)
            {
                EmplaceBack(Other.Data[Index]);
            }
        }
    }

    TArray(TArray&& Other) noexcept
        : Data(Other.Data), Size(Other.Size), Capacity(Other.Capacity)
    {
        Other.Data = nullptr;
        Other.Size = 0;
        Other.Capacity = 0;
    }

    ~TArray()
    {
        DestroyAndDeallocate();
    }

public:
    TArray& operator=(const TArray& Other)
    {
        if (this != &Other)
        {
            Clear();
            if (Other.Size > 0)
            {
                Reserve(Other.Size);
                for (size64 Index = 0; Index < Other.Size; ++Index)
                {
                    EmplaceBack(Other.Data[Index]);
                }
            }
        }
        return *this;
    }

    TArray& operator=(TArray&& Other) noexcept
    {
        if (this != &Other)
        {
            DestroyAndDeallocate();

            Data = Other.Data;
            Size = Other.Size;
            Capacity = Other.Capacity;

            Other.Data = nullptr;
            Other.Size = 0;
            Other.Capacity = 0;
        }
        return *this;
    }

    TArray& operator=(std::initializer_list<TElement> InInitializerList)
    {
        Clear();
        if (InInitializerList.size() > 0)
        {
            Reserve(InInitializerList.size());
            for (const auto& Element : InInitializerList)
            {
                EmplaceBack(Element);
            }
        }
        return *this;
    }

    constexpr TElement& operator[](const size64 Index)
    {
        assert(Index < Size && "Array index is out of bounds");
        return Data[Index];
    }

    constexpr const TElement& operator[](const size64 Index) const
    {
        assert(Index < Size && "Array index is out of bounds");
        return Data[Index];
    }

    auto operator<=>(const TArray& Other) const noexcept requires CThreeWayComparable<TElement>
    {
        const size64 MinSize = Size < Other.Size ? Size : Other.Size;
        for (size64 Index = 0; Index < MinSize; ++Index)
        {
            if (bool8 Result = Data[Index] <=> Other.Data[Index]; Result != 0)
            {
                return Result;
            }
        }
        return Size <=> Other.Size;
    }

    bool8 operator==(const TArray& Other) const noexcept requires CEqualityComparable<TElement>
    {
        if (Size != Other.Size)
        {
            return false;
        }
        for (size64 Index = 0; Index < Size; ++Index)
        {
            if (!(Data[Index] == Other.Data[Index]))
            {
                return false;
            }
        }
        return true;
    }

public:
public:
    [[nodiscard]] TElement& At(const size64 Index)
    {
        assert(Index < Size && "Array index is out of bounds");
        return Data[Index];
    }

    [[nodiscard]] const TElement& At(const size64 Index) const
    {
        assert(Index < Size && "Array index is out of bounds");
        return Data[Index];
    }

    [[nodiscard]] TElement& GetFirst()
    {
        assert(Size > 0 && "Cannot call GetFirst() on empty array");
        return Data[0];
    }

    [[nodiscard]] const TElement& GetFirst() const
    {
        assert(Size > 0 && "Cannot call GetFirst() on empty array");
        return Data[0];
    }

    [[nodiscard]] TElement& GetLast()
    {
        assert(Size > 0 && "Cannot call GetLast() on empty array");
        return Data[Size - 1];
    }

    [[nodiscard]] const TElement& GetLast() const
    {
        assert(Size > 0 && "Cannot call GetLast() on empty array");
        return Data[Size - 1];
    }

    [[nodiscard]] TElement* GetData() noexcept
    {
        return Data;
    }

    [[nodiscard]] const TElement* GetData() const noexcept
    {
        return Data;
    }

public:
    void PushBack(const TElement& InValue)
    {
        EmplaceBack(InValue);
    }

    void PushBack(TElement&& InValue)
    {
        EmplaceBack(std::move(InValue));
    }

    template <typename... TArguments>
    TElement& EmplaceBack(TArguments&&... Arguments) requires std::is_constructible_v<TElement, TArguments...>
    {
        if (Size >= Capacity)
        {
            const size64 NewCapacity = Capacity == 0 ? DefaultCapacity : Capacity * GrowthFactor;
            Reserve(NewCapacity);
        }
        TElement* NewElement = std::construct_at(&Data[Size], std::forward<TArguments>(Arguments)...);
        ++Size;
        return *NewElement;
    }

    void PopBack()
    {
        assert(Size > 0 && "Cannot pop from empty array");
        --Size;
        std::destroy_at(&Data[Size]);
    }

    void Reserve(const size64 NewCapacity)
    {
        if (NewCapacity > Capacity)
        {
            TElement* NewData = static_cast<TElement*>(FMemory::Allocate(sizeof(TElement) * NewCapacity));
            if (Data != nullptr)
            {
                if constexpr (std::is_trivially_copyable_v<TElement>)
                {
                    FMemory::Copy(Data, NewData, Size * sizeof(TElement));
                }
                else
                {
                    for (size64 Index = 0; Index < Size; ++Index)
                    {
                        std::construct_at(&NewData[Index], std::move(Data[Index]));
                        std::destroy_at(&Data[Index]);
                    }
                }

                FMemory::Free(Data);
            }
            Data = NewData;
            Capacity = NewCapacity;
        }
    }

    void Resize(const size64 NewSize)
    {
        if (NewSize > Size)
        {
            Reserve(NewSize);
            for (size64 Index = Size; Index < NewSize; ++Index)
            {
                std::construct_at(&Data[Index]);
            }
        }
        else if (NewSize < Size)
        {
            for (size64 Index = NewSize; Index < Size; ++Index)
            {
                std::destroy_at(&Data[Index]);
            }
        }
        Size = NewSize;
    }

    void ShrinkToFit()
    {
        if (Capacity > Size)
        {
            if (Size == 0)
            {
                FMemory::Free(Data);
                Data = nullptr;
                Capacity = 0;
            }
            else
            {
                TElement* NewData = static_cast<TElement*>(FMemory::Allocate(Size * sizeof(TElement)));
                if constexpr (std::is_trivially_copyable_v<TElement>)
                {
                    FMemory::Copy(Data, NewData, Size * sizeof(TElement));
                }
                else
                {
                    for (size64 Index = 0; Index < Size; ++Index)
                    {
                        std::construct_at(&NewData[Index], std::move(Data[Index]));
                        std::destroy_at(&Data[Index]);
                    }
                }
                FMemory::Free(Data);
                Data = NewData;
                Capacity = Size;
            }
        }
    }

    void Clear()
    {
        for (size64 Index = 0; Index < Size; ++Index)
        {
            std::destroy_at(&Data[Index]);
        }
        Size = 0;
    }

public:
    [[nodiscard]] size64 Num() const noexcept
    {
        return Size;
    }

    [[nodiscard]] size64 GetCapacity() const noexcept
    {
        return Capacity;
    }

    [[nodiscard]] size64 GetSizeInBytes() const noexcept
    {
        return Size * sizeof(TElement);
    }

    [[nodiscard]] size64 GetCapacityInBytes() const noexcept
    {
        return Capacity * sizeof(TElement);
    }

    [[nodiscard]] bool8 IsEmpty() const noexcept
    {
        return Size == 0;
    }

public:
    // TElement* support
    [[nodiscard]] TElement* begin() noexcept
    {
        return Data;
    }

    [[nodiscard]] const TElement* begin() const noexcept
    {
        return Data;
    }

    [[nodiscard]] TElement* end() noexcept
    {
        return Data + Size;
    }

    [[nodiscard]] const TElement* end() const noexcept
    {
        return Data + Size;
    }

    [[nodiscard]] const TElement* cbegin() const noexcept
    {
        return Data;
    }

    [[nodiscard]] const TElement* cend() const noexcept
    {
        return Data + Size;
    }

private:
    void DestroyAndDeallocate()
    {
        if (Data != nullptr)
        {
            for (size64 Index = 0; Index < Size; ++Index)
            {
                std::destroy_at(&Data[Index]);
            }
            FMemory::Free(Data);
            Data = nullptr;
        }
        Size = 0;
        Capacity = 0;
    }

private:
    TElement* Data = nullptr;
    size64 Size = 0;
    size64 Capacity = 0;
};

template <typename... TArguments>
TArray(TArguments...) -> TArray<std::common_type_t<TArguments...>>;
