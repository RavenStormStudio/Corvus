#pragma once

#include "Queue.hpp"

#include <stack>

template <typename T, typename TContainer = TDequeue<T>>
using TStack = std::stack<T, TContainer>;
