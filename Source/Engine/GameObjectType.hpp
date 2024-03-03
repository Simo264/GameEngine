#pragma once

enum class GameObjectType : int 
{
	NONE = 0,
	STATIC_MESH,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};