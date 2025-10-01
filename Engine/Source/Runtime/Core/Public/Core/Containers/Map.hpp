// RavenStorm Copyright @ 2025-2025

#pragma once

#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>

#include "Core/Memory/Memory.hpp"

template <typename TKey>
concept CValidMapKey = std::is_object_v<TKey> && !std::is_abstract_v<TKey> &&
    requires(const TKey& A, const TKey& B)
    {
        { A == B } -> std::convertible_to<bool>;
        { std::hash<TKey>{}(A) } -> std::convertible_to<size64>;
    };

template <typename TValue>
concept CValidMapValue = std::is_object_v<TValue> && !std::is_abstract_v<TValue>;

template <CValidMapKey TKey, CValidMapValue TValue, typename THasher = std::hash<TKey>, typename TComparer = std::equal_to<TKey>>
class TMap
{
public:
    using KeyType = TKey;
    using ValueType = std::pair<const TKey, TValue>;
    using MappedType = TValue;
    using SizeType = size64;
    using HasherType = THasher;
    using ComparerType = TComparer;

private:
    static constexpr size64 DefaultBucketCount = 16;
    static constexpr float64 MaxLoadFactor = 0.75;

    struct FNode
    {
        ValueType Data;
        FNode* NextNode;

        template <typename... TArguments>
        FNode(FNode* InNextNode, TArguments&&... Arguments)
            : Data(std::forward<TArguments>(Arguments)...), NextNode(InNextNode)
        {
        }
    };

    template <typename TNodePointer, typename TValueReference>
    class TMapIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ValueType;
        using difference_type = ptrdiff_t;
        using pointer = std::remove_reference_t<TValueReference>*;
        using reference = TValueReference;

    public:
        constexpr TMapIterator()
            : CurrentNode(nullptr), Buckets(nullptr), BucketCount(0), CurrentBucket(0)
        {
        }

        constexpr TMapIterator(TNodePointer InNode, FNode** InBuckets, const size64 InBucketCount, const size64 InCurrentBucket) noexcept
            : CurrentNode(InNode), Buckets(InBuckets), BucketCount(InBucketCount), CurrentBucket(InCurrentBucket)
        {
        }

        template <typename TOtherNodePointer, typename TOtherValueReference>
        constexpr TMapIterator(const TMapIterator<TOtherNodePointer, TOtherValueReference>& Other) noexcept requires std::convertible_to<TOtherNodePointer, TNodePointer>
            : CurrentNode(Other.CurrentNode), Buckets(Other.Buckets), BucketCount(Other.BucketCount), CurrentBucket(Other.CurrentBucket)
        {
        }

    public:
        constexpr reference operator*() const
        {
            assert(CurrentNode != nullptr && "Cannot dereference end iterator");
            return CurrentNode->Data;
        }

        constexpr pointer operator->() const
        {
            assert(CurrentNode != nullptr && "Cannot access end iterator");
            return &CurrentNode->Data;
        }

        constexpr TMapIterator& operator++()
        {
            assert(CurrentNode != nullptr && "Cannot increment end iterator");
            CurrentNode = CurrentNode->NextNode;
            if (CurrentNode == nullptr)
            {
                ++CurrentBucket;
                while (CurrentBucket < BucketCount && Buckets[CurrentBucket] == nullptr)
                {
                    ++CurrentBucket;
                }
                if (CurrentBucket < BucketCount)
                {
                    CurrentNode = Buckets[CurrentBucket];
                }
            }
            return *this;
        }

        constexpr TMapIterator operator++(int)
        {
            TMapIterator temp = *this;
            ++(*this);
            return temp;
        }

        constexpr bool8 operator==(const TMapIterator& Other) const noexcept
        {
            return CurrentNode == Other.CurrentNode;
        }

        constexpr auto operator<=>(const TMapIterator& Other) const noexcept
        {
            return CurrentNode <=> Other.CurrentNode;
        }

