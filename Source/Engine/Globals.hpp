#pragma once

constexpr int	WINDOW_WIDTH = 1600;
constexpr int	WINDOW_HEIGHT = 900;
constexpr float GAMMA_CORRECTION = 2.2f;

constexpr int VERTEX_SIZE =
	3 + // position -> x,y,z
	2 +	// tc				-> u,v
	3 +	// normal		-> x,y,z
	3; 	// tangent	-> x,y,z

extern float Z_NEAR;
extern float Z_FAR;
extern float LEFT;
extern float RIGHT;
extern float BOTTOM;
extern float TOP;

extern int DRAW_MODE;