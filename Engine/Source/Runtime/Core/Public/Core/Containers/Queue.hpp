// RavenStorm Copyright @ 2025-2025

#pragma once

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>

#include "Core/CoreConcepts.hpp"
#include "Core/Memory/Memory.hpp"

template <typename TElement>
class TQueueIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::remove_const_t<TElement>;
    using difference_type = ptrdiff_t;
    using pointer = TElement*;
    using reference = TElement&;

private:
    TElement* Data;
    size64 Index;
    size64 Capacity;
    size64 StartIndex;
    size64 Size;

public:
    constexpr TQueueIterator() noexcept
        : Data(nullptr), Index(0), Capacity(0), StartIndex(0), Size(0)
    {
    }

    constexpr TQueueIterator(TElement* InData, const size64 InIndex, const size64 InCapacity, const size64 InStartIndex, const size64 InSize) noexcept
        : Data(InData), Index(InIndex), Capacity(InCapacity), StartIndex(InStartIndex), Size(InSize)
    {
    }

    constexpr reference operator*() const
    {
        const size64 ActualIndex = (StartIndex + Index) % Capacity;
        return Data[ActualIndex];
    }

    constexpr pointer operator->() const
    {
        const size64 ActualIndex = (StartIndex + Index) % Capacity;
        return &Data[ActualIndex];
    }

    constexpr TQueueIterator& operator++()
    {
        ++Index;
        return *this;
    }

    constexpr TQueueIterator operator++(int)
    {
        TQueueIterator temp = *this;
        ++(*this);
        return temp;
    }

    constexpr bool operator==(const TQueueIterator& Other) const noexcept
    {
        return Data == Other.Data && Index == Other.Index;
    }

    constexpr auto operator<=>(const TQueueIterator& Other) const noexcept
    {
        if (Data != Other.Data)
        {
            return Data <=> Other.Data;
        }
        return Index <=> Other.Index;
    }
};

template <typename TElement> requires std::is_object_v<TElement> && !std::is_abstract_v<TElement>
class TQueue
{
public:
    using ValueType = TElement;
    using SizeType = size64;
    using DifferenceType = ptrdiff_t;
    using Reference = TElement&;
    using ConstReference = const TElement&;
    using Pointer = TElement*;
    using ConstPointer = const TElement*;
    using Iterator = TQueueIterator<TElement>;
    using ConstIterator = TQueueIterator<const TElement>;

private:
    static constexpr size64 DefaultCapacity = 8;
    static constexpr size64 GrowthFactor = 2;

public:
    constexpr TQueue()
        : Data(nullptr), QueueCapacity(0), QueueSize(0), HeadIndex(0), TailIndex(0)
    {
    }

    explicit TQueue(const size64 InCapacity)
        : Data(nullptr), QueueCapacity(0), QueueSize(0), HeadIndex(0), TailIndex(0)
    {
        if (InCapacity > 0)
        {
            Reserve(InCapacity);
        }
    }

    TQueue(std::initializer_list<TElement> InInitializerList)
        : Data(nullptr), QueueCapacity(0), QueueSize(0), HeadIndex(0), TailIndex(0)
    {
        if (InInitializerList.size() > 0)
        {
            Reserve(InInitializerList.size());
            for (const auto& Element : InInitializerList)
            {
                Enqueue(Element);
            }
        }
    }

    template <std::input_iterator TIterator>
    TQueue(TIterator InFirst, TIterator InLast)
        : Data(nullptr), QueueCapacity(0), QueueSize(0), HeadIndex(0), TailIndex(0)
    {
        if constexpr (std::random_access_iterator<TIterator>)
        {
            const size64 Count = static_cast<size64>(std::distance(InFirst, InLast));
            if (Count > 0)
            {
                Reserve(Count);
                for (auto It = InFirst; It != InLast; ++It)
                {
                    Enqueue(*It);
                }
            }
        }
        else
        {
            for (auto It = InFirst; It != InLast; ++It)
            {
                Enqueue(*It);
            }
        }
    }

    TQueue(const TQueue& Other)
        : Data(nullptr), QueueCapacity(0), QueueSize(0), HeadIndex(0), TailIndex(0)
    {
        if (Other.QueueSize > 0)
        {
            Reserve(Other.QueueSize);
            for (size64 Index = 0; Index < Other.QueueSize; ++Index)
            {
                const size64 SourceIndex = (Other.HeadIndex + Index) % Other.QueueCapacity;
                Enqueue(Other.Data[SourceIndex]);
            }
        }
    }