    public:
        TNodePointer CurrentNode;
        FNode** Buckets;
        size64 BucketCount;
        size64 CurrentBucket;

        template <typename, typename>
        friend class TMapIterator;
    };

public:
    using Iterator = TMapIterator<FNode*, ValueType&>;
    using ConstIterator = TMapIterator<const FNode*, const ValueType&>;

public:
    constexpr TMap()
        : Buckets(nullptr), BucketCount(0), ElementCount(0), Hasher(), Comparer()
    {
        InitializeBuckets(DefaultBucketCount);
    }

    explicit TMap(const size64 InBucketCount)
        : Buckets(nullptr), BucketCount(0), ElementCount(0), Hasher(), Comparer()
    {
        const size64 ActualBucketCount = InBucketCount > 0 ? InBucketCount : DefaultBucketCount;
        InitializeBuckets(ActualBucketCount);
    }

    TMap(std::initializer_list<ValueType> InInitializerList)
        : Buckets(nullptr), BucketCount(0), ElementCount(0), Hasher(), Comparer()
    {
        const size64 EstimatedBuckets = InInitializerList.size() > DefaultBucketCount ? InInitializerList.size() * 2 : DefaultBucketCount;
        InitializeBuckets(EstimatedBuckets);
        for (const auto& Pair : InInitializerList)
        {
            Insert(Pair);
        }
    }

    TMap(const TMap& Other)
        : Buckets(nullptr), BucketCount(0), ElementCount(0), Hasher(Other.Hasher), Comparer(Other.Comparer)
    {
        InitializeBuckets(Other.BucketCount);
        for (const auto& Pair : Other)
        {
            Insert(Pair);
        }
    }

    TMap(TMap&& Other) noexcept
        : Buckets(Other.Buckets), BucketCount(Other.BucketCount), ElementCount(Other.ElementCount), Hasher(std::move(Other.Hasher)), Comparer(std::move(Other.Comparer))
    {
        Other.Buckets = nullptr;
        Other.BucketCount = 0;
        Other.ElementCount = 0;
    }

    ~TMap()
    {
        DestroyAndDeallocate();
    }

public:
    TMap& operator=(const TMap& Other)
    {
        if (this != &Other)
        {
            Clear();
            if (BucketCount != Other.BucketCount)
            {
                DestroyAndDeallocate();
                InitializeBuckets(Other.BucketCount);
            }
            Hasher = Other.Hasher;
            Comparer = Other.Comparer;
            for (const auto& Pair : Other)
            {
                Insert(Pair);
            }
        }
        return *this;
    }

    TMap& operator=(TMap&& Other) noexcept
    {
        if (this != &Other)
        {
            DestroyAndDeallocate();
            Buckets = Other.Buckets;
            BucketCount = Other.BucketCount;
            ElementCount = Other.ElementCount;
            Hasher = std::move(Other.Hasher);
            Comparer = std::move(Other.Comparer);
            Other.Buckets = nullptr;
            Other.BucketCount = 0;
            Other.ElementCount = 0;
        }
        return *this;
    }

    TMap& operator=(std::initializer_list<ValueType> InInitializerList)
    {
        Clear();
        for (const auto& Pair : InInitializerList)
        {
            Insert(Pair);
        }
        return *this;
    }

    TValue& operator[](const TKey& Key)
    {
        return EmplaceOrGet(Key).first->second;
    }

    TValue& operator[](TKey&& Key)
    {
        return EmplaceOrGet(std::move(Key)).first->second;
    }

