#version 460

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 FragColor;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_heightTexture;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float heightScale);


/* Globals */
vec3 lightAmbient = vec3(0.1f);
vec3 lightDiffuse = vec3(1.0f);
vec3 lightSpecular = vec3(1.0f);
vec3 materialSpecular = vec3(0.0f);
float heightScale = 0.1f;

void main()
{
  // offset texture coordinates with Parallax Mapping
  vec3 viewDir   = normalize(TangentViewPos - TangentFragPos);
  vec2 texCoords = ParallaxMapping(TexCoords, viewDir, heightScale);
  if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    discard;

  // then sample textures with new texture coords
  vec3 diffColor = texture(u_diffuseTexture, texCoords).rgb;
  vec3 normal = texture(u_normalTexture, texCoords).rgb;
  normal = normalize(normal * 2.0 - 1.0);

  vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
  vec3 halfwayDir = normalize(lightDir + viewDir);  
   
  // ambient
  vec3 ambient = lightAmbient * diffColor;
  
  // diffuse
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = lightDiffuse * diff * diffColor;
  
  // specular
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

  vec3 specular = materialSpecular * spec * lightSpecular;
  FragColor = vec4(ambient + diffuse + specular, 1.0);
} 


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, float heightScale)
{ 
  float height = texture(u_heightTexture, texCoords).r;    
  vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
  return texCoords - p;    
} 