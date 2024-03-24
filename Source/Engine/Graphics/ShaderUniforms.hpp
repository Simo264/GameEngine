#pragma once



constexpr char SHADER_UNIFORM_GAMMA[]						= "UGamma";
constexpr char SHADER_UNIFORM_POST_PROCESSING[] = "UPostProcessingType";
constexpr char SHADER_UNIFORM_SHADOW_MAP[]			= "UShadowMap";
constexpr char SHADER_UNIFORM_LIGHT_POS[]				= "ULightPos";
constexpr char SHADER_UNIFORM_VIEW_POS[]				= "UViewPos";

constexpr char SHADER_UNIFORM_MODEL[]				= "UModel";
constexpr char SHADER_UNIFORM_VIEW[]				= "UView";
constexpr char SHADER_UNIFORM_PROJECTION[]	= "UProjection";
constexpr char SHADER_UNIFORM_LIGHTSPACE[]	= "ULightSpaceMatrix";

/* Struct Material_t */
constexpr char SHADER_UNIFORM_MATERIAL[]					= "UMaterial";
constexpr char SHADER_UNIFORM_MATERIAL_DIFFUSE[]	= "UMaterial.diffuse";
constexpr char SHADER_UNIFORM_MATERIAL_SPECULAR[] = "UMaterial.specular";

/* Struct DirLight_t */
constexpr char SHADER_UNIFORM_DIRLIGHT[]						= "UDirLight";
constexpr char SHADER_UNIFORM_DIRLIGHT_DIRECTION[]	= "UDirLight.direction";
constexpr char SHADER_UNIFORM_DIRLIGHT_AMBIENT[]		= "UDirLight.ambient";
constexpr char SHADER_UNIFORM_DIRLIGHT_DIFFUSE[]		= "UDirLight.diffuse";
constexpr char SHADER_UNIFORM_DIRLIGHT_SPECULAR[]		= "UDirLight.specular";

/* Struct PointLight_t */

//#define SHADER_UNIFORM_POINTLIGHT(i)	"UPointLight[" #i "]"
//constexpr int	SHADER_MAX_NUM_POINTLIGHTS = 5;
//constexpr char SHADER_UNIFORM_POINTLIGHT[] = "UPointLight";
