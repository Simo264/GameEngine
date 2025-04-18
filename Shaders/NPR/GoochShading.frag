#version 460

/*
  Gooch Shading - A non-photorealistic rendering (NPR) technique designed to enhance surface readability.
 
  Traditional Shading of Matte Objects:
    I = k_a*k_d + k_d*max(0, L dot N)
  where: 
    - I is the RGB color to be displayed for a given point on the surface
    - k_d is the RGB diffuse reflectance at the point 
    - k_a is the RGB ambient illumination 
    - L is the unit vector in the direction of the light source
    - N is the unit surface normal vector at the point


  Tone-based Shading of Matte Object:
  we can generalize the classic computer graphics shading model to experiment with tones by using the 
  cosine term (L dot N) of Equation 1 to blend between two RGB colors, k_cool and k_warm:
    I = ((1 + L dot N)/2)*k_cool + (1 - (1 + L dot N)/2)*k_warm
  
  Blue and yellow tones are chosen to insure a cool to warm color transition regardless of the diffuse color of the
  object. The blue-to-yellow tones range from a fully saturated blue:
    - k_blue = (0, 0, b)
    - k_yellow = (y, y, 0)
  We can simulate undertones by a linear blend between the blue/yellow and black/object-color tones:
    - k_cool = k_blue + alpha*k_d
    - k_warn = k_yellow + beta*k_d
  
  Plugging these values into Equation 2 leaves us with four free parameters: alpha, beta, b, y.

  The values for b and y will determine the strength of the overall temperature shift, and the values of 
  alpha and beta will determine the prominence of the object color and the strength of the luminance shift.

  For example: 
    - b = 0.4f
    - y = 0.4f
    - alpha = 0.2f
    - beta = 0.f

*/


// ========== IN attributes ==========
// ===================================
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 CameraPos;

// ========== OUT attributes ==========
// ====================================
out vec4 FragColor;

// ========== Structs ==========
// =============================
struct Material 
{
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  sampler2D normalTexture;
};
struct Lighting
{
  vec3 diffuse;
  vec3 specular;
};
struct Attenuation
{
	int range;        // If an objects distance is greater than the range, the light has no effect on the object
	float kl;	        // Linear attenuation factor
	float kq;         // Quadratic attenuation factor
  float __padding;  // Needed for std140 alignment
};

struct DirectionalLight 
{
  vec3  color;
  float intensity;
  vec3  direction;
  float __padding;  // Needed for std140 alignment
};
struct PointLight 
{
  vec3  color;
  float intensity;
  vec3  position;
  float __padding;  // Needed for std140 alignment
  Attenuation attenuation;
};


// ========== Uniforms ==========
// ==============================
layout (std140, binding = 1) uniform LightBlock
{
  DirectionalLight  u_directLight;
  PointLight        u_pointLight;
};
uniform Material u_material;

uniform float u_b, u_y, u_alpha, u_beta;

const float g_shininess = 16.f;
const float g_gammaCorrection = 2.2f;

Lighting GoochShading(vec3 L, vec3 N, vec3 V, vec3 kCool, vec3 kWarm, vec3 ks);
void CalculateAttenuation(float distance, float kl, float kq, inout Lighting lighting);

void main()
{
  vec3 N = normalize(Normal);
  vec3 V = normalize(CameraPos - FragPos);

  vec4 kd = texture(u_material.diffuseTexture, TexCoord);
  vec4 ka = kd * 0.1f;
  vec4 ks = texture(u_material.specularTexture, TexCoord);
  
  // k_cool = k_blue + alpha*k_d
  vec3 kBlue = vec3(0.f, 0.f, u_b);
  vec3 kCool = kBlue + u_alpha * kd.rgb;
  // k_warm = k_yellow + beta*k_d
  vec3 kYellow = vec3(u_y, u_y, 0.f);
  vec3 kWarm = kYellow + u_beta*kd.rgb;
  
  vec3 color = kCool;
  
  vec3 L;

  // calculate with directional light
  if(u_directLight.intensity > 0.f)
  {
    L = normalize(-u_directLight.direction);
    Lighting I = GoochShading(L, N, V, kCool, kWarm, ks.rgb);
    I.diffuse *= u_directLight.intensity;
    I.specular *= u_directLight.intensity;
    
    color += I.diffuse + I.specular;
  }

  // calculate with point light
  if(u_pointLight.intensity > 0.f)
  {
    L = normalize(u_pointLight.position - FragPos);
    Lighting I = GoochShading(L, N, V, kCool, kWarm, ks.rgb);
    I.diffuse *= u_pointLight.intensity;
    I.specular *= u_pointLight.intensity;
  
    float distance = length(u_pointLight.position - FragPos);
    float kl = u_pointLight.attenuation.kl;
    float kq = u_pointLight.attenuation.kq;
    CalculateAttenuation(distance, kl, kq, I);
    
    color += I.diffuse + I.specular;
  }

  color = pow(color, vec3(1.0f / g_gammaCorrection));
  FragColor = vec4(color, 1.f);
}

Lighting GoochShading(vec3 L, vec3 N, vec3 V, vec3 kCool, vec3 kWarm, vec3 ks)
{
  Lighting I;
  I.specular = vec3(0.f);
  
  // I = ((1 + L dot N)/2)*k_cool + (1 - (1 + L dot N)/2)*k_warm
  float LdotN = dot(L, N);
  float t = (1 + LdotN) * 0.5f;
  I.diffuse = mix(kCool, kWarm, t);
  
  float lambertian = max(dot(L, N), 0.0f);
  if (lambertian > 0.0f)
  {
    // calculate specular
    vec3 H = normalize(L + V);
    float specAngle = max(dot(H, N), 0.0f);
    float specular = pow(specAngle, g_shininess);
    vec3 ks = vec3(1.f);
    I.specular = ks * specular;
  }

  return I;
}

void CalculateAttenuation(float distance, float kl, float kq, inout Lighting lighting)
{
  // https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ 
  // attenuation = 1.0f / (1.0f + kl*d + kq*pow(d,2));  

  float attenuation = 1.0f / (1.0f + kl*distance + kq*pow(distance,2));
  lighting.diffuse  *= attenuation;
  lighting.specular *= attenuation;
}
