#version 460

/*
  https://en.wikipedia.org/wiki/Phong_reflection_model
  https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
  Phong and Blinn–Phong Reflection Models

  These models describe how light interacts with surfaces in 3D rendering.

  1. Phong Reflection Model:
      - Calculates illumination based on ambient, diffuse, and specular reflections.
      - Uses the reflection vector R and its alignment with the viewer direction V.
      - The specular component is computed as (R * V)^alpha where alpha controls shininess.

      Formula:
          I_p = k_a  
              + sum over lights ( k_d * (L * N) * i 
              + k_s * (R * V)^alpha * i )

  2. Blinn–Phong Reflection Model (Modified Phong Model):
      - Uses a halfway vector H, instead of calculating the reflection R.
      - The specular component is computed as (N * H)^alpha', where alpha' > alpha.

      Formula:
          H = (L + V) / ||L + V||
          I_p = k_a * i_a 
              + sum over lights ( k_d * (L * N) * i + k_s * (N * H)^alpha' * i )


  --------------- [Description of Parameters] ---------------
  - k_a: Ambient reflection coefficient
  - k_d: Diffuse reflection coefficient
  - k_s: Specular reflection coefficient
  - alpha: Shininess factor – higher values result in sharper and more concentrated highlights.
  - i: Light intensity
  - L: Light direction vector
  - N: Surface normal
  - R: Reflection direction – the ideal reflection path for specular light.
  - V: Viewer direction
  - H: Halfway vector – a vector midway between L and V.
*/


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

// ========== Uniforms ==========
// ==============================
struct Material 
{
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
};
struct Lighting
{
  vec3 diffuse;
  vec3 specular;
};
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


layout (std140, binding = 1) uniform LightBlock
{
  DirectLight u_directLight;
  PointLight u_pointLight;
};


uniform Material u_material;
uniform int u_normalMapping; // 1 -> enabled; 0 -> disabled

const float g_alpha = 16.0f;          // alpha controls shininess
const float g_gammaCorrection = 2.2f; // Assume the monitor is calibrated to the sRGB color space

vec4 g_objectAmbientColor;
vec4 g_objectDiffuseColor;
vec4 g_objectSpecularColor;

Lighting BlinnPhongLight(float I, vec3 C, vec3 L, vec3 N, vec3 V);
void CalculateAttenuation(float distance, float kl, float kq, inout Lighting lighting);

void main()
{
/*
  Blinn-Phong formula:
  color = <ambient> + <diffuse> + <specular>

  - diffuse = k_d * (L * N) * i
  - specular = k_s * (N * H)^alpha' * i
  - N = Normals
  - L = light direction
  - V = view direction
  - H = normalize(L + V)
*/

  g_objectDiffuseColor = texture(u_material.diffuseTexture, TexCoord);
  g_objectAmbientColor = g_objectDiffuseColor * 0.1f;
  g_objectSpecularColor = texture(u_material.specularTexture, TexCoord);

  vec3 N = Normal;
  vec3 V = normalize(ViewPos - FragPos);
  ivec2 normalSize = textureSize(u_material.normalTexture, 0);
  if(u_normalMapping == 1 && normalSize != ivec2(1))
  {
    // Obtain normal from normal map in range [0,1] and
    // transform normal vector to range [-1,1]
    N  = texture(u_material.normalTexture, TexCoord).xyz;
    N = N * 2.0f - 1.0f;
    V = normalize(TangentViewPos - TangentFragPos);
  }

  vec3 color = g_objectAmbientColor.rgb;
  vec3 sumOfLights = vec3(0.f);

  float I;
  vec3 C;
  vec3 L;

  // Calculate directional light
  {
    I = u_directLight.intensity;
    L = normalize(-u_directLight.direction);
    C = u_directLight.color;
    Lighting lighting = BlinnPhongLight(I, C, L, N, V);
    sumOfLights += lighting.diffuse + lighting.specular;
  }

  // Calculate point light
  {
    I = u_pointLight.intensity;
    L = normalize(u_pointLight.position - FragPos); // 3D position in space of the surface
    C = u_pointLight.color;
    Lighting lighting = BlinnPhongLight(I, C, L, N, V);
    
    float distance = length(u_pointLight.position - FragPos); // distance from light source
    float kl = u_pointLight.attenuation.kl;
    float kq = u_pointLight.attenuation.kq;
    CalculateAttenuation(distance, kl, kq, lighting);

    sumOfLights += lighting.diffuse + lighting.specular;
  }

  color += sumOfLights;
  color = pow(color, vec3(1.0f / g_gammaCorrection));
	FragColor = vec4(color, 1.0f);
}

Lighting BlinnPhongLight(float I, vec3 C, vec3 L, vec3 N, vec3 V)
{
  Lighting lighting;
  lighting.diffuse = vec3(0.f);
  lighting.specular = vec3(0.f);

  if(I == 0.0f)
    return lighting;
    
  float specular = 0.0f;
  float lambertian = max(dot(L, N), 0.0f);
  if (lambertian > 0.0f)
  {
    vec3 H = normalize(L + V);
    float specAngle = max(dot(H, N), 0.0f);
    specular = pow(specAngle, g_alpha);
  }
  
  lighting.diffuse  = g_objectDiffuseColor.rgb * lambertian * I * C;
  lighting.specular = g_objectSpecularColor.rgb * specular * I * C;
  return lighting;
}

void CalculateAttenuation(float distance, float kl, float kq, inout Lighting lighting)
{
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  // attenuation = 1.0f / (1.0f + kl*d + kq*pow(d,2));  

  float attenuation = 1.0f / (1.0f + kl*distance + kq*pow(distance,2));
  lighting.diffuse  *= attenuation;
  lighting.specular *= attenuation;
}
