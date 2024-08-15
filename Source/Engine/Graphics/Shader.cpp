#include "Shader.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/OpenGL.hpp"


/* ------------------------------
      Shader
------------------------------ */

Shader::Shader()
  : id { 0 }
{}

void Shader::Create(int shaderType)
{
  id = glCreateShader(shaderType);
}

void Shader::Delete()
{
  glDeleteShader(id);
  id = 0;
}

void Shader::LoadSource(const string& source) const
{
  const char* ptr = source.c_str();
  const int len = source.size();
  glShaderSource(id, 1, &ptr, &len);
}

bool Shader::Compile() const
{
  glCompileShader(id);

  int success = GetParameteri(GL_COMPILE_STATUS);
  return success == GL_TRUE;
}

int Shader::GetParameteri(int name) const
{
  int param;
  glGetShaderiv(id, name, &param);
  return param;
}

string Shader::GetShaderInfo() const
{
  char log[1024];
  glGetShaderInfoLog(id, sizeof(log), nullptr, log);

  return string(log);
}

/* ------------------------------
      Program
------------------------------ */

Program::Program()
  : id{ 0 }
{}

void Program::Create()
{
  id = glCreateProgram();
}

void Program::Delete()
{
  glDeleteProgram(id);
  id = 0;
}

void Program::AttachShader(const Shader& shader) const
{
  glAttachShader(id, shader.id);
}

void Program::DetachShader(const Shader& shader) const
{
  glDetachShader(id, shader.id);
}

bool Program::Link() const
{
  glLinkProgram(id);

  int link = GetParameteri(GL_LINK_STATUS);
  return link == GL_TRUE;
}

void Program::Use() const
{
  glUseProgram(id);
}

int Program::GetParameteri(int name) const
{
  int param;
  glGetProgramiv(id, name, &param);
  return param;
}

string Program::GetProgramInfo() const
{
  char log[1024];
  glGetProgramInfoLog(id, sizeof(log), nullptr, log);
  return string(log);
}

int Program::GetUniformLocation(const char* name) const 
{ 
  return glGetUniformLocation(id, name);
}

int Program::GetUniformBlockIndex(const char* name) const
{
  return glGetUniformBlockIndex(id, name);
}

void Program::SetUniformBlockBinding(const char* blockname, int uniformBlockBinding) const
{
  int blockIndex = GetUniformBlockIndex(blockname);
  if (blockIndex == GL_INVALID_INDEX)
    CONSOLE_WARN("Program {}: invalid uniform block index '{}'", name, blockname);

  glUniformBlockBinding(id, blockIndex, uniformBlockBinding);
}

void Program::SetUniform1i(const char* uniformname, int value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform1i(id, loc, value);
}
void Program::SetUniform2i(const char* uniformname, const vec2i32& value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform2i(id, loc, value.x, value.y);
}
void Program::SetUniform3i(const char* uniformname, const vec3i32& value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform3i(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4i(const char* uniformname, const vec4i32& value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform4i(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniform1f(const char* uniformname, float value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform1f(id, loc, value);
}
void Program::SetUniform2f(const char* uniformname, const vec2f& value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform2f(id, loc, value.x, value.y);
}
void Program::SetUniform3f(const char* uniformname, const vec3f& value) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform3f(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4f(const char* uniformname, const vec4f& value) const
{ 
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniform4f(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniformMat2f(const char* uniformname, const mat2f& value, bool transpose) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniformMatrix2fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat3f(const char* uniformname, const mat3f& value, bool transpose) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniformMatrix3fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat4f(const char* uniformname, const mat4f& value, bool transpose) const
{
  int loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", name, uniformname);

  glProgramUniformMatrix4fv(id, loc, 1, transpose, &value[0][0]);
}

