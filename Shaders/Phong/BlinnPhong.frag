#version 460

// ========== IN attributes ==========
// ===================================
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 CameraPos;
in mat3 TBN;

// ========== OUT attributes ==========
// ====================================
out vec4 FragColor;

// ========== Uniforms ==========
// ==============================
struct Attenuation
                                                                      {
	int range;        // If an objects distance is greater than the range, the light has no effect on the object
	float kl;	        // Linear attenuation factor
	float kq;         // Quadratic attenuation factor
  float __padding;  // Needed for std140 alignment
};
struct DirectLight 
{
  vec3  color;
  float intensity;
  vec3  direction;
  float __padding;  // Needed for std140 alignment
};
struct PointLight 
{
  vec3  color;
  float intensity;
  vec3  position;
  float __padding;  // Needed for std140 alignment
  Attenuation attenuation;
};

layout (std140, binding = 1) uniform LightingBlock
{
  DirectLight u_directLight;
  PointLight u_pointLight;
};

layout (binding = 10) uniform sampler2D u_textureColor;
layout (binding = 11) uniform sampler2D u_textureSpecular;
layout (binding = 12) uniform sampler2D u_textureNormal;
uniform int u_normalMapping; // 1 -> enabled; 0 -> disabled

const float g_shininess = 32.0f;
const float g_gammaCorrection = 2.2f;

vec3 LambertianDiffuseReflection (vec3 L, vec3 N, vec3 C, float I, vec3 meshDiffuse);
vec3 BlinnPhongSpecularReflection(vec3 L, vec3 N, vec3 V, vec3 C, float I, vec3 meshSpecular);
float CalculateAttenuation(float dist, float kl, float kq);

void main()
{
  vec4 objectDiffuse = texture(u_textureColor, TexCoord);
  vec4 objectAmbient = objectDiffuse * 0.1f;
  vec4 objectSpecular = texture(u_textureSpecular, TexCoord);

  vec3 V = normalize(CameraPos - FragPos);
  vec3 N = normalize(Normal);
  ivec2 normalSize = textureSize(u_textureNormal, 0);
  if(u_normalMapping == 1 && normalSize != ivec2(1))
  {
    N = texture(u_textureNormal, TexCoord).rgb;  // Obtain normal from normal map in range [0,1];
    N = N * 2.0f - 1.0f;                                  // transform normal vector to range [-1,1];
    N = normalize(TBN * N);                               // the resulting normal is now in world space,
  }

  vec3 color = objectAmbient.rgb;
  vec3 L;
  vec3 C;
  float I;

  if(u_directLight.intensity > 0.f) // Calculate directional light
  {
    L = normalize(-u_directLight.direction);
    C = u_directLight.color;
    I = u_directLight.intensity;
    color += LambertianDiffuseReflection(L, N, C, I, objectDiffuse.rgb);
    color += BlinnPhongSpecularReflection(L, N, V, C, I, objectSpecular.rgb);
  }
  
  if(u_pointLight.intensity > 0.f) // Calculate point light
  {
    L = normalize(u_pointLight.position - FragPos);
    C = u_pointLight.color;
    I = u_pointLight.intensity;
    vec3 diffuse = LambertianDiffuseReflection(L, N, C, I, objectDiffuse.rgb);
    vec3 specular = BlinnPhongSpecularReflection(L, N, V, C, I, objectSpecular.rgb);

    float dist = length(u_pointLight.position - FragPos);
    float kl = u_pointLight.attenuation.kl;
    float kq = u_pointLight.attenuation.kq;
    float attenuation = CalculateAttenuation(dist, kl, kq);
    diffuse *= attenuation;
    specular *= attenuation;

    color += diffuse;
    color += specular;
  }

  color = pow(color, vec3(1.0f / g_gammaCorrection));
	FragColor = vec4(color, 1.0f);
}

vec3 LambertianDiffuseReflection(vec3 L, vec3 N, vec3 C, float I, vec3 materialDiffuse)
{
  // The Lambertian Diffuse Reflection Model:
  //  - Models the way light is diffusely reflected from rough surfaces.
  //  - The reflected intensity is proportional to the cosine of the angle between
  //    the surface normal N and the light direction L:
  //      diffuse = max(dot(N, L), 0.0)
  //  - The final reflected color is scaled by the light color C, light intensity I,
  //    and the material's diffuse color (e.g. from a texture).
  //
  // Where:
  //  - N is the surface normal (normalized)
  //  - L is the light direction vector (normalized)
  //  - C is the light color
  //  - I is the light intensity
  //  - max(dot(N, L), 0.0) ensures no negative lighting from back-facing surfaces
  float NdotL = max(dot(N, L), 0.0f);
  vec3 reflection = NdotL * C * I * materialDiffuse;
  return reflection;
}

vec3 BlinnPhongSpecularReflection(vec3 L, vec3 N, vec3 V, vec3 C, float I, vec3 materialSpecular)
{
  // The Blinn-Phong Specular Reflection Model:
  //  - Computes the specular reflection using a halfway vector H instead of a reflection vector R.
  //  - The specular intensity is calculated as:
  //      spec = max(dot(N, H), 0)^a
  //  - The final reflected color is scaled by the light color C, light intensity I,
  //    and the material's specular color (e.g. from a texture).
  //
  // Where:
  //  - H = normalize(L + V) is the halfway vector between L and V
  //  - N is the surface normal (normalized)
  //  - L is the light direction vector (normalized)
  //  - V is the view (camera) direction vector (normalized)
  //  - a (g_shininess) controls the sharpness of the highlight
  //  - C is the light color
  //  - I is the light intensity
  vec3 H = normalize(L + V);
  float specAngle = max(dot(H, N), 0.0f);
  float spec = pow(specAngle, g_shininess);
  vec3 reflection = spec * C * I * materialSpecular;
  return reflection;
}

float CalculateAttenuation(float dist, float kl, float kq)
{
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  // attenuation = 1.0f / (1.0f + kl*d + kq*pow(d,2));  
  float attenuation = 1.0f / (1.0f + kl*dist + kq*pow(dist,2));
  return attenuation;
}
