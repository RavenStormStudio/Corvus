#include "Engine/Core/Containers/Name.hpp"

#include "Engine/Core/Utility/HashUtility.hpp"

void InitializeEmptyString()
{
    static bool bInitialized = false;
    if (!bInitialized)
    {
        FNameStringPool::Register(0, FString());
        bInitialized = true;
    }
}

bool8 FNameStringPool::IsRegistered(const uint64 Hash)
{
    return GetStringPool().contains(Hash);
}

void FNameStringPool::Register(uint64 Hash, const FString& String)
{
    if (IsRegistered(Hash))
    {
        return;
    }
    GetStringPool().emplace(Hash, String);
}

const FString& FNameStringPool::GetString(const uint64 Hash)
{
    InitializeEmptyString();
    if (!IsRegistered(Hash))
    {
        return GetStringPool().at(0); // Return the empty string registered at hash 0
    }
    return GetStringPool().at(Hash);
}

FStringView FNameStringPool::GetStringView(const uint64 Hash)
{
    return FStringView(GetString(Hash));
}

TUnorderedMap<uint64, FString>& FNameStringPool::GetStringPool()
{
    static TUnorderedMap<uint64, FString> StringPool;
    return StringPool;
}

FName::FName()
    : Hash(0)
{
}

FName::FName(const FStringView& InStringView)
    : Hash(HashUtility::FNV1A::Hash(InStringView.data(), InStringView.length()))
{
    FNameStringPool::Register(Hash, FString(InStringView));
}

FName::FName(const FChar* InString)
{
    const FString String = InString;
    Hash = HashUtility::FNV1A::Hash(String.data(), String.length());
    FNameStringPool::Register(Hash, String);
}

bool8 FName::IsValid() const
{
    return Hash != 0;
}

FString FName::GetString() const
{
    return FNameStringPool::GetString(Hash);
}

FStringView FName::GetStringView() const
{
    return FNameStringPool::GetStringView(Hash);
}

// ReSharper disable once CppMemberFunctionMayBeConst
FName::operator uint64()
{
    return Hash;
}

FName::operator const uint64() const
{
    return Hash;
}
