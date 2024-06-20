#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;  
in vec3 Normal;
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
uniform int u_hasNormalMap;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 g_diffuseColor;
vec3 g_specularColor;
vec3 g_normal; 
vec3 g_viewdir;
vec3 g_tangentFragPos;
vec3 g_tangentViewPos;
vec3 g_tangentLightPos;

/* ---------- Functions ---------- */
/* ------------------------------- */
vec3 CalculateDirectionalLight(DirectionalLight light);
vec3 CalculateBlinnPhongLight(PointLight light);
vec3 CalculateSpotLight(SpotLight light);

void main()
{
  /* Init globals */
  g_diffuseColor  = texture(u_material.diffuseTexture, TexCoords).rgb;
  g_specularColor = texture(u_material.specularTexture, TexCoords).rgb;

  if(u_hasNormalMap != 0)
  {
    vec3 lightpos = vec3(0.0f, 30.0f, 0.0f);
    g_tangentFragPos  = TBN * FragPos;
    g_tangentViewPos  = TBN * u_viewPos;
    g_tangentLightPos = TBN * lightpos;

    g_normal = texture(u_material.normalTexture, TexCoords).rgb;
    g_normal = normalize(g_normal * 2.0 - 1.0);
    g_viewdir = normalize(g_tangentViewPos - g_tangentFragPos);
  }
  else
  {
    g_normal = normalize(Normal);
    g_viewdir = normalize(u_viewPos - FragPos);
  }

  vec3 result = vec3(0, 0, 0);
  
  /* Calculate directional light */
  result += CalculateDirectionalLight(u_directionalLight);

  /* Calculate point light */
  //for(int i = 0; i < 4; i++)
  //  result += CalculateBlinnPhongLight(u_pointLight[i]);

  /* Calculate spot light */
  //result += CalculateSpotLight(u_spotLight);

  /* apply gamma correction */
  if(u_gamma != 0)
    result = pow(result, vec3(1.0 / u_gamma));
  
  FragColor = vec4(result, 1.0);
}



vec3 CalculateDirectionalLight(DirectionalLight light)
{
  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor;
  
  /* diffuse */
  //vec3 lightDir = normalize(g_tangentLightPos - g_tangentFragPos);
  vec3 lightDir = normalize(-light.direction);  
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor;  
    
  /* specular */
  vec3 halfwayDir = normalize(lightDir + g_viewdir);
  float specularFactor = pow(max(dot(g_normal, halfwayDir), 0.0), 64.0f);
  //vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor;  
  vec3 specular = vec3(0.2f) * specularFactor;  
  
  return ambient + diffuse + specular;
}

vec3 CalculateBlinnPhongLight(PointLight light)
{
  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor;

  /* diffuse */
  vec3 lightDir = normalize(light.position - FragPos);
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor;  
    
  /* specular */
  vec3 halfwayDir = normalize(lightDir + g_viewdir);
  float specularFactor = pow(max(dot(g_normal, halfwayDir), 0.0), 64.0f);
  vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor;  
    
  /* attenuation */
  float dist = length(light.position - FragPos);
  /* linear attenuation */
  //float attenuation = 1.0 / (dist);
  /* quadratic attenuation */
  //float attenuation = 1.0 / (dist * dist);
  float attenuation = 1.0 / (u_gamma != 0 ? dist * dist : dist);


  ambient  *= attenuation;  
  diffuse  *= attenuation;
  specular *= attenuation;   
  return ambient + diffuse + specular;
}

vec3 CalculateSpotLight(SpotLight light)
{
  vec3 lightDir = normalize(light.position - FragPos);
    
  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor;
    
  /* diffuse */
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor;  
      
  /* specular */
  vec3 reflectDir = reflect(-lightDir, g_normal);  
  float specularFactor = pow(max(dot(g_viewdir, reflectDir), 0.0), 64.0f);
  vec3 specular = (light.color * light.specular) * specularFactor * g_specularColor;  
            
  /* soft edges + intensity */
  float theta = dot(lightDir, normalize(-light.direction));
  float cutoff = cos(radians(light.cutOff));
  float outerCutoff = cos(radians(light.outerCutOff));

  float epsilon = (cutoff - outerCutoff);
  float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  float dist = length(light.position - FragPos);
  float attenuation = 1.0 / (1.0 + (light.linear * dist) + (light.quadratic * dist * dist));    
    
  ambient  *= attenuation; 
  diffuse  *= attenuation;
  specular *= attenuation;   
  return ambient + diffuse + specular;
}