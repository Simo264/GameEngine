#version 460

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_depthMapTexture;
uniform float u_nearPlane;
uniform float u_farPlane;

// required when using a perspective projection matrix
//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // Back to NDC 
//    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
//}

void main()
{             
    float depthValue = texture(u_depthMapTexture, TexCoords).r;
    
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}