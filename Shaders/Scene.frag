#version 330 core

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;  
in vec3 Normals;
in vec2 TexCoords; 

/* ---------- OUT attributes ---------- */
/* ------------------------------------ */
out vec4 FragColor;


/* ---------- Structs ---------- */
/* ----------------------------- */
struct Material_t {
  sampler2D diffuse;  // texture diffuse
  sampler2D specular; // texture specular
  float     shininess;
};

struct DirLight_t {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight_t {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float linear;
  float quadratic;
};

struct SpotLight_t {
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float cutOff;
  float outCutOff;

  float linear;
  float quadratic;
};

/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform Material_t    Material;
uniform DirLight_t    DirLight;
uniform PointLight_t  PointLight;
uniform SpotLight_t   SpotLight;
uniform vec3          ViewPos; 


/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 normal;
vec3 viewDir;
vec3 diffuseColor;
vec3 specularColor;
//vec3 textureNormal;


vec3 CalcDirLight();
vec3 CalcPointLight();
vec3 CalcSpotLight();

void main()
{
  // properties
  normal  = normalize(Normals);
  viewDir = normalize(ViewPos - FragPos);
  diffuseColor  = texture(Material.diffuse,  TexCoords).rgb;
  specularColor = texture(Material.specular, TexCoords).rgb;
  //texNorm       = texture(material.normal,   TexCoords).rgb;

  // if no texture: set diffuse to default color gray
  if(length(diffuseColor) == 0)
    diffuseColor = vec3(0.25f, 0.50f, 0.75f);

  // phase 1: Directional lighting
  vec3 result = CalcDirLight();

  // phase 2: Point lights
  // for(int i = 0; i < NR_POINT_LIGHTS; i++)
  //  result += CalcPointLight(pointLights[i]); 
  result += CalcPointLight(); 

  // phase 3: Spot light
  //result += CalcSpotLight(spotLight);  

  FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight()
{
  vec3 lightDir = normalize(-DirLight.direction);
  
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  
  // specular shading
  vec3  reflectDir = reflect(-lightDir, normal);
  float spec      = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
  
  // combine results
  vec3  ambient  = DirLight.ambient  * diffuseColor;
  vec3  diffuse  = DirLight.diffuse  * diff * diffuseColor;
  vec3  specular = DirLight.specular * spec * specularColor;
  return (ambient + diffuse + specular);
}

vec3 CalcPointLight()
{
  vec3 lightDir = normalize(PointLight.position - FragPos);
  
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
  
  // attenuation
  float distance    = length(PointLight.position - FragPos);
  float attenuation = 1.0 / (1.0 + PointLight.linear * distance + PointLight.quadratic * (distance * distance));    
  
  // combine results
  vec3 ambient  = PointLight.ambient  * diffuseColor;
  vec3 diffuse  = PointLight.diffuse  * diff * diffuseColor;
  vec3 specular = PointLight.specular * spec * specularColor;
  
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight()
{
  vec3 lightDir = normalize(SpotLight.position - FragPos);

  // check if lighting is inside the spotlight cone
  float theta = dot(lightDir, normalize(-SpotLight.direction));

  // remember that we're working with angles as cosines instead of degrees so a '>' is used.
  if(theta > SpotLight.cutOff) 
  {
    // ambient
    vec3 ambient = SpotLight.ambient * diffuseColor;
    
    // diffuse 
    vec3 norm     = normalize(normal);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = SpotLight.diffuse * diff * diffuseColor;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
    vec3 specular   = SpotLight.specular * spec * diffuseColor;  
    
    // spotlight (soft edges)
    float intensity = 1.0 - (1.0 - theta) / (1.0 - SpotLight.cutOff);
    diffuse  *= intensity;
    specular *= intensity;



    // attenuation
    float distance    = length(SpotLight.position - FragPos);
    float attenuation = 1.0 / (1.0 + SpotLight.linear * distance + SpotLight.quadratic * (distance * distance));    

    // remove attenuation from ambient, as otherwise at large distances the light would be 
    // darker inside than outside the spotlight due the ambient term in the else branch
    // ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   
    
    return vec3(ambient + diffuse + specular);
  }
  else 
  {
    return vec3(0.f,0.f,0.f);
  }
}
