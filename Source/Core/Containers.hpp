#pragma once

#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>

template <typename T, size_t size>
using Array = std::array<T, size>;

template <typename T>
using Vector = std::vector<T>;

template <typename T, typename TVal>
using Map = std::map<T, TVal>;

template <typename T, typename TVal>
using UnorderedMap = std::unordered_map<T, TVal>;

template <typename T>
using Set = std::set<T>;