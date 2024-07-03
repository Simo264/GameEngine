#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;  
in vec2 TexCoords;
in mat3 TBN;

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

uniform vec3  u_viewPos;
uniform float u_gamma;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float shininess = 32.0f;
vec3 g_tangentFragPos;
vec3 g_tangentViewPos;
vec2 g_textCoords;
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
  g_tangentFragPos  = TBN * FragPos;
  g_tangentViewPos  = TBN * u_viewPos;

  g_viewDir = normalize(g_tangentViewPos - g_tangentFragPos);
  
  g_textCoords = TexCoords;
  g_normal = texture(u_material.normalTexture, g_textCoords).rgb;
  g_normal = normalize(g_normal * 2.0 - 1.0);
  g_normal = normalize(TBN * g_normal);

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

  return vec3(ambient + diffuse + specular);
}

vec3 CalculateBlinnPhongLight(PointLight light) {
  vec3 lightDir = normalize(light.position - FragPos);

  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor.rgb;

  /* diffuse */
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor.rgb;  
    
  /* specular */
  vec3 halfwayDir = normalize(lightDir + g_viewDir);
  float specularFactor = pow(max(dot(g_normal, halfwayDir), 0.0), shininess);
  vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor.rgb;  

  /* attenuation */
  float dist = length(light.position - FragPos);
  float attenuation = 1.0 / (u_gamma != 0 ? dist * dist : dist);
  
  ambient  *= attenuation;  
  diffuse  *= attenuation;
  specular *= attenuation;   
  return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight light){
  vec3 lightDir = normalize(light.position - FragPos);
    
  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor.rgb;
    
  /* diffuse */
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor.rgb;  
      
  /* specular */
  vec3 reflectDir = reflect(-lightDir, g_normal);  
  float specularFactor = pow(max(dot(g_viewDir, reflectDir), 0.0), shininess);
  vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor.rgb;  
            
  /* soft edges + intensity */
  float theta = dot(lightDir, normalize(-light.direction));
  float cutoff = cos(radians(light.cutOff));
  float outerCutoff = cos(radians(light.outerCutOff));

  float epsilon = (cutoff - outerCutoff);
  float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  float dist = length(light.position - FragPos);
  //float attenuation = 1.0 / (1.0 + (light.linear * dist) + (light.quadratic * dist * dist));    
  float attenuation = 1.0 / (u_gamma != 0 ? dist * dist : dist);
    
  ambient  *= attenuation; 
  diffuse  *= attenuation;
  specular *= attenuation;   
  return ambient + diffuse + specular;
}
