#pragma once

#include <map>
#include <unordered_map>

template <typename TKey, typename TValue, typename TComparer = std::less<TKey>>
using TMap = std::map<TKey, TValue, TComparer>;

template <typename TKey, typename TValue, typename THasher = std::hash<TKey>>
using TUnorderedMap = std::unordered_map<TKey, TValue, THasher>;
