#version 460

// What is Gooch Shading? 
// Gooch shading is a non-photorealistic rendering technique for shading objects. 
// It is also known as "cool to warm" shading.
// Defines two colors in conjunction with the original model color: 
// 1. a warm color (such as red) that indicates surfaces that are facing toward the light source
// 2. a cool color (such as blue) that indicates surfaces facing away
// This allows shading to occur only in mid-tones so that edge lines and highlights remain visually prominent.
// The Gooch shader is typically implemented in two passes: 
// 1. all objects in the scene are first drawn with the "cool to warm" shading
// 2. and in the second pass the object’s edges are rendered in black.
//
// Gooch Shading general formula:
// c_shaded = s * c_highlight + (1 - s) * (t * c_warm + (1 - t) * c_cool)
// That is equivalent to:
// c_shaded = f_unlit(n, v) + c_light * f_lit(l, n, v)
//
// Where:
// - f_unlit(n, v): this function returns the ambient lighting representing areas in shadow
// - f_lit(l_i, n, v): calculates the direct lighting contribution from each light source.
// - t = ((n dot l) + 1) / 2 
//    Determines the interpolation between cool (shadowed) and warm (lit) areas.
//    Typically implemented using lerp() or mix() functions.
// - r = 2*(n dot l)*n - l = reflect(-l, n)
//    Computes the reflected light vector using GLSL's reflect() function.
// - s = clamp((100*(r dot v) - 97) , 0, 1) 
//    Controls the specular highlight intensity
// - c_highlight is the specular highlight color.
// - v is the view direction (normalized vector from fragment to camera).
// - n is the normalized surface normal.
// - l is the normalized light direction (pointing toward the fragment).
// - A light source does not contribute to shading if the light direction (l)
//   forms an angle greater than 90° with the surface normal (n), meaning:
//      If (n dot l) <= 0, the light does not affect the surface.

// Gooch Shading Model - Complete Lighting Formula:
// c_shaded = (1/2) * c_cool + SUM(i=1...N)[(l_i ⋅ n) * c_light_i * (s_i * c_highlight + (1 - s_i) * c_warm)]
// Is equivalent to:
// c_shaded = f_unlit(n, v) + SUM(i=1...N)[(l_i ⋅ n) * c_light_i * f_lit(l_i, n, v)]


// ========== IN attributes ==========
// ===================================
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
in vec3 ViewPos;
in mat3 TBN;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

// ========== OUT attributes ==========
// ====================================
out vec4 FragColor;

// ========== Structs ==========
// =============================
struct Material 
{
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
};
struct DirectionalLight 
{
  vec3  color;
  float intensity;
  vec3  direction;
  float __padding;  // Needed for std140 alignment
};
struct Attenuation
{
	int range;  // If an objects distance is greater than the range, the light has no effect on the object
	float kl;	  // Linear attenuation factor
	float kq;   // Quadratic attenuation factor
};
struct PointLight 
{
  vec3  color;
  float intensity;
  vec3  position;
  float __padding_1;  // Needed for std140 alignment
  Attenuation attenuation;
  float __padding_2;  // Needed for std140 alignment
};
struct SpotLight 
{
  vec3  color;
  float intensity;
  vec3  position;
  float __padding_1;      // Needed for std140 alignment
  vec3  direction;
  float __padding_2;      // Needed for std140 alignment
  float cutOff;
  float outerCutOff;      // smoother edges
  float __padding_3[2];   // Needed for std140 alignment
  Attenuation attenuation;
};

// ========== Uniforms ==========
// ==============================
layout (std140, binding = 1) uniform LightBlock
{
  DirectionalLight  u_directionalLight;
  PointLight        u_pointLight;
  SpotLight         u_spotLight;
};
uniform Material u_material;

const float shininess = 32.0; // Controllo della specularità

vec3 f_unlit(vec3 cCool)
{
  return cCool;
}

vec3 f_lit(vec3 l, vec3 n, vec3 v, vec3 cCool, vec3 cWarm, vec3 cHighlight) 
{
  float t = (dot(n, l) + 1.0f) * 0.5f;  // Interpolazione tra cool e warm
  vec3 cToon = mix(cCool, cWarm, t);

  // Calcolo della specularità
  vec3 r = reflect(-l, n);
  float s = pow(max(dot(r, v), 0.0), shininess); // Controllo più morbido della specularità
  return (s * cHighlight + (1.0 - s) * cToon);
}

void main() 
{
  vec4 cSurface = texture(u_material.diffuseTexture, TexCoord);
  vec3 cCool = vec3(0, 0, 0.55f) + 0.25f * cSurface.xyz;
  vec3 cWarm = vec3(1.f, 0.3f, 0.f) + 0.25f * cSurface.xyz;
  vec3 cHighlight = vec3(1.0f);
  
  vec3 n = normalize(Normal);
  vec3 v = normalize(ViewPos - FragPos);
  vec3 l = normalize(u_directionalLight.direction);

  vec3 cLight = (u_directionalLight.color * u_directionalLight.intensity);
  
  // Using the formula: c_shaded = f_unlit(n, v) + c_light * f_lit(l, n, v)
  vec3 cShaded = f_unlit(cCool) + cLight * f_lit(l, n, v, cCool, cWarm, cHighlight);
  
  FragColor = vec4(cShaded, 1.f);
}