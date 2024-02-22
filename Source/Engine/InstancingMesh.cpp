#if 0
#include "InstancingMesh.hpp"

#include "Engine/Mesh/Mesh.hpp"
#include "Engine/Texture2D.hpp"
#include "Engine/Renderer.hpp"
#include "Engine/Logger.hpp"

/* --------------------------------------
            PUBLIC METHODS
   -------------------------------------- */

InstancingMesh::InstancingMesh(Mesh* mesh, uint32_t nMaxInstances)
{
  _maxNumInstances = nMaxInstances;
  _nInstancesToDraw = 0;
  _instanceBuffer = 0;
  diffuse = mesh->diffuse;
  specular = mesh->specular;

  InitializeInstance(mesh);
}

void InstancingMesh::AddInstance(const Mat4f& instanceModel)
{
  if (_nInstancesToDraw >= _maxNumInstances)
  {
    CONSOLE_WARN("InstancingMesh::AddInstance can't add more instances");
    return;
  }
  SetInstanceModel(_nInstancesToDraw, instanceModel);
  _nInstancesToDraw++;
}

void InstancingMesh::SetInstanceModel(uint32_t instance, const Mat4f& model)
{
  _vertexArray.BindVertexArray();
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, instance * sizeof(Mat4f), sizeof(Mat4f), &model);
  _vertexArray.UnbindVertexArray();
}

void InstancingMesh::DrawInstMesh()
{
  if (_nInstancesToDraw <= 0)
    return;

  if (diffuse)
  {
    glActiveTexture(GL_TEXTURE0);
    diffuse->BindTexture();
  }
  if (specular)
  {
    glActiveTexture(GL_TEXTURE1);
    specular->BindTexture();
  }
  if(_vertexArray.numIndices == 0)
    Renderer::DrawArraysInstanced(&_vertexArray, _nInstancesToDraw);
  else
    Renderer::DrawIndexedInstanced(&_vertexArray, _nInstancesToDraw);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0); /* unbind specular */

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0); /* unbind diffuse */
}

void InstancingMesh::DestroyInstMesh() const
{
  _vertexArray.DestroyVertexArray();
  glDeleteBuffers(1, &_instanceBuffer);
}

/* --------------------------------------
            PRIVATE METHODS
   -------------------------------------- */

void InstancingMesh::InitializeInstance(Mesh* mesh)
{
  VertexArray& meshVao = mesh->vertexArray;
  
  /* Create empty vertex and index buffers */
  VertexArrayData data;
  data.vertDataSize = meshVao.vboSize;
  data.vertData = nullptr;
  data.indDataSize = meshVao.numIndices * sizeof(uint32_t);
  data.indData = nullptr;
  /* Initialize vertex array with same configuration of mesh */
  VertexArrayConfig config = meshVao.GetConfig();
  _vertexArray.InitVertexArray(data, config);
  /* Now copy vertex buffer data and index buffer data */
  meshVao.CopyVertexBufferData(_vertexArray.VertexBufferID());
  meshVao.CopyIndexBufferData(_vertexArray.IndexBufferID());

  _vertexArray.BindVertexArray();
  /* Initialize empty instance buffer */
  glGenBuffers(1, &_instanceBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferData(GL_ARRAY_BUFFER, _maxNumInstances * sizeof(Mat4f), nullptr, GL_DYNAMIC_DRAW);
  for (int i = 0; i < 4; i++)
  {
    glVertexAttribPointer(config.numAttrs + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4f), reinterpret_cast<void*>(i * sizeof(Vec4f)));
    glEnableVertexAttribArray(config.numAttrs + i);
    glVertexAttribDivisor(config.numAttrs + i, 1);
  }
}

#endif