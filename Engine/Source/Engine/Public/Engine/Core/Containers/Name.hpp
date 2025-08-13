#pragma once

#include "Map.hpp"
#include "String.hpp"

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"

class ENGINE_API FNameStringPool
{
public:
    [[nodiscard]] static bool8 IsRegistered(uint64 Hash);
    static void Register(uint64 Hash, const FString& String);

    [[nodiscard]] static const FString& GetString(uint64 Hash);
    [[nodiscard]] static FStringView GetStringView(uint64 Hash);

private:
    [[nodiscard]] static TUnorderedMap<uint64, FString>& GetStringPool();
};

class ENGINE_API FName
{
public:
    FName();
    FName(const FStringView& InStringView);
    FName(const FChar* InString);
    ~FName() = default;

    DEFAULT_COPY_MOVEABLE(FName)

public:
    [[nodiscard]] bool8 IsValid() const;

    [[nodiscard]] FString GetString() const;
    [[nodiscard]] FStringView GetStringView() const;

public:
    [[nodiscard]] uint64 GetHash() const { return Hash; }

public:
    operator uint64();
    operator const uint64() const;

private:
    uint64 Hash;
};
