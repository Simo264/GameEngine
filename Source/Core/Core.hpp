#pragma once

#include <array>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>

constexpr float GAMMA_CORRECTION	= 2.2f;
constexpr float	INITIAL_ZNEAR			= 0.1f;
constexpr float	INITIAL_ZFAR			= 100.0f;


/* -----------------------------
			STL library layer
   ----------------------------- */

/* Containers */
template<typename type, size_t size> 
using Array		= std::array<type, size>;
template<typename type>
using Vector	= std::vector<type>;
template<typename typeName, typename typeVal>
using Map			= std::map<typeName, typeVal>;

/* Smart pointers*/
template<typename type>
using UniquePointer = std::unique_ptr<type>;
template<typename type>
using SharedPointer = std::shared_ptr<type>;

/* Chrono */
using SystemClock = std::chrono::system_clock;
using TimePoint		= std::chrono::system_clock::time_point;

using String				= std::string;
using StringStream	= std::stringstream;
using IStringStream = std::istringstream;
using OStringStream = std::ostringstream;
using IFStream			= std::ifstream;
using OFStream			= std::ofstream;
using RuntimeError	= std::runtime_error;
using Path					= std::filesystem::path;

const Path ROOT_PATH				= std::filesystem::current_path().parent_path();
const Path SHADERS_PATH			= ROOT_PATH / (Path("Resources/Shaders/").lexically_normal());
const Path TEXTURES_PATH		= ROOT_PATH / (Path("Resources/Textures/").lexically_normal());
const Path ICONS_PATH				= ROOT_PATH / (Path("Resources/Icons/").lexically_normal());
const Path FONTS_PATH				= ROOT_PATH / (Path("Resources/Fonts/").lexically_normal());
const Path ASSETS_PATH			= ROOT_PATH / (Path("Resources/Assets/").lexically_normal());
const Path CONFIG_FILENAME	= "AppConfig.ini";
