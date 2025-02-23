#include "Shader.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/GL.hpp"


// ------------------------------
//      Shader
// ------------------------------ 

Shader::Shader() :
  id{ 0 }
{
}

void Shader::Create(i32 shaderType, StringView source)
{
  id = glCreateShader(shaderType);
  
  const char* ptr = source.data();
  i32 size = source.size();
  glShaderSource(id, 1, &ptr, &size);
}

void Shader::Delete()
{
  glDeleteShader(id);
  id = 0;
}

bool Shader::Compile() const
{
  glCompileShader(id);

  i32 success = GetParameteri(GL_COMPILE_STATUS);
  return success == GL_TRUE;
}

i32 Shader::GetParameteri(i32 name) const
{
  i32 param;
  glGetShaderiv(id, name, &param);
  return param;
}

const char* Shader::GetShaderInfo() const
{
  static Array<char, 1024> log;
  log.fill(0);

  glGetShaderInfoLog(id, log.size(), nullptr, log.data());
  return log.data();
}

// ------------------------------
//      Program
// ------------------------------

Program::Program() :
  id{ 0 }
{
}

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

  i32 link = GetParameteri(GL_LINK_STATUS);
  return link == GL_TRUE;
}

void Program::Use() const
{
  glUseProgram(id);
}

i32 Program::GetParameteri(i32 name) const
{
  i32 param;
  glGetProgramiv(id, name, &param);
  return param;
}

const char* Program::GetProgramInfo() const
{
  static Array<char , 1024> log;
	log.fill(0);

  glGetProgramInfoLog(id, log.size(), nullptr, log.data());
  return log.data();
}

i32 Program::GetUniformLocation(StringView name) const 
{ 
  return glGetUniformLocation(id, name.data());
}

i32 Program::GetUniformBlockIndex(StringView name) const
{
  return glGetUniformBlockIndex(id, name.data());
}

void Program::SetUniformBlockBinding(StringView blockname, i32 uniformBlockBinding) const
{
  i32 blockIndex = GetUniformBlockIndex(blockname);
  if (blockIndex == GL_INVALID_INDEX)
    CONSOLE_WARN("Program {}: invalid uniform block index '{}'", id, blockname);

  glUniformBlockBinding(id, blockIndex, uniformBlockBinding);
}

void Program::SetUniform1i(StringView uniformname, i32 value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform1i(id, loc, value);
}
void Program::SetUniform2i(StringView uniformname, const vec2i& value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform2i(id, loc, value.x, value.y);
}
void Program::SetUniform3i(StringView uniformname, const vec3i& value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform3i(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4i(StringView uniformname, const vec4i& value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform4i(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniform1f(StringView uniformname, f32 value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform1f(id, loc, value);
}
void Program::SetUniform2f(StringView uniformname, const vec2f& value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform2f(id, loc, value.x, value.y);
}
void Program::SetUniform3f(StringView uniformname, const vec3f& value) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform3f(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4f(StringView uniformname, const vec4f& value) const
{ 
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniform4f(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniformMat2f(StringView uniformname, const mat2f& value, bool transpose) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniformMatrix2fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat3f(StringView uniformname, const mat3f& value, bool transpose) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniformMatrix3fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat4f(StringView uniformname, const mat4f& value, bool transpose) const
{
  i32 loc = GetUniformLocation(uniformname);
  if (loc == -1)
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);

  glProgramUniformMatrix4fv(id, loc, 1, transpose, &value[0][0]);
}
