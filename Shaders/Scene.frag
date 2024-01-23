#version 460

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
uniform Material_t    UMaterial;
uniform DirLight_t    UDirLight;
uniform PointLight_t  UPointLight;
uniform SpotLight_t   USpotLight;
uniform vec3          UViewPos; 
uniform float         UGamma;


/* ---------- Globals variable ---------- */
/* -------------------------------------- */
vec3 normal;
vec3 viewDir;
vec3 diffuseColor;
vec3 specularColor;

vec3 CalcDirLight();
vec3 BlinnPhong(); /* Function to calcolate point lights */
vec3 CalcSpotLight();
vec3 ApplyGammaCorrection(vec3 inputValue);

void main()
{
  /* Init globals */
  normal  = normalize(Normals);
  viewDir = normalize(UViewPos - FragPos);
  diffuseColor  = texture(UMaterial.diffuse, TexCoords).rgb;
  specularColor = texture(UMaterial.specular, TexCoords).rgb;
  vec3 result = vec3(0.0f, 0.0f, 0.0f);

  /* If no texture: set diffuse to default color */
  if(length(diffuseColor) == 0)
    diffuseColor = vec3(0.25f, 0.50f, 0.75f);

  /*
    Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    For each phase, a calculate function is defined that calculates the corresponding color
    per lamp. In the main() function we take all the calculated colors and sum them up for
    this fragment's final color.
  */

  /* Phase 1: Directional lighting */
  result += CalcDirLight();

  /* Phase 2: Point lights */
  // for(int i = 0; i < NR_POINT_LIGHTS; i++)
  //  result += BlinnPhong(pointLights[i]); 
  result += BlinnPhong(); 

  /* Phase 3: Spot light */
  //result += CalcSpotLight(spotLight);  

  /* Apply gamma correction */
  result = ApplyGammaCorrection(result);

  FragColor = vec4(result, 1.0);
}


vec3 ApplyGammaCorrection(vec3 inputValue)
{
  if(UGamma != 0.0f)
    return pow(inputValue, 1.0f/UGamma);
  else
    return inputValue;
}

vec3 CalcDirLight()
{
  vec3 lightDir = normalize(-UDirLight.direction);
  
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), UMaterial.shininess);
  
  // combine results
  vec3 ambient  = UDirLight.ambient  * diffuseColor;
  vec3 diffuse  = UDirLight.diffuse  * diff * diffuseColor;
  vec3 specular = UDirLight.specular * spec * specularColor;
  return (ambient + diffuse + specular);
}

vec3 BlinnPhong()
{
  vec3 lightDir = normalize(UPointLight.position - FragPos);
  
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  if(diff == 0.0f)
    return vec3(0.0f);

  // specular
  vec3 halfwayDir = normalize(lightDir + viewDir);  
  float spec = pow(max(dot(normal, halfwayDir), 0.0), UMaterial.shininess);

  // attenuation
  float distance    = length(UPointLight.position - FragPos);
  float attenuation = 1.0f / (1.0f + UPointLight.linear * distance + UPointLight.quadratic * (distance * distance));    

  // combine results
  vec3 ambient  = UPointLight.ambient  * diffuseColor;
  vec3 diffuse  = UPointLight.diffuse  * diff * diffuseColor;
  vec3 specular = UPointLight.specular * spec * specularColor;

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight()
{
  vec3 lightDir = normalize(USpotLight.position - FragPos);

  // check if lighting is inside the spotlight cone
  float theta = dot(lightDir, normalize(-USpotLight.direction));

  // remember that we're working with angles as cosines instead of degrees so a '>' is used.
  if(theta > USpotLight.cutOff) 
  {
    // ambient
    vec3 ambient = USpotLight.ambient * diffuseColor;
    
    // diffuse 
    vec3 norm     = normalize(normal);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = USpotLight.diffuse * diff * diffuseColor;  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), UMaterial.shininess);
    vec3 specular   = USpotLight.specular * spec * diffuseColor;  
    
    // spotlight (soft edges)
    float intensity = 1.0 - (1.0 - theta) / (1.0 - USpotLight.cutOff);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance    = length(USpotLight.position - FragPos);
    float attenuation = 1.0 / (1.0 + USpotLight.linear * distance + USpotLight.quadratic * (distance * distance));    

    // remove attenuation from ambient, as otherwise at large distances the light would be 
    // darker inside than outside the spotlight due the ambient term in the else branch
    // ambient  *= attenuation; 
    diffuse  *= attenuation;
    specular *= attenuation;   
    
    return vec3(ambient + diffuse + specular);
  }
  else
    return vec3(0.f,0.f,0.f);
}
