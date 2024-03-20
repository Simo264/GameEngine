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

/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform sampler2D UDiffuseTexture;  /* GL_TEXTURE0 */
uniform sampler2D USpecularTexture; /* GL_TEXTURE1 */
uniform sampler2D UShadowMap;       /* GL_TEXTURE2 */
uniform vec3 ULightPos;
uniform vec3 UViewPos;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 diffuseColor;
vec3 normal;
vec3 lightDir;

/* ---------- Functions ---------- */
/* ------------------------------- */
float ShadowCalculation();


void main()
{     
  /* Init globals */
  diffuseColor  = texture(UDiffuseTexture, TexCoords).rgb;
  normal        = normalize(Normal);
  lightDir      = normalize(ULightPos - FragPos);

  /* Ambient */
  vec3 lightColor = vec3(0.25f);
  vec3 ambient    = 0.3 * lightColor;
  
  /* Diffuse */
  float diff    = max(dot(lightDir, normal), 0.0);
  vec3 diffuse  = diff * lightColor;
    
  /* Specular */
  vec3 viewDir    = normalize(UViewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 halfwayDir = normalize(lightDir + viewDir);  
  float spec      = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
  vec3 specular   = spec * lightColor;    
    
  /* Calculate shadow */
  float shadow  = ShadowCalculation();                      
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuseColor;    

  FragColor = vec4(lighting, 1.0);
}

float ShadowCalculation()
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
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
  //return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;

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