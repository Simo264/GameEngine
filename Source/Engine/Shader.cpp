#include "Shader.hpp"
#include "Engine/Logger.hpp"

/* -----------------------------------------------------
  *          PUBLIC METHODS
  * -----------------------------------------------------
*/

void Shader::InitShader(const char* label, const char* vertFile, const char* fragFile)
{
  _label = label;

  String vShaderSrc;
  String fShaderSrc;
  GetSourceFromFile(vertFile, vShaderSrc);
  GetSourceFromFile(fragFile, fShaderSrc);

  char infoLog[512];

  /* vertex shader */
  uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
  if (!Compile(vertexShader, vShaderSrc.c_str()))
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    CONSOLE_WARN("ERROR::SHADER::VERTEX::COMPILATION_FAILED {}", infoLog);
    return;
  }

  /* fragment shader */
  uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  if (!Compile(fragmentShader, fShaderSrc.c_str()))
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    CONSOLE_WARN("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED {}", infoLog);
    return;
  }

  /* build and compile our shader program */
  if (!CreateAndLink(vertexShader, fragmentShader))
  {
    glGetProgramInfoLog(_shaderID, 512, NULL, infoLog);
    CONSOLE_WARN("ERROR::SHADER::PROGRAM::LINKING_FAILED {}", infoLog);
    return;
  }
}

void Shader::SetBool(const char* name, bool value) const
{
  glUniform1i(glGetUniformLocation(_shaderID, name), (int)value);
}

void Shader::SetInt(const char* name, int value) const
{
  glUniform1i(glGetUniformLocation(_shaderID, name), value);
}

void Shader::SetFloat(const char* name, float value) const
{
  glUniform1f(glGetUniformLocation(_shaderID, name), value);
}

void Shader::SetVec2f(const char* name, const Vec2f& value) const
{
  glUniform2fv(glGetUniformLocation(_shaderID, name), 1, &value[0]);
}

void Shader::SetVec3f(const char* name, const Vec3f& value) const
{
  glUniform3fv(glGetUniformLocation(_shaderID, name), 1, &value[0]);
}

void Shader::SetVec4f(const char* name, const Vec4f& value) const
{
  glUniform4fv(glGetUniformLocation(_shaderID, name), 1, &value[0]);
}

void Shader::SetMat2f(const char* name, const Mat2f& mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(_shaderID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3f(const char* name, const Mat3f& mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(_shaderID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4f(const char* name, const Mat4f& mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(_shaderID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetIntArray(const char* name, uint32_t count, int* value)
{
  glUniform1iv(glGetUniformLocation(_shaderID, name), count, value);
}

void Shader::SetFloatArray(const char* name, uint32_t count, float* value)
{
  glUniform1fv(glGetUniformLocation(_shaderID, name), count, value);
}



/* -----------------------------------------------------
  *          PRIVATE METHODS
  * -----------------------------------------------------
*/


void Shader::GetSourceFromFile(const char* sourceFile, String& dest)
{
  OStringStream buffer;
  IFStream file(sourceFile);
  if (!file.is_open())
  {
    CONSOLE_ERROR("Shader::getSourceFromFile error on opening file {}", sourceFile);
    return;
  }

  buffer << file.rdbuf();

  dest = buffer.str();

  file.close();
}

bool Shader::Compile(uint32_t& shader, const char* src)
{
  int success;
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  return (bool)success;
}

bool Shader::CreateAndLink(uint32_t vShader, uint32_t fShader)
{
  int success;

  _shaderID = glCreateProgram();
  glAttachShader(_shaderID, vShader);
  glAttachShader(_shaderID, fShader);
  glLinkProgram(_shaderID);

  glGetProgramiv(_shaderID, GL_LINK_STATUS, &success);

  glDeleteShader(vShader);
  glDeleteShader(fShader);
  return (bool)success;
}
