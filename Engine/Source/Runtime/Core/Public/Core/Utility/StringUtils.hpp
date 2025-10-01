// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/Containers/String.hpp"

#include <type_traits>

namespace StringUtils
{
    template <typename... TArguments>
    FString Format(TFormatString<TArguments...> FormatString, TArguments&&... Arguments)
    {
        return std::format(FormatString, std::forward<TArguments>(Arguments)...);
    }

    template <typename... TArguments>
    FAnsiString FormatAnsi(TAnsiFormatString<TArguments...> FormatString, TArguments&&... Arguments)
    {
        return std::format(FormatString, std::forward<TArguments>(Arguments)...);
    }

    template <typename... TArguments>
    FWideString FormatWide(TWideFormatString<TArguments...> FormatString, TArguments&&... Arguments)
    {
        return std::format(FormatString, std::forward<TArguments>(Arguments)...);
    }

    CORE_API FAnsiString ToAnsiString(const FWideChar* String);
    CORE_API FAnsiString ToAnsiString(const FWideString& String);
    CORE_API FAnsiString ToAnsiString(const FWideStringView& String);

    CORE_API FWideString ToWideString(const FAnsiChar* String);
    CORE_API FWideString ToWideString(const FAnsiString& String);
    CORE_API FWideString ToWideString(const FAnsiStringView& String);
}
