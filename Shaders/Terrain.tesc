#version 460 core

layout (vertices=4) out;

/* varying input from vertex shader */ 
in vec2 TexCoord[];
/* varying output to evaluation shader */ 
out vec2 TextureCoord[];

uniform mat4 u_model;
uniform mat4 u_view;

void main()
{
  /* pass attributes through */ 
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

	/* invocation zero controls tessellation levels for the entire patch */ 
  if (gl_InvocationID == 0)
  {
    /* define constants to control tessellation parameters */
    const int MIN_TESS_LEVEL = 4;
    const int MAX_TESS_LEVEL = 64;
    const float MIN_DISTANCE = 20;
    const float MAX_DISTANCE = 800;
    
    /* transform each vertex into eye space */
    const mat4 mv = u_view * u_model;
    vec4 eyeSpacePos00 = mv * gl_in[0].gl_Position;
    vec4 eyeSpacePos01 = mv * gl_in[1].gl_Position;
    vec4 eyeSpacePos10 = mv * gl_in[2].gl_Position;
    vec4 eyeSpacePos11 = mv * gl_in[3].gl_Position;

    /* "distance" from camera scaled between 0 and 1 */
    float distance00 = clamp((abs(eyeSpacePos00.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0f, 1.0f);
    float distance01 = clamp((abs(eyeSpacePos01.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0f, 1.0f);
    float distance10 = clamp((abs(eyeSpacePos10.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0f, 1.0f);
    float distance11 = clamp((abs(eyeSpacePos11.z)-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0f, 1.0f);

    /* interpolate edge tessellation level based on closer vertex */
    float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
    float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
    float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
    float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

    /* set the corresponding outer edge tessellation levels */
    gl_TessLevelOuter[0] = tessLevel0;
    gl_TessLevelOuter[1] = tessLevel1;
    gl_TessLevelOuter[2] = tessLevel2;
    gl_TessLevelOuter[3] = tessLevel3;

    /* set the inner tessellation levels to the max of the two parallel edges */
    gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
    gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
  }
}