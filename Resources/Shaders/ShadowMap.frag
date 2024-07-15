#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;
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

uniform sampler2D u_shadowMapTexture; 
uniform float     u_gamma;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float g_shininess = 32.0f;
vec3 g_normal;
vec3 g_viewDir;
vec4 g_diffuseColor;
vec4 g_specularColor;

/* ---------- Functions ---------- */
/* ------------------------------- */
float CalculateAttenuation(float d, float kl, float kq);
vec3 CalculateDirectionalLight(DirectionalLight light);
float CalculateShadows(vec3 lightDir);
vec3 CalculateBlinnPhongLight(PointLight light);
vec3 CalculateSpotLight(SpotLight light);

void main()
{    
  /* Init globals */
  g_normal = texture(u_material.normalTexture, TexCoords).rgb;
  g_normal = normalize(g_normal*2.0 - 1.0);

  g_viewDir = normalize(TangentViewPos - TangentFragPos);
    
  g_diffuseColor  = texture(u_material.diffuseTexture, TexCoords);
  g_specularColor = texture(u_material.specularTexture, TexCoords);
  //g_diffuseColor  = vec4(1.0f,0.25f,0.75f,1.0f); 
  //g_specularColor = vec4(0.2f,0.2f,0.2f,1.0f);
  if(g_diffuseColor.a < 0.1)
    discard;

  const vec3 ambientLight = u_ambientLightColor * u_ambientLightIntensity;
  vec3 result = ambientLight;
  
  /* Calculate Directional lighting */
  result += CalculateDirectionalLight(u_directionalLight);

  /* Calculate point lights */
  for(int i = 0; i < 4; i++)
    result += CalculateBlinnPhongLight(u_pointLight[i]);

  /* Calculate spot light */
  result += CalculateSpotLight(u_spotLight);
  
  /* Apply gamma correction */
  if(u_gamma != 0)
    result = pow(result, vec3(1.0 / u_gamma));
  
  FragColor = vec4(result, 1.0);
}

float CalculateAttenuation(float d, float kl, float kq){
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  
  return 1.0f / (1.0f + kl*d + kq*pow(d,2));  
}
float CalculateShadows(vec3 lightDir){
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
  float bias = max(0.05 * (1.0 - dot(g_normal, lightDir)), 0.005);  

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

vec3 CalculateDirectionalLight(DirectionalLight light) {
  const vec3 lightDir = normalize(-light.direction);
    
  /* diffuse shading */ 
  const float diffFactor = max(dot(lightDir, g_normal), 0.0);
  const vec3 diffuse = light.color * light.diffuseIntensity * diffFactor * g_diffuseColor.rgb;

  /* specular shading */ 
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specFactor = pow(max(dot(g_normal, halfwayDir), 0.0), g_shininess);
  const vec3 specular = light.color * light.specularIntensity * specFactor * g_specularColor.rgb;

  /* calculate shadow */
  const float shadow = CalculateShadows(lightDir);

  return ((1.0f - shadow) * (diffuse + specular)) * g_diffuseColor.rgb;  
}
vec3 CalculateBlinnPhongLight(PointLight light) {
  const vec3 tangentLightPos = TBN*light.position;
  const vec3 lightDir = normalize(tangentLightPos - TangentFragPos);

  /* diffuse */
  const float diffFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = light.color * light.diffuseIntensity * diffFactor * g_diffuseColor.rgb;
   
  /* specular */
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specFactor = pow(max(dot(g_normal, halfwayDir), 0.0), g_shininess);
  vec3 specular = light.color * light.specularIntensity * specFactor * g_specularColor.rgb;

  /* attenuation */
  const float d = length(tangentLightPos - TangentFragPos);
  const float attenuation = CalculateAttenuation(d, light.attenuation.kl, light.attenuation.kq);
    
  diffuse  *= attenuation;
  specular *= attenuation;   
  return diffuse + specular;
}
vec3 CalculateSpotLight(SpotLight light){
  const vec3 tangentLightPos = TBN * light.position;
  const vec3 lightDir = normalize(tangentLightPos - TangentFragPos);

  /* diffuse */
  const float diffFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = light.color * light.diffuseIntensity * diffFactor * g_diffuseColor.rgb;
      
  /* specular */
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specFactor = pow(max(dot(g_normal, halfwayDir), 0.0), g_shininess);
  vec3 specular = light.color * light.specularIntensity * specFactor * g_specularColor.rgb;
            
  /* soft edges + intensity */
  const float cutoff = cos(radians(light.cutOff));
  const float outerCutoff = cos(radians(light.outerCutOff));
  const float theta = dot(lightDir, normalize(-light.direction));
  const float epsilon = (cutoff - outerCutoff);
  const float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  /* attenuation */
  const float d = length(tangentLightPos - TangentFragPos);
  const float attenuation = CalculateAttenuation(d, light.attenuation.kl, light.attenuation.kq);

  diffuse  *= attenuation;
  specular *= attenuation;
  return diffuse + specular;
}
