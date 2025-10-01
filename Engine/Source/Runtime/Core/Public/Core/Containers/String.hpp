// RavenStorm Copyright @ 2025-2025

#pragma once

#include <format>
#include <string>

using FAnsiChar = char;
using FAnsiString = std::string;
using FAnsiStringView = std::string_view;

template <typename... TArguments>
using TAnsiFormatString = std::format_string<TArguments...>;

using FWideChar = wchar_t;
using FWideString = std::wstring;
using FWideStringView = std::wstring_view;

template <typename... TArguments>
using TWideFormatString = std::wformat_string<TArguments...>;

#ifndef CV_USE_ANSI_STRINGS
#   define CV_USE_ANSI_STRINGS 1
#endif

#if CV_USE_ANSI_STRINGS
using FChar = FAnsiChar;
using FString = FAnsiString;
using FStringView = FAnsiStringView;

template <typename... TArguments>
using TFormatString = TAnsiFormatString<TArguments...>;
#else
using FChar = FWideChar;
using FString = FWideString;
using FStringView = FWideStringView;

template <typename... TArguments>
using TFormatString = TWideFormatString<TArguments...>;
#endif
