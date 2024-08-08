#pragma once

#include <array>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <format>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <set>
#include <string>
#include <string_view>
#include <sstream>
#include <tuple>
#include <vector>


/* -----------------------------
			STL aliases
   ----------------------------- */

template<typename type, size_t size> 
using array		= std::array<type, size>;
template<typename type>
using vector	= std::vector<type>;
template<typename typeName, typename typeVal>
using map			= std::map<typeName, typeVal>;
template<typename type>
using set			= std::set<type>;
template<typename... types>
using tuple		= std::tuple<types...>;

template<typename type>
using unique_pointer = std::unique_ptr<type>;
template<typename type>
using shared_pointer = std::shared_ptr<type>;

namespace chrono		= std::chrono;
namespace fs				= std::filesystem;

using string				= std::string;
using string_view		= std::string_view;
using stringstream	= std::stringstream;
using istringstream = std::istringstream;
using ostringstream = std::ostringstream;
using ifStream			= std::ifstream;
using ofstream			= std::ofstream;
using byte					= std::byte;

const fs::path ROOT_PATH			= fs::current_path().parent_path();
const fs::path SHADERS_PATH		= ROOT_PATH / (fs::path("Resources/Shaders/").lexically_normal());
const fs::path TEXTURES_PATH	= ROOT_PATH / (fs::path("Resources/Textures/").lexically_normal());
const fs::path ICONS_PATH			= ROOT_PATH / (fs::path("Resources/Icons/").lexically_normal());
const fs::path FONTS_PATH			= ROOT_PATH / (fs::path("Resources/Fonts/").lexically_normal());
const fs::path ASSETS_PATH		= ROOT_PATH / (fs::path("Resources/Assets/").lexically_normal());
