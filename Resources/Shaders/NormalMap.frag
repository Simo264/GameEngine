#version 460

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform vec3 viewPos;
uniform vec3 lightPos;

void main()
{
  const vec3 diffColor = texture(diffuseTexture, TexCoords).rgb;
  const vec3 lightAmbient = vec3(0.1f);
  const vec3 lightDiffuse = vec3(0.5f);
  const vec3 lightSpecular = vec3(1.0f);
  const vec3 materialSpecular = vec3(0.5f);
  
  vec3 normal;

  normal = texture(normalTexture, TexCoords).rgb;
  normal = normalize(normal * 2.0 - 1.0);  

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 lightDir = normalize(lightPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, normal); 

  // ambient
  vec3 ambient = lightAmbient * diffColor;
  	
  // diffuse 
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = lightDiffuse * diff * diffColor;  
    
  // specular
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
  vec3 specular = lightSpecular * spec * materialSpecular;  
        
  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
} 