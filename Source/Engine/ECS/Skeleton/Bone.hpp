#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

struct BoneNode
{
	mat4f transformation{};
	String name{};
	Vector<BoneNode> children{};
};
struct Bone
{
	mat4f offset{};
	mat4f	localTransform{};
};