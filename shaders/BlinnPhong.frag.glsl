#version 460

// ========== IN attributes ==========
// ===================================
in vec2 vs_out_TextureCoord;
in vec3 vs_out_Normal;
in vec3 vs_out_FragPos;
in vec3 vs_out_CameraPos;
in mat3 vs_out_TBN;


// ========== Uniforms ==========
// ==============================
struct DirectionalLight 
{
  vec4 color;
  vec4 direction;
  float intensity;
  
  float __padding_1;
  float __padding_2;
  float __padding_3;
};
struct PointLight 
{
  vec4 color;
  vec4 position;
  float intensity;
  float kl;           // Linear attenuation factor
  float kq;           // Quadratic attenuation factor
  
  float __padding; // Needed for std140 alignment
};
struct SpotLight
{
	vec4 direction; // Vec4F needed for std140 alignment 
	vec4 position;	// Vec4F needed for std140 alignment
	vec4 color;
	float intensity;
	// attenuation
	float kl;			// Linear attenuation factor
	float kq;			// Quadratic attenuation factor
	// Shadow angle
	float thetaU;	// Penumbral angle. This is the angle of the inner cone (in degrees).
	float thetaP;	// Shadow angle. This is the angle of the outer cone (in degrees).
  
  float __padding_1;
  float __padding_2;
  float __padding_3;
};

// ============================================================================
// UNIFORMS
// ============================================================================
layout (std140, binding = 1) uniform LightingBlock
{
	DirectionalLight u_DirLight;
	PointLight u_PointLight;
	SpotLight u_SpotLight;
};
layout (binding = 10) uniform sampler2D u_TextureAlbedo;
layout (binding = 11) uniform sampler2D u_TextureNormal;


// ============================================================================
// BLINN-PHONG SHADING MODEL REFERENCE
// ============================================================================
/*
 * BLINN-PHONG MODEL COMPONENTS:
 * color = ambient + diffuse + specular
 *
 * 1. AMBIENT COMPONENT:
 * Simulates indirect lighting from the environment.
 * ambient = k_a * c_light
 *
 * 2. DIFFUSE COMPONENT:
 * Simulates light scattered equally in all directions (Lambertian reflection).
 * This is the main component for the object's perceived color.
 * diffuse = k_d * c_light * max(0.0, dot(n, l))
 *
 * 3. SPECULAR COMPONENT:
 * Simulates the shiny highlight or "glare" on a surface.
 * specular = k_s * c_light * pow(max(0.0, dot(n, h)), alpha)
 *
 * Where:
 * n = surface normal
 * l = light direction (toward light)
 * v = view direction (toward camera)
 * h = halfway vector between light and view vectors (h = normalize(l + v))
 *
 * Parameters:
 * k_a     = ambient strength/material ambient color
 * k_d     = diffuse strength/material diffuse color
 * k_s     = specular strength/material specular color
 * c_light = light color
 * alpha   = shininess exponent (higher value = smaller, sharper highlight)
 *
 * GENERAL FORMULA:
 * final_color = k_a*c_light + k_d*c_light*max(0.0, dot(n,l)) + k_s*c_light*pow(max(0.0, dot(n,h)), alpha)
 *
 * If textures are used, the diffuse component (k_d) is often a texture color sampled from a texture map.
 * In this case, the formula would be:
 * final_color = k_a*c_light + texture_color*c_light*max(0.0, dot(n,l)) + ...
 */

float calculateAttenuation(float r, float kl, float kq);
float calculateWindowing(float r, float rMax);


