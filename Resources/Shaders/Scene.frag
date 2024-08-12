#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
in vec3 ViewPos;
in mat3 TBN;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

/* ---------- OUT attributes ---------- */
/* ------------------------------------ */
out vec4 FragColor;


/* ---------- Structs ---------- */
/* ----------------------------- */
struct Attenuation{
  float kl; /* linear attenuation factor */
  float kq; /* quadratic attenuation factor */
};
struct Material {
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
  sampler2D heightTexture;
};
struct DirectionalLight {
  vec3  color;
  float diffuseIntensity;
  float specularIntensity;
  vec3  direction;
};
struct PointLight {
  vec3  color;
  float diffuseIntensity;
  float specularIntensity;
  vec3  position;
  
  Attenuation attenuation;
};
struct SpotLight {
  vec3  color;
  float diffuseIntensity;
  float specularIntensity;
  vec3  position;
  vec3  direction;
  float cutOff;
  float outerCutOff; /* smoother edges */

  Attenuation attenuation;
};


/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform Material          u_material;
uniform DirectionalLight  u_directionalLight;
uniform PointLight        u_pointLight[4];
uniform SpotLight         u_spotLight;

uniform vec3  u_ambientLightColor;
uniform float u_ambientLightIntensity;

uniform int u_useNormalMap;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float g_shininess = 32.0f;
vec4 g_diffuseColor;
vec4 g_specularColor;

ivec2 g_normalTexSize;

/* ---------- Function headers ---------- */
/* -------------------------------------- */
bool IsNormalMapActive();
bool HasNormalTexture();

vec3 CalculateNormalVector(vec2 textureCoord);
vec3 CalculateViewDirVector();
float CalculateAttenuation(float d, float kl, float kq);

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculateBlinnPhongLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir);


/* ---------- Main function ---------- */
/* ----------------------------------- */

void main() 
{
  g_normalTexSize = textureSize(u_material.normalTexture, 0);

  const vec3 viewDir = CalculateViewDirVector();
  const vec3 normal = CalculateNormalVector(TexCoord);

  g_diffuseColor  = texture(u_material.diffuseTexture, TexCoord);
  g_specularColor = texture(u_material.specularTexture, TexCoord);

  const vec3 ambientLight = (u_ambientLightColor * u_ambientLightIntensity) * g_diffuseColor.rgb;
  vec3 result = vec3(0.0f);

  /* =========================== */
  /* Calculate directional light */
  /* =========================== */
  result += CalculateDirectionalLight(u_directionalLight, normal, viewDir);


  /* ===================== */
  /* Calculate point light */
  /* ===================== */
  //for(int i = 0; i < 4; i++)
  //  result += CalculateBlinnPhongLight(u_pointLight[i], normal, viewDir);
  result += CalculateBlinnPhongLight(u_pointLight[0], normal, viewDir);


  /* ==================== */
  /* Calculate spot light */
  /* ==================== */
  //result += CalculateSpotLight(u_spotLight, normal, viewDir);

  /* Apply gamma correction */
  result = pow(result, vec3(1.0f / 2.2f));
  
  FragColor = vec4(result, 1.0f);
}




/* ---------- Function bodys ------------ */
/* -------------------------------------- */
bool IsNormalMapActive()
{
  return u_useNormalMap != 0;
}
bool HasNormalTexture()
{
  return g_normalTexSize != ivec2(1);
}


vec3 CalculateNormalVector(vec2 textureCoord) 
{
  if(HasNormalTexture() && IsNormalMapActive())
  {
    /* Obtain normal from normal map in range [0,1] */
    vec3 N = texture(u_material.normalTexture, textureCoord).rgb;
    /* Transform normal vector to range [-1,1] */
    N = N * 2.0 - 1.0;
    return N;
  }
  
  return Normal;
}
vec3 CalculateViewDirVector()
{
  if(HasNormalTexture() && IsNormalMapActive())
    return normalize(TangentViewPos - TangentFragPos);
  
  return normalize(ViewPos - FragPos);
}
float CalculateAttenuation(float d, float kl, float kq) 
{
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  
  return 1.0f / (1.0f + kl*d + kq*pow(d,2));  
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
  const vec3 lightDir = normalize(-light.direction);
    
  /* Diffuse shading */ 
  const float diffFactor = max(dot(lightDir, normal), 0.0);
  const vec3 diffuse = (light.color * light.diffuseIntensity) * diffFactor * g_diffuseColor.rgb;

  /* Specular shading */ 
  const vec3 halfwayDir = normalize(lightDir + viewDir);
  const float specFactor = pow(max(dot(normal, halfwayDir), 0.0), g_shininess);
  const vec3 specular = (light.color * light.specularIntensity) * specFactor * g_specularColor.rgb;

  return diffuse + specular;
}
vec3 CalculateBlinnPhongLight(PointLight light, vec3 normal, vec3 viewDir) 
{
  const vec3 tangentLightPosition = TBN * light.position;
  
  /* Light direction */
  vec3 lightDir;
  if(IsNormalMapActive() && HasNormalTexture())
    lightDir = normalize(tangentLightPosition - TangentFragPos);
  else
    lightDir = normalize(light.position - FragPos);
   
  /* Diffuse shading */
  const float diffFactor = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuseIntensity) * diffFactor * g_diffuseColor.rgb;
   
  /* Specular shading */
  const vec3 halfwayDir = normalize(lightDir + viewDir);  
  const float specFactor = pow(max(dot(normal, halfwayDir), 0.0), g_shininess);
  vec3 specular = (light.color * light.specularIntensity) * specFactor * g_specularColor.rgb;

  /* Attenuation */
  const float lightDist = length(light.position - FragPos);
  const float attenuation = CalculateAttenuation(lightDist, light.attenuation.kl, light.attenuation.kq);
  diffuse  *= attenuation;
  specular *= attenuation;

  return diffuse + specular;
}
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
  const vec3 tangentLightPosition = TBN * light.position;

  /* Light direction */
  vec3 lightDir;
  if(IsNormalMapActive() && HasNormalTexture())
    lightDir = normalize(tangentLightPosition - TangentFragPos);
  else
    lightDir = normalize(light.position - FragPos);

  /* Diffuse */
  const float diffFactor = max(dot(normal, lightDir), 0.0f);
  vec3 diffuse = (light.color * light.diffuseIntensity) * diffFactor * g_diffuseColor.rgb;

  /* Specular */
  const vec3 halfwayDir = normalize(lightDir + viewDir);
  const float specFactor = pow(max(dot(normal, halfwayDir), 0.0f), g_shininess);
  vec3 specular = (light.color * light.specularIntensity) * specFactor * g_specularColor.rgb;

  /* Soft edges + intensity */
  const float cutoff = cos(radians(light.cutOff));
  const float outerCutoff = cos(radians(light.outerCutOff));
  const float theta = dot(lightDir, normalize(-light.direction));
  const float epsilon = (cutoff - outerCutoff);
  const float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  /* Attenuation */
  const float lightDist = length(light.position - FragPos);
  const float attenuation = CalculateAttenuation(lightDist, light.attenuation.kl, light.attenuation.kq);
  diffuse  *= attenuation;
  specular *= attenuation;

  return diffuse + specular;
}
