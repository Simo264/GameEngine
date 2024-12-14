#include "Vertex.hpp"

void Vertex_P_N_UV_T_B::AddBone(u32 id, f32 weight)
{
  i32 i = std::count_if(std::begin(boneIds), std::end(boneIds), [](u32 id) { return id != -1; });
  if (i < MAX_BONES_INFLUENCE)
  {
    boneIds[i] = id;
    boneWeights[i] = weight;
  }
}