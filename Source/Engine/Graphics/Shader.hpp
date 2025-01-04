#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

/**
 * @brief
 * https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (chapter 7)
 * 
 * A shader specifies operations that are meant to occur on data as it moves
 * through different programmable stages of the OpenGL processing pipeline, 
 * starting with vertices specified by the application and ending with fragments prior to
 * being written to the framebuffer. The programming language used for shaders is
 * described in the OpenGL Shading Language Specification.
 * 
 * To use a shader, shader source code is first loaded into a shader object and then
 * compiled. A shader object corresponds to a stage in the rendering pipeline referred
 * to as its shader stage or shader type.
 */
class Shader
{
public:
  Shader() : 
    id{ 0 } 
  {}
  ~Shader() = default;

  /**
   * @brief
   * Creates an empty shader object and returns a non-zero value by which it can be referenced.
   * A shader object is used to maintain the source code strings that define a shader.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml
   */
  void Create(i32 shaderType);

  /**
   * @brief
   * Frees the memory and invalidates the name associated with the shader object
   */
  void Delete();

  /**
   * @brief
   * Replaces the source code in a shader object.
   * If shader previously had source code loaded into it, the existing source code is completely replaced.
   * 
   * @param source: specifies the source code to be loaded into the shader
   */
  void LoadSource(StringView source) const;

  /**
   * @brief
   * Compiles the source code strings that have been stored in the shader object.
   */
  bool Compile() const;

  /**
   * @return
   * A parameter from a shader object. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShader.xhtml
   */
  i32 GetParameteri(i32 name) const;

  /**
   * @return
   * The information log for the specified shader object.
   * The log for a shader object is modified when the shader is compiled.
   */
  const char* GetShaderInfo() const;

  u32 id;
};

/**
 * @brief
 * https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (chapter 7)
 * 
 * One or more shader objects are attached to a program object. 
 * The program object is then linked, which generates executable code from all the compiled shader
 * objects attached to the program.
 * 
 * When program objects are bound to a shader stage, they become the current
 * program object for that stage. When the current program object for a shader stage
 * includes a shader of that type, it is considered the active program object for that stage.
 * 
 * The current program object for all stages may be set at once using a single unified program object,
 * or the current program object may be set for each stage individually using a separable program object
 * where different separable program objects may be current for other stages.
 * 
 * The set of separable program objects current for all stages are collected in a program pipeline object
 * that must be bound for use. 
 * When a linked program object is made active for one of the stages, the corresponding executable code
 * is used to perform processing for that stage.
 */
class Program
{
public:
  Program(StringView programName);
  ~Program() = default;

  /**
   * @brief
   * Creates an empty program object and returns a non-zero value by which it can be referenced. 
   * A program object is an object to which shader objects can be attached.
   */
  void Create();

  /**
   * @brief
   * Frees the memory and invalidates the name associated with the program object
   */
  void Delete();

  /**
   * @brief
   * Attaches a shader object to the program object
   */
  void AttachShader(const Shader& shader) const;

  /**
   * @brief
   * Detaches the shader object specified from the program object
   */
  void DetachShader(const Shader& shader) const;

  /**
   * @brief
   * Links the program object specified.
   * 
   * @return
   * The status of the link operation.
   */
  bool Link() const;
    
  /**
   * @return
   * A parameter from the program object. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgram.xhtml
   */
  i32 GetParameteri(i32 name) const;

  /**
   * @return 
   * The information log for the program object
   */
  const char* GetProgramInfo() const;

  /**
   * @brief
   * Installs the program object specified as part of current rendering state. 
   * One or more executables are created in a program object by successfully attaching shader objects,
   * successfully compiling the shader objects, and successfully linking the program object.
   */
  void Use() const;

  /**
   * @return
   * The location of a uniform variable
   */
  i32 GetUniformLocation(StringView name) const;

  /**
   * @return
   * The index of a named uniform block
   */
  i32 GetUniformBlockIndex(StringView name) const;

  /**
   * @brief
   * Assign a binding poi32 to an active uniform block
   */
  void SetUniformBlockBinding(StringView blockname, i32 uniformBlockBinding) const;

  /**
   * @brief
   * Specify the value of a uniform variable for the program object
   */
  void SetUniform1i(StringView uniformname, i32 value) const;
  void SetUniform2i(StringView uniformname, const vec2i& value) const;
  void SetUniform3i(StringView uniformname, const vec3i& value) const;
  void SetUniform4i(StringView uniformname, const vec4i& value) const;
  void SetUniform1f(StringView uniformname, f32 value) const;
  void SetUniform2f(StringView uniformname, const vec2f& value) const;
  void SetUniform3f(StringView uniformname, const vec3f& value) const;
  void SetUniform4f(StringView uniformname, const vec4f& value) const;
  void SetUniformMat2f(StringView uniformname, const mat2f& value, bool transpose = false) const;
  void SetUniformMat3f(StringView uniformname, const mat3f& value, bool transpose = false) const;
  void SetUniformMat4f(StringView uniformname, const mat4f& value, bool transpose = false) const;

  u32 id;

  char name[64]; /* useful for log */
};
