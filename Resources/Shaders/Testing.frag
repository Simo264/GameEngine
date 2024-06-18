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

vec2 ParallaxMapping(const vec2 texCoords, const vec3 viewDir, const float heightScale);


/* Globals */
const vec3 lightAmbient = vec3(0.1f);
const vec3 lightDiffuse = vec3(1.0f);
const vec3 lightSpecular = vec3(1.0f);
const vec3 materialSpecular = vec3(0.0f);
const float heightScale = 0.1f;

void main()
{
  // offset texture coordinates with Parallax Mapping
  const vec3 viewDir   = normalize(TangentViewPos - TangentFragPos);
  const vec2 texCoords = ParallaxMapping(TexCoords, viewDir, heightScale);
  if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    discard;

  // then sample textures with new texture coords
  const vec3 diffColor = texture(u_diffuseTexture, texCoords).rgb;
  vec3 normal = texture(u_normalTexture, texCoords).rgb;
  normal = normalize(normal * 2.0 - 1.0);

  const vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
  const vec3 halfwayDir = normalize(lightDir + viewDir);  
   
  // ambient
  const vec3 ambient = lightAmbient * diffColor;
  
  // diffuse
  const float diff = max(dot(lightDir, normal), 0.0);
  const vec3 diffuse = lightDiffuse * diff * diffColor;
  
  // specular
  const float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
  const vec3 specular = materialSpecular * spec * lightSpecular;
  
  FragColor = vec4(ambient + diffuse + specular, 1.0);
} 


vec2 ParallaxMapping(const vec2 texCoords, const vec3 viewDir, const float heightScale)
{ 
  // number of depth layers
  const float numLayers = 10;
  
  // calculate the size of each layer
  const float layerDepth = 1.0 / numLayers;
  // the amount to shift the texture coordinates per layer (from vector P)
  const vec2 P = viewDir.xy * heightScale; 
  const vec2 deltaTexCoords = P / numLayers;
  
  // get initial values
  float currentLayerDepth = 0.0;
  vec2 currentTexCoords = texCoords;
  float currentDepthMapValue = texture(u_heightTexture, currentTexCoords).r;

  while(currentLayerDepth < currentDepthMapValue)
  {
    // shift texture coordinates along direction of P
    currentTexCoords -= deltaTexCoords;
    // get depthmap value at current texture coordinates
    currentDepthMapValue = texture(u_heightTexture, currentTexCoords).r;  
    // get depth of next layer
    currentLayerDepth += layerDepth;  
  }
  return currentTexCoords;
} 