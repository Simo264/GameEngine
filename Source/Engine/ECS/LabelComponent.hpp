#pragma once

#include "Core/Core.hpp"

class LabelComponent
{
public:
	LabelComponent(const char* label) 
		: label{ label } 
	{}

	String label;
};