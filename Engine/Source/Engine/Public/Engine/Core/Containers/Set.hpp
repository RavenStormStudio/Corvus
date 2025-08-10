#pragma once

#include <set>
#include <unordered_set>

template <typename T, typename TComparer = std::less<T>>
using TSet = std::set<T, TComparer>;

template <typename T, typename THasher = std::hash<T>, typename TComparer = std::equal_to<T>>
using TUnorderedSet = std::unordered_set<T, THasher, TComparer>;
