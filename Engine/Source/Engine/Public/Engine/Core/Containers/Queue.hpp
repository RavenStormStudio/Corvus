#pragma once

#include <queue>

#include "Vector.hpp"

template <typename T>
using TDequeue = std::deque<T>;

template <typename T, typename TContainer = TDequeue<T>>
using TQueue = std::queue<T, TContainer>;

template <typename T, typename TContainer = TVector<T>, typename TComparer = std::less<T>>
using TPriorityQueue = std::priority_queue<T, TContainer, TComparer>;