void main()
{
  vec3 surfaceColor = texture(u_TextureAlbedo, vs_out_TextureCoord).rgb;

	// Calculate correct normal for double-sided surfaces
  // Uses gl_FrontFacing to handle both sides of faces when culling is disabled
  vec3 n = normalize(vs_out_Normal) * sign(float(gl_FrontFacing) - 0.5);
  if(textureSize(u_TextureNormal, 0) != ivec2(1))
  {
    n = texture(u_TextureNormal, vs_out_TextureCoord).rgb;  // in range [0,1]
    n = n * 2.0f - 1.0f;                                    // transform to range [-1,1]
    n = normalize(vs_out_TBN * n);                          // normal is now in world space
  }
  vec3 v = normalize(vs_out_CameraPos - vs_out_FragPos);

  vec3 ambient = vec3(surfaceColor * 0.1);
  vec4 outColor = vec4(ambient, 1.0);
  
  if(u_DirLight.intensity > 0.0)
  {
    // Compute diffuse color
    vec3 l = normalize(-u_DirLight.direction.xyz);
    float NdotL = max(dot(n, l), 0.0);
    float raylightsIntensity = NdotL * u_DirLight.intensity;
    vec3 litColor = u_DirLight.color.rgb * surfaceColor;
    outColor.rgb += raylightsIntensity * litColor;
  }

  if(u_PointLight.intensity > 0.0)
  {
    // Calculate vector from fragment to light
    vec3 d = u_PointLight.position.xyz - vs_out_FragPos;
    float r = length(d);  // Distance to light
    vec3 l = normalize(d);

    const float rMax = 10.0;  // Maximum range of the light
    // Early exit for performance if fragment is out of range
    if (r < rMax)
    {
      float attenuation = calculateAttenuation(r, u_PointLight.kl, u_PointLight.kq);
      // Windowing function to smoothly fade the light to zero at rMax
      float windowing = calculateWindowing(r, rMax);

      float NdotL = max(dot(n, l), 0.0);
      float raylightsIntensity = NdotL * u_PointLight.intensity * attenuation * windowing;
      vec3 litColor = u_PointLight.color.rgb * surfaceColor;
      outColor.rgb += raylightsIntensity * litColor;
    }
  }

  if(u_SpotLight.intensity > 0.0)
  {
    float theta_u_rads = radians(u_SpotLight.thetaU);
    float theta_p_rads = radians(u_SpotLight.thetaP);
    float cos_theta_u = cos(theta_u_rads);
    float cosa_theta_p = cos(theta_p_rads);

    // Calculate vector from fragment to light
    vec3 d = u_SpotLight.position.xyz - vs_out_FragPos;
    float r = length(d);  // Distance to light
    vec3 l = normalize(d);
    float cos_theta_s = dot(-l, normalize(u_SpotLight.direction.xyz));
    
    const float rMax = 10.0;  // Maximum range of the light
    if (cos_theta_s > cos_theta_u && r < rMax)
    {
      const float kl = u_SpotLight.kl;
      const float kq = u_SpotLight.kq;
      float attenuation = calculateAttenuation(r, kl, kq);
      // Windowing function to smoothly fade the light to zero at rMax
      float windowing = calculateWindowing(r, rMax);

      // Calcola il fattore di decadimento direzionale
      float t = (cos_theta_s - cos_theta_u) / (cosa_theta_p - cos_theta_u);
      // Applica la funzione smoothstep per una transizione morbida
      float fDir = smoothstep(0.0, 1.0, t); 

      // Applica il modello di illuminazione (ad esempio, diffusa)
      float NdotL = max(dot(n, l), 0.0);
      float spotlightIntensity = NdotL * u_SpotLight.intensity * attenuation * windowing * fDir;
            
      vec3 litColor = u_SpotLight.color.rgb * surfaceColor;
      outColor.rgb += spotlightIntensity * litColor;
    }
  }

  outColor = pow(outColor, vec4(1.0f / 2.2));
  gl_FragColor = outColor;
}


/*
vec3 BlinnPhongSpecularReflection(vec3 L, vec3 N, vec3 V, vec3 C, float I, vec3 materialSpecular)
{
  // The Blinn-Phong Specular Reflection Model:
  //  - Computes the specular reflection using a halfway vector H instead of a reflection vector R.
  //  - The specular intensity is calculated as:
  //      spec = max(dot(N, H), 0)^a
  //  - The final reflected color is scaled by the light color C, light intensity I,
  //    and the material's specular color (e.g. from a texture).
  //
  // Where:
  //  - H = normalize(L + V) is the halfway vector between L and V
  //  - N is the surface normal (normalized)
  //  - L is the light direction vector (normalized)
  //  - V is the view (camera) direction vector (normalized)
  //  - a (g_shininess) controls the sharpness of the highlight
  //  - C is the light color
  //  - I is the light intensity
  vec3 H = normalize(L + V);
  float specAngle = max(dot(H, N), 0.0f);
  float spec = pow(specAngle, g_Shininess);
  vec3 reflection = spec * C * I * materialSpecular;
  return reflection;
}
*/

float calculateAttenuation(float r, float kl, float kq)
{
  // https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
  // This is an approximation of the physical inverse-square attenuation model.
  // It simulates how light disperses in space:
  // attenuation = 1.0 / (r*r + epsilon);
  //
  // However, it can cause an overly rapid attenuation, making light sources less visible at a distance.
  // Therefore, the following notation is often preferred:
  // attenuation = 1.0 / (1.0 + kL*r + kQ*r*r)
  //
  // This model is not physically accurate (real light follows the inverse-square law, i.e., 1/r²),
  // but it is widely used because it:
  // - avoids infinite values at zero distance (thanks to the constant term)
  // - allows for "artistic" control over the light's radius of influence
  // - is numerically stable
  float attenuation = 1.0 / (1.0 + kl*r + kq*r*r);
  return attenuation;
}

float calculateWindowing(float r, float rMax)
{
  float t = clamp(r / rMax, 0.0, 1.0);
  
  //float windowing = 1.0 - pow(t, 4.0);   // 1 - (r/r_max)^2
  //windowing *= windowing;                // ()^2
  
  float windowing = 1.0 - t * t;
  return windowing;
}


