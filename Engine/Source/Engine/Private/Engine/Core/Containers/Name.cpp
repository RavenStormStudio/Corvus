#include "Engine/Core/Containers/Name.hpp"

#include "Engine/Core/Utility/HashUtility.hpp"

TUnorderedMap<uint64, FString> FNameStringPool::StringPool;

bool8 FNameStringPool::IsRegistered(const uint64 Hash)
{
    return StringPool.contains(Hash);
}

void FNameStringPool::Register(uint64 Hash, const FString& String)
{
    if (IsRegistered(Hash))
    {
        return;
    }
    StringPool.emplace(Hash, String);
}

const FString& FNameStringPool::GetString(const uint64 Hash)
{
    if (!IsRegistered(Hash))
    {
        return EmptyString;
    }
    return StringPool.at(Hash);
}

FStringView FNameStringPool::GetStringView(const uint64 Hash)
{
    return FStringView(GetString(Hash));
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

FName::operator uint64() const
{
    return Hash;
}

FName::operator const uint64() const
{
    return Hash;
}
