#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

/* ---------- OUT attributes ---------- */
/* ------------------------------------ */
out vec4 FragColor;


/* ---------- Structs ---------- */
/* ----------------------------- */
struct Material_t {
  sampler2D diffuse;    
  sampler2D specular;   
  float     shininess;
};
struct DirLight_t {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform sampler2D UShadowMap; 
uniform vec3 ULightPos;
uniform vec3 UViewPos;

uniform Material_t UMaterial;
uniform DirLight_t UDirLight;

uniform float UGamma;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 g_diffuseColor;
vec3 g_specularColor;
vec3 g_normal;
vec3 g_viewDir;

/* ---------- Functions ---------- */
/* ------------------------------- */
vec3 CalcDirLight(vec3 lightPos, DirLight_t dirLight, Material_t material);
float CalcDirLightShadow(vec3 lightDir);
void GammaCorrection(inout vec3 value); 

void main()
{     
  /* Init globals */
  g_normal        = normalize(Normal);
  g_diffuseColor  = texture(UMaterial.diffuse, TexCoords).rgb;
  g_specularColor = texture(UMaterial.specular, TexCoords).rgb;
  g_viewDir       = normalize(UViewPos - FragPos);
  vec3 result     = vec3(0.0f);

  /* If no texture: set diffuse to default color */
  if(length(g_diffuseColor) == 0)
    g_diffuseColor = vec3(0.25f, 0.50f, 0.75f);

  /* Phase 1: Directional lighting */
  result += CalcDirLight(ULightPos, UDirLight, UMaterial);

  /* Apply gamma correction */
  if(UGamma != 0.0f)
    GammaCorrection(result);

  FragColor = vec4(result, 1.0);
}

void GammaCorrection(inout vec3 value)
{
  value = pow(value, vec3(1.0f / UGamma));
}

float CalcDirLightShadow(vec3 lightDir)
{
  /* Perform perspective divide */
  vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
  
  /* Transform to [0,1] range */
  projCoords = projCoords * 0.5 + 0.5;
  
  /* Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords) */
  float closestDepth = texture(UShadowMap, projCoords.xy).r; 
  
  /* Get depth of current fragment from light's perspective */
  float currentDepth = projCoords.z;
  
  /* Resolve the problem of over sampling */
  if(projCoords.z > 1.0) 
    return 0.0;
  
  float shadow = 0.0f;
  
  /* Resolve the problem of shadow acne with bias */
  float bias = max(0.05 * (1.0 - dot(g_normal, lightDir)), 0.005);  
  return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;

  /* Percentage-closer filtering (PCF) */
  vec2 texelSize = 1.0 / textureSize(UShadowMap, 0);
  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcfDepth = texture(UShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
    }    
  }
  shadow /= 9.0;
  return shadow;
}

vec3 CalcDirLight(vec3 lightPos, DirLight_t dirLight, Material_t material)
{
  vec3 lightDir = normalize(lightPos - FragPos);
  
  /* ambient shading */
  vec3 ambient  = dirLight.ambient * g_diffuseColor;

  /* diffuse shading */
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
    if(diffuseFactor == 0.0f)
      return ambient;

  vec3 diffuse = dirLight.diffuse * diffuseFactor * g_diffuseColor;
  
  /* specular shading */
  vec3 reflectDir       = reflect(-lightDir, g_normal);
  float specularFactor  = pow(max(dot(g_viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular         = dirLight.specular * specularFactor * g_specularColor;

  /* Calculate shadow */
  float shadow  = CalcDirLightShadow(lightDir);                      
  return (ambient + (1.0 - shadow) * (diffuse + specular)) * g_diffuseColor;   
}
