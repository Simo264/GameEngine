#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/**
 * https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (chapter 7)
 * 
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
   * Creates an empty shader object and returns a non-zero value by which it can be referenced.
   * A shader object is used to maintain the source code strings that define a shader.
   * 
   * @param shaderType: specifies the type of shader to be created. Must be one of 
   *                    GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, 
   *                    GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
   */
  void Create(i32 shaderType);

  /**
   * Frees the memory and invalidates the name associated with the shader object
   */
  void Delete();

  /**
   * Replaces the source code in a shader object.
   * If shader previously had source code loaded into it, the existing source code is completely replaced.
   * 
   * @param source: specifies the source code to be loaded into the shader
   * @param size: specifies the string length
   */
  void LoadSource(StringView source) const;

  /**
   * Compiles the source code strings that have been stored in the shader object.
   */
  bool Compile() const;

  /**
   * Return a parameter from a shader object
   * 
   * @param name: accepted symbolic names are:
   *              GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, 
   *              GL_SHADER_SOURCE_LENGTH.
   */
  i32 GetParameteri(i32 name) const;

  /**
   * Returns the information log for the specified shader object. 
   * The information log for a shader object is modified when the shader is compiled.
   */
  const char* GetShaderInfo() const;

  u32 id;
};

/**
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
   * Creates an empty program object and returns a non-zero value by which it can be referenced. 
   * A program object is an object to which shader objects can be attached.
   */
  void Create();

  /**
   * Frees the memory and invalidates the name associated with the program object
   */
  void Delete();

  /**
   * Attaches a shader object to the program object
   */
  void AttachShader(const Shader& shader) const;

  /**
   * Detaches the shader object specified from the program object
   */
  void DetachShader(const Shader& shader) const;

  /**
   * Links the program object specified.
   * Returns the status of the link operation.
   */
  bool Link() const;
    
  /**
   * Returns a parameter from the program object. 
   * 
   * @param name: accepted symbolic names are:
   *              GL_DELETE_STATUS, GL_LINK_STATUS, GL_VALIDATE_STATUS, GL_INFO_LOG_LENGTH, GL_ATTACHED_SHADERS, 
   *              GL_ACTIVE_ATOMIC_COUNTER_BUFFERS, GL_ACTIVE_ATTRIBUTES, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, GL_ACTIVE_UNIFORMS, 
   *              GL_ACTIVE_UNIFORM_BLOCKS, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, GL_ACTIVE_UNIFORM_MAX_LENGTH, 
   *              GL_COMPUTE_WORK_GROUP_SIZE, GL_PROGRAM_BINARY_LENGTH, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, 
   *              GL_TRANSFORM_FEEDBACK_VARYINGS, GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH, GL_GEOMETRY_VERTICES_OUT, 
   *              GL_GEOMETRY_INPUT_TYPE, and GL_GEOMETRY_OUTPUT_TYPE.
   */
  i32 GetParameteri(i32 name) const;

  /**
   * Returns the information log for the program object
   */
  const char* GetProgramInfo() const;

  /**
   * Installs the program object specified as part of current rendering state. 
   * One or more executables are created in a program object by successfully attaching shader objects,
   * successfully compiling the shader objects, and successfully linking the program object.
   */
  void Use() const;

  /**
   * Returns the location of a uniform variable
   */
  i32 GetUniformLocation(StringView name) const;

  /**
   * Retrieve the index of a named uniform block
   */
  i32 GetUniformBlockIndex(StringView name) const;

  /**
   * Assign a binding poi32 to an active uniform block
   */
  void SetUniformBlockBinding(StringView blockname, i32 uniformBlockBinding) const;

  /**
   * Specify the value of a uniform variable for the program object
   */
  void SetUniform1i(StringView uniformname, i32 value) const;
  void SetUniform2i(StringView uniformname, const vec2i32& value) const;
  void SetUniform3i(StringView uniformname, const vec3i32& value) const;
  void SetUniform4i(StringView uniformname, const vec4i32& value) const;
  void SetUniform1f(StringView uniformname, f32 value) const;
  void SetUniform2f(StringView uniformname, const vec2f& value) const;
  void SetUniform3f(StringView uniformname, const vec3f& value) const;
  void SetUniform4f(StringView uniformname, const vec4f& value) const;
  void SetUniformMat2f(StringView uniformname, const mat2f& value, bool transpose = false) const;
  void SetUniformMat3f(StringView uniformname, const mat3f& value, bool transpose = false) const;
  void SetUniformMat4f(StringView uniformname, const mat4f& value, bool transpose = false) const;

  u32 id;

  char name[64]; /* <- only for debugging use */
};
