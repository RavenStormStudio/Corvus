#include "Engine/Core/Utility/Guid.hpp"

#include <combaseapi.h>

int32 ParseHexDigit(const FChar Character)
{
    int32 Hex;
    if (Character >= '0' && Character <= '9')
    {
        Hex = Character - '0';
    }
    else if (Character >= 'a' && Character <= 'f')
    {
        Hex = Character + 10 - 'a';
    }
    else if (Character >= 'A' && Character <= 'F')
    {
        Hex = Character + 10 - 'A';
    }
    else
    {
        Hex = 0;
    }
    return Hex;
}

uint32 ParseHexNumber(const FStringView String)
{
    uint32 Hex = 0;
    for (const FChar HexChar : String)
    {
        Hex *= 16;
        Hex += ParseHexDigit(HexChar);
    }
    return Hex;
}

FGuid FGuid::NewGuid()
{
    FGuid Guid;
    CoCreateGuid(reinterpret_cast<GUID*>(&Guid));
    return Guid;
}

bool8 FGuid::Parse(const FChar* String, FGuid& OutGuid)
{
    return Parse(FStringView(String), OutGuid);
}

bool8 FGuid::Parse(const FString& String, FGuid& OutGuid)
{
    return Parse(FStringView(String), OutGuid);
}

bool8 FGuid::Parse(const FStringView String, FGuid& OutGuid)
{
    if (String.length() != 36 || String[8] != '-' || String[13] != '-' || String[18] != '-' || String[23] != '-')
    {
        return false;
    }
    OutGuid = FGuid(
        ParseHexNumber(String.substr(0, 8)),
        ParseHexNumber(String.substr(9, 4)) << 16 | ParseHexNumber(String.substr(14, 4)),
        ParseHexNumber(String.substr(19, 4)) << 16 | ParseHexNumber(String.substr(24, 4)),
        ParseHexNumber(String.substr(28, 8))
    );
    return true;
}

void FGuid::Invalidate()
{
    A = 0;
    B = 0;
    C = 0;
    D = 0;
}

FString FGuid::ToString()
{
    return String::Format("{:08x}-{:04x}-{:04x}-{:04x}-{:04x}{:08x}", A, B >> 16, B & 0xFFFF, C >> 16, C & 0xFFFF, D);
}