    TQueue(TQueue&& Other) noexcept
        : Data(Other.Data), QueueCapacity(Other.QueueCapacity), QueueSize(Other.QueueSize), HeadIndex(Other.HeadIndex), TailIndex(Other.TailIndex)
    {
        Other.Data = nullptr;
        Other.QueueCapacity = 0;
        Other.QueueSize = 0;
        Other.HeadIndex = 0;
        Other.TailIndex = 0;
    }

    ~TQueue()
    {
        DestroyAndDeallocate();
    }

public:
    TQueue& operator=(const TQueue& Other)
    {
        if (this != &Other)
        {
            Clear();
            if (Other.QueueSize > 0)
            {
                Reserve(Other.QueueSize);
                for (size64 Index = 0; Index < Other.QueueSize; ++Index)
                {
                    const size64 SourceIndex = (Other.HeadIndex + Index) % Other.QueueCapacity;
                    Enqueue(Other.Data[SourceIndex]);
                }
            }
        }
        return *this;
    }

    TQueue& operator=(TQueue&& Other) noexcept
    {
        if (this != &Other)
        {
            DestroyAndDeallocate();

            Data = Other.Data;
            QueueCapacity = Other.QueueCapacity;
            QueueSize = Other.QueueSize;
            HeadIndex = Other.HeadIndex;
            TailIndex = Other.TailIndex;

            Other.Data = nullptr;
            Other.QueueCapacity = 0;
            Other.QueueSize = 0;
            Other.HeadIndex = 0;
            Other.TailIndex = 0;
        }
        return *this;
    }

    TQueue& operator=(std::initializer_list<TElement> InitializerList)
    {
        Clear();
        if (InitializerList.size() > 0)
        {
            Reserve(InitializerList.size());
            for (const auto& Element : InitializerList)
            {
                Enqueue(Element);
            }
        }
        return *this;
    }

    auto operator<=>(const TQueue& Other) const noexcept requires CThreeWayComparable<TElement>
    {
        const size64 MinSize = QueueSize < Other.QueueSize ? QueueSize : Other.QueueSize;

        for (size64 Index = 0; Index < MinSize; ++Index)
        {
            const size64 ThisIndex = (HeadIndex + Index) % QueueCapacity;
            const size64 OtherIndex = (Other.HeadIndex + Index) % Other.QueueCapacity;

            if (auto Result = Data[ThisIndex] <=> Other.Data[OtherIndex]; Result != 0)
            {
                return Result;
            }
        }

        return QueueSize <=> Other.QueueSize;
    }

    bool8 operator==(const TQueue& Other) const noexcept requires CEqualityComparable<TElement>
    {
        if (QueueSize != Other.QueueSize)
        {
            return false;
        }
        for (size64 Index = 0; Index < QueueSize; ++Index)
        {
            const size64 ThisIndex = (HeadIndex + Index) % QueueCapacity;
            const size64 OtherIndex = (Other.HeadIndex + Index) % Other.QueueCapacity;

            if (!(Data[ThisIndex] == Other.Data[OtherIndex]))
            {
                return false;
            }
        }
        return true;
    }

public:
    void Enqueue(const TElement& Value)
    {
        EmplaceBack(Value);
    }

    void Enqueue(TElement&& Value)
    {
        EmplaceBack(std::move(Value));
    }

    template <typename... TArguments>
    TElement& EmplaceBack(TArguments&&... Arguments) requires std::is_constructible_v<TElement, TArguments...>
    {
        EnsureCapacity();
        TElement* NewElement = std::construct_at(&Data[TailIndex], std::forward<TArguments>(Arguments)...);
        TailIndex = (TailIndex + 1) % QueueCapacity;
        ++QueueSize;
        return *NewElement;
    }

    TElement Dequeue()
    {
        assert(!IsEmpty() && "Cannot dequeue from empty queue");
        TElement Result = std::move(Data[HeadIndex]);
        std::destroy_at(&Data[HeadIndex]);
        HeadIndex = (HeadIndex + 1) % QueueCapacity;
        --QueueSize;
        return Result;
    }

    [[nodiscard]] TElement& Front()
    {
        assert(!IsEmpty() && "Cannot access front of empty queue");
        return Data[HeadIndex];
    }

    [[nodiscard]] const TElement& Front() const
    {
        assert(!IsEmpty() && "Cannot access front of empty queue");
        return Data[HeadIndex];
    }

    [[nodiscard]] TElement& Back()
    {
        assert(!IsEmpty() && "Cannot access back of empty queue");
        const size64 BackIndex = (TailIndex + QueueCapacity - 1) % QueueCapacity;
        return Data[BackIndex];
    }

