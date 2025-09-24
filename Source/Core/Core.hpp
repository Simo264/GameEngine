#pragma once

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <typeindex>
#include <unordered_map>
#include <vector>


/************** Namespaces **************/
/****************************************/
namespace chrono = std::chrono;
namespace fs = std::filesystem;

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

template <typename T>
using UniquePointer = std::unique_ptr<T>;
template <typename T>
using SharedPointer = std::shared_ptr<T>;

template <typename T>
using Optional = std::optional<T>;
template <typename T>
using Function = std::function<T>;

/************** Data types **************/
/****************************************/
using byte	= std::byte;
using u8		= std::uint8_t;
using i8		= std::int8_t;
using u16		= std::uint16_t;
using i16		= std::int16_t;
using u32		= std::uint32_t;
using i32		= std::int32_t;
using u64		= std::uint64_t;
using i64		= std::int64_t;
using f32		= std::float_t;
using f64		= std::double_t;

using String							= std::string;
using StringView					= std::string_view;
using StringStream				= std::stringstream;
using InputStringStream		= std::istringstream;
using OutputStringStream	= std::ostringstream;
using InputFileStream			= std::ifstream;
using OutputFileStream		= std::ofstream;
