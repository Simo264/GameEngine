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
#include <vector>

constexpr float GAMMA_CORRECTION	= 2.2f;
constexpr float	INITIAL_ZNEAR			= 0.1f;
constexpr float	INITIAL_ZFAR			= 100.0f;

constexpr char CONFIG_FILENAME[]	= "AppConfig.ini";

/* -----------------------------
			STL library layer
   ----------------------------- */

/* Containers */
template<typename type, size_t size> 
using array		= std::array<type, size>;
template<typename type>
using vector	= std::vector<type>;
template<typename typeName, typename typeVal>
using map			= std::map<typeName, typeVal>;
template<typename type>
using set			= std::set<type>;

/* Smart pointers*/
template<typename type>
using unique_pointer = std::unique_ptr<type>;
template<typename type>
using shared_pointer = std::shared_ptr<type>;

/* Chrono */
using system_clock	= std::chrono::system_clock;
using time_point		= std::chrono::system_clock::time_point;

using string				= std::string;
using string_view		= std::string_view;
using stringstream	= std::stringstream;
using istringstream = std::istringstream;
using ostringstream = std::ostringstream;
using ifStream			= std::ifstream;
using ofstream			= std::ofstream;
using runtime_error	= std::runtime_error;
using fspath				= std::filesystem::path;

const fspath ROOT_PATH			= std::filesystem::current_path().parent_path();
const fspath SHADERS_PATH		= ROOT_PATH / (fspath("Resources/Shaders/").lexically_normal());
const fspath TEXTURES_PATH	= ROOT_PATH / (fspath("Resources/Textures/").lexically_normal());
const fspath ICONS_PATH			= ROOT_PATH / (fspath("Resources/Icons/").lexically_normal());
const fspath FONTS_PATH			= ROOT_PATH / (fspath("Resources/Fonts/").lexically_normal());
const fspath ASSETS_PATH		= ROOT_PATH / (fspath("Resources/Assets/").lexically_normal());