    [[nodiscard]] const TElement& Back() const
    {
        assert(!IsEmpty() && "Cannot access back of empty queue");
        const size64 BackIndex = (TailIndex + QueueCapacity - 1) % QueueCapacity;
        return Data[BackIndex];
    }

    void Reserve(const size64 NewCapacity)
    {
        if (NewCapacity > QueueCapacity)
        {
            TElement* NewData = static_cast<TElement*>(FMemory::Allocate((NewCapacity + 1) * sizeof(TElement)));
            if (Data != nullptr && QueueSize > 0)
            {
                for (size64 Index = 0; Index < QueueSize; ++Index)
                {
                    const size64 SourceIndex = (HeadIndex + Index) % QueueCapacity;
                    if constexpr (std::is_trivially_copyable_v<TElement>)
                    {
                        FMemory::Copy(&Data[SourceIndex], &NewData[Index], sizeof(TElement));
                    }
                    else
                    {
                        std::construct_at(&NewData[Index], std::move(Data[SourceIndex]));
                        std::destroy_at(&Data[SourceIndex]);
                    }
                }

                FMemory::Free(Data);
            }
            Data = NewData;
            QueueCapacity = NewCapacity + 1;
            HeadIndex = 0;
            TailIndex = QueueSize;
        }
    }

    void Clear()
    {
        while (!IsEmpty())
        {
            Dequeue();
        }
    }

    void Swap(TQueue& Other) noexcept
    {
        std::swap(Data, Other.Data);
        std::swap(QueueCapacity, Other.QueueCapacity);
        std::swap(QueueSize, Other.QueueSize);
        std::swap(HeadIndex, Other.HeadIndex);
        std::swap(TailIndex, Other.TailIndex);
    }

public:
    // Iterator Support
    [[nodiscard]] Iterator begin() noexcept
    {
        return Iterator(Data, 0, QueueCapacity, HeadIndex, QueueSize);
    }

    [[nodiscard]] ConstIterator begin() const noexcept
    {
        return ConstIterator(Data, 0, QueueCapacity, HeadIndex, QueueSize);
    }

    [[nodiscard]] Iterator end() noexcept
    {
        return Iterator(Data, QueueSize, QueueCapacity, HeadIndex, QueueSize);
    }

    [[nodiscard]] ConstIterator end() const noexcept
    {
        return ConstIterator(Data, QueueSize, QueueCapacity, HeadIndex, QueueSize);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        return ConstIterator(Data, 0, QueueCapacity, HeadIndex, QueueSize);
    }

    [[nodiscard]] ConstIterator cend() const noexcept
    {
        return ConstIterator(Data, QueueSize, QueueCapacity, HeadIndex, QueueSize);
    }

public:
    [[nodiscard]] size64 Num() const noexcept
    {
        return QueueSize;
    }

    [[nodiscard]] size64 GetCapacity() const noexcept
    {
        return QueueCapacity > 0 ? QueueCapacity - 1 : 0; // -1 for circular buffer gap
    }

    [[nodiscard]] size64 GetSizeInBytes() const noexcept
    {
        return QueueSize * sizeof(TElement);
    }

    [[nodiscard]] size64 GetCapacityInBytes() const noexcept
    {
        return GetCapacity() * sizeof(TElement);
    }

    [[nodiscard]] bool8 IsEmpty() const noexcept
    {
        return QueueSize == 0;
    }

    [[nodiscard]] bool8 IsFull() const noexcept
    {
        return QueueSize >= GetCapacity();
    }

    [[nodiscard]] TElement* GetData() noexcept
    {
        return Data;
    }

    [[nodiscard]] const TElement* GetData() const noexcept
    {
        return Data;
    }

private:
    void EnsureCapacity()
    {
        if (QueueSize >= GetCapacity())
        {
            const size64 NewCapacity = QueueCapacity == 0 ? DefaultCapacity : GetCapacity() * GrowthFactor;
            Reserve(NewCapacity);
        }
    }

    void DestroyAndDeallocate()
    {
        if (Data != nullptr)
        {
            Clear();
            FMemory::Free(Data);
            Data = nullptr;
        }
        QueueCapacity = 0;
        QueueSize = 0;
        HeadIndex = 0;
        TailIndex = 0;
    }

private:
    TElement* Data;
    size64 QueueCapacity;
    size64 QueueSize;
    size64 HeadIndex;
    size64 TailIndex;
};
