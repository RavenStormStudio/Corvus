#pragma once

#include "Engine/Core/Containers/String.hpp"

template <typename T>
struct TypeName
{
    static constexpr auto Get() noexcept
    {
        constexpr FStringView Name = __FUNCSIG__;
        constexpr FStringView Prefix = "auto __cdecl TypeName<";
        constexpr FStringView Suffix = ">::Get(void) noexcept";

        constexpr size64 Start = Name.find(Prefix) + Prefix.length();
        constexpr size64 End = Name.rfind(Suffix);
        constexpr FStringView RawName = Name.substr(Start, End - Start);

        constexpr size64 SkipChars = StartsWith(RawName, "class ") ? 6 : StartsWith(RawName, "struct ") ? 7 : 0;
        constexpr auto CleanName = RawName.substr(SkipChars);

        constexpr size64 LastColon = CleanName.rfind("::");
        if constexpr (LastColon != FStringView::npos)
        {
            return CleanName.substr(LastColon + 2);
        }
        return CleanName;
    }

private:
    static constexpr bool StartsWith(const FStringView String, const FStringView Prefix) noexcept
    {
        if (String.size() < Prefix.size())
        {
            return false;
        }
        for (size64 Index = 0; Index < Prefix.size(); ++Index)
        {
            if (String[Index] != Prefix[Index])
            {
                return false;
            }
        }
        return true;
    }
};

template <typename T>
constexpr auto GetTypeNameWithoutPrefix()
{
    constexpr auto FullName = TypeName<T>::Get();
    if constexpr (FullName.size() > 1)
    {
        constexpr auto WithoutPrefix = FullName.substr(1);
        return WithoutPrefix;
    }
    return FullName;
}
