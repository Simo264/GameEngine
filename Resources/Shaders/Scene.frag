#version 460
#define NR_POINT_LIGHTS 5

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec3 FragPos;  
in vec3 Normal;
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
uniform Material_t    UMaterial;
uniform DirLight_t    UDirLight;
//uniform PointLight_t  UPointLight[NR_POINT_LIGHTS];
//uniform SpotLight_t   USpotLight;
uniform vec3          UViewPos; 
uniform float         UGamma;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 normal;
vec3 viewDir;
vec3 diffuseColor;
vec3 specularColor;

/* ---------- Functions ---------- */
/* ------------------------------- */
vec3 CalcDirLight(DirLight_t dirLight);
//vec3 BlinnPhong(PointLight_t pointLight); /* Function to calcolate point lights */
//vec3 CalcSpotLight(SpotLight_t spotLight);
void GammaCorrection(inout vec3 value); /* value to apply gamma correction */

void main()
{
  /* Init globals */
  normal        = normalize(Normal);
  viewDir       = normalize(UViewPos - FragPos);
  diffuseColor  = texture(UMaterial.diffuse, TexCoords).rgb;
  specularColor = texture(UMaterial.specular, TexCoords).rgb;
  vec3 result   = vec3(0.0f);

  /* If no texture: set diffuse to default color */
  if(length(diffuseColor) == 0)
    diffuseColor = vec3(0.25f, 0.50f, 0.75f);

  /* Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
     For each phase, a calculate function is defined that calculates the corresponding color
     per lamp. In the main() function we take all the calculated colors and sum them up for
     this fragment's final color. */

  /* Phase 1: Directional lighting */
  result += CalcDirLight(UDirLight);

  /* Phase 2: Point lights */
  //  for(int i = 0; i < NR_POINT_LIGHTS; i++)
  //    result += BlinnPhong(UPointLight[i]); 
  

  /* Phase 3: Spot light */
  //result += CalcSpotLight(spotLight);  

  /* Apply gamma correction */
  if(UGamma != 0.0f)
    GammaCorrection(result);

  FragColor = vec4(result, 1.0);
}

void GammaCorrection(inout vec3 value)
{
  value = pow(value, vec3(1.0f/UGamma));
}

vec3 CalcDirLight(DirLight_t dirLight)
{
  vec3 lightDir = normalize(-dirLight.direction);
  
  /* ambient shading */
  vec3 ambient  = dirLight.ambient * diffuseColor;

  /* diffuse shading */
  float diffuseFactor = max(dot(normal, lightDir), 0.0);
  if(diffuseFactor == 0.0f)
    return ambient;

  vec3 diffuse = dirLight.diffuse * diffuseFactor * diffuseColor;
  
  /* specular shading */
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), UMaterial.shininess);
  vec3 specular = dirLight.specular * specularFactor * specularColor;

  /* combine results */
  return (ambient + diffuse + specular);
}

//vec3 BlinnPhong(PointLight_t pointLight)
//{
//  /* attenuation */
//  float distance = length(pointLight.position - FragPos);
//  float attenuation;
//  if(UGamma != 0.0f)
//    attenuation = 1.0f / (distance);    
//  else
//    attenuation = 1.0f / (distance * distance);
//
//  /* ambient shading */
//  vec3 ambient = pointLight.ambient  * diffuseColor;
//  ambient *= attenuation;
//
//  /* diffuse shading */
//  vec3 lightDir = normalize(pointLight.position - FragPos);
//  float diffuseFactor = max(dot(normal, lightDir), 0.0);
//  if(diffuseFactor == 0.0f)
//    return ambient;
//    
//  vec3 diffuse  = pointLight.diffuse * diffuseFactor * diffuseColor;
//  diffuse *= attenuation;
//
//  /* specular shading */
//  vec3 halfwayDir = normalize(lightDir + viewDir);  
//  float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), UMaterial.shininess);
//  vec3 specular = pointLight.specular * specularFactor * specularColor;
//  specular *= attenuation;
//
//  /* combine results */
//  return (ambient + diffuse + specular);
//}

//vec3 CalcSpotLight(SpotLight_t spotLight)
//{
//  vec3 lightDir = normalize(spotLight.position - FragPos);
//
//  /* check if lighting is inside the spotlight cone */
//  float theta = dot(lightDir, normalize(-spotLight.direction));
//
//  /* remember that we're working with angles as cosines instead of degrees so a '>' is used. */
//  if(theta > spotLight.cutOff) 
//  {
//    /* ambient */
//    vec3 ambient = spotLight.ambient * diffuseColor;
//    
//    /* diffuse */
//    vec3 norm     = normalize(normal);
//    float diff    = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse  = spotLight.diffuse * diff * diffuseColor;  
//    
//    /* specular */
//    vec3 reflectDir = reflect(-lightDir, norm);  
//    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), UMaterial.shininess);
//    vec3 specular   = spotLight.specular * spec * diffuseColor;  
//    
//    /* spotlight (soft edges) */
//    float intensity = 1.0 - (1.0 - theta) / (1.0 - spotLight.cutOff);
//    diffuse  *= intensity;
//    specular *= intensity;
//
//    /* attenuation */
//    float distance    = length(spotLight.position - FragPos);
//    float attenuation = 1.0 / (1.0 + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    
//
//
//    /* remove attenuation from ambient, as otherwise at large distances the light would be 
//       darker inside than outside the spotlight due the ambient term in the else branch
//       ambient  *= attenuation; */
//    diffuse  *= attenuation;
//    specular *= attenuation;   
//    return vec3(ambient + diffuse + specular);
//  }
//  else
//    return vec3(0.f,0.f,0.f);
//}