public:
    std::pair<Iterator, bool8> Insert(const ValueType& Pair)
    {
        return EmplaceImpl(Pair.first, Pair.second);
    }

    std::pair<Iterator, bool8> Insert(ValueType&& Pair)
    {
        return EmplaceImpl(std::move(Pair.first), std::move(Pair.second));
    }

    template <typename... TArguments>
    std::pair<Iterator, bool8> Emplace(TArguments&&... Arguments)
        requires std::is_constructible_v<ValueType, TArguments...>
    {
        ValueType TempPair(std::forward<TArguments>(Arguments)...);
        return EmplaceImpl(std::move(TempPair.first), std::move(TempPair.second));
    }

    Iterator Find(const TKey& Key)
    {
        if (BucketCount == 0)
        {
            return end();
        }
        const size64 BucketIndex = GetBucketIndex(Key);
        FNode* CurrentNode = Buckets[BucketIndex];
        while (CurrentNode != nullptr)
        {
            if (Comparer(CurrentNode->Data.first, Key))
            {
                return Iterator(CurrentNode, Buckets, BucketCount, BucketIndex);
            }
            CurrentNode = CurrentNode->NextNode;
        }
        return end();
    }

    ConstIterator Find(const TKey& Key) const
    {
        if (BucketCount == 0)
        {
            return end();
        }
        const size64 BucketIndex = GetBucketIndex(Key);
        const FNode* CurrentNode = Buckets[BucketIndex];
        while (CurrentNode != nullptr)
        {
            if (Comparer(CurrentNode->Data.first, Key))
            {
                return ConstIterator(CurrentNode, Buckets, BucketCount, BucketIndex);
            }
            CurrentNode = CurrentNode->NextNode;
        }
        return end();
    }

    bool Contains(const TKey& Key) const
    {
        return Find(Key) != end();
    }

    size64 Remove(const TKey& Key)
    {
        if (BucketCount == 0)
        {
            return 0;
        }
        const size64 BucketIndex = GetBucketIndex(Key);
        FNode** CurrentNodePtr = &Buckets[BucketIndex];
        while (*CurrentNodePtr != nullptr)
        {
            FNode* CurrentNode = *CurrentNodePtr;
            if (Comparer(CurrentNode->Data.first, Key))
            {
                *CurrentNodePtr = CurrentNode->NextNode;
                DestroyNode(CurrentNode);
                --ElementCount;
                return 1;
            }
            CurrentNodePtr = &CurrentNode->NextNode;
        }
        return 0;
    }

    Iterator Remove(ConstIterator Position)
    {
        assert(Position != end() && "Cannot remove end iterator");

        const TKey& Key = Position->first;
        const size64 BucketIndex = GetBucketIndex(Key);
        FNode** CurrentNodePtr = &Buckets[BucketIndex];
        while (*CurrentNodePtr != nullptr)
        {
            FNode* CurrentNode = *CurrentNodePtr;
            if (CurrentNode == Position.CurrentNode)
            {
                FNode* NextNode = CurrentNode->NextNode;
                *CurrentNodePtr = NextNode;
                DestroyNode(CurrentNode);
                --ElementCount;
                if (NextNode != nullptr)
                {
                    return Iterator(NextNode, Buckets, BucketCount, BucketIndex);
                }
                size64 NextBucket = BucketIndex + 1;
                while (NextBucket < BucketCount && Buckets[NextBucket] == nullptr)
                {
                    ++NextBucket;
                }

                if (NextBucket < BucketCount)
                {
                    return Iterator(Buckets[NextBucket], Buckets, BucketCount, NextBucket);
                }
                return end();
            }
            CurrentNodePtr = &CurrentNode->NextNode;
        }
        return end();
    }

    void Clear()
    {
        for (size64 BucketIndex = 0; BucketIndex < BucketCount; ++BucketIndex)
        {
            FNode* CurrentNode = Buckets[BucketIndex];
            while (CurrentNode != nullptr)
            {
                FNode* NextNode = CurrentNode->NextNode;
                DestroyNode(CurrentNode);
                CurrentNode = NextNode;
            }
            Buckets[BucketIndex] = nullptr;
        }
        ElementCount = 0;
    }

    void Reserve(const size64 ExpectedElements)
    {
        const size64 NeededBuckets = static_cast<size64>(static_cast<float64>(ExpectedElements) / MaxLoadFactor) + 1;
        if (NeededBuckets > BucketCount)
        {
            Rehash(NeededBuckets);
        }
    }

    void Swap(TMap& Other) noexcept
    {
        std::swap(Buckets, Other.Buckets);
        std::swap(BucketCount, Other.BucketCount);
        std::swap(ElementCount, Other.ElementCount);
        std::swap(Hasher, Other.Hasher);
        std::swap(Comparer, Other.Comparer);
    }

    size64 Num() const noexcept
    {
        return ElementCount;
    }

    size64 GetBucketCount() const noexcept
    {
        return BucketCount;
    }

    bool8 IsEmpty() const noexcept
    {
        return ElementCount == 0;
    }

    float64 GetLoadFactor() const noexcept
    {
        return BucketCount > 0 ? static_cast<float64>(ElementCount) / static_cast<float64>(BucketCount) : 0.0;
    }

    static constexpr float64 GetMaxLoadFactor() noexcept
    {
        return MaxLoadFactor;
    }

