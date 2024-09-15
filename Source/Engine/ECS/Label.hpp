#pragma once

#include "Core/Core.hpp"

class Label
{
public:
	Label(StringView label) 
		: value{ String(label.data()) } 
	{}
	~Label() = default;

	String value;
};