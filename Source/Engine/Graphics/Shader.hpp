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
 * 
 */
class Shader
{
public:
  Shader();

  ~Shader() = default;

  /**
   * Creates an empty shader object and returns a non-zero value by which it can be referenced.
   * A shader object is used to maintain the source code strings that define a shader.
   * 
   * @param shaderType: specifies the type of shader to be created. Must be one of 
   *                    GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, 
   *                    GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
   */
  void Create(int shaderType);

  /**
   * Frees the memory and invalidates the name associated with the shader object
   */
  void Delete();

  /**
   * Replaces the source code in a shader object.
   * If shader previously had source code loaded into it, the existing source code is completely replaced.
   * 
   * @param string: specifies the source code to be loaded into the shader
   * 
   * @param length: specifies the string length
   */
  void LoadSource(const char* string, int length) const;

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
  int GetParameteri(int name) const;

  /**
   * Returns the information log for the specified shader object. 
   * The information log for a shader object is modified when the shader is compiled.
   */
  string GetShaderInfo() const;

  constexpr bool IsValid() const { return id != static_cast<uint32_t>(-1); }

  uint32_t id;

  string filename; /* The shader file name */
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
  Program();

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
  int GetParameteri(int name) const;

  /**
   * Returns the information log for the program object
   */
  string GetProgramInfo() const;

  /**
   * Installs the program object specified as part of current rendering state. 
   * One or more executables are created in a program object by successfully attaching shader objects,
   * successfully compiling the shader objects, and successfully linking the program object.
   */
  void Use() const;

  /**
   * Returns the location of a uniform variable
   */
  int GetUniformLocation(const char* name) const;

  /**
   * Specify the value of a uniform variable for the program object
   */
  void SetUniform1i(const char* uniformname, int value) const;
  void SetUniform2i(const char* uniformname, const Vec2i32& value) const;
  void SetUniform3i(const char* uniformname, const Vec3i32& value) const;
  void SetUniform4i(const char* uniformname, const Vec4i32& value) const;
  void SetUniform1f(const char* uniformname, float value) const;
  void SetUniform2f(const char* uniformname, const Vec2f& value) const;
  void SetUniform3f(const char* uniformname, const Vec3f& value) const;
  void SetUniform4f(const char* uniformname, const Vec4f& value) const;
  void SetUniformMat2f(const char* uniformname, const Mat2f& value, bool transpose = false) const;
  void SetUniformMat3f(const char* uniformname, const Mat3f& value, bool transpose = false) const;
  void SetUniformMat4f(const char* uniformname, const Mat4f& value, bool transpose = false) const;

  uint32_t id;

  string name; /* The name of the program */
};
