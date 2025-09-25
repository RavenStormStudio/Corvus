// RavenStorm Copyright @ 2025-2025

#pragma once

#include <cassert>
#include <compare>
#include <concepts>
#include <initializer_list>
#include <type_traits>

#include "Core/CoreConcepts.hpp"

template <typename TElement, size64 TSize> requires std::is_object_v<TElement> && !std::is_abstract_v<TElement> && std::is_trivially_constructible_v<TElement> && (TSize > 0)
class TStaticArray
{
public:
    constexpr TStaticArray()
        : Data()
    {
    }

    constexpr explicit TStaticArray(const TElement& InInitialValue)
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            Data[Index] = InInitialValue;
        }
    }

    constexpr explicit TStaticArray(std::initializer_list<TElement> InInitializerList)
    {
        const size64 Count = InInitializerList.size() < TSize ? InInitializerList.size() : TSize;
        for (size64 Index = 0; Index < Count; ++Index)
        {
            Data[Index] = InInitializerList[Index];
        }
    }

    constexpr TStaticArray(const TStaticArray& Other)
    {
        if constexpr (std::is_trivially_copyable_v<TElement>)
        {
            if (std::is_constant_evaluated())
            {
                for (size64 Index = 0; Index < TSize; ++Index)
                {
                    Data[Index] = Other.Data[Index];
                }
            }
            else
            {
                std::memcpy(Data, Other.Data, sizeof(Data));
            }
        }
        else
        {
            for (size64 Index = 0; Index < TSize; ++Index)
            {
                Data[Index] = Other.Data[Index];
            }
        }
    }

    constexpr TStaticArray(TStaticArray&& Other) noexcept
    {
        if constexpr (std::is_trivially_copyable_v<TElement>)
        {
            if (std::is_constant_evaluated())
            {
                for (size64 Index = 0; Index < TSize; ++Index)
                {
                    Data[Index] = std::move(Other.Data[Index]);
                }
            }
            else
            {
                std::memcpy(Data, Other.Data, sizeof(Data));
            }
        }
        else
        {
            for (size64 Index = 0; Index < TSize; ++Index)
            {
                Data[Index] = std::move(Other.Data[Index]);
            }
        }
    }

    constexpr ~TStaticArray() requires std::is_trivially_destructible_v<TElement> = default;

    ~TStaticArray() requires (!std::is_trivially_destructible_v<TElement>)
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            Data[Index].~TElement();
        }
    }

public:
    constexpr TStaticArray& operator=(const TStaticArray& Other)
    {
        if (this != &Other)
        {
            if constexpr (std::is_trivially_copyable_v<TElement>)
            {
                if (std::is_constant_evaluated())
                {
                    for (size64 Index = 0; Index < TSize; ++Index)
                    {
                        Data[Index] = Other.Data[Index];
                    }
                }
                else
                {
                    std::memcpy(Data, Other.Data, sizeof(Data));
                }
            }
            else
            {
                for (size64 Index = 0; Index < TSize; ++Index)
                {
                    Data[Index] = Other.Data[Index];
                }
            }
        }
        return *this;
    }

    constexpr TStaticArray& operator=(TStaticArray&& Other) noexcept
    {
        if (this != &Other)
        {
            if constexpr (std::is_trivially_copyable_v<TElement>)
            {
                if (std::is_constant_evaluated())
                {
                    for (size64 Index = 0; Index < TSize; ++Index)
                    {
                        Data[Index] = std::move(Other.Data[Index]);
                    }
                }
                else
                {
                    std::memcpy(Data, Other.Data, sizeof(Data));
                }
            }
            else
            {
                for (size64 Index = 0; Index < TSize; ++Index)
                {
                    Data[Index] = std::move(Other.Data[Index]);
                }
            }
        }
        return *this;
    }

    constexpr TElement& operator[](size64 Index)
    {
        assert(Index >= 0 && Index < TSize && "Array index is out of bounds");
        return Data[Index];
    }

    constexpr const TElement& operator[](size64 Index) const
    {
        assert(Index >= 0 && Index < TSize && "Array index is out of bounds");
        return Data[Index];
    }

    constexpr auto operator<=>(const TStaticArray& Other) const noexcept requires CThreeWayComparable<TElement>
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            if (auto Result = Data[Index] <=> Other.Data[Index]; Result != 0)
            {
                return Result;
            }
        }
        return std::strong_ordering::equal;
    }

    constexpr bool8 operator==(const TStaticArray& Other) const noexcept requires CEqualityComparable<TElement>
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            if (!(Data[Index] == Other.Data[Index]))
            {
                return false;
            }
        }
        return true;
    }

