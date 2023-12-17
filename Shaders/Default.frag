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
  sampler2D diffuse;
  sampler2D specular; 
  float     shininess;
};

struct DirLight_t {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};


/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform Material_t Material;
uniform DirLight_t DirLight;
uniform vec3       ViewPos; 


/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 normal;
vec3 viewDir;
vec3 textureDiffuse;
vec3 textureSpecular;
//vec3 textureNormal;


vec3 CalcDirLight();

void main()
{
  // properties
  normal  = normalize(Normals);
  viewDir = normalize(ViewPos - FragPos);
  textureDiffuse  = texture(Material.diffuse,  TexCoords).rgb;
  textureSpecular = texture(Material.specular, TexCoords).rgb;
  //texNorm = texture(material.normal,   TexCoords).rgb;

  // phase 1: Directional lighting
  vec3 result = CalcDirLight();

  // phase 2: Point lights
  // for(int i = 0; i < NR_POINT_LIGHTS; i++)
  //  result += CalcPointLight(pointLights[i]); 
  //result += CalcPointLight(pointLight); 

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
  vec3  ambient  = DirLight.ambient  * textureDiffuse;
  vec3  diffuse  = DirLight.diffuse  * diff * textureDiffuse;
  vec3  specular = DirLight.specular * spec * textureSpecular;
  return (ambient + diffuse + specular);
}