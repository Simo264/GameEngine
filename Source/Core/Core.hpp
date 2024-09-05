#pragma once

#include <chrono>
#include <filesystem>

#include <array>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <vector>

#include <cstdint>

#include <iostream>
#include <format>
#include <cassert>


/************** Namespaces **************/
/****************************************/
namespace chrono = std::chrono;
namespace fs = std::filesystem;


/************** Containers **************/
/****************************************/
template<typename Type, size_t size>
using Array		= std::array<Type, size>;
template<typename Type>
using Vector	= std::vector<Type>;
template<typename TypeName, typename TypeVal>
using Map			= std::map<TypeName, TypeVal>;
template<typename Type>
using Set			= std::set<Type>;
template<typename... Types>
using Tuple		= std::tuple<Types...>;


/************** Smart pointers **************/
/********************************************/
template<typename Type>
using UniquePtr = std::unique_ptr<Type>;
template<typename Type>
using SharedPtr = std::shared_ptr<Type>;


/************** Data types **************/
/****************************************/
using u8	= std::uint8_t;
using i8	= std::int8_t;
using u16	= std::uint16_t;
using i16	= std::int16_t;
using u32	= std::uint32_t;
using i32	= std::int32_t;
using u64	= std::uint64_t;
using i64	= std::int64_t;
using f32 = std::float_t;
using f64 = std::double_t;

using String				= std::string;
using StringView		= std::string_view;
using StringStream	= std::stringstream;
using IStringStream = std::istringstream;
using OStringStream = std::ostringstream;
using IStream				= std::ifstream;
using OStream				= std::ofstream;
using Byte					= std::byte;


/************** Default paths **************/
/*******************************************/
const fs::path ROOT_PATH			= fs::current_path().parent_path();
const fs::path SHADERS_PATH		= ROOT_PATH / (fs::path("Resources/Shaders/").lexically_normal());
const fs::path TEXTURES_PATH	= ROOT_PATH / (fs::path("Resources/Textures/").lexically_normal());
const fs::path ICONS_PATH			= ROOT_PATH / (fs::path("Resources/Icons/").lexically_normal());
const fs::path FONTS_PATH			= ROOT_PATH / (fs::path("Resources/Fonts/").lexically_normal());
const fs::path ASSETS_PATH		= ROOT_PATH / (fs::path("Resources/Assets/").lexically_normal());