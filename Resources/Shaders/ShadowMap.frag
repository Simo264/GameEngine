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
struct Material{
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
  sampler2D heightTexture;
};
struct DirectionalLight{
  vec3  color;
  float ambient;
  float diffuse;
  float specular;
  vec3  direction;
};
struct PointLight{
  vec3  color;
  float ambient;
  float diffuse;
  float specular;
  vec3  position;
  float linear;     /* attenuation */
  float quadratic;  /* attenuation */
};
struct SpotLight{
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

uniform sampler2D u_shadowMapTexture; 
uniform vec3  u_lightPos;
uniform vec3  u_viewPos;
uniform float u_gamma;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 g_diffuseColor;
vec3 g_specularColor;
vec3 g_normal;
vec3 g_viewDir;

/* ---------- Functions ---------- */
/* ------------------------------- */
vec3 CalculateDirectionalLight(DirectionalLight light);
float CalculateShadows(vec3 lightDir);
vec3 CalculateBlinnPhongLight(PointLight light);
vec3 CalculateSpotLight(SpotLight light);

void main()
{     
    /* Init globals */
    g_normal        = normalize(Normal);
    g_viewDir       = normalize(u_viewPos - FragPos);
    g_diffuseColor  = texture(u_material.diffuseTexture, TexCoords).rgb;
    g_specularColor = texture(u_material.specularTexture, TexCoords).rgb;
    
    vec3 result = vec3(0, 0, 0);
  
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


vec3 CalculateDirectionalLight(DirectionalLight light)
{
  vec3 lightDir = normalize(u_lightPos - FragPos);
  
  /* ambient shading */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor;

  /* diffuse shading */
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor; 
  
  /* specular shading */
  vec3 reflectDir       = reflect(-lightDir, g_normal);
  float specularFactor  = pow(max(dot(g_viewDir, reflectDir), 0.0), 64.0f);
  vec3 specular         = (light.color * light.specular) * specularFactor * g_specularColor;  

  /* Calculate shadow */
  float shadow = CalculateShadows(lightDir);
  return (ambient + (1.0f - shadow) * (diffuse + specular)) * g_diffuseColor;   
}

float CalculateShadows(vec3 lightDir)
{
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

vec3 CalculateBlinnPhongLight(PointLight light)
{
  /* ambient */
  vec3 ambient = (light.color * light.ambient) * g_diffuseColor;

  /* diffuse */
  vec3 lightDir = normalize(light.position - FragPos);
  float diffuseFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = (light.color * light.diffuse) * diffuseFactor * g_diffuseColor;  
    
  /* specular */
  vec3 halfwayDir = normalize(lightDir + g_viewDir);
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
  float specularFactor = pow(max(dot(g_viewDir, reflectDir), 0.0), 64.0f);
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


