#version 460

/* ---------- IN attributes ---------- */
/* ----------------------------------- */
in vec2 TexCoords;
in vec3 Normals;
in vec3 FragPos;  
in mat3 TBN;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 ViewPos;

/* ---------- OUT attributes ---------- */
/* ------------------------------------ */
out vec4 FragColor;


/* ---------- Structs ---------- */
/* ----------------------------- */
struct Attenuation{
  float kl; /* linear attenuation factor */
  float kq; /* quadratic attenuation factor */
};
struct Material {
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
  sampler2D heightTexture;
};
struct DirectionalLight {
  vec3  color;
  float diffuseIntensity;
  float specularIntensity;
  vec3  direction;
};
struct PointLight {
  vec3  color;
  float diffuseIntensity;
  float specularIntensity;
  vec3  position;
  
  Attenuation attenuation;
};
struct SpotLight {
  vec3  color;
  float diffuseIntensity;
  float specularIntensity;
  vec3  position;
  vec3  direction;
  float cutOff;
  float outerCutOff; /* smoother edges */

  Attenuation attenuation;
};


/* ---------- Uniforms ---------- */
/* ------------------------------ */
uniform Material          u_material;
uniform DirectionalLight  u_directionalLight;
uniform PointLight        u_pointLight[4];
uniform SpotLight         u_spotLight;

uniform vec3  u_ambientLightColor;
uniform float u_ambientLightIntensity;  

uniform float u_gamma;

/* ---------- Globals variable ---------- */
/* -------------------------------------- */
const float g_shininess = 32.0f;
vec3 g_tangentFragPos;
vec3 g_tangentViewPos;
vec3 g_normal; 
vec3 g_viewDir;
vec4 g_diffuseColor;
vec4 g_specularColor;


/* ---------- Functions ---------- */
/* ------------------------------- */
float CalculateAttenuation(float d, float kl, float kq);
vec3 CalculateDirectionalLight(DirectionalLight light);
vec3 CalculateBlinnPhongLight(PointLight light);
vec3 CalculateSpotLight(SpotLight light);
vec2 ParallaxMapping(const vec2 texCoords, const vec3 viewDir, const float heightScale);

void main() {

  /* Init globals */
  g_normal = texture(u_material.normalTexture, TexCoords).rgb;
  g_normal = normalize(g_normal * 2.0 - 1.0);
  //g_normal = normalize(Normals);

  g_viewDir = normalize(TangentViewPos - TangentFragPos);
  //g_viewDir = normalize(ViewPos - FragPos);

  g_diffuseColor  = texture(u_material.diffuseTexture, TexCoords);
  g_specularColor = vec4(0.2f,0.2f,0.2f,1.0f); //texture(u_material.specularTexture, TexCoords); //vec4(0.2f,0.2f,0.2f,1.0f); 
  if(g_diffuseColor.a < 0.1)
    discard;

  const vec3 ambientLight = u_ambientLightColor * u_ambientLightIntensity;
  vec3 result = ambientLight;
  
  /* Calculate directional light */
  //result += CalculateDirectionalLight(u_directionalLight);

  /* Calculate point light */
  for(int i = 0; i < 4; i++)
    result += CalculateBlinnPhongLight(u_pointLight[i]);

  /* Calculate spot light */
  //result += CalculateSpotLight(u_spotLight);

  /* apply gamma correction */
  if(u_gamma != 0)
    result = pow(result, vec3(1.0 / u_gamma));
  
  FragColor = vec4(result, 1.0);
}

float CalculateAttenuation(float d, float kl, float kq){
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  
  return 1.0f / (1.0f + kl*d + kq*pow(d,2));  
}
vec3 CalculateDirectionalLight(DirectionalLight light){
  const vec3 lightDir = normalize(-light.direction);
    
  /* diffuse shading */ 
  const float diffFactor = max(dot(lightDir, g_normal), 0.0);
  const vec3 diffuse = light.color * light.diffuseIntensity * diffFactor * g_diffuseColor.rgb;

  /* specular shading */ 
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specFactor = pow(max(dot(g_normal, halfwayDir), 0.0), g_shininess);
  const vec3 specular = light.color * light.specularIntensity * specFactor * g_specularColor.rgb;

  return diffuse + specular;
}
vec3 CalculateBlinnPhongLight(PointLight light) {
  const vec3 tangentLightPos = TBN * light.position;
  const vec3 lightDir = normalize(tangentLightPos - TangentFragPos);
  //const vec3 lightDir = normalize(light.position - FragPos);

  /* diffuse */
  const float diffFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = light.color * light.diffuseIntensity * diffFactor * g_diffuseColor.rgb;
   
  /* specular */
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specFactor = pow(max(dot(g_normal, halfwayDir), 0.0), g_shininess);
  vec3 specular = light.color * light.specularIntensity * specFactor * g_specularColor.rgb;

  /* attenuation */
  const float d = length(tangentLightPos - TangentFragPos);
  //const float d = length(light.position - FragPos);
  const float attenuation = CalculateAttenuation(d, light.attenuation.kl, light.attenuation.kq);
    
  diffuse  *= attenuation;
  specular *= attenuation;   
  return diffuse + specular;
}
vec3 CalculateSpotLight(SpotLight light){
  const vec3 tangentLightPos = TBN * light.position;
  const vec3 lightDir = normalize(tangentLightPos - TangentFragPos);
    
  /* diffuse */
  const float diffFactor = max(dot(g_normal, lightDir), 0.0);
  vec3 diffuse = light.color * light.diffuseIntensity * diffFactor * g_diffuseColor.rgb;
      
  /* specular */
  const vec3 halfwayDir = normalize(lightDir + g_viewDir);
  const float specFactor = pow(max(dot(g_normal, halfwayDir), 0.0), g_shininess);
  vec3 specular = light.color * light.specularIntensity * specFactor * g_specularColor.rgb;
            
  /* soft edges + intensity */
  const float cutoff = cos(radians(light.cutOff));
  const float outerCutoff = cos(radians(light.outerCutOff));
  const float theta = dot(lightDir, normalize(-light.direction));
  const float epsilon = (cutoff - outerCutoff);
  const float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
  diffuse  *= intensity;
  specular *= intensity;

  /* attenuation */
  const float d = length(tangentLightPos - TangentFragPos);
  const float attenuation = CalculateAttenuation(d, light.attenuation.kl, light.attenuation.kq);

  diffuse  *= attenuation;
  specular *= attenuation;
  return diffuse + specular;
}
