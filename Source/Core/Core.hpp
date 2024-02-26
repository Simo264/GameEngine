#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <array>
#include <algorithm>
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

/* -----------------------------
			STL library layer
   ----------------------------- */

template<typename type, size_t size> 
using Array					= std::array<type, size>;
template<typename type>
using Vector				= std::vector<type>;
template<typename typeName, typename typeVal>
using Map						= std::map<typeName, typeVal>;

template<typename type>
using UniquePointer = std::unique_ptr<type>;
template<typename type>
using SharedPointer = std::shared_ptr<type>;

using String				= std::string;
using StringStream	= std::stringstream;
using IStringStream = std::istringstream;
using OStringStream = std::ostringstream;
using IFStream			= std::ifstream;
using OFStream			= std::ofstream;
using RuntimeError	= std::runtime_error;
using Path					= std::filesystem::path;

const Path ROOT_PATH = std::filesystem::current_path().parent_path();
const Path CONFIG_FILENAME = "AppConfig.ini";