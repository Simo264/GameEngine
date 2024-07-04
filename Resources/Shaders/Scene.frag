#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;  
in vec2 TexCoords;
in mat3 TBN;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

/* ---------- OUT attributes ---------- */
/* ------------------------------------ */
out vec4 FragColor;


/* ---------- Structs ---------- */
/* ----------------------------- */
struct Material {
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
  sampler2D heightTexture;
};
struct DirectionalLight {
  vec3  color;
  float ambient;
  float diffuse;
  float specular;
  vec3  direction;
};
struct PointLight {
  vec3  color;
  float ambient;
  float diffuse;
  float specular;
  vec3  position;
  float linear;     /* attenuation */
  float quadratic;  /* attenuation */
};
struct SpotLight {
  vec3  color;
  float ambient;
  float diffuse;
  float specular;
  vec3  position;
  vec3  direction;
  float linear;     /* attenuation */
  float quadratic;  /* attenuation */
  float cutOff;
  float outerCutOff; /* smoother edges */
};


/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform Material          u_material;
uniform DirectionalLight  u_directionalLight;
uniform PointLight        u_pointLight[4];
uniform SpotLight         u_spotLight;

uniform float u_gamma;
uniform vec3 u_viewPos;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float shininess = 32.0f;
vec3 g_tangentFragPos;
vec3 g_tangentViewPos;
vec3 g_normal; 
vec3 g_viewDir;
vec4 g_diffuseColor;
vec4 g_specularColor;


/* ---------- Functions ---------- */
/* ------------------------------- */
vec3 CalculateDirectionalLight(DirectionalLight light);
vec3 CalculateBlinnPhongLight(PointLight light);
vec3 CalculateSpotLight(SpotLight light);
vec2 ParallaxMapping(const vec2 texCoords, const vec3 viewDir, const float heightScale);

void main()
{
  /* Init globals */
  g_normal = texture(u_material.normalTexture, TexCoords).rgb;
  g_normal = normalize(g_normal * 2.0 - 1.0);

  g_viewDir = normalize(TangentViewPos - TangentFragPos);

  g_diffuseColor  = texture(u_material.diffuseTexture, TexCoords);
  g_specularColor = texture(u_material.specularTexture, TexCoords);
  if(g_diffuseColor.a < 0.5)
    discard;

  vec3 result = vec3(0, 0, 0);
  
  /* Calculate directional light */
  result += CalculateDirectionalLight(u_directionalLight);

  /* Calculate point light */
  for(int i = 0; i < 4; i++)
    result += CalculateBlinnPhongLight(u_pointLight[i]);

  /* Calculate spot light */
  result += CalculateSpotLight(u_spotLight);

  /* apply gamma correction */
  if(u_gamma != 0)
    result = pow(result, vec3(1.0 / u_gamma));
  
  FragColor = vec4(result, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light){
  const vec3 lightDir = normalize(-light.direction);
    
  /* ambient shading */
  const vec3 ambient = (light.color * light.ambient) * g_diffuseColor.rgb;

  /* diffuse shading */ 
  const float diff = max(dot(lightDir, g_normal), 0.0);
  const vec3 diffuse = (light.color * light.diffuse) * diff * g_diffuseColor.rgb;

  /* specular shading */ 
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float spec = pow(max(dot(g_normal, halfwayDir), 0.0), shininess);
  const vec3 specular = (light.color * light.specular) * spec * g_specularColor.rgb;

  return ambient + diffuse + specular;
}

vec3 CalculateBlinnPhongLight(PointLight light) {
  const vec3 tangentLightPos = TBN * light.position;
  const vec3 lightDir = normalize(tangentLightPos - TangentFragPos);

  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor.rgb;

  /* diffuse */
  const float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor.rgb;  
   
  /* specular */
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specularFactor = pow(max(dot(g_normal, halfwayDir), 0.0), shininess);
  vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor.rgb;  

  /* attenuation */
  const float dist = length(tangentLightPos - TangentFragPos);
  const float attenuation = 1.0f / pow(dist, 2);
    
  ambient  *= attenuation;  
  diffuse  *= attenuation;
  specular *= attenuation;   
  return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight light){
  const vec3 tangentLightPos = TBN * light.position;
  const vec3 lightDir = normalize(tangentLightPos - TangentFragPos);
    
  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor.rgb;
    
  /* diffuse */
  const float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor.rgb;  
      
  /* specular */
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specularFactor = pow(max(dot(g_normal, halfwayDir), 0.0), shininess);
  vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor.rgb;  
            
  /* soft edges + intensity */
  const float theta = dot(lightDir, normalize(-light.direction));
  const float cutoff = cos(radians(light.cutOff));
  const float outerCutoff = cos(radians(light.outerCutOff));
  float epsilon = (cutoff - outerCutoff);
  float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  /* attenuation */
  float dist = length(tangentLightPos - TangentFragPos);
  float attenuation = 1.0f / (1.0f + light.linear * dist + light.quadratic * pow(dist, 2));    
  //const float attenuation = 1.0f / pow(dist, 2);

  ambient  *= attenuation; 
  diffuse  *= attenuation;
  specular *= attenuation;
  return ambient + diffuse + specular;
}
