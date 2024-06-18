#version 460

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform vec3 viewPos;

void main()
{
  vec3 diffColor = texture(diffuseTexture, TexCoords).rgb;
  vec3 lightAmbient = vec3(0.1f);
  vec3 lightDiffuse = vec3(0.5f);
  vec3 lightSpecular = vec3(1.0f);
  vec3 materialSpecular = vec3(0.25f);

  vec3 normal;
  normal = texture(normalTexture, TexCoords).rgb;
  normal = normalize(normal * 2.0 - 1.0);

  vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
  vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
  vec3 halfwayDir = normalize(lightDir + viewDir);  
   
  // ambient
  vec3 ambient = lightAmbient * diffColor;
  
  // diffuse
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * diffColor;
  
  // specular
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

  vec3 specular = materialSpecular * spec * lightSpecular;
  FragColor = vec4(ambient + diffuse + specular, 1.0);
} 