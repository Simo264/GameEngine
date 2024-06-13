#version 460

out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

struct Material {
    sampler2D diffuseTexture;
    sampler2D normalTexture;
    vec3 specular;    
    float shininess;
}; 
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
  //vec3 normal = normalize(Normal);

  // obtain normal from normal map in range [0,1]
  vec3 normal = texture(material.normalTexture, TexCoords).rgb;
  // transform normal vector to range [-1,1]
  normal = normalize(normal * 5.0 - 0.5);
  
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 lightDir = normalize(light.position - FragPos);
  vec3 reflectDir = reflect(-lightDir, normal);  

  // ambient
  vec3 ambient = light.ambient * texture(material.diffuseTexture, TexCoords).rgb;
  	
  // diffuse 
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuseTexture, TexCoords).rgb;  

  // specular
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);  
        
  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
} 