public:
    [[nodiscard]] constexpr TElement& At(size64 Index)
    {
        return (*this)[Index];
    }

    [[nodiscard]] constexpr const TElement& At(size64 Index) const
    {
        return (*this)[Index];
    }

    [[nodiscard]] constexpr TElement& GetFirst()
    {
        return Data[0];
    }

    [[nodiscard]] constexpr const TElement& GetFirst() const
    {
        return Data[0];
    }

    [[nodiscard]] constexpr TElement& GetLast()
    {
        return Data[TSize - 1];
    }

    [[nodiscard]] constexpr const TElement& GetLast() const
    {
        return Data[TSize - 1];
    }

    [[nodiscard]] constexpr TElement* GetData() noexcept
    {
        return Data;
    }

    [[nodiscard]] constexpr const TElement* GetData() const noexcept
    {
        return Data;
    }

public:
    constexpr void Fill(const TElement& Value)
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            Data[Index] = Value;
        }
    }

    [[nodiscard]] constexpr size64 Find(const TElement& Item) const requires CEqualityComparable<TElement>
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            if (Data[Index] == Item)
            {
                return Index;
            }
        }
        return std::numeric_limits<size64>::max();
    }

    [[nodiscard]] constexpr bool8 Contains(const TElement& Item) const requires CEqualityComparable<TElement>
    {
        return Find(Item) != std::numeric_limits<size64>::max();
    }

public:
    template <std::invocable<TElement&> TFunc>
    constexpr void ForEach(TFunc&& Function)
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            std::invoke(std::forward<TFunc>(Function), Data[Index]);
        }
    }

    template <std::invocable<const TElement&> TFunc>
    constexpr void ForEach(TFunc&& Function) const
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            std::invoke(std::forward<TFunc>(Function), Data[Index]);
        }
    }

    template <std::predicate<const TElement&> TPredicate>
    [[nodiscard]] constexpr bool8 AllOf(TPredicate&& Predicate) const
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            if (!std::invoke(std::forward<TPredicate>(Predicate), Data[Index]))
            {
                return false;
            }
        }
        return true;
    }

    template <std::predicate<const TElement&> TPredicate>
    [[nodiscard]] constexpr bool8 AnyOf(TPredicate&& Predicate) const
    {
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            if (std::invoke(std::forward<TPredicate>(Predicate), Data[Index]))
            {
                return true;
            }
        }
        return false;
    }

    template <std::predicate<const TElement&> TPredicate>
    [[nodiscard]] constexpr size64 CountIf(TPredicate&& Predicate) const
    {
        size64 Count = 0;
        for (size64 Index = 0; Index < TSize; ++Index)
        {
            if (std::invoke(std::forward<TPredicate>(Predicate), Data[Index]))
            {
                ++Count;
            }
        }
        return Count;
    }

public:
    // Iterator Support
    [[nodiscard]] constexpr TElement* begin() noexcept
    {
        return Data;
    }

    [[nodiscard]] constexpr const TElement* begin() const noexcept
    {
        return Data;
    }

    [[nodiscard]] constexpr TElement* end() noexcept
    {
        return Data + TSize;
    }

    [[nodiscard]] constexpr const TElement* end() const noexcept
    {
        return Data + TSize;
    }

    [[nodiscard]] constexpr const TElement* cbegin() const noexcept
    {
        return Data;
    }

    [[nodiscard]] constexpr const TElement* cend() const noexcept
    {
        return Data + TSize;
    }

public:
    [[nodiscard]] static constexpr size64 Num() noexcept
    {
        return TSize;
    }

    [[nodiscard]] static constexpr size64 GetSizeInBytes() noexcept
    {
        return TSize * sizeof(TElement);
    }

private:
    TElement Data[TSize];
};

template <typename... TArguments>
TStaticArray(TArguments...) -> TStaticArray<std::common_type_t<TArguments...>, sizeof...(TArguments)>;
