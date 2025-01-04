#include "Vertex.hpp"

Vertex_P_N_UV_T_B::Vertex_P_N_UV_T_B()
{
  std::fill_n(boneIds, MAX_BONES_INFLUENCE, -1);
  std::fill_n(boneWeights, MAX_BONES_INFLUENCE, 0.f);
}

void Vertex_P_N_UV_T_B::AddBone(i32 id, f32 weight)
{
  i32 i = std::count_if(std::begin(boneIds), std::end(boneIds), [](u32 id) { return id != -1; });
  if (i < MAX_BONES_INFLUENCE)
  {
    boneIds[i] = id;
    boneWeights[i] = weight;
  }
}