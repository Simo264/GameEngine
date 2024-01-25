#version 460
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D UDiffuseTexture;  /* 0 */
uniform sampler2D USpecularTexture; /* 1 */
uniform sampler2D UShadowMap;       /* 2 */

uniform vec3 ULightPos;
uniform vec3 UViewPos;

float ShadowCalculation();

void main()
{           
  vec3 diffuseColor = texture(UDiffuseTexture, TexCoords).rgb;
  
  vec3 normal = normalize(Normal);
  vec3 lightColor = vec3(1.0);
  
  /* Ambient */
  vec3 ambient = 0.3 * lightColor;
  
  /* Diffuse */
  vec3 lightDir = normalize(ULightPos - FragPos);
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * lightColor;
  
  /* Specular */
  vec3 viewDir = normalize(UViewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  vec3 halfwayDir = normalize(lightDir + viewDir);  
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec3 specular = spec * lightColor;    
  
  /* Calculate shadow */
  float shadow = ShadowCalculation();                      
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
  /* Check whether current frag pos is in shadow */
  float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
  return shadow;
}