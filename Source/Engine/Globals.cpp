int	WINDOW_WIDTH				= 1600;
int	WINDOW_HEIGHT				= 900;
float GAMMA_CORRECTION	= 2.2f;

float	Z_NEAR		= 0.1f;
float	Z_FAR			= 200.0f;
float LEFT			= -30.0f;
float RIGHT			= 30.0f;
float BOTTOM		= -30.0f;
float TOP				= 30.0f;

int VERTEX_COMPONENTS =
	3 + // position -> x,y,z
	2 +	// tc				-> u,v
	3 +	// normal		-> x,y,z
	3 +	// tangent	-> x,y,z
	3;	// bitangent-> x,y,z
