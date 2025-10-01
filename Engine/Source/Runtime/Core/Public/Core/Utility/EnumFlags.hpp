// RavenStorm Copyright @ 2025-2025

#pragma once

#include <type_traits>

#define REGISTER_AS_ENUM_FLAG(EnumName) \
    template<> \
    [[nodiscard]] constexpr bool8 IsEnumFlag(EnumName) { return true; }

template <typename T>
[[nodiscard]] constexpr bool8 IsEnumFlag(T)
{
    return false;
}

template <typename T>
concept CIsUnsignedEnum = std::is_enum_v<T> && std::is_unsigned_v<std::underlying_type_t<T>>;

template <typename T>
concept CIsEnumFlag = CIsUnsignedEnum<T> && IsEnumFlag(T{});

template <typename T>
[[nodiscard]] bool8 HasEnumFlag(T Value, T Flag)
{
    return (Value & Flag) == Flag;
}

template <typename T>
[[nodiscard]] bool8 HasEnumFlag(T Value, std::underlying_type_t<T> Flag)
{
    return (static_cast<std::underlying_type_t<T>>(Value) & Flag) == Flag;
}

template <typename T>
[[nodiscard]] bool8 HasEnumFlag(std::underlying_type_t<T> Value, T Flag)
{
    return (Value & static_cast<std::underlying_type_t<T>>(Flag)) == static_cast<std::underlying_type_t<T>>(Flag);
}

template <typename T> requires CIsEnumFlag<T>
constexpr T operator~(T Value)
{
    return static_cast<T>(~static_cast<std::underlying_type_t<T>>(Value));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator|(T Left, T Right)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(Left) | static_cast<std::underlying_type_t<T>>(Right));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator|(std::underlying_type_t<T> Left, T Right)
{
    return static_cast<std::underlying_type_t<T>>(Left | static_cast<std::underlying_type_t<T>>(Right));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator|(T Left, std::underlying_type_t<T> Right)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(Left) | Right);
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator|(std::underlying_type_t<T>& Left, const T Right)
{
    return Left = Left | Right;
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator&(T Left, T Right)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(Left) & static_cast<std::underlying_type_t<T>>(Right));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator&(std::underlying_type_t<T> Left, T Right)
{
    return static_cast<std::underlying_type_t<T>>(Left & static_cast<std::underlying_type_t<T>>(Right));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator&(T Left, std::underlying_type_t<T> Right)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(Left) & Right);
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator&=(std::underlying_type_t<T>& Left, const T Right)
{
    return Left = Left & Right;
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator^(T Left, T Right)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(Left) ^ static_cast<std::underlying_type_t<T>>(Right));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator^(std::underlying_type_t<T> Left, T Right)
{
    return static_cast<std::underlying_type_t<T>>(Left ^ static_cast<std::underlying_type_t<T>>(Right));
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator^(T Left, std::underlying_type_t<T> Right)
{
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(Left) ^ Right);
}

template <typename T> requires CIsEnumFlag<T>
constexpr auto operator^=(std::underlying_type_t<T>& Left, const T Right)
{
    return Left = Left ^ Right;
}
