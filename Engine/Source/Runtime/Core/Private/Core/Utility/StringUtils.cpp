// RavenStorm Copyright @ 2025-2025

#include "Core/Utility/StringUtils.hpp"

#include <Windows.h>

FAnsiString StringUtils::ToAnsiString(const FWideChar* String)
{
    if (!String || *String == L'\0')
    {
        return {};
    }
    const int32 Length = WideCharToMultiByte(CP_UTF8, 0, String, -1, nullptr, 0, nullptr, nullptr);
    if (Length <= 0)
    {
        return {};
    }
    FAnsiString Result(Length - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, String, -1, Result.data(), Length, nullptr, nullptr);
    return Result;
}

FAnsiString StringUtils::ToAnsiString(const FWideString& String)
{
    if (String.empty())
    {
        return {};
    }
    const int32 Length = WideCharToMultiByte(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), nullptr, 0, nullptr, nullptr);
    if (Length <= 0)
    {
        return {};
    }
    FAnsiString Result(Length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), Result.data(), Length, nullptr, nullptr);
    return Result;
}

FAnsiString StringUtils::ToAnsiString(const FWideStringView& String)
{
    if (String.empty())
    {
        return {};
    }
    const int32 Length = WideCharToMultiByte(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), nullptr, 0, nullptr, nullptr);
    if (Length <= 0)
    {
        return {};
    }
    FAnsiString Result(Length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), Result.data(), Length, nullptr, nullptr);
    return Result;
}

FWideString StringUtils::ToWideString(const FAnsiChar* String)
{
    if (!String || *String == '\0')
    {
        return {};
    }
    const int32 Length = MultiByteToWideChar(CP_UTF8, 0, String, -1, nullptr, 0);
    if (Length <= 0)
    {
        return {};
    }
    FWideString Result(Length - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, String, -1, Result.data(), Length);
    return Result;
}

FWideString StringUtils::ToWideString(const FAnsiString& String)
{
    if (String.empty())
    {
        return {};
    }
    const int32 Length = MultiByteToWideChar(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), nullptr, 0);
    if (Length <= 0)
    {
        return {};
    }
    FWideString Result(Length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), Result.data(), Length);
    return Result;
}

FWideString StringUtils::ToWideString(const FAnsiStringView& String)
{
    if (String.empty())
    {
        return {};
    }
    const int32 Length = MultiByteToWideChar(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), nullptr, 0);
    if (Length <= 0)
    {
        return {};
    }
    FWideString Result(Length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, String.data(), static_cast<int32>(String.size()), Result.data(), Length);
    return Result;
}
