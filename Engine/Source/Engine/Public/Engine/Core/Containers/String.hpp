#pragma once

#include <format>
#include <sstream>
#include <string>
#include <Windows.h>

#include "Engine/Core/CoreTypes.hpp"

using FChar = char;
using FString = std::string;
using FStringView = std::string_view;
using FStringStream = std::stringstream;

template <typename... TArguments>
using TFormatString = std::format_string<TArguments...>;

using FWideChar = wchar_t;
using FWideString = std::wstring;
using FWideStringView = std::wstring_view;
using FWideStringStream = std::wstringstream;

template <typename... TArguments>
using TWideFormatString = std::wformat_string<TArguments...>;

namespace String
{
    template <typename... TArguments>
    constexpr FString Format(TFormatString<TArguments...> Format, TArguments&&... Arguments)
    {
        return std::format<TArguments...>(Format, std::forward<TArguments>(Arguments)...);
    }

    template <typename... TArguments>
    constexpr FWideString Format(TWideFormatString<TArguments...> Format, TArguments&&... Arguments)
    {
        return std::format<TArguments...>(Format, std::forward<TArguments>(Arguments)...);
    }

    inline FWideString Convert(const FString& String)
    {
        if (String.empty())
        {
            return {};
        }
        const int32 Size = MultiByteToWideChar(CP_UTF8, 0, String.c_str(), static_cast<int32>(String.size()), nullptr, 0);
        FWideString Result(Size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, String.c_str(), static_cast<int32>(String.size()), Result.data(), Size);
        return Result;
    }

    inline FString Convert(const FWideString& String)
    {
        if (String.empty())
        {
            return {};
        }
        const int32 Size = WideCharToMultiByte(CP_UTF8, 0, String.c_str(), static_cast<int>(String.size()), nullptr, 0, nullptr, nullptr);
        FString Result(Size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, String.c_str(), static_cast<int>(String.size()), Result.data(), Size, nullptr, nullptr);
        return Result;
    }
}
