#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
in vec4 FragPosLightSpace;
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

uniform int   u_useNormalMap;
uniform int   u_useParallaxMap;
uniform float u_parallaxHeight;

uniform sampler2D   u_depthMapTexture;
uniform samplerCube u_depthCubeMapTexture;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float g_shininess = 32.0f;
vec4 g_diffuseColor;
vec4 g_specularColor;

ivec2 g_normalTexSize;
ivec2 g_heightTexSize;


/* ---------- Function headers ---------- */
/* -------------------------------------- */
bool IsNormalMapActive();
bool HasNormalTexture();
bool IsParallaxMapActive();
bool HasHeightTexture();

vec2 CalculateParallaxCoord(vec3 viewDir, float heightScale);
vec3 CalculateNormalVector(vec2 textureCoord);
vec3 CalculateViewDirVector();
float CalculateAttenuation(float d, float kl, float kq);

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculateBlinnPhongLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

float CalculateDirLightShadows(vec3 normal, vec3 lightDir);
float CalculatePointLightShadows(float zFar, vec3 lightPos);

void main()
{
  g_normalTexSize = textureSize(u_material.normalTexture, 0);
  g_heightTexSize = textureSize(u_material.heightTexture, 0);

  const vec3 viewDir = CalculateViewDirVector();
  vec2 textureCoord = TexCoord;
  //if(IsParallaxMapActive() && HasHeightTexture())
  //  textureCoord = CalculateParallaxCoord(viewDir, u_parallaxHeight);
  
  const vec3 normal = CalculateNormalVector(textureCoord);

  g_diffuseColor  = texture(u_material.diffuseTexture, textureCoord);
  g_specularColor = texture(u_material.specularTexture, textureCoord);

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
  result = pow(result, vec3(1.0 / 2.2f));
  
  FragColor = vec4(result, 1.0);
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
bool IsParallaxMapActive()
{
  return u_useParallaxMap != 0;
}
bool HasHeightTexture()
{
  return g_heightTexSize != ivec2(1);
}

vec2 CalculateParallaxCoord(vec3 viewDir, float heightScale)
{
  /* Number of depth layers */ 
  const float minLayers = 8;
  const float maxLayers = 32;
  const float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
  
  /* Calculate the size of each layer */ 
  const float layerDepth = 1.0 / numLayers;
  
  /* Depth of current layer */ 
  float currentLayerDepth = 0.0;

  /* The amount to shift the texture coordinates per layer (from vector P) */ 
  const vec2 P = viewDir.xy / viewDir.z * heightScale; 
  const vec2 deltaTexCoords = P / numLayers;
  
  /* Get initial values */ 
  vec2 currentTexCoords = TexCoord;
  float currentDepthMapValue = texture(u_material.heightTexture, currentTexCoords).r;
      
  while(currentLayerDepth < currentDepthMapValue)
  {
    /* Shift texture coordinates along direction of P */ 
    currentTexCoords -= deltaTexCoords;
    /* Get depthmap value at current texture coordinates */ 
    currentDepthMapValue = texture(u_material.heightTexture, currentTexCoords).r;  
    /* Get depth of next layer */ 
    currentLayerDepth += layerDepth;  
  }
    
  /* Get texture coordinates before collision (reverse operations) */ 
  const vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

  /* Get depth after and before collision for linear interpolation */ 
  const float afterDepth  = currentDepthMapValue - currentLayerDepth;
  const float beforeDepth = texture(u_material.heightTexture, prevTexCoords).r - currentLayerDepth + layerDepth;
 
  /* Interpolation of texture coordinates */ 
  const float weight = afterDepth / (afterDepth - beforeDepth);
  const vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

  return finalTexCoords;
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
    
  /* diffuse shading */ 
  const float diffFactor = max(dot(lightDir, normal), 0.0);
  const vec3 diffuse = (light.color * light.diffuseIntensity) * diffFactor * g_diffuseColor.rgb;

  /* specular shading */ 
  const vec3 halfwayDir = normalize(lightDir + viewDir);
  const float specFactor = pow(max(dot(normal, halfwayDir), 0.0), g_shininess);
  const vec3 specular = (light.color * light.specularIntensity) * specFactor * g_specularColor.rgb;

  /* calculate shadow */
  const float shadow = CalculateDirLightShadows(normal, lightDir);
  return ((1.0f - shadow) * (diffuse + specular)) * g_diffuseColor.rgb;  
}
vec3 CalculateBlinnPhongLight(PointLight light, vec3 normal, vec3 viewDir) 
{
  const vec3 tangentLightPosition = TBN * light.position;
  const float zFar = 15.0f;
  
  /* Light direction */
  vec3 lightDir;
  if(IsNormalMapActive() && HasNormalTexture())
    lightDir = normalize(tangentLightPosition - TangentFragPos);
  else
    lightDir = normalize(light.position - FragPos);
   
  /* Diffuse */
  const float diffFactor = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuseIntensity) * diffFactor * g_diffuseColor.rgb;
   
  /* Specular */
  const vec3 halfwayDir = normalize(lightDir + viewDir);  
  const float specFactor = pow(max(dot(normal, halfwayDir), 0.0), g_shininess);
  vec3 specular = (light.color * light.specularIntensity) * specFactor * g_specularColor.rgb;

  /* Attenuation */
  const float lightDist = length(light.position - FragPos);
  const float attenuation = CalculateAttenuation(lightDist, light.attenuation.kl, light.attenuation.kq);
  diffuse  *= attenuation;
  specular *= attenuation;

  float shadow = CalculatePointLightShadows(zFar, light.position);
  return ((1.0 - shadow) * (diffuse + specular)) * g_diffuseColor.rgb;   
}
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) 
{
  const vec3 tangentLightPosition = TBN * light.position;

  /* Light direction */
  vec3 lightDir;
  if(u_useNormalMap != 0)
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

float CalculateDirLightShadows(vec3 normal, vec3 lightDir)
{
  /* Perform perspective divide */
  vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
  /* Transform to [0,1] range */
  projCoords = projCoords * 0.5 + 0.5;
  /* Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords) */
  float closestDepth = texture(u_depthMapTexture, projCoords.xy).r; 
  /* Get depth of current fragment from light's perspective */
  float currentDepth = projCoords.z;
  
  /* Resolve the problem of over sampling */
  if(projCoords.z > 1.0) 
    return 0.0;
  
  /* Percentage-closer filtering */
  const float bias = 0.002f;
  const int pcfCount = 2;
  const int totalTexels = (pcfCount * 2 + 1) * (pcfCount * 2 + 1);
  const vec2 texelSize = 1.0 / textureSize(u_depthMapTexture, 0);
  float shadow = 0.0f;
  for(int x = -pcfCount; x <= pcfCount; ++x)
  {
    for(int y = -pcfCount; y <= pcfCount; ++y)
    {
      const float pcfDepth = texture(u_depthMapTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
      if(currentDepth - bias > pcfDepth)
        shadow++;
    }    
  }
  shadow /= totalTexels;
  return shadow;
}
float CalculatePointLightShadows(float zFar, vec3 lightPos)
{
  /* get vector between fragment position and light position */ 
  const vec3 fragToLight = FragPos - lightPos;
  /* now get current linear depth as the length between the fragment and light position */ 
  const float currentDepth = length(fragToLight);

  const float viewDistance = length(ViewPos - FragPos);

  /* Percentage-closer filtering (PCF) */
  const vec3 sampleOffsetDirections[20] = vec3[]
  (
     vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
     vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
     vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
     vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
     vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
  );
  const int samples = 20;
  const float offset  = 0.1f;
  const float diskRadius = 0.05;
  const float bias = 0.10f;
  float shadow = 0.0f;
  for(int i = 0; i < samples; i++)
  {
    float closestDepth = texture(u_depthCubeMapTexture, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
    closestDepth *= zFar;

    if(currentDepth - bias > closestDepth)
        shadow++;
  }

  shadow /= samples;
  return shadow;
}