public:
    // Iterator Support
    Iterator begin() noexcept
    {
        if (ElementCount == 0)
        {
            return end();
        }
        for (size64 BucketIndex = 0; BucketIndex < BucketCount; ++BucketIndex)
        {
            if (Buckets[BucketIndex] != nullptr)
            {
                return Iterator(Buckets[BucketIndex], Buckets, BucketCount, BucketIndex);
            }
        }
        return end();
    }

    ConstIterator begin() const noexcept
    {
        if (ElementCount == 0)
        {
            return end();
        }
        for (size64 BucketIndex = 0; BucketIndex < BucketCount; ++BucketIndex)
        {
            if (Buckets[BucketIndex] != nullptr)
            {
                return ConstIterator(Buckets[BucketIndex], Buckets, BucketCount, BucketIndex);
            }
        }
        return end();
    }

    Iterator end() noexcept
    {
        return Iterator(nullptr, Buckets, BucketCount, BucketCount);
    }

    ConstIterator end() const noexcept
    {
        return ConstIterator(nullptr, Buckets, BucketCount, BucketCount);
    }

    ConstIterator cbegin() const noexcept
    {
        return begin();
    }

    ConstIterator cend() const noexcept
    {
        return end();
    }

private:
    void InitializeBuckets(const size64 Count)
    {
        BucketCount = Count;
        Buckets = static_cast<FNode**>(FMemory::Allocate(BucketCount * sizeof(FNode*)));
        for (size64 Index = 0; Index < BucketCount; ++Index)
        {
            Buckets[Index] = nullptr;
        }
    }

    void DestroyAndDeallocate()
    {
        if (Buckets != nullptr)
        {
            Clear();
            FMemory::Free(static_cast<void*>(Buckets));
            Buckets = nullptr;
        }
        BucketCount = 0;
        ElementCount = 0;
    }

    size64 GetBucketIndex(const TKey& Key) const
    {
        return Hasher(Key) % BucketCount;
    }

    FNode* CreateNode(FNode* NextNode, const TKey& Key, const TValue& Value)
    {
        FNode* NewNode = static_cast<FNode*>(FMemory::Allocate(sizeof(FNode)));
        return std::construct_at(NewNode, NextNode, Key, Value);
    }

    FNode* CreateNode(FNode* NextNode, const TKey& Key, TValue&& Value)
    {
        FNode* NewNode = static_cast<FNode*>(FMemory::Allocate(sizeof(FNode)));
        return std::construct_at(NewNode, NextNode, Key, std::move(Value));
    }

    FNode* CreateNode(FNode* NextNode, TKey&& Key, const TValue& Value)
    {
        FNode* NewNode = static_cast<FNode*>(FMemory::Allocate(sizeof(FNode)));
        return std::construct_at(NewNode, NextNode, std::move(Key), Value);
    }

    FNode* CreateNode(FNode* NextNode, TKey&& Key, TValue&& Value)
    {
        FNode* NewNode = static_cast<FNode*>(FMemory::Allocate(sizeof(FNode)));
        return std::construct_at(NewNode, NextNode, std::move(Key), std::move(Value));
    }

    void DestroyNode(FNode* NodeToDestroy)
    {
        std::destroy_at(NodeToDestroy);
        FMemory::Free(NodeToDestroy);
    }

    template <typename TKeyArg, typename TValueArg>
    std::pair<Iterator, bool> EmplaceImpl(TKeyArg&& Key, TValueArg&& Value)
    {
        CheckLoadFactorAndRehash();
        const size64 BucketIndex = GetBucketIndex(Key);
        FNode* CurrentNode = Buckets[BucketIndex];
        while (CurrentNode != nullptr)
        {
            if (Comparer(CurrentNode->Data.first, Key))
            {
                return std::make_pair(Iterator(CurrentNode, Buckets, BucketCount, BucketIndex), false);
            }
            CurrentNode = CurrentNode->NextNode;
        }
        FNode* NewNode = CreateNode(Buckets[BucketIndex], std::forward<TKeyArg>(Key), std::forward<TValueArg>(Value));
        Buckets[BucketIndex] = NewNode;
        ++ElementCount;
        return std::make_pair(Iterator(NewNode, Buckets, BucketCount, BucketIndex), true);
    }

    template <typename TKeyArg>
    std::pair<Iterator, bool> EmplaceOrGet(TKeyArg&& Key)
    {
        CheckLoadFactorAndRehash();
        const size64 BucketIndex = GetBucketIndex(Key);
        FNode* CurrentNode = Buckets[BucketIndex];
        while (CurrentNode != nullptr)
        {
            if (Comparer(CurrentNode->Data.first, Key))
            {
                return std::make_pair(Iterator(CurrentNode, Buckets, BucketCount, BucketIndex), false);
            }
            CurrentNode = CurrentNode->NextNode;
        }
        FNode* NewNode = CreateNode(Buckets[BucketIndex], std::forward<TKeyArg>(Key), TValue{});
        Buckets[BucketIndex] = NewNode;
        ++ElementCount;
        return std::make_pair(Iterator(NewNode, Buckets, BucketCount, BucketIndex), true);
    }

    void CheckLoadFactorAndRehash()
    {
        if (GetLoadFactor() > MaxLoadFactor)
        {
            Rehash(BucketCount * 2);
        }
    }

    void Rehash(const size64 NewBucketCount)
    {
        FNode** OldBuckets = Buckets;
        const size64 OldBucketCount = BucketCount;
        InitializeBuckets(NewBucketCount);
        ElementCount = 0;
        for (size64 BucketIndex = 0; BucketIndex < OldBucketCount; ++BucketIndex)
        {
            FNode* CurrentNode = OldBuckets[BucketIndex];
            while (CurrentNode != nullptr)
            {
                FNode* NextNode = CurrentNode->NextNode;
                const size64 NewBucketIndex = GetBucketIndex(CurrentNode->Data.first);
                CurrentNode->NextNode = Buckets[NewBucketIndex];
                Buckets[NewBucketIndex] = CurrentNode;
                ++ElementCount;
                CurrentNode = NextNode;
            }
        }
        FMemory::Free(static_cast<void*>(OldBuckets));
    }

private:
    FNode** Buckets;
    size64 BucketCount;
    size64 ElementCount;
    THasher Hasher;
    TComparer Comparer;
};

template <typename... TArguments>
TMap(TArguments...) -> TMap<
    typename std::tuple_element_t<0, std::tuple<TArguments...>>::first_type,
    typename std::tuple_element_t<0, std::tuple<TArguments...>>::second_type
>;
