#pragma once

#include <functional>
#include <optional>

template <typename T>
using Optional = std::optional<T>;

template <typename T>
using Function = std::function<T>;