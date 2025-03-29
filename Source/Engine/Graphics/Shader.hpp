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
 * 
 * ===========================================================
 *     SHADING MODELS OVERVIEW
 * ===========================================================
 * 
 * Shading models can be categorized into two main groups:
 * 1. Realistic Shading Models
 * 2. Non-Photorealistic Rendering (NPR) Models.
 * 
 * ===========================================================
 *     REALISTIC SHADING MODELS
 * ===========================================================
 * These models aim to simulate light behavior as accurately as possible.
 * 
 * 1. Classic Models (Local Illumination)
 *    - Flat Shading: One color per triangle, sharp edges, low realism.
 *    - Gouraud Shading: Interpolates lighting per vertex, efficient but can lose details.
 *    - Phong Shading: Interpolates normals per fragment, smoother and more realistic.
 * 
 * 2. Physically Based Rendering (PBR) Models
 *    - Blinn-Phong: An optimized version of Phong, used for specular highlights.
 *    - Cook-Torrance: Uses microfacet distribution for complex surface reflection.
 *    - Oren-Nayar: Extends Lambertian reflection for rough surfaces.
 *    - Disney BRDF: A flexible shading model combining multiple techniques.
 * 
 * PBR models are the standard in modern game engines (e.g., Unreal Engine, Unity).
 * 
 * ===========================================================
 *     NON-PHOTOREALISTIC RENDERING (NPR) MODELS
 * ===========================================================
 * These models focus on artistic styles rather than realism.
 * 
 * 1. Toon Shading (Cel Shading) � Uses flat colors and strong outlines (e.g., Zelda: Breath of the Wild).
 * 2. Gooch Shading � Transitions from blue to yellow to enhance shape perception.
 * 3. X-Toon Shading � Advanced toon shading with more gradient levels.
 * 4. Hatching Shading � Simulates pencil sketching using texture overlays.
 * 5. Silhouette Shading � Highlights model edges for blueprint-style rendering.
 * 
 * ===========================================================
 *     IMPLEMENTATION IN A CUSTOM ENGINE
 * ===========================================================
 * - For a realistic rendering engine, focus on Phong, Blinn-Phong, or PBR.
 * - For a flexible engine, support NPR techniques like Toon and Gooch shading.
 */

class Shader
{
public:
  Shader();
  ~Shader() = default;

  /**
   * @brief
   * Creates an empty shader object and returns a non-zero value by which it can be referenced.
   * A shader object is used to maintain the source code strings that define a shader.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml
   */
  void Create(i32 shaderType, StringView source);

  /** @brief Frees the memory and invalidates the name associated with the shader object */
  void Delete();

  /** @brief Compiles the source code strings that have been stored in the shader object. */
  bool Compile() const;

  /** @return A parameter from a shader object. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShader.xhtml */
  i32 GetParameteri(i32 name) const;

  /**
   * @return
   * The information log for the specified shader object.
   * The log for a shader object is modified when the shader is compiled.
   */
  const char* GetShaderInfo() const;

  bool IsValid() const { return id != 0; }

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
  Program();
  ~Program() = default;

  /**
   * @brief
   * Creates an empty program object and returns a non-zero value by which it can be referenced. 
   * A program object is an object to which shader objects can be attached.
   */
  void Create();

  /** @brief Frees the memory and invalidates the name associated with the program object */
  void Delete();

  /** @brief Attaches a shader object to the program object */
  void AttachShader(Shader shader) const;

  /** @brief Detaches the shader object specified from the program object */
  void DetachShader(Shader shader) const;

  /**
   * @brief Links the program object specified.
   * @return The status of the link operation.
   */
  bool Link() const;
    
  /** @return A parameter from the program object. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgram.xhtml */
  i32 GetParameteri(i32 name) const;

  /** @return The information log for the program object */
  const char* GetProgramInfo() const;

  /**
   * @brief
   * Installs the program object specified as part of current rendering state. 
   * One or more executables are created in a program object by successfully attaching shader objects,
   * successfully compiling the shader objects, and successfully linking the program object.
   */
  void Use() const;

  /** @return The location of a uniform variable */
  i32 GetUniformLocation(StringView name) const;

  /** @return The index of a named uniform block */
  u32 GetUniformBlockIndex(StringView name) const;

  /** @brief Assign a binding poi32 to an active uniform block */
  void SetUniformBlockBinding(StringView blockname, i32 uniformBlockBinding) const;

  /** @brief Specify the value of a uniform variable for the program object */
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

  bool IsValid() const { return id != 0; }

  u32 id;
};
