#include "InstancedMesh.hh"

void InstancedMesh::Init(uint32_t vertSize, float* vertices, uint32_t indSize, uint32_t* indices, Graphics::VAConfiguration& config)
{
	vertexArray.Init(vertSize, vertices, indSize, indices, config);

	// instancing
	// -------------------------
  mat4f models[MAX_NUM_INSTANCES];
  for (int i = 0; i < MAX_NUM_INSTANCES; i++)
    models[i] = glm::translate(mat4f(1.0f), vec3f(i * 3.f, 0.0f, (i + 2) * 3.0f));

  glBindVertexArray(vertexArray.vao);

  // instance buffer for model matrices
  glGenBuffers(1, &instanceBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(mat4f) * MAX_NUM_INSTANCES, models, GL_DYNAMIC_DRAW);
  //glBufferData(GL_ARRAY_BUFFER, sizeof(mat4f) * MAX_NUM_INSTANCES, nullptr, GL_DYNAMIC_DRAW);


  /* 
    example:
    layout (location = 2) in mat4 aModel;
    mat4 = vec4 x 4 = (layout 2) in vec4
                      (layout 3) in vec4
                      (layout 4) in vec4
                      (layout 5) in vec4
  */
  int nLayoutModel = config.numAttrs;
  for (int i = 0; i < 4; i++)
  {
    glVertexAttribPointer(nLayoutModel, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), reinterpret_cast<void*>(i * sizeof(vec4f)));
    glEnableVertexAttribArray(nLayoutModel);
    glVertexAttribDivisor(nLayoutModel, 1);
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedMesh::Init(uint32_t vertSize, float* vertices, Graphics::VAConfiguration& config)
{
	vertexArray.Init(vertSize, vertices, config);

	// instancing
	// -------------------------
}

void InstancedMesh::Destroy()
{
	vertexArray.Destroy();
  glDeleteBuffers(1, &instanceBuffer);
}