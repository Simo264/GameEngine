#version 460

//  The Phong reflection model describes how a surface reflects light as a combination of diffuse reflection and specular reflection. 
//  It is based on Phong's observation that glossy surfaces have small, intense highlights, whereas matte surfaces have broader, more gradually fading highlights.
//
//  The model also includes an ambient term to account for the small amount of scattered light throughout the scene.
//
//  Components of the Phong reflection model:
//  For each light source in the scene, the following intensities are defined:
//  - i_s: Specular component intensity of the light source
//  - i_d: Diffuse component intensity of the light source
//  - i_a: Ambient component intensity, sometimes calculated as the sum of contributions from all light sources
//
//  For each material in the scene, the following parameters are defined:
//  - k_s: Specular reflection coefficient, indicating the amount of specular light reflected
//  - k_d: Diffuse reflection coefficient, indicating the amount of diffuse light reflected (Lambertian reflection)
//  - k_a: Ambient reflection coefficient, indicating the amount of ambient light reflected
//  - alpha: Shininess factor, higher for smoother and more reflective surfaces (higher values produce smaller, sharper highlights)
//
//  Additional variables used in the model:
//  - L: Light direction (normalized vector)
//  - N: Surface normal at the point being considered (normalized vector)
//  - R: Ideal reflected light direction (normalized vector)
//  - V: Observer or camera direction (normalized vector)
//
//  Phong model equation:
//  The total illumination at a point on the surface is given by the sum of three components:
//
//      I_p = k_a * i_a + sum(m in lights) ( k_d * (L_m * N) * i_m,d + k_s * (R_m * V)^alpha * i_m,s )
//
//  Where:
//  - (L_m * N) represents the intensity of diffuse reflection
//  - (R_m * V)^alpha represents specular reflection, which is more intense when the observer aligns with the reflection
//
//  The reflection vector R_m is calculated as:
//
//      R_m = 2 * (L_m * N) * N - L_m
//
//  If the dot product between two vectors is negative, the corresponding term in the lighting formula is ignored. Moreover, the specular term is considered only if the diffuse term is positive.
//
//  When colors are represented as RGB values, the equation is applied separately to the three channels, allowing different coefficients for k_a, k_d, and k_s in each channel.


// ========== IN attributes ==========
// ===================================
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos; 

// ========== OUT attributes ==========
// ====================================
out vec4 FragColor;

// ========== Uniforms ==========
// ==============================
struct Material 
{
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
};
struct DirectionalLight 
{
  vec3  color;
  float intensity;
  vec3  direction;
  float __padding;  // Needed for std140 alignment
};
struct Attenuation
{
	int range;  // If an objects distance is greater than the range, the light has no effect on the object
	float kl;	  // Linear attenuation factor
	float kq;   // Quadratic attenuation factor
};
struct PointLight 
{
  vec3  color;
  float intensity;
  vec3  position;
  float __padding_1;  // Needed for std140 alignment
  Attenuation attenuation;
  float __padding_2;  // Needed for std140 alignment
};
struct SpotLight 
{
  vec3  color;
  float intensity;
  vec3  position;
  float __padding_1;      // Needed for std140 alignment
  vec3  direction;
  float __padding_2;      // Needed for std140 alignment
  float cutOff;
  float outerCutOff;      // smoother edges
  float __padding_3[2];   // Needed for std140 alignment
  Attenuation attenuation;
};

layout (std140, binding = 1) uniform LightBlock
{
  DirectionalLight  u_directionalLight;
  PointLight        u_pointLight;
  SpotLight         u_spotLight;
};

uniform Material u_material;

void main()
{
  const float shininess = 16.0f;
  const vec3 ambientColor = vec3(0.1f);
  const vec4 diffuseColor = texture(u_material.diffuseTexture, TexCoord);
  const vec3 specularColor = vec3(1.0f);

  if(u_directionalLight.intensity == 0.0f)
  {
    FragColor = vec4(ambientColor, 1.0f);	
    return;
  }

  const vec3 normal = Normal;
  const vec3 lightDir = normalize(u_directionalLight.direction);
  const float lambertian = max(dot(lightDir, normal), 0.0f);
  float specular = 0.0f;
  if (lambertian > 0.0f)
  {
    vec3 viewDir = normalize(ViewPos - FragPos);

    // this is blinn phong
    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0f);
    specular = pow(specAngle, shininess);
  
    // this is phong (for comparison)
    // if(false)
    // {
    //  vec3 reflectDir = reflect(-lightDir, normal);
    //  specAngle = max(dot(reflectDir, viewDir), 0.0f);
    //  // note that the exponent is different here
    //  specular = pow(specAngle, shininess/4.0f);
    // }
  }
  
  vec3 color =  ambientColor + 
                diffuseColor.xyz * lambertian * u_directionalLight.color * u_directionalLight.intensity + 
                specularColor * specular * u_directionalLight.color * u_directionalLight.intensity;

	FragColor = vec4(color, 1.0f);	
}