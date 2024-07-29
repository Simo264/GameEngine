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
uniform float u_heightScale;

uniform sampler2D u_shadowMapTexture;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float g_shininess = 32.0f;
const float g_gamma = 2.2f;
vec4 g_diffuseColor;
vec4 g_specularColor;

/* ---------- Functions ---------- */
/* ------------------------------- */
vec2 CalculateParallaxCoord(vec3 viewDir, float heightScale);
vec3 CalculateNormalVector(vec2 textureCoord);
vec3 CalculateViewDirVector();
float CalculateAttenuation(float d, float kl, float kq);
float CalculateDirLightShadows(vec3 normal, vec3 lightDir);

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculateBlinnPhongLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

void main() {    

  vec3 viewDir = CalculateViewDirVector();
  vec2 textureCoord = TexCoord;
  if(u_useNormalMap != 0 && u_useParallaxMap != 0)
    textureCoord = CalculateParallaxCoord(viewDir, u_heightScale);
  
  vec3 normal = CalculateNormalVector(textureCoord);

  g_diffuseColor  = texture(u_material.diffuseTexture, textureCoord);
  g_specularColor = vec4(vec3(0.2f), 1.0f); 

  const vec3 ambientLight = u_ambientLightColor * u_ambientLightIntensity;
  vec3 result = vec3(0.0f);

  /* =========================== */
  /* Calculate directional light */
  /* =========================== */
  result += CalculateDirectionalLight(u_directionalLight, normal, viewDir);


  /* ===================== */
  /* Calculate point light */
  /* ===================== */
  for(int i = 0; i < 4; i++)
    result += CalculateBlinnPhongLight(u_pointLight[i], normal, viewDir);


  /* ==================== */
  /* Calculate spot light */
  /* ==================== */
  result += CalculateSpotLight(u_spotLight, normal, viewDir);

  /* Apply gamma correction */
  result = pow(result, vec3(1.0 / g_gamma));
  
  FragColor = vec4(result, 1.0);
}

vec2 CalculateParallaxCoord(vec3 viewDir, float heightScale) {

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
vec3 CalculateNormalVector(vec2 textureCoord) {
  vec3 N;
 
  if(u_useNormalMap != 0){
    /* Obtain normal from normal map in range [0,1] */
    N = texture(u_material.normalTexture, textureCoord).rgb;
    /* Transform normal vector to range [-1,1] */
    N = N * 2.0 - 1.0;
  }
  else 
    N = Normal;

 return N;
}
vec3 CalculateViewDirVector() {
  vec3 viewDir;
  
  if(u_useNormalMap != 0)
    viewDir = normalize(TangentViewPos - TangentFragPos);
  else
    viewDir = normalize(ViewPos - FragPos);

  return viewDir;
}
float CalculateAttenuation(float d, float kl, float kq) {
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  
  return 1.0f / (1.0f + kl*d + kq*pow(d,2));  
}
float CalculateDirLightShadows(vec3 normal, vec3 lightDir){
  /* Perform perspective divide */
  vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
  
  /* Transform to [0,1] range */
  projCoords = projCoords * 0.5 + 0.5;
  
  /* Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords) */
  float closestDepth = texture(u_shadowMapTexture, projCoords.xy).r; 
  
  /* Get depth of current fragment from light's perspective */
  float currentDepth = projCoords.z;
  
  /* Resolve the problem of over sampling */
  if(projCoords.z > 1.0) 
    return 0.0;
  
  float shadow = 0.0f;
  
  /* Resolve the problem of shadow acne with bias */
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

  /* Percentage-closer filtering (PCF) */
  vec2 texelSize = 1.0 / textureSize(u_shadowMapTexture, 0);
  for(int x = -1; x <= 1; ++x)
  {
    for(int y = -1; y <= 1; ++y)
    {
      float pcfDepth = texture(u_shadowMapTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
    }    
  }
  shadow /= 9.0;
  return shadow;
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
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
vec3 CalculateBlinnPhongLight(PointLight light, vec3 normal, vec3 viewDir) {
  const vec3 tangentLightPosition = TBN * light.position;
  
  /* Light direction */
  vec3 lightDir;
  if(u_useNormalMap != 0)
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
  float lightDist;
  if(u_useNormalMap != 0)
    lightDist = length(tangentLightPosition - TangentFragPos);
  else
    lightDist = length(light.position - FragPos);

  const float attenuation = CalculateAttenuation(lightDist, light.attenuation.kl, light.attenuation.kq);
    
  diffuse  *= attenuation;
  specular *= attenuation;
  return diffuse + specular;
}
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
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
  float lightDist;
  if(u_useNormalMap != 0)
    lightDist = length(tangentLightPosition - TangentFragPos);
  else
    lightDist = length(light.position - FragPos);
  
  const float attenuation = CalculateAttenuation(lightDist, light.attenuation.kl, light.attenuation.kq);
  
  diffuse  *= attenuation;
  specular *= attenuation;
  return diffuse + specular;
}
