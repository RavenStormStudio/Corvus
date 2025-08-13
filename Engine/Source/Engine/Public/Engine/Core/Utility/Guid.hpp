#pragma once
#include "Engine/Core/CoreTypes.hpp"
#include "Engine/Core/Containers/String.hpp"

struct ENGINE_API FGuid
{
public:
    constexpr FGuid()
        : A(0), B(0), C(0), D(0)
    {
    }

    constexpr FGuid(const uint32 InA, const uint32 InB, const uint32 InC, const uint32 InD)
        : A(InA), B(InB), C(InC), D(InD)
    {
    }

    FGuid(const FString& InString)
    {
        if (!Parse(InString, *this))
        {
            Invalidate();
        }
    }

public:
    [[nodiscard]] constexpr bool8 IsValid() const
    {
        return (A | B | C | D) != 0;
    }

    void Invalidate();
    [[nodiscard]] FString ToString();

public:
    constexpr friend bool8 operator==(const FGuid& First, const FGuid& Second)
    {
        return ((First.A ^ Second.A) | (First.B ^ Second.B) | (First.C ^ Second.C) | (First.D ^ Second.D)) == 0;
    }

    constexpr friend bool8 operator!=(const FGuid& First, const FGuid& Second)
    {
        return !(First == Second);
    }

    constexpr friend bool8 operator<(const FGuid& First, const FGuid& Second)
    {
        if (First.D > Second.D)
            return First.A < Second.A
                       ? true
                       : First.A > Second.A
                       ? false
                       : First.B < Second.B
                       ? true
                       : First.B > Second.B
                       ? false
                       : First.C < Second.C
                       ? true
                       : First.C > Second.C
                       ? false
                       : First.D < Second.D;
        return First.A < Second.A
                   ? true
                   : First.A > Second.A
                   ? false
                   : First.B < Second.B
                   ? true
                   : First.B > Second.B
                   ? false
                   : First.C < Second.C
                   ? true
                   : First.C > Second.C
                   ? false
                   : First.D < Second.D;
    }

    constexpr friend bool8 operator>(const FGuid& First, const FGuid& Second)
    {
        return !(First < Second);
    }

public:
    [[nodiscard]] static FGuid NewGuid();

    static bool8 Parse(const FChar* String, FGuid& OutGuid);
    static bool8 Parse(const FString& String, FGuid& OutGuid);
    static bool8 Parse(FStringView String, FGuid& OutGuid);

private:
    uint32 A;
    uint32 B;
    uint32 C;
    uint32 D;
};
