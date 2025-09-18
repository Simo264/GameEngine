#version 460

// ========== IN attributes ==========
// ===================================
in vec3 vs_out_Normal;
in vec3 vs_out_FragPos;
in vec3 vs_out_CameraPos;

// ============================================================================
// STRUCTURES
// ============================================================================
struct DirectionalLight 
{
  vec4 color; // vec4 needed for std140 alignment
  vec3 direction;
  float intensity;
};

struct PointLight 
{
  vec4 color;  // vec4 needed for std140 alignment
  vec3 position;
  float intensity;

  float kl;           // Linear attenuation factor
  float kq;           // Quadratic attenuation factor
  float __padding[2]; // Needed for std140 alignment
};

// ============================================================================
// UNIFORMS
// ============================================================================
uniform DirectionalLight u_DirLight;
uniform PointLight u_PointLight;

// ============================================================================
// GOOCH SHADING MODEL REFERENCE
// ============================================================================
/*
 * GOOCH MODEL FORMULA:
 * color = s*c_highlight + (1 - s)*(t*c_warm + (1 - t)*c_cool);
 * 
 * Where:
 * t = (dot(n,l) + 1.0) * 0.5;           // Temperature factor
 * r = reflect(-l, n);                   // Reflection vector
 * s = smoothstep(0.7, 1.0, dot(r, v));  // Specular factor
 * 
 * Parameters:
 * n = surface normal
 * l = light direction (toward light)
 * v = view direction (toward camera)
 * c_cool = cold/shadow color
 * c_warm = warm/lit color  
 * c_highlight = specular highlight color
 *
 * 
 * Light sources affect surfaces based on the angle between light direction (l) 
 * and surface normal (n). The intensity follows Lambert's cosine law:
 * intensity = cos(a) = dot(l,n)
 * 
 * We clamp to [0,1] because negative values mean light from underneath.
 * 
 * GENERAL FORMULA:
 * color = f_unlit(n,v) + clamp(dot(l,n), 0.0, 1.0) * k_i * c_light * f_lit(l,n,v)
 * 
 * Where:
 * - f_unlit = surface color when unlit (c_cool)
 * - f_lit = surface color when lit (c_warm) 
 * - k_i = light intensity
 * - c_light = light color
 */

const vec3 g_ColorCool = vec3(0.376, 0.643, 0.956);  // #60a4f4 — Soft Blue
const vec3 g_ColorWarm = vec3(0.956, 0.643, 0.376);  // #f4a460 — Sandy Brown
const vec3 g_ColorHighlight = vec3(2.2, 2.2, 2.2);

vec3 specularHighlight(float RdotV, float sharpness);
float calculateAttenuation(float r, float kl, float kq);
float calculateWindowing(float r, float rMax);

// ============================================================================
// MAIN FUNCTION
// ============================================================================
void main()
{
	// Calculate correct normal for double-sided surfaces
  // Uses gl_FrontFacing to handle both sides of faces when culling is disabled
  vec3 n = normalize(vs_out_Normal) * sign(float(gl_FrontFacing) - 0.5);
  vec3 v = normalize(vs_out_CameraPos - vs_out_FragPos);

  vec3 f_unlit = g_ColorCool * 0.25; 
  vec4 outColor = vec4(f_unlit, 1.0);

  if(u_DirLight.intensity > 0.0)
  {
    vec3 l = normalize(-u_DirLight.direction);
    float NdotL = max(dot(n, l), 0.0);
    float raylightsIntensity = NdotL * u_DirLight.intensity;
    vec3 litColor = u_DirLight.color.rgb * g_ColorWarm;
    outColor.rgb += raylightsIntensity * litColor;

    // Reflection vector for specular calculation
    vec3 r = reflect(-l, n);
    float RdotV = max(dot(r, v), 0.0);
    if(RdotV > 0.0)
    {
      vec3 highlightColor = specularHighlight(RdotV, 32.0) * u_DirLight.intensity;
      outColor.rgb += u_DirLight.color.rgb * highlightColor;
    }
  }
  
  if(u_PointLight.intensity > 0.0)
  {
    vec3 d = u_PointLight.position - vs_out_FragPos;
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
      vec3 litColor = u_PointLight.color.rgb * g_ColorWarm;
      outColor.rgb += raylightsIntensity * litColor;

      // Reflection vector for specular calculation
      vec3 r = reflect(-l, n);
      float RdotV = max(dot(r, v), 0.0);
      if(RdotV > 0.0)
      {
        vec3 highlightColor = specularHighlight(RdotV, 32.0) * u_PointLight.intensity;
        outColor.rgb += u_PointLight.color.rgb * highlightColor * attenuation * windowing;
      }
    }
  }

	gl_FragColor = outColor;
}


vec3 specularHighlight(float RdotV, float sharpness)
{
  float s = pow(RdotV, sharpness); // Phong-style specular
  vec3 highlightColor = mix(g_ColorWarm, g_ColorHighlight, s);
  return s * highlightColor;
}

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


