#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in ivec4 aBoneIds; 
layout (location = 5) in vec4 aWeights;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 ViewPos;
out mat3 TBN;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

layout (std140, binding = 0) uniform cameraBlock
{
  mat4 u_view;
  mat4 u_projection;
};
uniform mat4 u_model;
uniform vec3 u_viewPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_finalBonesMatrices[MAX_BONES];

void main()
{
  vec4 totalPosition = vec4(0.0f);
  for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
  {
    if(aBoneIds[i] == -1) 
      continue;
    
    if(aBoneIds[i] >= MAX_BONES) 
    {
      totalPosition = vec4(aPos, 1.0f);
      break;
    }
    
    vec4 localPosition = u_finalBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
    totalPosition += localPosition * aWeights[i];
    //vec3 localNormal = mat3(u_finalBonesMatrices[aBoneIds[i]]) * aNormal;
  }


  mat3 normalMatrix = mat3(transpose(inverse(u_model)));
  vec3 N = normalize(normalMatrix * aNormal);
  vec3 T = normalize(mat3(u_model) * aTangent);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);
  TBN = transpose(mat3(T,B,N));

  FragPos = vec3(u_model * vec4(aPos, 1.0));
  TexCoord = aUv;
  ViewPos = u_viewPos;
  Normal = N;

  TangentViewPos = TBN * ViewPos;
  TangentFragPos = TBN * FragPos;
  gl_Position = u_projection * u_view * u_model * totalPosition;
}