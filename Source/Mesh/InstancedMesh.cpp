#include "InstancedMesh.hh"
#include "../Logger.hh"

/* --------------------------------------
            PUBLIC METHODS
   -------------------------------------- */

InstancedMesh::InstancedMesh(uint32_t nMaxInstances) : MAX_NUM_INSTANCES{ nMaxInstances }
{
  _nInstancesToDraw = 1;
}

void InstancedMesh::Init(MeshData& data, VAConfiguration& config)
{
	vertexArray.Create(data.verticesDataSize, data.verticesData, data.indicesDataSize, data.indicesData, config);

	// instancing
	// -------------------------
  Instancing(config);
}

void InstancedMesh::Destroy()
{
	vertexArray.Destroy();
  glDeleteBuffers(1, &_instanceBuffer);
}

void InstancedMesh::SetInstanceNumber(uint32_t n)
{
  if (n < 0 || n > MAX_NUM_INSTANCES)
  {
    CONSOLE_WARN("InstancedMesh::SetInstances invalid value");
    return;
  }

  _nInstancesToDraw = n;
}

void InstancedMesh::SetInstanceModel(mat4f* model, uint32_t instance)
{
  if (instance < 0 || instance >= MAX_NUM_INSTANCES)
  {
    CONSOLE_WARN("InstancedMesh::SetModel invalid instance value!");
    return;
  }
  vertexArray.Bind();
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, instance * sizeof(mat4f), sizeof(mat4f), model);
  vertexArray.Unbind();
}

void InstancedMesh::SetInstancesModelRange(uint32_t start, uint32_t n, mat4f* models)
{
  if (start < 0 || n > MAX_NUM_INSTANCES)
  {
    CONSOLE_WARN("InstancedMesh::SetModel invalid instance value!");
    return;
  }
  
  vertexArray.Bind();
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, start * sizeof(mat4f), n * sizeof(mat4f), models);
  vertexArray.Unbind();
}

/* --------------------------------------
            PRIVATE METHODS
   -------------------------------------- */

void InstancedMesh::Instancing(VAConfiguration& config)
{
  vertexArray.Bind();

  vector<mat4f> models(MAX_NUM_INSTANCES, mat4f(1.0f));

  // instance buffer for model matrices
  glGenBuffers(1, &_instanceBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(mat4f), models.data(), GL_DYNAMIC_DRAW);

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

  vertexArray.Unbind();
}