#include "InstancedMesh.hh"

#include <spdlog/spdlog.h>

/* --------------------------------------
            PUBLIC METHODS
   -------------------------------------- */

InstancedMesh::InstancedMesh() : MAX_NUM_INSTANCES{10}
{
  _nInstancesToDraw = 1;
  _models.resize(MAX_NUM_INSTANCES, mat4f(1.0f));
  for (int i = 0; i < MAX_NUM_INSTANCES; i++)
    _models[i] = glm::translate(mat4f(1.0f), vec3f(i * 3.f, 0.0f, 0.0f));
}

void InstancedMesh::Init(uint32_t vertSize, 
  float*    vertices, 
  uint32_t  indSize, 
  uint32_t* indices, 
  Graphics::VAConfiguration& config
)
{
	vertexArray.Init(vertSize, vertices, indSize, indices, config);

	// instancing
	// -------------------------
  Instancing(config);
}

void InstancedMesh::Init(uint32_t vertSize, float* vertices, Graphics::VAConfiguration& config)
{
	vertexArray.Init(vertSize, vertices, config);

	// instancing
	// -------------------------
  Instancing(config);
}

void InstancedMesh::Destroy()
{
	vertexArray.Destroy();
  glDeleteBuffers(1, &_instanceBuffer);
}

void InstancedMesh::SetInstances(uint32_t n)
{
  if (n < 1 || n > MAX_NUM_INSTANCES)
  {
    spdlog::warn("InstancedMesh::SetInstances invalid value");
    return;
  }

  _nInstancesToDraw = n;
}

void InstancedMesh::SetModel(mat4f* model, uint32_t instance)
{
  if (instance < 0 || instance >= _models.size())
  {
    spdlog::warn("InstancedMesh::SetModel invalid instance value!");
    return;
  }
  glBindVertexArray(vertexArray.vao);
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, instance * sizeof(mat4f), sizeof(mat4f), model);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedMesh::SetModelsRange(uint32_t start, uint32_t end, mat4f* models)
{
  if (start < 0 || end >= _models.size())
  {
    spdlog::warn("InstancedMesh::SetModel invalid instance value!");
    return;
  }

  uint32_t diff = end - start;
  glBindVertexArray(vertexArray.vao);
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, start * sizeof(mat4f), diff * sizeof(mat4f), models);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* --------------------------------------
            PRIVATE METHODS
   -------------------------------------- */

void InstancedMesh::Instancing(Graphics::VAConfiguration& config)
{
  glBindVertexArray(vertexArray.vao);

  // instance buffer for model matrices
  glGenBuffers(1, &_instanceBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferData(GL_ARRAY_BUFFER, _models.size() * sizeof(mat4f), _models.data(), GL_DYNAMIC_DRAW);

  /* layout (location = 2) in mat4 aModel;
    mat4 = vec4 x 4 -> (layout 2) in vec4
                    -> (layout 3) in vec4
                    -> (layout 4) in vec4
                    -> (layout 5) in vec4 */

  uint32_t nLayoutModel = config.numAttrs;
  for (int i = 0; i < 4; i++, nLayoutModel++)
  {
    glVertexAttribPointer(nLayoutModel, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), reinterpret_cast<void*>(i * sizeof(vec4f)));
    glEnableVertexAttribArray(nLayoutModel);
    glVertexAttribDivisor(nLayoutModel, 1);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}