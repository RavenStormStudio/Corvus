// RavenStorm Copyright @ 2025-2025

#pragma once

#include <compare>
#include <type_traits>

template <typename T>
concept CThreeWayComparable = requires(const T& a, const T& b)
{
    { a <=> b } -> std::convertible_to<std::partial_ordering>;
};

template <typename T>
concept CEqualityComparable = requires(const T& a, const T& b)
{
    { a == b } -> std::convertible_to<bool8>;
};
