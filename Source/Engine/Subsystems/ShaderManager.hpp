#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Shader.hpp"

/**
 * @class ShaderManager.
 * 
 * @brief Singleton class responsible for managing shader and program objects.
 * 
 * This class handles shader compilation, program linking, and cleanup.
 */
class ShaderManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	ShaderManager(const ShaderManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const ShaderManager&) = delete;

	/** Get the singleton instance of ShaderManager. */
	static ShaderManager& Get()
	{
		static ShaderManager shaderManager;
		return shaderManager;
	}

	/** @brief Load all shaders and programs. */
	void Initialize();
	/** @brief Destroy all shader objects and program objects. */
	void CleanUp();

	/**
	 * @brief Retrieve a shader by its filename.
	 * @throw std::out_of_range If the shader is not found, throws an exception.
	 */
	Shader& GetShaderByName(StringView filename);
	
	/**
	 * @brief Retrieve a program by its name.
	 * @throw std::out_of_range If the program is not found, throws an exception.
	 */
	Program& GetProgramByName(StringView name);
	/** @return A constant reference to the map of shaders. */
	const auto& GetShaders() const { return _shaders; }
	/** @return A constant reference to the map of programs. */
	const auto& GetPrograms() const { return _programs; }
	
private:
	ShaderManager() = default;
	~ShaderManager() = default;

	/** @brief Map storing shader objects by their filename. */
	UnorderedMap<String, Shader> _shaders;
	/** @brief Map storing program objects by their name. */
	UnorderedMap<String, Program> _programs;

	/** @brief Load shaders from files. */
	void LoadShaders();
	/** @brief Compile loaded shaders. */
	void CompileShaders();
	/** @brief Load programs from configuration or files. */
	void LoadPrograms();
	/** @brief Link loaded shaders to create program objects. */
	void LinkPrograms();
	/** @brief Set the uniforms for shader programs. */
	void SetProgramsUniforms();
};
