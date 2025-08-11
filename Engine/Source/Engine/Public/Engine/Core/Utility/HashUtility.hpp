#pragma once

#include "Engine/Core/CoreTypes.hpp"
#include "Engine/Core/Containers/String.hpp"

#include <type_traits>

namespace HashUtility::FNV1A
{
    namespace Internal
    {
        constexpr uint32 OFFSET_BASIS_32 = 2166136261u;
        constexpr uint32 PRIME_32 = 16777619u;

        constexpr uint64 OFFSET_BASIS_64 = 14695981039346656037ull;
        constexpr uint64 PRIME_64 = 1099511628211ull;

        template <typename T>
        struct FNVTraits;

        template <>
        struct FNVTraits<uint32>
        {
            static constexpr uint32 OffsetBasis = OFFSET_BASIS_32;
            static constexpr uint32 Prime = PRIME_32;
        };

        template <>
        struct FNVTraits<uint64>
        {
            static constexpr uint64 OffsetBasis = OFFSET_BASIS_64;
            static constexpr uint64 Prime = PRIME_64;
        };
    }

    template <typename HashType = uint32> requires std::is_same_v<uint32, HashType> || std::is_same_v<uint64, HashType>
    constexpr HashType Hash(const FChar* String, const size64 Length) noexcept
    {
        HashType Hash = Internal::FNVTraits<HashType>::OffsetBasis;
        for (size64 Index = 0; Index < Length; ++Index)
        {
            Hash ^= static_cast<HashType>(static_cast<unsigned char>(String[Index]));
            Hash *= Internal::FNVTraits<HashType>::Prime;
        }
        return Hash;
    }

    template <typename HashType = uint32> requires std::is_same_v<uint32, HashType> || std::is_same_v<uint64, HashType>
    constexpr HashType Hash(const FStringView String) noexcept
    {
        return Hash<HashType>(String.data(), String.size());
    }
}
