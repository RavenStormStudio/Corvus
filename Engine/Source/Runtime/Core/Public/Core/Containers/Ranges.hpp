// RavenStorm Copyright @ 2025-2025

#pragma once

#include <functional>
#include <type_traits>

template <typename TRange>
concept CRange = requires(TRange Range)
{
    { Range.begin() } -> std::input_or_output_iterator;
    { Range.end() } -> std::input_or_output_iterator;
    requires std::same_as<decltype(Range.begin()), decltype(Range.end())>;
};

template <typename TRange>
concept CRangeWithValueType = CRange<TRange> && requires
{
    typename TRange::ValueType;
};

template <typename TPredicate, typename TElement>
concept CRangeBoolPredicate = std::is_invocable_v<TPredicate, TElement> && std::convertible_to<std::invoke_result_t<TPredicate, TElement>, bool8>;

template <typename TPredicate, typename TElement>
concept CRangeTransformPredicate = std::is_invocable_v<TPredicate, TElement> && std::convertible_to<std::invoke_result_t<TPredicate, TElement>, TElement>;

namespace Ranges
{
    template <typename TRange, typename TFunc> requires CRangeWithValueType<std::remove_cvref_t<TRange>>
    constexpr void ForEach(TRange&& Range, const TFunc& Function)
    {
        for (auto&& Element : std::forward<TRange>(Range))
        {
            std::invoke(Function, Element);
        }
    }

    template <typename TRange, typename TFunc> requires CRangeWithValueType<std::remove_cvref_t<TRange>>
    constexpr void ForEach(const TRange& Range, const TFunc& Function)
    {
        for (const auto& Element : Range)
        {
            std::invoke(Function, Element);
        }
    }

    template <typename TRange, typename TPredicate> requires CRangeWithValueType<std::remove_cvref_t<TRange>> && CRangeBoolPredicate<TPredicate, typename std::remove_cvref_t<TRange>::ValueType>
    [[nodiscard]] constexpr bool8 AllOf(const TRange& Range, const TPredicate& Predicate)
    {
        for (const auto& Element : Range)
        {
            if (!std::invoke(Predicate, Element))
            {
                return false;
            }
        }
        return true;
    }

    template <typename TRange, typename TPredicate> requires CRangeWithValueType<std::remove_cvref_t<TRange>> && CRangeBoolPredicate<TPredicate, typename std::remove_cvref_t<TRange>::ValueType>
    [[nodiscard]] constexpr bool8 AnyOf(const TRange& Range, const TPredicate& Predicate)
    {
        for (const auto& Element : Range)
        {
            if (std::invoke(Predicate, Element))
            {
                return true;
            }
        }
        return false;
    }

    template <typename TRange, typename TPredicate> requires CRangeWithValueType<std::remove_cvref_t<TRange>> && CRangeBoolPredicate<TPredicate, typename std::remove_cvref_t<TRange>::ValueType>
    [[nodiscard]] constexpr size64 CountIf(const TRange& Range, const TPredicate& Predicate)
    {
        size64 Count = 0;
        for (const auto& Element : Range)
        {
            if (std::invoke(Predicate, Element))
            {
                ++Count;
            }
        }
        return Count;
    }

    template <typename TRange>
        requires CRangeWithValueType<std::remove_cvref_t<TRange>> && requires
        {
            requires requires(const typename std::remove_cvref_t<TRange>::ValueType& a,
                              const typename std::remove_cvref_t<TRange>::ValueType& b)
            {
                { a == b } -> std::convertible_to<bool8>;
            };
        }
    [[nodiscard]] constexpr size64 Find(const TRange& Range, const typename std::remove_cvref_t<TRange>::ValueType& Value)
    {
        size64 Index = 0;
        for (const auto& Element : Range)
        {
            if (Element == Value)
            {
                return Index;
            }
            ++Index;
        }
        return static_cast<size64>(-1);
    }

    template <typename TRange, typename TPredicate> requires CRangeWithValueType<std::remove_cvref_t<TRange>> && CRangeBoolPredicate<TPredicate, typename std::remove_cvref_t<TRange>::ValueType>
    [[nodiscard]] constexpr size64 FindIf(const TRange& Range, const TPredicate& Predicate)
    {
        size64 Index = 0;
        for (const auto& Element : Range)
        {
            if (std::invoke(Predicate, Element))
            {
                return Index;
            }
            ++Index;
        }
        return static_cast<size64>(-1);
    }

    template <typename TRange>
        requires CRangeWithValueType<std::remove_cvref_t<TRange>> && requires
        {
            requires requires(const typename std::remove_cvref_t<TRange>::ValueType& a,
                              const typename std::remove_cvref_t<TRange>::ValueType& b)
            {
                { a == b } -> std::convertible_to<bool8>;
            };
        }
    [[nodiscard]] constexpr bool8 Contains(const TRange& Range, const typename std::remove_cvref_t<TRange>::ValueType& Value)
    {
        return Find(Range, Value) != static_cast<size64>(-1);
    }

    template <typename TRange, typename TPredicate> requires CRangeWithValueType<std::remove_cvref_t<TRange>> && CRangeBoolPredicate<TPredicate, typename std::remove_cvref_t<TRange>::ValueType>
    [[nodiscard]] constexpr bool8 ContainsIf(const TRange& Range, const TPredicate& Predicate)
    {
        return FindIf(Range, Predicate) != static_cast<size64>(-1);
    }

    template <typename TRange> requires CRangeWithValueType<std::remove_cvref_t<TRange>>
    constexpr void Fill(TRange&& Range, const typename std::remove_cvref_t<TRange>::ValueType& Value)
    {
        for (auto&& Element : std::forward<TRange>(Range))
        {
            Element = Value;
        }
    }

    template <typename TRange, typename TTransform> requires CRangeWithValueType<std::remove_cvref_t<TRange>> && CRangeTransformPredicate<TTransform, typename std::remove_cvref_t<TRange>::ValueType>
    constexpr void Transform(TRange&& Range, const TTransform& Transform)
    {
        for (auto&& Element : std::forward<TRange>(Range))
        {
            Element = std::invoke(Transform, Element);
        }
    }
}
